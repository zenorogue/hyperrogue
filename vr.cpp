// Hyperbolic Rogue -- VR support
// Copyright (C) 2020-2020 Zeno Rogue, see 'hyper.cpp' for details

/** \file vr.cpp
 *  \brief VR support
 */

#include "hyper.h"
namespace hr {

EX namespace vrhr {

#if !CAP_VR
#if HDR
inline bool active() { return false; }
inline bool rendering() { return false; }
inline bool rendering_eye() { return false; }
#endif
#endif

#if CAP_VR

/** VR is active */
EX bool active() { return state; }
/** called in drawqueue to see if we should switch to vrhr::render() */
EX bool should_render() { return state == 1; }
/** currently rendering a VR-aware screen */
EX bool rendering() { return state == 2 || state == 4; }
/** currently rendering a VR eye */
EX bool rendering_eye() { return state == 2; }

#if HDR
enum class eHeadset { none, rotation_only, reference, holonomy, model_viewing };
enum class eEyes { none, equidistant, truesim };
enum class eCompScreen { none, reference, single, eyes };
#endif

EX eHeadset hsm = eHeadset::reference;
EX eEyes eyes = eEyes::equidistant;
EX eCompScreen cscr = eCompScreen::eyes;

EX bool controllers_on_desktop = true;

EX cell *forward_cell;

EX ld vraim_x, vraim_y, vrgo_x, vrgo_y;

EX ld pointer_length = 1;

vector<pair<string, string> > headset_desc = {
  {"none", "Ignore the headset movement and rotation."},
  {"rotation only", "Ignore the headset movement but do not ignore its rotation."},
  {"reference", "The reference point in the real world corresponds to the reference point in VR. When you move your head in a loop, you return to where you started."},
  {"holonomy", "Headsets movements in the real world are translated to the same movements in VR. Since the geometry is different, when you move your head in a loop, you usually don't return "
   "to where you started."},
  {"view model", "Fix a 3D projection of the non-Euclidean world, and see it from many viewpoints."}
  };

vector<pair<string, string> > eyes_desc = {
  {"none", "Both eyes see the same image."},
  {"equidistant", "Render the image so that the perceived direction and distance is correct."},
  {"true vision", "Simulate the actual binocular vision in the non-Euclidean space. Hyperbolic spaces look smaller than they are (stretched Klein model), spherical spaces look weird, "
    "nonisotropic spaces are incomprehensible."}, /* not implemented */
  };

/* not implemented */
vector<pair<string, string> > comp_desc = {
  {"none", "Do not display anything on the computer screen."},
  {"reference", "Display the standard HyperRogue view from the reference point."},
  {"single", "Display a a single monocular image from the headset."},
  {"eyes", "Display a copy of the VR display."},
  };

struct vr_rendermodel {
  string name;
  GLuint texture_id;
  vector<glhr::textured_vertex> vertices;
  };

struct vr_framebuffer {
  bool ok;
  GLuint m_nDepthBufferId;
  GLuint m_nRenderTextureId;
  GLuint m_nRenderFramebufferId;
  GLuint m_nResolveTextureId;
  GLuint m_nResolveFramebufferId;
  vr_framebuffer(int x, int y);
  ~vr_framebuffer();
  };

vr_framebuffer::vr_framebuffer(int xsize, int ysize) {
  resetbuffer rb;
  glGenFramebuffers(1, &m_nRenderFramebufferId );
  glBindFramebuffer(GL_FRAMEBUFFER, m_nRenderFramebufferId);

  glGenRenderbuffers(1, &m_nDepthBufferId);
  glBindRenderbuffer(GL_RENDERBUFFER, m_nDepthBufferId);
  glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, xsize, ysize );
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_nDepthBufferId );
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_nDepthBufferId );

  glGenTextures(1, &m_nRenderTextureId );
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_nRenderTextureId );
  glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, xsize, ysize, true);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_nRenderTextureId, 0);

  glGenFramebuffers(1, &m_nResolveFramebufferId );
  glBindFramebuffer(GL_FRAMEBUFFER, m_nResolveFramebufferId);

  glGenTextures(1, &m_nResolveTextureId );
  glBindTexture(GL_TEXTURE_2D, m_nResolveTextureId );
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, xsize, ysize, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_nResolveTextureId, 0);

  // check FBO status
  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  
  ok = status == GL_FRAMEBUFFER_COMPLETE;
  
  rb.reset();
  }

EX transmatrix eyeproj, eyeshift;

vr_framebuffer::~vr_framebuffer() {
  glDeleteRenderbuffers( 1, &m_nDepthBufferId );
  glDeleteTextures( 1, &m_nRenderTextureId );
  glDeleteFramebuffers( 1, &m_nRenderFramebufferId );
  glDeleteTextures( 1, &m_nResolveTextureId );
  glDeleteFramebuffers( 1, &m_nResolveFramebufferId );
  }

struct controller_data {
  int x, y, clicked;
  };

struct vrdata_t {
  vr::IVRSystem *vr;
  uint32_t xsize, ysize;
  vr_framebuffer *eyes[2];
  transmatrix proj[2];
  transmatrix iproj[2]; /* inverse of proj */
  transmatrix eyepos[2];
  vr::TrackedDevicePose_t poses[ vr::k_unMaxTrackedDeviceCount ];
  transmatrix pose_matrix[vr::k_unMaxTrackedDeviceCount ];
  transmatrix last_pose_matrix[ vr::k_unMaxTrackedDeviceCount ];
  vector<vr_rendermodel*> models;
  vr_rendermodel* device_models[ vr::k_unMaxTrackedDeviceCount ];
  controller_data cdata [ vr::k_unMaxTrackedDeviceCount ];
  };

/** 0,1 == eyes, 2 == headset */
EX transmatrix hmd_mv_for[3];
EX transmatrix hmd_pre_for[3];

vrdata_t vrdata;

/** how far is the object pointed to */
EX ld pointer_distance;

/** should we try to access VR */
EX bool enabled = false;

/** we tried to access VR but failed */
EX bool failed;

/** VR error message */
EX string error_msg;

/** 0 = not loaded, 1 = loaded but not currently rendering, 2 = currently rendering the VR screen, 3 = currently rendering the reference computer screen, 4 = currently rendering the single computer screen */
EX int state = 0;

#if HDR
// use E4 when working with real-world matrices to ensure that inverses, multiplications, etc. are computed correctly
#define E4 dynamicval<eGeometry> g(geometry, gCubeTiling)
#endif

#define IN_E4(x) [&]{ E4; return x; }()

std::string GetTrackedDeviceString( vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError *peError = NULL ) {
  uint32_t unRequiredBufferLen = vr::VRSystem()->GetStringTrackedDeviceProperty( unDevice, prop, NULL, 0, peError );
  if( unRequiredBufferLen == 0 ) return "";

  char *pchBuffer = new char[ unRequiredBufferLen ];
  unRequiredBufferLen = vr::VRSystem()->GetStringTrackedDeviceProperty( unDevice, prop, pchBuffer, unRequiredBufferLen, peError );
  std::string sResult = pchBuffer;
  delete [] pchBuffer;
  return sResult;
  }

