/* a library for loading 3D models */

#include "rogueviz.h"

namespace rogueviz {
#if CAP_MODELS
namespace objmodels {

bool scan(fhstream& hs, char& c) { return fscanf(hs.f, "%c", &c) == 1; }

char peek(fhstream& fs) {
  char g = fgetc(fs.f);
  ungetc(g, fs.f);
  return g;
  }

bool model::available() {
  if(av_checked) return is_available;
  av_checked = true;
  is_available = file_exists(path + fname);
  return false;
  }

string ignore_mtlname = "XXX";

int debug_objects = 0;

map<string, texture::texture_data> materials_global;

texture::texture_data& by_name(string g) {
  if(materials_global.count(g)) return materials_global[g];
  if(debug_objects > 2) println(hlog, "loading texture from: ", g);
  auto& mat = materials_global[g];
  mat.twidth = mat.theight = 0; mat.stretched = true;
  mat.readtexture(g);
  mat.loadTextureGL();
  if(debug_objects > 3) println(hlog, "texture ID: ", mat.textureid);
  return mat;
  }

void model::load_obj(model_data& md) {
  md.prec_used = prec;

  auto& objects = md.objs;
  fhstream fs(path+fname, "rt");
  if(debug_objects > 0) println(hlog, "loading 3D model from: ", path+fname);
  indenter ind(2);

  if(!fs.f) 
    throw hr_exception("failed to open model file: " + path + fname);
  
  prepare();

  vector<hyperpoint> vertices;
  vector<hyperpoint> normals;
  vector<hyperpoint> tvertices;

  while(!feof(fs.f)) {
    string s;
    scan(fs, s);
    if(s == "#") { scanline(fs); }
    else if(s == "mtllib") {
      string mtllib;
      scan(fs, mtllib);
      fhstream fsm(path+mtllib, "rt");
      if(debug_objects > 1)
        println(hlog, "loading material library from: ", path+mtllib);
      indenter ind(2);
      if(!fsm.f) 
        throw hr_exception("failed to open mtllib: " + mtllib);
      color_t nextcol = 0xFFFFFFFF;
      string mtlname, texname = "";
      auto emit_material = [&] {
        if(texname != "" && !materials.count(mtlname))
          materials[mtlname] = by_name(path + texname);
        colors[mtlname] = nextcol;
        if(debug_objects > 2) println(hlog, "color of ", mtlname, " is ", nextcol);
        };
      while(!feof(fsm.f)) {
        string s;
        scan(fsm, s);
        if(s == "#") { scanline(fsm); }
        if(s == "Kd") {
          ld a, b, c;
          scan(fsm, a, b, c);
          nextcol = read_color(a, b, c);
          }
        if(s == "newmtl") {
          emit_material();
          nextcol = 0xFFFFFFFF;
          texname = "";
          mtlname = scanline_noblank(fsm);
          }
        if(s == "map_Kd") {
          texname = scanline_noblank(fsm);
          }
        }
      emit_material();
      }
    else if(s == "o" || s == "g") {
      next_object:
      object *co = nullptr;
      bool textured = false;
      string oname = scanline_noblank(fs);
      if(debug_objects > 2) println(hlog, "reading object: ", oname);
      md.objindex.push_back(isize(md.objs));
      hyperpoint ctr = Hypc;
      int cqty = 0;
      while(true) {
        if(feof(fs.f)) {
          if(co) cgi.finishshape();
          if(co && debug_objects > 4) println(hlog, "vertices = ", co->sh.e-co->sh.s, " tvertices = ", isize(co->tv.tvertices));
          break;
          }
        scan(fs, s);
        if(s == "#") {
          scanline(fs);
          }
        else if(s == "o" || s == "g") {
          if(co) cgi.finishshape();
          if(co && debug_objects > 4) println(hlog, "vertices = ", co->sh.e-co->sh.s, " tvertices = ", isize(co->tv.tvertices));
          goto next_object;
          }
        else if(s == "v") {        
          hyperpoint h = C03;
          scan(fs, h[0], h[1], h[2]); // assume all
          h[0] /= 100;
          h[1] /= 100;
          h[2] /= 100;
          vertices.push_back(h);
          ctr += h; cqty++;
          }
        else if(s == "vt") {
          ld u, v;
          scan(fs, u, v);
          tvertices.push_back(point3(u, 1-v, 0));
          }
        else if(s == "vn") {
          hyperpoint hn = C0;
          scan(fs, hn[0], hn[1], hn[2]);
          normals.push_back(hn);
          }
        else if(s == "s") {
          scan(fs, s);
          }
        else if(s == "usemtl") {
          if(co) cgi.finishshape();
          if(co && debug_objects > 4) println(hlog, "vertices = ", co->sh.e-co->sh.s, " tvertices = ", isize(co->tv.tvertices));
          string mtlname = scanline_noblank(fs);
          co = nullptr;
          if(mtlname.find("Layer_Layer0") != string::npos) continue;
          objects.push_back(make_shared<object>());
          co = &*objects.back();
          co->mtlname = mtlname;
          cgi.bshape(co->sh, PPR::WALL);
          cgi.last->flags |= POLY_TRIANGLES;
          cgi.last->texture_offset = 0;
          if(materials.count(mtlname)) {
            textured = true;
            cgi.last->tinf = &co->tv;
            co->tv.texture_id = materials[mtlname].textureid;
            if(debug_objects > 0) println(hlog, "using texture_id : ", co->tv.texture_id);
            co->color = 0xFFFFFFFF;
            }
          else {
            textured = false;
            cgi.last->tinf = &co->tv;
            co->tv.texture_id = floor_textures->renderedTexture;
            if(colors.count(mtlname))
              co->color = colors[mtlname];
            else
              co->color = 0xFFFFFFFF;
            }
          if(mtlname.find(ignore_mtlname) != string::npos) co->color = 0;
          if(debug_objects > 4) println(hlog, "set textured to ", textured, " color ", co->color, " mtlname = '", mtlname, "'");
          }
        else if(s == "f") {
          struct vertexinfo { int f, t, n; };
          array<vertexinfo, 3> vis;
          char bar;
          
          auto get_vi = [&] (vertexinfo& vi) {
            vi.f = vi.t = vi.n = 1;
            scan(fs, vi.f);
            if(peek(fs) == '/') {
              scan(fs, bar);
              if(peek(fs) != '/') scan(fs, vi.t);
              }
            if(peek(fs) == '/') {
              scan(fs, bar);
              scan(fs, vi.n);
              }

            vi.f--; vi.t--; vi.n--;
            if(vi.f < 0 || vi.f >= isize(vertices)) 
              throw hr_exception("illegal ID");
            };
          
          get_vi(vis[0]);
          get_vi(vis[1]);
          next_triangle:
          get_vi(vis[2]);

          vector<hyperpoint> hys;
          vector<hyperpoint> tot;

          for(int i=0; i<3; i++) {            
            hys.push_back(vertices[vis[i].f]);
            tot.push_back(textured ? tvertices[vis[i].t] : point3(0,0,0));
            }
          if(!co) continue;

          if(shift_to_ctr) {
            hyperpoint ctr1 = ctr / cqty;
            ctr1[3] = 0;
            if(debug_objects > 5) println(hlog, "ctr1 = ", ctr1, "hys = ", hys[0]);
            for(auto& h: hys)
              h -= ctr1;
            }

          process_triangle(hys, tot, textured, co);
          
          while(among(peek(fs), ' ', '\r', '\n')) scan(fs, bar);
          if(isdigit(peek(fs))) { vis[1] = vis[2]; goto next_triangle; }
          }
        else if(s == "l") {
          int a, b;
          scan(fs, a, b);
          /* ignore */
          }
        else if(s == "") { }
        else 
          throw hr_exception("unknown command: " + s);
        }      
      
      }
    else 
      throw("unknown command: " + s);
    }
  
  if(debug_objects > 0) println(hlog, "reading finished");

  md.objindex.push_back(isize(md.objs));
  cgi.extra_vertices();
  }

hyperpoint model::transform(hyperpoint h) { return direct_exp(h); }

int model::subdivision(vector<hyperpoint>& hys) {
  if(euclid) return 1;
  ld maxlen = prec * max(hypot_d(3, hys[1] - hys[0]), max(hypot_d(3, hys[2] - hys[0]), hypot_d(3, hys[2] - hys[1])));
  return int(ceil(maxlen));
  }

color_t model::read_color(ld a, ld b, ld c) {
  color_t nextcol = 0xFFFFFFFF;
  part(nextcol, 3) = a * 255.99;
  part(nextcol, 2) = b * 255.99;
  part(nextcol, 1) = c * 255.99;
  return nextcol;
  }

void model::process_triangle(vector<hyperpoint>& hys, vector<hyperpoint>& tot, bool textured, object *co) {

  hyperpoint norm = (hys[1] - hys[0]) ^ (hys[2] - hys[0]);
  norm /= hypot_d(3, norm);
  ld y = .5 + (.2 * norm[0] + .16 * norm[1] + .14 * norm[2]);
  glvertex shade = glhr::makevertex(0, y, 0);
  glvertex shadecol = glhr::makevertex(y, y, y);

  int parts = subdivision(hys);
  auto tri = [&] (int a, int b) {
    cgi.hpcpush(transform(hys[0] + (hys[1] - hys[0]) * a / parts + (hys[2] - hys[0]) * b / parts));
    // cgi.hpcpush(tf(tot[0] + (tot[1] - tot[0]) * a / parts + (tot[2] - tot[0]) * b / parts).second);
    if(textured) {
      co->tv.tvertices.push_back(glhr::pointtogl(tot[0] + (tot[1] - tot[0]) * a / parts + (tot[2] - tot[0]) * b / parts));
      co->tv.colors.push_back(shadecol);
      }
    else {
      co->tv.tvertices.push_back(shade);
      }
    };

  for(int a=0; a<parts; a++)
  for(int b=0; b<parts-a; b++) {
    tri(a, b);
    tri(a+1, b);
    tri(a, b+1);
    if(a+b < parts-1) {
      tri(a, b+1);
      tri(a+1, b);
      tri(a+1, b+1);
      }
    }
  }

model_data& model::get() {

  auto& md = (unique_ptr<model_data>&) cgi.ext[fname];
  
  if(!md) {
    md = std::make_unique<model_data>();
    load_obj(*md);
    }
  
  if(md && md->prec_used < prec) {
    if(debug_objects > 0) println(hlog, "need prec=", prec, " used = ", md->prec_used);
    md->objs.clear();
    load_obj(*md);
    }

  return *md;
  }

void model_data::render(const shiftmatrix& V) {
  for(auto& obj: objs) if(obj->color) {
    queuepoly(V, obj->sh, obj->color);
    }
  }

void model_settings() {
  emptyscreen();
  dialog::init();
  add_edit(prec);
  dialog::addBack();
  dialog::display();
  }

void o_key(o_funcs& v) {
  v.push_back(named_dialog("set model settings", model_settings));
  }

void add_model_settings() {
  rogueviz::rv_hook(hooks_o_key, 200, o_key);
  }

auto cf = addHook(hooks_configfile, 100, [] {
  param_f(prec, "obj_prec")
  ->editable(1, 100, 1, "3D model precision", "higher-precision models take more time to load and to render.", 'p')
  ->set_sets([] { cmode = sm::NOSCR; })
  ; 
  param_b(shift_to_ctr, "shift_to_ctr");
  }) + arg::add2("-debobj", [] { arg::shift(); debug_objects = arg::argi(); });

}
#endif
}