transmatrix vr_to_hr(vr::HmdMatrix44_t mat) {
  transmatrix T;
  for(int i=0; i<4; i++)
  for(int j=0; j<4; j++)
    T[i][j] = mat.m[i][j];
  return T;
  }

transmatrix vr_to_hr(vr::HmdMatrix34_t mat) {
  transmatrix T;
  for(int i=0; i<3; i++)
  for(int j=0; j<4; j++)
    T[i][j] = mat.m[i][j];
  T[3][0] = 0;
  T[3][1] = 0;
  T[3][2] = 0;
  T[3][3] = 1;
  return T;
  }

string device_class_name(vr::ETrackedDeviceClass v) {
  if(v == vr::TrackedDeviceClass_Controller)
    return "controller";
  if(v == vr::TrackedDeviceClass_HMD)
    return "HMD";
  if(v == vr::TrackedDeviceClass_Invalid)
    return "invalid";
  if(v == vr::TrackedDeviceClass_GenericTracker)
    return "tracker";
  if(v == vr::TrackedDeviceClass_TrackingReference)
    return "reference";
  return "unknown";
  }

EX bool first = true;

EX transmatrix hmd_at_ui = Id;
EX transmatrix hmd_at = Id;
EX transmatrix hmd_ref_at = Id;

EX transmatrix hmd_mvp, hmd_pre, hmd_mv;

EX transmatrix sm;

EX int ui_xmin, ui_ymin, ui_xmax, ui_ymax;

EX reaction_t change_ui_bounds;

#if HDR
struct frustum_info {
  transmatrix pre;
  transmatrix nlp;
  bool screen;
  transmatrix proj;
  };
#endif

EX vector<frustum_info> frusta;

EX void set_ui_bounds() {
  ui_xmin = 0;
  ui_ymin = 0;
  ui_xmax = current_display->xsize;
  ui_ymax = current_display->ysize;  
  if(change_ui_bounds)
    change_ui_bounds();
  }

EX void size_and_draw_ui_box() {
  if(!vrhr::active()) return;
  if(!vrhr::in_menu()) return;

  vrhr::set_ui_bounds();
  color_t col = 0x000000C0;
  current_display->next_shader_flags = 0;
  dynamicval<eModel> m(pmodel, mdPixel);
  
  vrhr::in_vr_ui([&] {
    glhr::color2(col);
    glhr::set_depthtest(false);
    vector<glvertex> vs;
    vs.emplace_back(glhr::makevertex(ui_xmin, ui_ymin, 0));
    vs.emplace_back(glhr::makevertex(ui_xmax, ui_ymin, 0));
    vs.emplace_back(glhr::makevertex(ui_xmax, ui_ymax, 0));
    vs.emplace_back(glhr::makevertex(ui_xmin, ui_ymin, 0));
    vs.emplace_back(glhr::makevertex(ui_xmin, ui_ymax, 0));
    vs.emplace_back(glhr::makevertex(ui_xmax, ui_ymax, 0));
    glhr::current_vertices = NULL;
    glhr::vertices(vs);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    });
  }

vr_rendermodel *get_render_model(string name) {
  for(auto& m: vrdata.models)
    if(m->name == name)
      return m;
    
  println(hlog, "trying to load model ", name);
  
  vr::RenderModel_t *pModel;
  vr::EVRRenderModelError error;
  while(1) {
    error = vr::VRRenderModels()->LoadRenderModel_Async(name.c_str(), &pModel );
    if(error != vr::VRRenderModelError_Loading) break;
    usleep(1000);
    }
  if(error != vr::VRRenderModelError_None) {
    println(hlog, "Unable to load render model %s - %s\n", name, vr::VRRenderModels()->GetRenderModelErrorNameFromEnum( error ) );
    return NULL;
    }

  vr::RenderModel_TextureMap_t *pTexture;
  while (1) {
    error = vr::VRRenderModels()->LoadTexture_Async( pModel->diffuseTextureId, &pTexture );
    if(error != vr::VRRenderModelError_Loading) break;
    usleep(1000);
    }
  if(error != vr::VRRenderModelError_None) {
    println(hlog, "Unable to load render texture id:%d for render model %s\n", pModel->diffuseTextureId, name);
    vr::VRRenderModels()->FreeRenderModel( pModel );
    return NULL; // move on to the next tracked device
    }

  auto md = new vr_rendermodel;
  vrdata.models.emplace_back(md);
  md->name = name;
  
  int cnt = pModel->unTriangleCount * 3;
  for(int i=0; i<cnt; i++) {
    glhr::textured_vertex tv;
    int id = pModel->rIndexData[i];
    for(int j=0; j<3; j++)
      tv.coords[j] = pModel->rVertexData[id].vPosition.v[j];
    tv.coords[3] = 1;
    for(int j=0; j<2; j++)
      tv.texture[j] = pModel->rVertexData[id].rfTextureCoord[j];
    md->vertices.push_back(tv);
    }

  glGenTextures(1, &md->texture_id);
  glBindTexture( GL_TEXTURE_2D, md->texture_id);

  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, pTexture->unWidth, pTexture->unHeight,
    0, GL_RGBA, GL_UNSIGNED_BYTE, pTexture->rubTextureMapData );

  glGenerateMipmap(GL_TEXTURE_2D);

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

  GLfloat fLargest;
  glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest );

  glBindTexture( GL_TEXTURE_2D, 0 );
  
  println(hlog, "model loaded successfully");
  return md;
  }

EX bool need_poses = true;

void track_poses() {
  E4;

  if(!need_poses) return;
  need_poses = false;
  vr::VRCompositor()->WaitGetPoses(vrdata.poses, vr::k_unMaxTrackedDeviceCount, NULL, 0 );
  // println(hlog, "poses received");
  
  for(int i=0; i<(int)vr::k_unMaxTrackedDeviceCount; i++) {
    auto& p = vrdata.poses[i];
    vrdata.device_models[i] = nullptr;
    if(!p.bPoseIsValid)
      continue;
    transmatrix T = vr_to_hr(p.mDeviceToAbsoluteTracking) * sm;
    
    // println(hlog, "found ", device_class_name(vrdata.vr->GetTrackedDeviceClass(i)), " at ", T);
    
    vrdata.pose_matrix[i] = T;

    if(i == vr::k_unTrackedDeviceIndex_Hmd) {
      hmd_at = inverse(T);
      if(first) hmd_ref_at = hmd_at, first = false;
      }
    
    auto& cd = vrdata.cdata[i];
    cd.x = cd.y = 0;

    if(vrdata.vr->GetTrackedDeviceClass(i) == vr::TrackedDeviceClass_Controller) {
      string mname = GetTrackedDeviceString(i, vr::Prop_RenderModelName_String );
      vrdata.device_models[i] = get_render_model(mname);
      
      /*
      cd.last = cd.cur;
      bool ok = vrdata.vr->GetControllerState(i, &cd.cur, sizeof(state));
      if(ok) {
        println(hlog, "pressed = ", color_t(cd.cur.ulButtonPressed), " touched = ", color_t(cd.cur.ulButtonTouched), " on ", i);
        for(int i=0; i<5; i++)
          if(cd.cur.rAxis[i].x || cd.cur.rAxis[i].y)
            println(hlog, "axis ", i, " = ", tie(cd.cur.rAxis[i].x, cd.cur.rAxis[i].y));
        }
      */
      
      if(which_pointer == i) {
        if(in_menu()) {
          hyperpoint h1 = sm * hmd_at_ui * vrdata.pose_matrix[i] * sm * C0;
          hyperpoint h2 = sm * hmd_at_ui * vrdata.pose_matrix[i] * sm * point31(0, 0, -0.01);
          ld p = ilerp(h1[2], h2[2], -ui_depth);
          hyperpoint pxo = lerp(h1, h2, p);
          hyperpoint px = pxo;
          px[0] /= ui_size * ui_size_unit;
          px[1] /= -ui_size * ui_size_unit;
          px[0] += (ui_xmin+ui_xmax) / 2;
          px[1] += (ui_ymin+ui_ymax) / 2;
          targeting_menu = px[0] >= 0 && px[1] >= 0 && px[0] <= vid.xres && px[1] <= vid.yres;
          if(targeting_menu) {
            mousex = px[0];
            mousey = px[1];
            pointer_distance = hdist(pxo, h1);
            }
          }
        else targeting_menu = false;
        }
      
      if(hdist(vrdata.pose_matrix[i] * C0, vrdata.last_pose_matrix[i] * C0) > .05) {
        vrdata.last_pose_matrix[i] = vrdata.pose_matrix[i];
        mousing = true;
        which_pointer = i;
        println(hlog, "setting which_pointer to ", i);
        }
      }
    }    

  if(!in_menu()) hmd_at_ui = hmd_at;
  }

EX bool targeting_menu;

EX void send_click() {
  holdmouse = false;
  fix_mouseh();
  println(hlog, "sending a click, getcstat = ", getcstat, " in menu = ", in_menu());
  if(in_menu() && targeting_menu)
    handlekey(getcstat, getcstat);
  else
    handlekey('-', '-');
  }

EX void send_release() {
  holdmouse = false;
  fix_mouseh();
  println(hlog, "sending a release");
  handlekey(PSEUDOKEY_RELEASE, PSEUDOKEY_RELEASE);
  }
  
EX void vr_control() {
  if(!enabled || !vid.usingGL) {
    if(state) shutdown_vr();
    return;
    }
  if(enabled && vid.usingGL && !state && !failed) {
    start_vr();
    }
  if(state == 1) {
    track_actions();
    need_poses = true;
    }
  static bool last_vr_clicked = false;
  
  shiftmul = getcshift;
  
  if(which_pointer) mousemoved = true;

  if(vr_clicked && last_vr_clicked && holdmouse) send_click();
  
  mousepressed = vr_clicked;

  if(vr_clicked && !last_vr_clicked && vid.quickmouse) send_click();

  if(vr_clicked && !last_vr_clicked && !vid.quickmouse)
    actonrelease = true;

  if(!vr_clicked && last_vr_clicked && !vid.quickmouse && actonrelease) {
    send_click();
    actonrelease = false;
    }

  else if(!vr_clicked && last_vr_clicked) {
    send_release();
    }

  if(mousepressed && inslider) {
    send_click();
    }
  
  last_vr_clicked = vr_clicked;
  }

EX void be_33(transmatrix& T) {
  for(int i=0; i<3; i++) T[i][3] = T[3][i] = 0;
  T[3][3] = 1;
  }

EX void apply_movement(const transmatrix& rel) {
  hyperpoint h0 = IN_E4(inverse(rel) * C0);
  hyperpoint h = h0;
  for(int i=0; i<3; i++) h[i] /= -absolute_unit_in_meters;
  
  shift_view(h);
  transmatrix Rot = rel;
  be_33(Rot);
  rotate_view(Rot);
  }

EX void vr_shift() {
  if(first) return;
  rug::using_rugview urv;
  if(GDIM == 2) return;
       
  if(GDIM == 3 && hsm == eHeadset::holonomy) {    
    apply_movement(IN_E4(hmd_at * inverse(hmd_ref_at)));
    hmd_ref_at = hmd_at;
    playermoved = false;
    if(!rug::rugged) optimizeview();
    }
  }

EX ld absolute_unit_in_meters = 3;

/** what point and cell is the controller number id pointing to */

EX eModel pmodel_3d_version() {
  if(pmodel == mdGeodesic) return mdEquidistant;
  if(pmodel == mdPerspective) return nonisotropic ? mdHorocyclic : mdEquidistant;
  return pmodel;
  }

/** convert model coordinates to controller-relative coordinates */
EX transmatrix model_to_controller(int id) {
  return inverse(sm * hmd_at * vrdata.pose_matrix[id] * sm) * hmd_mv;
  }

EX hyperpoint model_location(shiftpoint h, bool& bad) {
  if(eyes == eEyes::truesim) {
  
    hyperpoint eye_at[2], tangent[2];
    bad = false;
    for(int i=0; i<2; i++) {
      shiftpoint h1 = h;
      h1.h = hmd_pre_for[i] * h1.h;
      eModel md = pmodel_3d_version();
      hyperpoint hscr;
      apply_other_model(h1, hscr, md);
      E4;      
      hscr[3] = 1;
      eye_at[i] = vrdata.eyepos[i] * C0;
      tangent[i] = vrdata.eyepos[i] * sm * hmd_mv_for[i] * (hscr - C0);
      }

    // eye_at[0] + tangent[0] * a == eye_at[1] + tangent[1] * b
    // (in coordinates 0,2; in nonisotropic geometries, [1] may be different)
    
    E4;
    
    auto t10 = tangent[1][0];
    auto t12 = tangent[1][2];
    auto t00 = tangent[0][0];
    auto t02 = tangent[0][2];
    
    ld a = (t10 * eye_at[0][2] - t12 * eye_at[0][0] - t10 * eye_at[1][2] + t12 * eye_at[1][0]) / (t00 * t12 - t02 * t10);
    ld b = (t00 * eye_at[1][2] - t02 * eye_at[1][0] - t00 * eye_at[0][2] + t02 * eye_at[0][0]) / (t10 * t02 - t12 * t00);
    
    hyperpoint hit0 = eye_at[0] + tangent[0] * a;
    hyperpoint hit1 = eye_at[1] + tangent[1] * b;
    // we should have hit0 == hit1, except coordinate [1]

    return (hit0 + hit1) / 2;
    }
  else {
    hyperpoint hscr;
    h.h = hmd_pre_for[2] * h.h;
    eModel md = pmodel_3d_version();
    apply_other_model(h, hscr, md);
    bad = in_vr_sphere && get_side(hscr) == (sphere_flipped ? -1 : 1);
  
    hscr[3] = 1;
    return hscr;
    }
  }

ld vr_distance(const shiftpoint& h, int id, ld& dist) {

  bool bad;
  hyperpoint hscr = model_location(h, bad);
  if(bad) return 1e5;
  bool flat = WDIM == 2;
  E4; hyperpoint hc = model_to_controller(id) * hscr;
  if(flat) {
    if(hc[2] > 0.1) return 1e6; /* behind */
    dist = -hc[2];
    return sqhypot_d(2, hc);
    }
  else {
    hc[2] += dist;
    return sqhypot_d(3, hc);
    }
  }

EX hyperpoint vr_direction;

EX void compute_vr_direction(int id) {
  E4;
  transmatrix T = (hsm == eHeadset::none ? hmd_at : hmd_ref_at) * vrdata.pose_matrix[id] * sm;
  vrhr::be_33(T);
  vr_direction = T * point31(0, 0, -0.01);
  }

EX void compute_point(int id, shiftpoint& res, cell*& c, ld& dist) {

  if(WDIM == 3) {
    compute_vr_direction(id);
    movedir md = vectodir(vr_direction);
    cellwalker xc = cwt + md.d + wstep;
    forward_cell = xc.at;
    }
  
  dist = pointer_length;

  gen_mv();
  set_vr_sphere();
  c = nullptr;
  ld best = 1e9;
  
  shiftmatrix T;
  
  // needed so that applymodel gives the VR coordinates
  dynamicval<int> dvs (vrhr::state, 2); 
  
  for(auto p: current_display->all_drawn_copies) {
    for(auto& V: p.second) {
      shiftpoint h = V * pointable();
      ld d = vr_distance(h, id, dist);
      if(d < best) best = d, c = p.first, T = V;
      }
    }
  
  auto rel = pointable();
  
  T = minimize_point_value(T, [&] (const shiftmatrix& T1) { return vr_distance(T1*rel, id, dist); });
  
  res = T * rel;
  }  

EX bool vr_clicked;

void move_according_to(vr::ETrackedControllerRole role, bool last, bool cur) {
  if(cur) vr_clicked = true;
  int id = vr::VRSystem()->GetTrackedDeviceIndexForControllerRole(role);
  if((last || cur) && id >= 0 && id < int(vr::k_unMaxTrackedDeviceCount)) {
    println(hlog, "click setting which_pointer to ", id);
    which_pointer = id;
    }
  }

struct digital_action_data {
  string action_name;
  vr::VRActionHandle_t handle;
  bool last, curr;
  function<void(bool, bool)> act;
  bool_reaction_t when;
  digital_action_data(string s, bool_reaction_t when, function<void(bool, bool)> f) : when(when) { action_name = s; act = f; handle = vr::k_ulInvalidActionHandle; }
  };

struct analog_action_data {
  string action_name;
  vr::VRActionHandle_t handle;
  ld x, y;
  function<void(ld, ld)> act;
  analog_action_data(string s, function<void(ld, ld)> f) { action_name = s; act = f; handle = vr::k_ulInvalidActionHandle; }
  };

struct set_data {
  string set_name;
  int prio;
  vr::VRActionHandle_t handle;
  bool_reaction_t when;
  set_data(string s, int p, bool_reaction_t w) { set_name = s; prio = p; when = w; handle = vr::k_ulInvalidActionHandle; }
  };

vector<digital_action_data> dads = {
  digital_action_data("/actions/general/in/ClickLeft", [] { return true; }, [] (bool last, bool curr) {
    move_according_to(vr::TrackedControllerRole_LeftHand, last, curr);
    }),
  digital_action_data("/actions/general/in/ClickRight", [] { return true; }, [] (bool last, bool curr) {
    move_according_to(vr::TrackedControllerRole_RightHand, last, curr);
    }),
  digital_action_data("/actions/game/in/Drop", [] { return (cmode && sm::NORMAL); }, [] (bool last, bool curr) {
    if(curr && !last) dialog::queue_key('g');
    }),
  digital_action_data("/actions/game/in/Skip turn", [] { return (cmode && sm::NORMAL); }, [] (bool last, bool curr) {
    if(curr && !last) dialog::queue_key('s');
    }),
  digital_action_data("/actions/general/in/Menu", [] { return true; }, [] (bool last, bool curr) {
    if(curr && !last) {
      always_show_hud = !always_show_hud;
      hmd_at_ui = hmd_at;
      }
    }),
  digital_action_data("/actions/general/in/SetReference", [] { return true; }, [] (bool last, bool curr) {
    if(curr && !last) hmd_ref_at = hmd_at;
    })
  };

vector<analog_action_data> aads = {
  analog_action_data("/actions/general/in/MoveCamera", [] (ld x, ld y) {
    vrgo_x = x;
    vrgo_y = y;
    }),
  analog_action_data("/actions/general/in/RotateCamera", [] (ld x, ld y) {
    vraim_x = x;
    vraim_y = y;
    }),
  };

EX bool always_show_hud = false;
EX bool in_actual_menu() { return (cmode & sm::VR_MENU) || !(cmode & (sm::NORMAL | sm::DRAW)); }
EX bool in_menu() { return always_show_hud || in_actual_menu(); }

vector<set_data> sads = {
  set_data("/actions/game", 20, [] { return cmode & sm::NORMAL; }),
  set_data("/actions/general", 10, [] { return true; })
  };

void init_input() {
  const auto& vi = vr::VRInput();

  string cwd;
  
  char cwdbuf[PATH_MAX];
  if (getcwd(cwdbuf, sizeof(cwdbuf)) != NULL) {
    cwd = cwdbuf;
    println(hlog, "Found cwd: ", cwd);
    if(cwd.back() == '/' || cwd.back() == '\\') ;
    else cwd += (ISWINDOWS ? '\\' : '/');
    cwd += "hypervr_actions.json";
    }

  vi->SetActionManifestPath(cwd.c_str());
  
  for(auto& sad: sads)
    vi->GetActionSetHandle(sad.set_name.c_str(), &sad.handle);

  for(auto& dad: dads)
    vi->GetActionHandle(dad.action_name.c_str(), &dad.handle);

  for(auto& aad: aads)
    vi->GetActionHandle(aad.action_name.c_str(), &aad.handle);
  }

EX void track_actions() {

  for(auto& cd: vrdata.cdata)
    cd.clicked = false;
  
  vr_clicked = false;
  
  forward_cell = nullptr;

  vector<vr::VRActiveActionSet_t> sets;
  
  for(auto& sad: sads) if(sad.when()) {
    sets.emplace_back();
    auto& s = sets.back();
    s.ulActionSet = sad.handle;
    s.ulRestrictedToDevice = vr::k_ulInvalidInputValueHandle;
    s.ulSecondaryActionSet = vr::k_ulInvalidInputValueHandle;
    s.nPriority = sad.prio;
    }

  if(isize(sets))
    vr::VRInput()->UpdateActionState( &sets[0], sizeof(vr::VRActiveActionSet_t), isize(sets));
  
  for(auto& dad: dads) {
    if(!dad.when()) continue;
    vr::InputDigitalActionData_t actionData;    
    vr::VRInput()->GetDigitalActionData(dad.handle, &actionData, sizeof(actionData), vr::k_ulInvalidInputValueHandle );
    dad.last = dad.curr;
    dad.curr = actionData.bState;
    dad.act(dad.last, dad.curr);
    }
  
  for(auto& aad: aads) {
    vr::InputAnalogActionData_t actionData;
    vr::VRInput()->GetAnalogActionData(aad.handle, &actionData, sizeof(actionData), vr::k_ulInvalidInputValueHandle );
    aad.x = actionData.x;
    aad.y = actionData.y;
    aad.act(aad.x, aad.y);
    }  
  }

EX void get_eyes() {
  for(int a=0; a<2; a++) {
    auto eye = vr::EVREye(a);
    E4;
    vrdata.proj[a] =
      vr_to_hr(vrdata.vr->GetProjectionMatrix(eye, 0.01, 300));

    vrdata.iproj[a] = MirrorZ * inverse(vrdata.proj[a]);

    // println(hlog, "projection = ", vrdata.proj[a]);

    vrdata.eyepos[a] =
      vr_to_hr(vrdata.vr->GetEyeToHeadTransform(eye));

    // println(hlog, "eye-to-head = ", vrdata.eyepos[a]);
    }
  }

EX void start_vr() {

  if(true) { sm = Id; sm[1][1] = sm[2][2] = -1; }

  vr::EVRInitError eError = vr::VRInitError_None;
  vrdata.vr = vr::VR_Init( &eError, vr::VRApplication_Scene );
  
  for(auto& m: vrdata.last_pose_matrix) m = Id;

  if(eError != vr::VRInitError_None) {
    error_msg = vr::VR_GetVRInitErrorAsEnglishDescription( eError );
    println(hlog, "Unable to init VR: ", error_msg);
    failed = true;
    return;
    }  
  else println(hlog, "VR initialized successfully");
  
  apply_screen_settings();

  string driver = GetTrackedDeviceString( vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_TrackingSystemName_String );
  string display = GetTrackedDeviceString( vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_SerialNumber_String );

  println(hlog, "HyperRogue VR: driver=", driver, " display=", display);
  
  if(!vr::VRCompositor()) {
    println(hlog,  "Compositor initialization failed. See log file for details\n" );
    exit(1);
    }
  
  init_input();
  
  vrdata.vr->GetRecommendedRenderTargetSize( &vrdata.xsize, &vrdata.ysize);
  
  println(hlog, "recommended size: ", int(vrdata.xsize), " x ", int(vrdata.ysize));
  
  for(int a=0; a<2; a++) {
    vrdata.eyes[a] = new vr_framebuffer(vrdata.xsize, vrdata.ysize);
    println(hlog, "eye ", a, " : ", vrdata.eyes[a]->ok ? "OK" : "Error");
    }
    
  get_eyes();

  //CreateFrameBuffer( m_nRenderWidth, m_nRenderHeight, leftEyeDesc );
  //CreateFrameBuffer( m_nRenderWidth, m_nRenderHeight, rightEyeDesc );
  
  state = 1;
  }

EX void shutdown_vr() {
  vr::VR_Shutdown();
  vrdata.vr = nullptr;
  for(auto& e: vrdata.eyes) {
    delete e;
    e = nullptr;
    }
  for(auto& m: vrdata.models) {
    if(m) delete m;
    }
  vrdata.models.clear();
  for(auto& m: vrdata.device_models) m = nullptr;
  state = 0;
  }

EX void clear() {
  if(!state) return;
  resetbuffer rb;
  for(int i=0; i<2; i++) {
    auto& ey = vrdata.eyes[i];
    glBindFramebuffer( GL_FRAMEBUFFER, ey->m_nRenderFramebufferId );
    glViewport(0, 0, vrdata.xsize, vrdata.ysize );
    glhr::set_depthtest(false);
    glhr::set_depthtest(true);
    glhr::set_depthwrite(false);
    glhr::set_depthwrite(true);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
  rb.reset();
  current_display->set_viewport(0);
  }

EX ld ui_depth = 1.5;
EX ld ui_size = 2;

#if HDR
const ld ui_size_unit = 0.001;
#endif

struct vr_eye_settings {
  dynamicval<int> vx, vy;
  dynamicval<ld> xmin, ymin, xmax, ymax;
  
  vr_eye_settings() :
    vx(vid.xres, vrdata.xsize),
    vy(vid.yres, vrdata.ysize),
    xmin(current_display->xmin, 0),
    ymin(current_display->ymin, 0),
    xmax(current_display->xmax, 1),
    ymax(current_display->ymax, 1)
    { }    
  
  void use(int i) {
    glBindFramebuffer( GL_FRAMEBUFFER, vrdata.eyes[i]->m_nRenderFramebufferId );
    glViewport(0, 0, vrdata.xsize, vrdata.ysize );
    calcparam();
    }
  
  };

EX void in_vr_ui(reaction_t what) {
  
  resetbuffer rb;
  if(!state) return;

  int ui_xmed = (ui_xmin + ui_xmax) / 2;
  int ui_ymed = (ui_ymin + ui_ymax) / 2;
  state = 2;

  for(int i=0; i<2; i++) {
    E4;
    vr_eye_settings ey;
    ey.use(i);
    glhr::set_depthtest(false);
    hmd_mvp = Id;
    hmd_mvp = xpush(-ui_xmed) * ypush(-ui_ymed) * hmd_mvp;
    transmatrix Sca = Id;
    Sca[0][0] *= ui_size * ui_size_unit;
    Sca[1][1] *= -ui_size * ui_size_unit;
    Sca[2][2] *= 0;
    hmd_mvp = Sca * hmd_mvp;
    hmd_mvp = zpush(-ui_depth) * hmd_mvp;
    hmd_mvp = sm * hmd_at * inverse(hmd_at_ui) * sm * hmd_mvp;
    hmd_mvp = vrdata.proj[i] * inverse(vrdata.eyepos[i]) * hmd_mvp;
    reset_projection();
    current_display->set_all(0, 0);
    current_display->xcenter = 0;
    current_display->ycenter = 0;
    what();
    }
  state = 1;

  rb.reset();
  calcparam();
  current_display->set_viewport(0);
  calcparam();
  reset_projection();
  current_display->set_all(0, 0);
  glhr::set_modelview(glhr::translate(-current_display->xcenter,-current_display->ycenter, 0));
  what();
  }

EX void draw_eyes() {
  state = 1;
  for(int i=0; i<2; i++) {
    resetbuffer rb;
    auto& ey = vrdata.eyes[i];
    glBindFramebuffer(GL_READ_FRAMEBUFFER, ey->m_nRenderFramebufferId);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, ey->m_nResolveFramebufferId );
    glBlitFramebuffer( 0, 0, vrdata.xsize, vrdata.ysize, 0, 0, vrdata.xsize, vrdata.ysize, GL_COLOR_BUFFER_BIT, GL_LINEAR);
    rb.reset();

    current_display->next_shader_flags = GF_TEXTURE;
    dynamicval<eModel> m(pmodel, mdPixel);
    current_display->set_all(0, 0);
    glBindTexture(GL_TEXTURE_2D, ey->m_nResolveTextureId );
    glhr::id_modelview();
    glhr::set_depthtest(false);
    glhr::color2(0xFFFFFFFF);
    vector<glhr::textured_vertex> tvx;
    for(int a=0; a<6; a++) {
      int dx[6] = {0, 1, 1, 0, 0, 1};
      int dy[6] = {0, 0, 1, 0, 1, 1};
      glhr::textured_vertex tx;
      tx.coords[2] = 0;
      tx.coords[3] = 1;
      tx.coords[0] = (dx[a]+i) * current_display->xsize / 2 - current_display->xcenter;
      tx.coords[1] = (1-dy[a]) * current_display->ysize - current_display->ycenter;
      tx.texture[0] = dx[a];
      tx.texture[1] = dy[a];
      tvx.push_back(tx);
      }
    glhr::prepare(tvx);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    }
  }

EX void gen_mv() {
  transmatrix mu;
  bool pers = in_perspective();
  ld sca = pers ? absolute_unit_in_meters : pconf.vr_scale_factor;
  for(int i=0; i<4; i++)
  for(int j=0; j<4; j++)
    mu[i][j] = i!=j ? 0 : i==3 ? 1 : sca;
  if(!pers) mu[1][1] *= pconf.stretch;

  hmd_mv = Id;
  bool nlpu = nisot::local_perspective_used();
  if(1) {
    E4;
    if(nlpu) {
      be_33(NLP);
      hmd_mv = NLP * hmd_mv;          
      }
    hmd_mv = sm * hmd_mv;

    if(pconf.vr_angle) hmd_mv = cspin(1, 2, -pconf.vr_angle * degree) * hmd_mv;
    if(pconf.vr_zshift) hmd_mv = euclidean_translate(0, 0, -pconf.vr_zshift) * hmd_mv;
    hmd_mv = mu * hmd_mv;
    if(GDIM == 2 || hsm == eHeadset::model_viewing) {
      hmd_mv = sm * hmd_at * inverse(hmd_ref_at) * sm * hmd_mv;
      }
    }
  }

EX shiftmatrix master_cview;

EX void render() {
  track_poses();
  get_eyes();
  resetbuffer rb;
  state = 2;
  vrhr::frusta.clear();
  
  // cscr = lshiftclick ? eCompScreen::eyes : eCompScreen::single;

  for(int i=0; i<3; i++) {

    if(1) {
      make_actual_view();
      master_cview = cview();

      /* unfortunately we need to backup everything that could change by shift_view... */
      dynamicval<transmatrix> tN(NLP, NLP);
      dynamicval<transmatrix> tV(View, View);
      dynamicval<transmatrix> tC(current_display->which_copy, current_display->which_copy);
      dynamicval<transmatrix> trt(current_display->radar_transform);
      
      /* changed in intra */
      dynamicval<ld> tcs(camera_speed);
      dynamicval<ld> tcl(anims::cycle_length);
      dynamicval<ld> tau(vrhr::absolute_unit_in_meters);
      dynamicval<ld> tel(walking::eye_level);
      dynamicval<int> tfd(walking::floor_dir);
      dynamicval<cell*> tof(walking::on_floor_of);

      int id = intra::current;
      cell *co = centerover;
      finalizer fin([&] {
        if(intra::current != id) {
          println(hlog, "rendering via portal");
          intra::switch_to(id);
          centerover = co;
          }
        });

      if(hsm == eHeadset::rotation_only) {
        transmatrix T = hmd_at;
        be_33(T);
        apply_movement(T);
        }
      
      else if(GDIM == 3 && hsm == eHeadset::reference) {
        apply_movement(IN_E4(hmd_at * inverse(hmd_ref_at)));
        }

      if(eyes == eEyes::truesim && i != 2) {
        apply_movement(IN_E4(inverse(vrdata.eyepos[i])));
        }

      make_actual_view();
      hmd_pre = hmd_pre_for[i] = cview().T * inverse(master_cview.T);
      current_display->radar_transform = trt.backup * inverse(hmd_pre);
      
      if(i < 2)
        frusta.push_back(frustum_info{hmd_pre, NLP, false, vrdata.proj[i]});
      else
        frusta.push_back(frustum_info{hmd_pre, NLP, true, Id});
      
      if(1) {
        gen_mv();
        E4;
        if(eyes == eEyes::equidistant && i != 2) {
          eyeshift = vrdata.eyepos[i];
          hmd_mv = inverse(eyeshift) * hmd_mv;
          }
        else eyeshift = Id;
        hmd_mv_for[i] = hmd_mv;
        if(i != 2) {
          hmd_mvp = vrdata.proj[i] * hmd_mv;
          eyeproj = vrdata.iproj[i];
          }
        }

      if(i != 2) {
  
        vr_eye_settings ey;
        ey.use(i);
        glhr::set_depthtest(false);
        glhr::set_depthtest(true);
        glhr::set_depthwrite(false);
        glhr::set_depthwrite(true);
      // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
        calcparam();
        drawqueue();
        }
      
      if(i == 2) {
        rb.reset();
        calcparam();
        current_display->set_viewport(0);
        calcparam();
        current_display->next_shader_flags = 0;
        current_display->set_all(0, 0);

        if(cscr == eCompScreen::single) {
          state = 4;
          drawqueue();
          }
      
        }
            
      }
    
    }

  if(cscr == eCompScreen::reference) {
    state = 3;
    drawqueue();
    }

  if(cscr == eCompScreen::eyes && !controllers_on_desktop) draw_eyes();
  
  render_controllers();

  if(cscr == eCompScreen::eyes && controllers_on_desktop) draw_eyes();

  state = 1;  
  }

EX void show_vr_demos() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("VR demos"));
  dialog::addInfo(XLAT("warning: these will restart your game!"));
  
  dialog::addBreak(100);

  dialog::addItem(XLAT("standard HyperRogue but in VR"), 'a');
  dialog::add_action([] {
    if(rug::rugged) rug::close();
    hmd_ref_at = hmd_at;
    stop_game();
    set_geometry(gNormal);
    if(GDIM == 3) geom3::switch_fpp();
    specialland = laIce;
    set_variation(eVariation::bitruncated);
    pmodel = mdDisk;
    pconf.alpha = 1;
    pconf.vr_scale_factor = 1;
    pconf.vr_angle = 0;
    pconf.vr_zshift = 0;
    hsm = eHeadset::model_viewing;
    eyes = eEyes::equidistant;
    start_game();
    popScreenAll();
    });

  dialog::addItem(XLAT("HyperRogue FPP"), 'b');
  dialog::add_action([] {
    if(rug::rugged) rug::close();
    hmd_ref_at = hmd_at;
    stop_game();
    pmodel = mdDisk;
    set_geometry(gNormal);
    if(GDIM == 2) geom3::switch_fpp();
    specialland = laIce;
    set_variation(eVariation::bitruncated);
    pconf.alpha = 1;
    pconf.vr_scale_factor = 1;
    pconf.vr_angle = 0;
    pconf.vr_zshift = 0;
    hsm = eHeadset::reference;
    eyes = eEyes::equidistant;
    start_game();
    popScreenAll();
    });

  #if CAP_RUG
  dialog::addItem(XLAT("Hypersian Rug"), 'c');
  dialog::add_action([] {
    if(rug::rugged) rug::close();
    hmd_ref_at = hmd_at;
    stop_game();
    set_geometry(gNormal);
    if(GDIM == 3) geom3::switch_fpp();
    specialland = laIce;
    set_variation(eVariation::bitruncated);
    pmodel = mdDisk;
    pconf.alpha = 1;
    pconf.vr_scale_factor = 1;
    pconf.vr_angle = 0;
    pconf.vr_zshift = 0;
    hsm = eHeadset::model_viewing;
    rug::modelscale = 0.5;
    eyes = eEyes::equidistant;
    start_game();
    rug::init();
    popScreenAll();
    });
  #endif

  dialog::addItem(XLAT("sphere from the inside"), 'd');
  dialog::add_action([] {
    if(rug::rugged) rug::close();
    hmd_ref_at = hmd_at;
    stop_game();
    set_geometry(gSphere);
    if(GDIM == 3) geom3::switch_fpp();
    specialland = laHalloween;
    set_variation(eVariation::bitruncated);
    pmodel = mdDisk;
    pconf.alpha = 0;
    pconf.vr_scale_factor = 2;
    pconf.vr_angle = 0;
    pconf.vr_zshift = 0;
    hsm = eHeadset::model_viewing;
    eyes = eEyes::equidistant;
    start_game();
    popScreenAll();
    });

  dialog::addItem(XLAT("sphere from the outside"), 'e');
  dialog::add_action([] {
    if(rug::rugged) rug::close();
    hmd_ref_at = hmd_at;
    stop_game();
    set_geometry(gSphere);
    if(GDIM == 3) geom3::switch_fpp();
    specialland = laHalloween;
    set_variation(eVariation::bitruncated);
    pmodel = mdDisk;
    pconf.alpha = 2;
    pconf.vr_scale_factor = 0.5;
    pconf.vr_angle = 0;
    pconf.vr_zshift = 0;
    hsm = eHeadset::model_viewing;
    eyes = eEyes::equidistant;
    start_game();
    popScreenAll();
    });

  dialog::addItem(XLAT("Thurston racing"), 'f');
  dialog::add_action([] {
    if(rug::rugged) rug::close();
    hmd_ref_at = hmd_at;
    hsm = eHeadset::reference;
    eyes = eEyes::equidistant;
    popScreenAll();
    pushScreen(racing::thurston_racing);
    });

  dialog::addItem(XLAT("raytracing in H3"), 'g');
  dialog::add_action([] {
    if(rug::rugged) rug::close();
    hmd_ref_at = hmd_at;
    hsm = eHeadset::holonomy;
    eyes = eEyes::truesim;
    stop_game();
    specialland = laEmerald;
    set_geometry(gSpace534);
    check_cgi();
    cgi.require_basics();
    cgi.require_shapes();
    fieldpattern::field_from_current();
    set_geometry(gFieldQuotient);
    int p = 2;
    for(;; p++) { currfp.Prime = p; currfp.force_hash = 0x72414D0C; if(!currfp.solve()) break; }
    start_game();
    popScreenAll();
    });

  dialog::addBreak(100);
  dialog::addBack();
  
  dialog::display();
  }

EX void enable_button() {
  dialog::addBoolItem(XLAT("VR enabled"), enabled, 'o');
  dialog::add_action([] {
    enabled = !enabled;
    });
  if(!enabled)
    dialog::addBreak(100);
  else if(failed)
    dialog::addInfo(XLAT("error: ") + error_msg, 0xC00000);
  else
    dialog::addInfo(XLAT("VR initialized correctly"), 0x00C000);
  }

EX void reference_button() {
  if(enabled && among(hsm, eHeadset::reference, eHeadset::model_viewing)) {
    E4;
    hyperpoint h = hmd_at * inverse(hmd_ref_at) * C0;
      
    dialog::addSelItem(XLAT("reset the reference point"), state ? fts(hypot_d(3, h)) + "m" : "", 'r');
    dialog::add_action([] { hmd_ref_at = hmd_at; });
    }
  else dialog::addBreak(100);
  }

EX void show_vr_settings() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("VR settings"));

  dialog::addItem(XLAT("VR demos"), 'D');
  dialog::add_action_push(show_vr_demos);

  enable_button();
  
  dialog::addBreak(100);

  add_edit(hsm);   
  if(enabled && GDIM == 2 && among(hsm, eHeadset::holonomy, eHeadset::reference))
    dialog::addInfo(XLAT("(this setting is for 3D geometries only, use 'model viewing' instead)"));
  add_edit(eyes);
  add_edit(cscr);
  
  dialog::addSelItem(XLAT("absolute unit in meters"), fts(absolute_unit_in_meters), 'a');
  dialog::add_action([] {
    dialog::editNumber(absolute_unit_in_meters, .01, 100, 0.1, 1, XLAT("absolute unit in meters"), 
      XLAT(
        "The size of the absolute unit of the non-Euclidean geometry in meters. "
        "This affects the headset movement and binocular vision.\n\n"
        "In spherical geometry, the absolute unit is the radius of the sphere. "
        "The smaller the absolute unit, the stronger the non-Euclidean effects.\n\n"
        "Elements of the HyperRogue world have fixed size in terms of absolute units, "
        "so reducing the absolute unit makes them smaller. "
        "If you are playing in the Euclidean mode, this feature just scales everything "
        "(e.g., in the cube tiling, the 'absolute unit' is just the edge of the cube). "
        "Only perspective projections are affected, other models use the 'VR scale' setting "
        "from the Projections menu."
        ));
      dialog::scaleLog();
      });

  dialog::addSelItem(XLAT("projection"), current_proj_name(), 'M');
  dialog::add_action_push(models::model_menu);
    
  reference_button();

  dialog::addSelItem(XLAT("pointer length"), fts(pointer_length) + "m", 'p');
  dialog::add_action([] {
    dialog::editNumber(pointer_length, 0, 2, 0.1, 1, XLAT("pointer length"), 
      XLAT(
        "If the pointer length is 0.5m, the object pointed to is 0.5 meter from the controller. "
        "This is used in situations where the controller is used as a 3D mouse, e.g., "
        "the drawing tool in three-dimensional geometries. When pointing at two-dimensional surfaces, "
        "this is not relevant (the pointer is as long as needed to hit the surface)."
        ));
      });

  dialog::addSelItem(XLAT("UI size"), fts(ui_size) + "mm", 'u');
  dialog::add_action([] {
    dialog::editNumber(ui_size, 0, 10, 0.1, 2, XLAT("UI size"), 
      XLAT(
        "How big is a pixel of the user interface (HUD and menus). The user interface is as big as the window on the desktop."
        ));
      });
    
  dialog::addSelItem(XLAT("UI depth"), fts(ui_depth) + "m", 'U');
  dialog::add_action([] {
    dialog::editNumber(ui_depth, 0, 2, 0.1, 1, XLAT("UI depth"), 
      XLAT(
        "How far to show the user interface (HUD and menus)."
        ));
      });    
  
  dialog::addBack();
  dialog::display();
  }

#if CAP_COMMANDLINE
int readArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-vr-enabled")) {
    PHASEFROM(2);
    shift(); enabled = argi();
    }
  else if(argis("-vr-absunit")) {
    PHASEFROM(2);
    shift_arg_formula(absolute_unit_in_meters);
    }
  else if(argis("-vr-scale")) {
    PHASEFROM(2);
    shift_arg_formula(pconf.vr_scale_factor);
    }
  else if(argis("-vr-z")) {
    PHASEFROM(2);
    shift_arg_formula(pconf.vr_zshift);
    }
  else if(argis("-vr-pl")) {
    PHASEFROM(2);
    shift_arg_formula(pointer_length);
    }
  else if(argis("-vr-angle")) {
    PHASEFROM(2);
    shift_arg_formula(pconf.vr_angle);
    }
  else if(argis("-d:vr")) {
    PHASEFROM(2); launch_dialog(show_vr_settings);
    }
  else if(argis("-d:vrd")) {
    PHASEFROM(2); launch_dialog(show_vr_demos);
    }
  else if(argis("-vr-mode")) {
    PHASEFROM(2); 
    shift(); hsm = (eHeadset) argi();
    shift(); eyes = (eEyes) argi();
    shift(); cscr = (eCompScreen) argi();
    }
  else return 1;
  return 0;
  }
auto hooka = addHook(hooks_args, 100, readArgs);
#endif

#if CAP_CONFIG
void addconfig() {
  addsaver(enabled, "vr-enabled");

  param_f(absolute_unit_in_meters, "vr-abs-unit");

  param_f(pconf.vr_scale_factor, "vr_scale");
  param_f(pconf.vr_zshift, "vr_zshift");
  param_f(pconf.vr_angle, "vr_angle");

  auto& rrconf = vid.rug_config;
  param_f(rrconf.vr_scale_factor, "rug_vr_scale");
  param_f(rrconf.vr_zshift, "rug_vr_shift");
  param_f(rrconf.vr_angle, "rug_vr_angle");

  param_f(vrhr::pointer_length, "vr_pointer_length");
  param_f(vrhr::ui_depth, "vr_ui_depth");
  param_f(vrhr::ui_size, "vr_ui_size");
  
  param_enum(vrhr::hsm, "vr_headset_mode", "vr_headset_mode", vrhr::hsm)
    ->editable(headset_desc, "VR headset movement", 'h');
  param_enum(vrhr::eyes, "vr_eyes_mode", "vr_eyes_mode", vrhr::eyes)
    ->editable(eyes_desc, "VR binocular vision", 'b');
  param_enum(vrhr::cscr, "vr_screen_mode", "vr_screen_mode", vrhr::cscr)
    ->editable(comp_desc, "VR computer screen", 'c');
  }
auto hookc = addHook(hooks_configfile, 100, addconfig);
#endif

EX bool rec;

EX void render_controllers() {
  if(!state) return;
  dynamicval<bool> rc(rec, true);
  for(int i=0; i<(int)vr::k_unMaxTrackedDeviceCount; i++) 
    if(vrdata.device_models[i]) {
      resetbuffer rb;
    
      state = 2;
      dynamicval<eModel> m(pmodel, mdPerspective);
      dynamicval<ld> ms(sightranges[geometry], 100);
      
      for(int e=0; e<3; e++) {
      
        if(e < 2) {
          vr_eye_settings ey;
          ey.use(e);
          }
        
        else {
          state = 1;
          
          rb.reset();
          calcparam();
          current_display->set_viewport(0);
          calcparam();
          reset_projection();
          current_display->set_all(0, 0);

          if(cscr != eCompScreen::single || !controllers_on_desktop) goto next_model;
          state = 4;
          }

        E4;

        hmd_mvp = sm * hmd_at * vrdata.pose_matrix[i] * sm * Id;
        if(e < 2) 
          hmd_mvp = vrdata.proj[e] * inverse(vrdata.eyepos[e]) * hmd_mvp;
        else
          hmd_mv = hmd_mvp;
        hmd_pre = Id;
        
        reset_projection();        
        current_display->next_shader_flags = GF_TEXTURE;
        current_display->set_all(0, 0);
        glhr::set_depthtest(false);
        glhr::set_depthtest(true);
        glhr::set_depthwrite(false);
        glhr::set_depthwrite(true);
        glClear(GL_DEPTH_BUFFER_BIT);
        glhr::id_modelview();
        glhr::color2(0xFFFFFFFF);
        prepare(vrdata.device_models[i]->vertices);
        
        glBindTexture(GL_TEXTURE_2D, vrdata.device_models[i]->texture_id);
        glDrawArrays(GL_TRIANGLES, 0, isize(vrdata.device_models[i]->vertices));

        if(i == which_pointer) {
          current_display->next_shader_flags = 0;
          current_display->set_all(0, 0);
          vector<glvertex> vex;
          vex.push_back(glhr::makevertex(0.01, 0, 0));
          vex.push_back(glhr::makevertex(-0.01, 0, 0));
          vex.push_back(glhr::makevertex(0, 0, -pointer_distance));
          glhr::current_vertices = nullptr;
          glhr::vertices(vex);
          glhr::color2(0xC0FFC0C0);
          glDrawArrays(GL_TRIANGLES, 0, 3);
          }

        }
      
      next_model: ;
      }
  }

EX void submit() {

  if(!state) return;
      
  for(int i=0; i<2; i++) {
    auto eye = vr::EVREye(i);
    auto& ey = vrdata.eyes[i];
        
    resetbuffer rb;
    glBindFramebuffer(GL_READ_FRAMEBUFFER, ey->m_nRenderFramebufferId);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, ey->m_nResolveFramebufferId );
    glBlitFramebuffer( 0, 0, vrdata.xsize, vrdata.ysize, 0, 0, vrdata.xsize, vrdata.ysize, GL_COLOR_BUFFER_BIT, GL_LINEAR);
    rb.reset();

    vr::Texture_t texture = {(void*)(uintptr_t)ey->m_nResolveTextureId, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
    vr::VRCompositor()->Submit(eye, &texture );
    }
  }

EX void handoff() {
  if(!state) return;
  vr::VRCompositor()->PostPresentHandoff();  
  }

#endif

EX }
}
