// Hyperbolic Rogue -- models of hyperbolic geometry
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file models.cpp
 *  \brief models of hyperbolic geometry: their properties, projection menu
 *
 *  The actual models are implemented in hypgraph.cpp. Also shaders.cpp, 
 *  drawing.cpp, and basegraph.cpp are important.
 */

#include "hyper.h"
namespace hr {

EX namespace polygonal {

  #if ISMOBWEB
  typedef double precise;
  #else
  typedef long double precise;
  #endif

  #if HDR
  static const int MSI = 120;
  #endif

  typedef long double xld;

  typedef complex<xld> cxld;

  EX int SI = 4;
  EX ld  STAR = 0;
  
  EX int deg = ISMOBWEB ? 2 : 20;

  precise matrix[MSI][MSI];
  precise ans[MSI];
  
  cxld coef[MSI];
  EX ld coefr[MSI], coefi[MSI]; 
  EX int maxcoef, coefid;
  
  EX void solve() {
    if(pmodel == mdPolynomial) {
      for(int i=0; i<MSI; i++) coef[i] = cxld(coefr[i], coefi[i]);
      return;
      } 
    if(pmodel != mdPolygonal) return;
    if(SI < 3) SI = 3;
    for(int i=0; i<MSI; i++) ans[i] = cos(M_PI / SI);
    for(int i=0; i<MSI; i++)
      for(int j=0; j<MSI; j++) {
        precise i0 = (i+0.) / (MSI-1);
        // i0 *= i0;
        // i0 = 1 - i0;
        i0 *= M_PI;
        matrix[i][j] = 
          cos(i0 * (j + 1./SI)) * (STAR > 0 ? (1+STAR) : 1)
        - sin(i0 * (j + 1./SI)) * (STAR > 0 ? STAR : STAR/(1+STAR));
        }
    
    for(int i=0; i<MSI; i++) {
      precise dby = matrix[i][i];
      for(int k=0; k<MSI; k++) matrix[i][k] /= dby;
      ans[i] /= dby; 
      for(int j=i+1; j<MSI; j++) {
        precise sub = matrix[j][i];
        ans[j] -= ans[i] * sub;
        for(int k=0; k<MSI; k++)
           matrix[j][k] -= sub * matrix[i][k];
        }
      }
    for(int i=MSI-1; i>=0; i--) {
      for(int j=0; j<i; j++) {
        precise sub = matrix[j][i];
        ans[j] -= ans[i] * sub;
        for(int k=0; k<MSI; k++)
           matrix[j][k] -= sub * matrix[i][k];
        }
      }
    }
  
  EX pair<ld, ld> compute(ld x, ld y, int prec) {
    if(x*x+y*y > 1) {
      xld r  = hypot(x,y);
      x /= r;
      y /= r;
      }
    if(pmodel == mdPolynomial) {
      cxld z(x,y);
      cxld res (0,0);
      for(int i=maxcoef; i>=0; i--) { res += coef[i]; if(i) res *= z; }
      return make_pair(real(res), imag(res));    
      }
      
    cxld z(x, y);
    cxld res (0,0);
    cxld zp = 1; for(int i=0; i<SI; i++) zp *= z;

    for(int i=prec; i>0; i--) { 
      res += ans[i]; 
      res *= zp;
      }
    res += ans[0]; res *= z;
    return make_pair(real(res), imag(res));
    }

  EX pair<ld, ld> compute(ld x, ld y) { return compute(x,y,deg); }
  EX }

#if HDR
inline bool mdAzimuthalEqui() { return among(pmodel, mdEquidistant, mdEquiarea, mdEquivolume); }
inline bool mdBandAny() { return mdinf[pmodel].flags & mf::pseudoband; }
inline bool mdPseudocylindrical() { return mdBandAny() && !(mdinf[pmodel].flags & mf::cylindrical); }
#endif

EX namespace models {

  EX ld rotation = 0;
  EX ld rotation_xz = 90;
  EX ld rotation_xy2 = 90;
  EX int do_rotate = 1;
  EX ld ocos, osin, ocos_yz, osin_yz;
  EX ld cos_ball, sin_ball;
  EX bool model_straight, model_straight_yz;

  #if HDR
    // screen coordinates to logical coordinates: apply_orientation(x,y)
  // logical coordinates back to screen coordinates: apply_orientation(y,x)
  template<class A>
  void apply_orientation(A& x, A& y) { if(!model_straight) tie(x,y) = make_pair(x*ocos + y*osin, y*ocos - x*osin); }
  template<class A>
  void apply_orientation_yz(A& x, A& y) { if(!model_straight_yz) tie(x,y) = make_pair(x*ocos_yz + y*osin_yz, y*ocos_yz - x*osin_yz); }
  template<class A>
  void apply_ball(A& x, A& y) { tie(x,y) = make_pair(x*cos_ball + y*sin_ball, y*cos_ball - x*sin_ball); }
  #endif

  EX transmatrix rotmatrix() {
    if(GDIM == 2 || prod) return spin(rotation * degree);
    return spin(rotation_xy2 * degree) * cspin(0, 2, -rotation_xz * degree) * spin(rotation * degree);
    }
  
  int spiral_id = 7;
  
  EX cld spiral_multiplier;
  EX ld spiral_cone_rad;
  EX bool ring_not_spiral;
  
  /** the matrix to rotate the Euclidean view from the standard coordinates to the screen coordinates */
  EX transmatrix euclidean_spin;
  
  EX void configure() {
    ld ball = -pconf.ballangle * degree;
    cos_ball = cos(ball), sin_ball = sin(ball);
    ocos = cos(pconf.model_orientation * degree);
    osin = sin(pconf.model_orientation * degree);
    ocos_yz = cos(pconf.model_orientation_yz * degree);
    osin_yz = sin(pconf.model_orientation_yz * degree);
    model_straight = (ocos > 1 - 1e-9);
    model_straight_yz = GDIM == 2 || (ocos_yz > 1-1e-9);
    if(history::on) history::apply();
    
    if(!euclid) {
      ld b = pconf.spiral_angle * degree;
      ld cos_spiral = cos(b);
      ld sin_spiral = sin(b);
      spiral_cone_rad = pconf.spiral_cone * degree;
      ring_not_spiral = abs(cos_spiral) < 1e-3;
      ld mul = 1;
      if(sphere) mul = .5 * pconf.sphere_spiral_multiplier;
      else if(ring_not_spiral) mul = pconf.right_spiral_multiplier;
      else mul = pconf.any_spiral_multiplier * cos_spiral;
      
      spiral_multiplier = cld(cos_spiral, sin_spiral) * cld(spiral_cone_rad * mul / 2., 0);
      }
    if(euclid) {
      euclidean_spin = pispin * iso_inverse(cview().T * currentmap->master_relative(centerover, true));
      euclidean_spin = gpushxto0(euclidean_spin * C0) * euclidean_spin;
      hyperpoint h = inverse(euclidean_spin) * (C0 + (euc::eumove(gp::loc{1,0})*C0 - C0) * vpconf.spiral_x + (euc::eumove(gp::loc{0,1})*C0 - C0) * vpconf.spiral_y);
      spiral_multiplier = cld(0, 2 * M_PI) / cld(h[0], h[1]);
      }
    
    if(centerover && !history::on)
    if(isize(history::path_for_lineanimation) == 0 || ((quotient || arb::in()) && history::path_for_lineanimation.back() != centerover)) {
      history::path_for_lineanimation.push_back(centerover);
      }
    }
  
  EX bool model_available(eModel pm) {
    if(mdinf[pm].flags & mf::technical) return false;
    if(prod) {
      if(pm == mdPerspective) return true;
      if(among(pm, mdBall, mdHemisphere)) return false;
      return PIU(model_available(pm));
      }
    if(sl2) return pm == mdGeodesic;
    if(nonisotropic) return among(pm, mdDisk, mdPerspective, mdHorocyclic, mdGeodesic, mdEquidistant, mdFisheye, mdLiePerspective, mdLieOrthogonal);
    if(sphere && (pm == mdHalfplane || pm == mdBall))
      return false;
    if(GDIM == 2 && is_perspective(pm)) return false;
    if(pm == mdGeodesic && !nonisotropic) return false;
    if(pm == mdLiePerspective && sphere) return false;
    if(pm == mdLieOrthogonal && sphere) return false;
    if(GDIM == 2 && pm == mdEquivolume) return false;
    if(pm == mdThreePoint && !(GDIM == 3 && !nonisotropic && !prod)) return false;
    if(GDIM == 3 && among(pm, mdBall, mdHyperboloid, mdFormula, mdPolygonal, mdRotatedHyperboles, mdSpiral, mdHemisphere)) return false;
    if(pm == mdCentralInversion && !euclid) return false;
    if(pm == mdPoorMan) return hyperbolic;
    if(pm == mdRetroHammer) return hyperbolic;
    return true;
    }    
  
  EX bool has_orientation(eModel m) {
    if(among(m, mdHorocyclic, mdLieOrthogonal, mdLiePerspective))
      return hyperbolic || in_h2xe();
    if((m == mdPerspective || m == mdGeodesic) && panini_alpha) return true;
    return
      among(m, mdHalfplane, mdPolynomial, mdPolygonal, mdTwoPoint, mdJoukowsky, mdJoukowskyInverted, mdSpiral, mdSimulatedPerspective, mdTwoHybrid, mdHorocyclic, mdAxial, mdAntiAxial, mdQuadrant,
        mdWerner, mdAitoff, mdHammer, mdLoximuthal, mdWinkelTripel, mdThreePoint) || mdBandAny();
    }

  /** @brief returns the broken coordinate, or zero */
  EX int get_broken_coord(eModel m) {
    if(m == mdWerner) return 1;
    if(sphere) return (mdinf[m].flags & mf::broken) ? 2 : 0;
    return 0;
    }

  EX bool is_hyperboloid(eModel m) {
    return m == (sphere ? mdHemisphere : mdHyperboloid);
    }
  
  EX bool is_perspective(eModel m) {
    return among(m, mdPerspective, mdGeodesic, mdLiePerspective);
    }

  EX bool is_3d(const projection_configuration& p) {
    if(GDIM == 3) return true;
    return among(p.model, mdBall, mdHyperboloid, mdHemisphere) || (p.model == mdSpiral && p.spiral_cone != 360);
    }
  
  EX bool has_transition(eModel m) {
    return among(m, mdJoukowsky, mdJoukowskyInverted, mdBand, mdAxial) && GDIM == 2;
    }
  
  EX bool product_model(eModel m) {
    if(!prod) return false;
    if(among(m, mdPerspective, mdHyperboloid, mdEquidistant, mdThreePoint)) return false;
    return true;
    }
  
  int editpos = 0;
  
  EX string get_model_name(eModel m) {
    if(m == mdDisk && GDIM == 3 && (hyperbolic || nonisotropic)) return XLAT("ball model/Gans");
    if(m == mdPerspective && prod) return XLAT("native perspective");
    if(prod) return PIU(get_model_name(m));
    if(nonisotropic) {
      if(m == mdHorocyclic && !sol) return XLAT("simple model: projection");
      if(m == mdPerspective) return XLAT("simple model: perspective");
      if(m == mdGeodesic) return XLAT("native perspective");
      if(among(m, mdEquidistant, mdFisheye, mdHorocyclic, mdLiePerspective, mdLieOrthogonal)) return XLAT(mdinf[m].name_hyperbolic);
      }
    if(m == mdDisk && GDIM == 3) return XLAT("perspective in 4D");
    if(m == mdHalfplane && GDIM == 3 && hyperbolic) return XLAT("half-space");
    if(sphere) 
      return XLAT(mdinf[m].name_spherical);
    if(euclid) 
      return XLAT(mdinf[m].name_euclidean);
    if(hyperbolic) 
      return XLAT(mdinf[m].name_hyperbolic);
    return "?";
    }

  vector<gp::loc> torus_zeros;

  void match_torus_period() {
    torus_zeros.clear();
    for(int y=0; y<=200; y++)
    for(int x=-200; x<=200; x++) {
      if(y == 0 && x <= 0) continue;
      transmatrix dummy = Id;
      euc::coord v(x, y, 0);
      bool mirr = false;
      auto t = euc::eutester;
      euc::eu.canonicalize(v, t, dummy, mirr);
      if(v == euc::euzero && t == euc::eutester)
        torus_zeros.emplace_back(x, y);      
      }
    sort(torus_zeros.begin(), torus_zeros.end(), [] (const gp::loc p1, const gp::loc p2) {
      ld d1 = hdist0(tC0(euc::eumove(p1)));
      ld d2 = hdist0(tC0(euc::eumove(p2)));
      if(d1 < d2 - 1e-6) return true;
      if(d1 > d2 + 1e-6) return false;
      return p1 < p2;
      });
    if(spiral_id > isize(torus_zeros)) spiral_id = 0;
    dialog::editNumber(spiral_id, 0, isize(torus_zeros)-1, 1, 10, XLAT("match the period of the torus"), "");
    dialog::reaction = [] () {
      auto& co = torus_zeros[spiral_id];
      vpconf.spiral_x = co.first;
      vpconf.spiral_y = co.second;
      };
    dialog::bound_low(0);
    dialog::bound_up(isize(torus_zeros)-1);
    }
  
  EX void edit_formula() {
    if(vpconf.model != mdFormula) vpconf.basic_model = vpconf.model;
    dialog::edit_string(vpconf.formula, "formula", 
      XLAT(
      "This lets you specify the projection as a formula f. "
      "The formula has access to the value 'z', which is a complex number corresponding to the (x,y) coordinates in the currently selected model; "
      "the point z is mapped to f(z). You can also use the underlying coordinates ux, uy, uz."
      )
      );
    #if CAP_QUEUE && CAP_CURVE
    dialog::extra_options = [] () {
      dialog::parser_help();
      initquickqueue();
      queuereset(mdPixel, PPR::LINE);              
      for(int a=-1; a<=1; a++) {
        curvepoint(point2(-M_PI/2 * current_display->radius, a*current_display->radius));
        curvepoint(point2(+M_PI/2 * current_display->radius, a*current_display->radius));
        queuecurve(shiftless(Id), forecolor, 0, PPR::LINE);
        curvepoint(point2(a*current_display->radius, -M_PI/2*current_display->radius));
        curvepoint(point2(a*current_display->radius, +M_PI/2*current_display->radius));
        queuecurve(shiftless(Id), forecolor, 0, PPR::LINE);
        }
      queuereset(vpconf.model, PPR::LINE);
      quickqueue();
      };
    #endif
    dialog::reaction_final = [] () {
      vpconf.model = mdFormula;
      };
    }
  
  EX void edit_rotation(ld& which) {
    dialog::editNumber(which, 0, 360, 90, 0, XLAT("rotation"), 
      "This controls the automatic rotation of the world. "
      "It affects the line animation in the history mode, and "
      "lands which have a special direction. Note that if finding this special direction is a part of the puzzle, "
      "it works only in the cheat mode.");
    dialog::dialogflags |= sm::CENTER;
    dialog::extra_options = [] () {
      dialog::addBreak(100);
      dialog::addBoolItem_choice("line animation only", models::do_rotate, 0, 'N');
      dialog::addBoolItem_choice("gravity lands", models::do_rotate, 1, 'G');
      dialog::addBoolItem_choice("all directional lands", models::do_rotate, 2, 'D');
      if(GDIM == 3) {
        dialog::addBreak(100);
        dialog::addSelItem(XLAT("XY plane"), fts(models::rotation) + "°", 'A');
        dialog::add_action([] { popScreen(); edit_rotation(models::rotation); });
        dialog::addSelItem(XLAT("XZ plane"), fts(models::rotation_xz) + "°", 'B');
        dialog::add_action([] { popScreen(); edit_rotation(models::rotation_xz); });
        dialog::addSelItem(XLAT("XY plane #2"), fts(models::rotation_xy2) + "°", 'C');
        dialog::add_action([] { popScreen(); edit_rotation(models::rotation_xy2); });
        }
      };
    }

  EX void model_list() {
    cmode = sm::SIDE | sm::MAYDARK | sm::CENTER;
    gamescreen();
    dialog::init(XLAT("models & projections"));
    #if CAP_RUG
    USING_NATIVE_GEOMETRY_IN_RUG;
    #endif

    for(int i=0; i<isize(mdinf); i++) {
      eModel m = eModel(i);
      if(m == mdFormula && ISMOBILE) continue;
      if(model_available(m)) {
        dialog::addBoolItem(get_model_name(m), vpconf.model == m, (i < 26 ? 'a'+i : 'A'+i-26));
        dialog::add_action([m] () {
          if(m == mdFormula) {
            edit_formula();
            return;
            }
          vpconf.model = m;
          polygonal::solve();
          vpconf.alpha = 1; vpconf.scale = 1;
          if(pmodel == mdBand && sphere)
            vpconf.scale = .3;
          if(pmodel == mdDisk && sphere)
            vpconf.scale = .4;
          popScreen();
          });
        }
      }
    
    dialog::display();
    }
      
  void stretch_extra() {
    dialog::addBreak(100);
    if(sphere && pmodel == mdBandEquiarea) {
      dialog::addBoolItem("Gall-Peters", vpconf.stretch == 2, 'O');
      dialog::add_action([] { vpconf.stretch = 2; dialog::ne.s = "2"; });
      }
    if(pmodel == mdBandEquiarea) {
      // y = K * sin(phi)
      // cos(phi) * cos(phi) = 1/K
      if(sphere && vpconf.stretch >= 1) {
        ld phi = acos(sqrt(1/vpconf.stretch));
        dialog::addInfo(XLAT("The current value makes the map conformal at the latitude of %1 (%2°).", fts(phi), fts(phi / degree)));
        }
      else if(hyperbolic && abs(vpconf.stretch) <= 1 && abs(vpconf.stretch) >= 1e-9) {
        ld phi = acosh(abs(sqrt(1/vpconf.stretch)));
        dialog::addInfo(XLAT("The current value makes the map conformal %1 units from the main line.", fts(phi)));
        }
      else dialog::addInfo("");
      }
    }
  
  bool set_vr_settings = true;

  EX void model_menu() {
    cmode = sm::SIDE | sm::MAYDARK | sm::CENTER;
    gamescreen();
    #if CAP_RUG
    USING_NATIVE_GEOMETRY_IN_RUG;
    #endif
    dialog::init(XLAT("models & projections"));
    
    auto vpmodel = vpconf.model;
    
    dialog::addSelItem(XLAT("projection type"), get_model_name(vpmodel), 'm');
    dialog::add_action_push(model_list);
    
    if(nonisotropic && !sl2)
      dialog::addBoolItem_action(XLAT("geodesic movement in Sol/Nil"), nisot::geodesic_movement, 'G');

    dialog::addBoolItem(XLAT("rotation"), do_rotate == 2, 'r');
    if(do_rotate == 0) dialog::lastItem().value = XLAT("NEVER");
    if(GDIM == 2)
      dialog::lastItem().value += " " + its(rotation) + "°";
    else
      dialog::lastItem().value += " " + its(rotation) + "°" + its(rotation_xz) + "°" + its(rotation_xy2) + "°";
    dialog::add_action([] { edit_rotation(rotation); });
    
    bool vr_settings = vrhr::active() && set_vr_settings;

    if(vrhr::active()) {
      dialog::addBoolItem_action(XLAT("edit VR or non-VR settings"), set_vr_settings, 'V');
      if(set_vr_settings) dialog::items.back().value = "VR";
      else dialog::items.back().value = "non-VR";
      }
    
    // if(vpmodel == mdBand && sphere)
    if(!in_perspective_v() && !vr_settings) {
      dialog::addSelItem(XLAT("scale factor"), fts(vpconf.scale), 'z');
      dialog::add_action(editScale);
      }
    
    if(abs(vpconf.alpha-1) > 1e-3 && vpmodel != mdBall && vpmodel != mdHyperboloid && vpmodel != mdHemisphere && vpmodel != mdDisk) {
      dialog::addBreak(50);
      dialog::addInfo("NOTE: this works 'correctly' only if the Poincaré model/stereographic projection is used.");
      dialog::addBreak(50);
      }
    
    if(among(vpmodel, mdDisk, mdBall, mdHyperboloid, mdRotatedHyperboles, mdPanini)) {
      dynamicval<eModel> v(vpconf.model, vpconf.model);
      if(vpmodel == mdHyperboloid) vpconf.model = mdDisk;
      add_edit(vpconf.alpha);
      }
                                  
    if(has_orientation(vpmodel)) {
      dialog::addSelItem(XLAT("model orientation"), fts(vpconf.model_orientation) + "°", 'l');
      dialog::add_action([] () {
        dialog::editNumber(vpconf.model_orientation, 0, 360, 90, 0, XLAT("model orientation"), "");
        });
      if(GDIM == 3) {
        dialog::addSelItem(XLAT("model orientation (y/z plane)"), fts(vpconf.model_orientation_yz) + "°", 'L');
        dialog::add_action([] () {
          dialog::editNumber(vpconf.model_orientation_yz, 0, 360, 90, 0, XLAT("model orientation (y/z plane)"), "");
          });
        }
      }
     
    if(among(vpmodel, mdPerspective, mdHorocyclic) && nil) {
      dialog::addSelItem(XLAT("model orientation"), fts(vpconf.model_orientation) + "°", 'l');
      dialog::add_action([] () {
        dialog::editNumber(vpconf.model_orientation, 0, 360, 90, 0, XLAT("model orientation"), "");
        });
      dialog::addSelItem(XLAT("rotational or Heisenberg"), fts(vpconf.rotational_nil), 'L');
      dialog::add_action([] () {
        dialog::editNumber(vpconf.rotational_nil, 0, 1, 1, 1, XLAT("1 = Heisenberg, 0 = rotational"), "");
        });
      }

  if(GDIM == 3 && vpmodel != mdPerspective && !vr_settings) {
    const string cliphelp = XLAT(
      "Your view of the 3D model is naturally bounded from four directions by your window. "
      "Here, you can also set up similar bounds in the Z direction. Radius of the ball/band "
      "models, and the distance from the center to the plane in the halfspace model, are 1.\n\n");
    dialog::addSelItem(XLAT("near clipping plane"), fts(vpconf.clip_max), 'c');
    dialog::add_action([cliphelp] () {
      dialog::editNumber(vpconf.clip_max, -10, 10, 0.2, 1, XLAT("near clipping plane"), 
        cliphelp + XLAT("Objects with Z coordinate "
          "bigger than this parameter are not shown. This is useful with the models which "
          "extend infinitely in the Z direction, or if you want things close to your character "
          "to be not obscured by things closer to the camera."));
      });
    dialog::addSelItem(XLAT("far clipping plane"), fts(vpconf.clip_min), 'C');
    dialog::add_action([cliphelp] () {
      dialog::editNumber(vpconf.clip_min, -10, 10, 0.2, -1, XLAT("far clipping plane"), 
        cliphelp + XLAT("Objects with Z coordinate "
          "smaller than this parameter are not shown; it also affects the fog effect"
          " (near clipping plane = 0% fog, far clipping plane = 100% fog)."));
      });
    }
    
    if(vpmodel == mdPolynomial) {
      dialog::addSelItem(XLAT("coefficient"), 
        fts(polygonal::coefr[polygonal::coefid]), 'x');
      dialog::add_action([] () {
        polygonal::maxcoef = max(polygonal::maxcoef, polygonal::coefid);
        int ci = polygonal::coefid + 1;
        dialog::editNumber(polygonal::coefr[polygonal::coefid], -10, 10, .01/ci/ci, 0, XLAT("coefficient"), "");
        });
      dialog::addSelItem(XLAT("coefficient (imaginary)"), 
        fts(polygonal::coefi[polygonal::coefid]), 'y');
      dialog::add_action([] () {
        polygonal::maxcoef = max(polygonal::maxcoef, polygonal::coefid);
        int ci = polygonal::coefid + 1;
        dialog::editNumber(polygonal::coefi[polygonal::coefid], -10, 10, .01/ci/ci, 0, XLAT("coefficient (imaginary)"), "");
        });
      dialog::addSelItem(XLAT("which coefficient"), its(polygonal::coefid), 'n');
      dialog::add_action([] () {
        dialog::editNumber(polygonal::coefid, 0, polygonal::MSI-1, 1, 0, XLAT("which coefficient"), "");
        dialog::bound_low(0); dialog::bound_up(polygonal::MSI-1);
        });
      }

    if(vpmodel == mdHalfplane) {
      dialog::addSelItem(XLAT("half-plane scale"), fts(vpconf.halfplane_scale), 'b');
      dialog::add_action([] () {
        dialog::editNumber(vpconf.halfplane_scale, 0, 2, 0.25, 1, XLAT("half-plane scale"), "");
        });
      }

    if(vpmodel == mdRotatedHyperboles) {
      dialog::addBoolItem_action(XLAT("use atan to make it finite"), vpconf.use_atan, 'x');
      }

    if(among(vpmodel, mdLieOrthogonal, mdLiePerspective)) {
      if(in_s2xe() || (sphere && GDIM == 2)) dialog::addInfo(XLAT("this is not a Lie group"), 0xC00000);
      else if(!hyperbolic && !sol && !nih && !nil && !euclid && !in_h2xe() && !in_e2xe())
        dialog::addInfo(XLAT("not implemented"));
      }

    if(vpmodel == mdBall && !vr_settings) {
      dialog::addSelItem(XLAT("projection in ball model"), fts(vpconf.ballproj), 'x');
      dialog::add_action([] () {
        dialog::editNumber(vpconf.ballproj, 0, 100, .1, 0, XLAT("projection in ball model"), 
          "This parameter affects the ball model the same way as the projection parameter affects the disk model.");
        });
      }

    if(vpmodel == mdPolygonal) {
      dialog::addSelItem(XLAT("polygon sides"), its(polygonal::SI), 'x');
      dialog::add_action([] () {
        dialog::editNumber(polygonal::SI, 3, 10, 1, 4, XLAT("polygon sides"), "");
        dialog::reaction = polygonal::solve;
        });
      dialog::addSelItem(XLAT("star factor"), fts(polygonal::STAR), 'y');
      dialog::add_action([]() {
        dialog::editNumber(polygonal::STAR, -1, 1, .1, 0, XLAT("star factor"), "");
        dialog::reaction = polygonal::solve;
        });
      dialog::addSelItem(XLAT("degree of the approximation"), its(polygonal::deg), 'n');
      dialog::add_action([](){
        dialog::editNumber(polygonal::deg, 2, polygonal::MSI-1, 1, 2, XLAT("degree of the approximation"), "");
        dialog::reaction = polygonal::solve;
        dialog::bound_low(0); dialog::bound_up(polygonal::MSI-1);
        });
      }
    
    if(is_3d(vpconf) && GDIM == 2 && !vr_settings) 
      add_edit(vpconf.ballangle);     
    
    if(vr_settings) {
      dialog::addSelItem(XLAT("VR: rotate the 3D model"), fts(vpconf.vr_angle) + "°", 'B');
      dialog::add_action([] { 
        dialog::editNumber(vpconf.vr_angle, 0, 90, 5, 0, XLAT("VR: rotate the 3D model"), 
          "How the VR model should be rotated."
          );
        });
      dialog::addSelItem(XLAT("VR: shift the 3D model"), fts(vpconf.vr_zshift), 'Z');
      dialog::add_action([] { 
        dialog::editNumber(vpconf.vr_zshift, 0, 5, 0.1, 1, XLAT("VR: shift the 3D model"), 
          "How the VR model should be shifted forward, in units. "
          "The Poincaré disk has the size of 1 unit. You probably do not want this in perspective projections, but "
          "it is useful to see e.g. the Poincaré ball not from the center."
          );
        });
      dialog::addSelItem(XLAT("VR: scale the 3D model"), fts(vpconf.vr_scale_factor) + "m", 'S');
      dialog::add_action([] { 
        dialog::editNumber(vpconf.vr_scale_factor, 0, 5, 0.1, 1, XLAT("VR: scale the 3D model"), 
          "How the VR model should be scaled. At scale 1, 1 unit = 1 meter. Does not affect perspective projections, "
          "where the 'absolute unit' setting is used instead."
          );
        });
      }
    
    if(is_hyperboloid(vpmodel))
      add_edit(vpconf.top_z);
    
    if(has_transition(vpmodel)) 
      add_edit(vpconf.model_transition);

    if(among(vpmodel, mdJoukowsky, mdJoukowskyInverted, mdSpiral) && GDIM == 2) 
      add_edit(vpconf.skiprope);

    if(vpmodel == mdJoukowskyInverted)
      add_edit(vpconf.dualfocus_autoscale);
    
    if(vpmodel == mdHemisphere && euclid) 
      add_edit(vpconf.euclid_to_sphere);
      
    if(among(vpmodel, mdTwoPoint, mdSimulatedPerspective, mdTwoHybrid, mdThreePoint)) 
      add_edit(vpconf.twopoint_param);
    
    if(vpmodel == mdFisheye) 
      add_edit(vpconf.fisheye_param);

    if(is_hyperboloid(vpmodel))
      add_edit(pconf.show_hyperboloid_flat);
    
    if(vpmodel == mdCollignon) 
      add_edit(vpconf.collignon_parameter);
    
    if(vpmodel == mdMiller) {
      dialog::addSelItem(XLAT("parameter"), fts(vpconf.miller_parameter), 'b');
      dialog::add_action([](){
        dialog::editNumber(vpconf.miller_parameter, -1, 1, .1, 4/5., XLAT("parameter"), 
          "The Miller projection is obtained by multiplying the latitude by 4/5, using Mercator projection, and then multiplying Y by 5/4. "
          "Here you can change this parameter."
          );
        });
      }
    
    if(among(vpmodel, mdLoximuthal, mdRetroHammer, mdRetroCraig)) 
      add_edit(vpconf.loximuthal_parameter);

    if(among(vpmodel, mdAitoff, mdHammer, mdWinkelTripel)) 
      add_edit(vpconf.aitoff_parameter);
    
    if(vpmodel == mdWinkelTripel) 
      add_edit(vpconf.winkel_parameter);
    
    if(vpmodel == mdSpiral && !euclid) {
      add_edit(vpconf.spiral_angle);

      add_edit(
        sphere ? vpconf.sphere_spiral_multiplier :
        ring_not_spiral ? vpconf.right_spiral_multiplier :
        vpconf.any_spiral_multiplier
        );

      add_edit(vpconf.spiral_cone);
      }

    if(vpmodel == mdSpiral && euclid) {
      add_edit(vpconf.spiral_x);
      add_edit(vpconf.spiral_y);
      if(euclid && quotient) {
        dialog::addSelItem(XLAT("match the period"), its(spiral_id), 'n');
        dialog::add_action(match_torus_period);
        }
      }

    add_edit(vpconf.stretch);
    
    if(product_model(vpmodel))
      add_edit(vpconf.product_z_scale);

    #if CAP_GL
    dialog::addBoolItem(XLAT("use GPU to compute projections"), vid.consider_shader_projection, 'G');
    bool shaderside_projection = get_shader_flags() & SF_DIRECT;
    if(vid.consider_shader_projection && !shaderside_projection)
      dialog::lastItem().value = XLAT("N/A");
    if(vid.consider_shader_projection && shaderside_projection && vpmodel)
      dialog::lastItem().value += XLAT(" (2D only)");
    dialog::add_action([] { vid.consider_shader_projection = !vid.consider_shader_projection; });
    #endif

    menuitem_sightrange('R');
      
    dialog::addBreak(100);
    dialog::addItem(XLAT("history mode"), 'a');
    dialog::add_action_push(history::history_menu);
#if CAP_RUG
    if(GDIM == 2 || rug::rugged) {
      dialog::addItem(XLAT("hypersian rug mode"), 'u');
      dialog::add_action_push(rug::show);
      }
#endif
    dialog::addBack();

    dialog::display();
    mouseovers = XLAT("see http://www.roguetemple.com/z/hyper/models.php");
    }
    
  EX void quick_model() {
    cmode = sm::CENTER | sm::SIDE | sm::MAYDARK;
    gamescreen();
    dialog::init("models & projections");
    
    if(GDIM == 2 && !euclid) {
      dialog::addItem(hyperbolic ? XLAT("Gans model") : XLAT("orthographic projection"), '1');
      dialog::add_action([] { if(rug::rugged) rug::close(); pconf.alpha = 999; pconf.scale = 998; pconf.xposition = pconf.yposition = 0; popScreen(); });
      dialog::addItem(hyperbolic ? XLAT("Poincaré model") : XLAT("stereographic projection"), '2');
      dialog::add_action([] { if(rug::rugged) rug::close(); pconf.alpha = 1; pconf.scale = 1; pconf.xposition = pconf.yposition = 0; popScreen(); });
      dialog::addItem(hyperbolic ? XLAT("Beltrami-Klein model") : XLAT("gnomonic projection"), '3');
      dialog::add_action([] { if(rug::rugged) rug::close(); pconf.alpha = 0; pconf.scale = 1; pconf.xposition = pconf.yposition = 0; popScreen(); });
      if(sphere) {
        dialog::addItem(XLAT("stereographic projection") + " " + XLAT("(zoomed out)"), '4');
        dialog::add_action([] { if(rug::rugged) rug::close(); pconf.alpha = 1; pconf.scale = 0.4; pconf.xposition = pconf.yposition = 0; popScreen(); });
        }
      if(hyperbolic) {
        dialog::addItem(XLAT("Gans model") + " " + XLAT("(zoomed out)"), '4');
        dialog::add_action([] { if(rug::rugged) rug::close(); pconf.alpha = 999; pconf.scale = 499; pconf.xposition = pconf.yposition = 0; popScreen(); });
        #if CAP_RUG
        dialog::addItem(XLAT("Hypersian Rug"), 'u');
        dialog::add_action([] {  
          if(rug::rugged) pushScreen(rug::show);
          else {
            pconf.alpha = 1, pconf.scale = 1; if(!rug::rugged) rug::init(); popScreen(); 
            }
          });
        #endif
        }
      }
    else if(GDIM == 2 && euclid) {
      auto zoom_to = [] (ld s) {
        pconf.xposition = pconf.yposition = 0;
        ld maxs = 0;
        auto& cd = current_display;
        for(auto& p: gmatrix) for(int i=0; i<p.first->type; i++) {
          shiftpoint h = tC0(p.second * currentmap->adj(p.first, i));
          hyperpoint onscreen;
          applymodel(h, onscreen);
          maxs = max(maxs, onscreen[0] / cd->xsize);
          maxs = max(maxs, onscreen[1] / cd->ysize);
          }
        pconf.alpha = 1;
        pconf.scale = s * pconf.scale / 2 / maxs / cd->radius;
        popScreen();
        };
      dialog::addItem(XLAT("zoom 2x"), '1');
      dialog::add_action([zoom_to] { zoom_to(2); });
      dialog::addItem(XLAT("zoom 1x"), '2');
      dialog::add_action([zoom_to] { zoom_to(1); });
      dialog::addItem(XLAT("zoom 0.5x"), '3');
      dialog::add_action([zoom_to] { zoom_to(.5); });
      #if CAP_RUG
      if(quotient) {
        dialog::addItem(XLAT("cylinder/donut view"), 'u');
        dialog::add_action([] {
          if(rug::rugged) pushScreen(rug::show);
          else {
            pconf.alpha = 1, pconf.scale = 1; if(!rug::rugged) rug::init(); popScreen(); 
            }
          });
        }
      #endif
      }
    else if(GDIM == 3) {
      auto& ysh = (WDIM == 2 ? vid.camera : vid.yshift);
      dialog::addItem(XLAT("first-person perspective"), '1');
      dialog::add_action([&ysh] { ysh = 0; vid.sspeed = 0; popScreen(); } );
      dialog::addItem(XLAT("fixed point of view"), '2');
      dialog::add_action([&ysh] { ysh = 0; vid.sspeed = -10; popScreen(); } );
      dialog::addItem(XLAT("third-person perspective"), '3');
      dialog::add_action([&ysh] { ysh = 1; vid.sspeed = 0; popScreen(); } );
      }
    if(WDIM == 2) {
      dialog::addItem(XLAT("toggle full 3D graphics"), 'f');
      dialog::add_action([] { geom3::switch_fpp(); popScreen(); });
      }
    dialog::addItem(XLAT("advanced projections"), 'a');
    dialog::add_action_push(model_menu);
    menuitem_sightrange('r');
    dialog::addBack();
    dialog::display();
    }

  #if CAP_COMMANDLINE
  
  EX eModel read_model(const string& ss) {
    for(int i=0; i<isize(mdinf); i++) {
      if(hyperbolic && appears(mdinf[i].name_hyperbolic, ss)) return eModel(i);
      if(euclid && appears(mdinf[i].name_euclidean, ss)) return eModel(i);
      if(sphere && appears(mdinf[i].name_spherical, ss)) return eModel(i);
      }
    for(int i=0; i<isize(mdinf); i++) {
      if(appears(mdinf[i].name_hyperbolic, ss)) return eModel(i);
      if(appears(mdinf[i].name_euclidean, ss)) return eModel(i);
      if(appears(mdinf[i].name_spherical, ss)) return eModel(i);
      }
    return eModel(atoi(ss.c_str()));
    }
    
  int readArgs() {
    using namespace arg;
             
    if(0) ;
    else if(argis("-els")) {
      shift_arg_formula(history::extra_line_steps);
      }
    else if(argis("-stretch")) {
      PHASEFROM(2); shift_arg_formula(vpconf.stretch);
      }
    else if(argis("-PM")) { 
      PHASEFROM(2); shift(); vpconf.model = read_model(args());
      if(vpconf.model == mdFormula) {
        shift(); vpconf.basic_model = eModel(argi());
        shift(); vpconf.formula = args();
        }
      }
    else if(argis("-ballangle")) { 
      PHASEFROM(2); 
      shift_arg_formula(vpconf.ballangle);
      }
    else if(argis("-topz")) { 
      PHASEFROM(2); 
      shift_arg_formula(vpconf.top_z);
      }
    else if(argis("-twopoint")) { 
      PHASEFROM(2); 
      shift_arg_formula(vpconf.twopoint_param);
      }
    else if(argis("-hp")) { 
      PHASEFROM(2); 
      shift_arg_formula(vpconf.halfplane_scale);
      }
    else if(argis("-mori")) { 
      PHASEFROM(2); 
      shift_arg_formula(vpconf.model_orientation);
      }
    else if(argis("-mets")) { 
      PHASEFROM(2); 
      shift_arg_formula(vpconf.euclid_to_sphere);
      }
    else if(argis("-mhyp")) { 
      PHASEFROM(2); 
      shift_arg_formula(vpconf.hyperboloid_scaling);
      }
    else if(argis("-mdepth")) { 
      PHASEFROM(2); 
      shift_arg_formula(vpconf.depth_scaling);
      }
    else if(argis("-mnil")) { 
      PHASEFROM(2); 
      shift_arg_formula(vpconf.rotational_nil);
      }
    else if(argis("-mori2")) { 
      PHASEFROM(2); 
      shift_arg_formula(vpconf.model_orientation);
      shift_arg_formula(vpconf.model_orientation_yz);
      }
    else if(argis("-crot")) { 
      PHASEFROM(2); 
      shift_arg_formula(models::rotation);
      if(GDIM == 3) shift_arg_formula(models::rotation_xz);
      if(GDIM == 3) shift_arg_formula(models::rotation_xy2);
      }
    else if(argis("-clip")) { 
      PHASEFROM(2); 
      shift_arg_formula(vpconf.clip_min);
      shift_arg_formula(vpconf.clip_max);
      }
    else if(argis("-mtrans")) { 
      PHASEFROM(2); 
      shift_arg_formula(vpconf.model_transition);
      }
    else if(argis("-mparam")) { 
      PHASEFROM(2); 
      if(pmodel == mdCollignon) shift_arg_formula(vpconf.collignon_parameter);
      else if(pmodel == mdMiller) shift_arg_formula(vpconf.miller_parameter);
      else if(among(pmodel, mdLoximuthal, mdRetroCraig, mdRetroHammer)) shift_arg_formula(vpconf.loximuthal_parameter);
      else if(among(pmodel, mdAitoff, mdHammer, mdWinkelTripel)) shift_arg_formula(vpconf.aitoff_parameter);
      if(pmodel == mdWinkelTripel) shift_arg_formula(vpconf.winkel_parameter);
      }
    else if(argis("-sang")) { 
      PHASEFROM(2); 
      shift_arg_formula(vpconf.spiral_angle);
      if(sphere)
        shift_arg_formula(vpconf.sphere_spiral_multiplier);
      else if(vpconf.spiral_angle == 90)
        shift_arg_formula(vpconf.right_spiral_multiplier);
      }
    else if(argis("-ssm")) { 
      PHASEFROM(2);
      shift_arg_formula(vpconf.any_spiral_multiplier);
      }
    else if(argis("-scone")) { 
      PHASEFROM(2); 
      shift_arg_formula(vpconf.spiral_cone);
      }
    else if(argis("-sxy")) { 
      PHASEFROM(2); 
      shift_arg_formula(vpconf.spiral_x);
      shift_arg_formula(vpconf.spiral_y);
      }
    else if(argis("-mob")) { 
      PHASEFROM(2); 
      shift_arg_formula(vpconf.skiprope);
      }
    else if(argis("-palpha")) { 
      PHASEFROM(2); 
      #if CAP_GL
      shift_arg_formula(panini_alpha, reset_all_shaders);
      #else
      shift_arg_formula(panini_alpha);
      #endif
      }
    else if(argis("-salpha")) { 
      PHASEFROM(2); 
      #if CAP_GL
      shift_arg_formula(stereo_alpha, reset_all_shaders);
      #else
      shift_arg_formula(stereo_alpha);
      #endif
      }
    else if(argis("-zoom")) { 
      PHASEFROM(2); shift_arg_formula(vpconf.scale);
      }
    else if(argis("-alpha")) { 
      PHASEFROM(2); shift_arg_formula(vpconf.alpha);
      }
    else if(argis("-d:model")) 
      launch_dialog(model_menu);
    else if(argis("-d:formula")) {
      launch_dialog();
      edit_formula();
      }
    else if(argis("-d:match")) {
      launch_dialog(match_torus_period);
      edit_formula();
      }
    else return 1;
    return 0;    
    }
  
  auto hookArg = addHook(hooks_args, 100, readArgs); 
  #endif  

  void add_model_config() {
    addsaver(polygonal::SI, "polygon sides");
    param_f(polygonal::STAR, "star", "polygon star factor");
    addsaver(polygonal::deg, "polygonal degree");

    addsaver(polygonal::maxcoef, "polynomial degree");
    for(int i=0; i<polygonal::MSI; i++) {
      addsaver(polygonal::coefr[i], "polynomial "+its(i)+".real");
      addsaver(polygonal::coefi[i], "polynomial "+its(i)+".imag");
      }

    param_f(models::rotation, "rotation", "conformal rotation");
    addsaver(models::rotation_xz, "conformal rotation_xz");
    addsaver(models::rotation_xy2, "conformal rotation_2");
    addsaver(models::do_rotate, "conformal rotation mode", 1);

    param_f(pconf.halfplane_scale, "hp", "halfplane scale", 1);
    
    auto add_all = [&] (projection_configuration& p, string pp, string sp) {

      bool rug = pp != "";
      dynamicval<function<bool()>> ds(auto_restrict);
      auto_restrict = [&p] { return &vpconf == &p; };

      addsaverenum(p.model, pp+"used model", mdDisk);
      param_custom(pmodel, "projection|Poincare|Klein|half-plane|perspective", menuitem_projection, '1');

      param_f(p.model_orientation, pp+"mori", sp+"model orientation", 0);
      param_f(p.model_orientation_yz, pp+"mori_yz", sp+"model orientation-yz", 0);

      param_f(p.top_z, sp+"topz", 5)
      -> editable(1, 20, .25, "maximum z coordinate to show", "maximum z coordinate to show", 'l');       

      param_f(p.model_transition, pp+"mtrans", sp+"model transition", 1)
      -> editable(0, 1, .1, "model transition", 
          "You can change this parameter for a transition from another model to this one.", 't');          
      
      param_f(p.rotational_nil, sp+"rotnil", 1);
  
      param_f(p.clip_min, pp+"clipmin", sp+"clip-min", rug ? -100 : -1);
      param_f(p.clip_max, pp+"clipmax", sp+"clip-max", rug ? +10 : +1);
  
      param_f(p.euclid_to_sphere, pp+"ets", sp+"euclid to sphere projection", 1.5)
      -> editable(1e-1, 10, .1, "ETS parameter", "Stereographic projection to a sphere. Choose the radius of the sphere.", 'l')
      -> set_sets(dialog::scaleLog);

      param_f(p.twopoint_param, pp+"twopoint", sp+"twopoint parameter", 1)
      -> editable(1e-3, 10, .1, "two-point parameter", "In two-point-based models, this parameter gives the distance from each of the two points to the center.", 'b')
      -> set_sets(dialog::scaleLog)
;
      param_f(p.fisheye_param, pp+"fisheye", sp+"fisheye parameter", 1)
      -> editable(1e-3, 10, .1, "fisheye parameter", "Size of the fish eye.", 'b')
      -> set_sets(dialog::scaleLog);

      param_f(p.stretch, pp+"stretch", 1)
      -> editable(0, 10, .1, "vertical stretch", "Vertical stretch factor.", 's')
      -> set_extra(stretch_extra);
      
      param_f(p.product_z_scale, pp+"zstretch")
      -> editable(0.1, 10, 0.1, "product Z stretch", "", 'Z');
  
      param_f(p.collignon_parameter, pp+"collignon", sp+"collignon-parameter", 1)
      -> editable(-1, 1, .1, "Collignon parameter", "", 'b')
      -> modif([] (float_setting* f) {
        f->unit = vpconf.collignon_reflected ? " (r)" : "";
        })
      -> set_extra([&p] { 
        add_edit(p.collignon_reflected);
        });
      param_b(p.collignon_reflected, sp+"collignon-reflect", false)
      -> editable("Collignon reflect", 'R');
  
      param_f(p.aitoff_parameter, sp+"aitoff")
      -> editable(-1, 1, .1, "Aitoff parameter", 
          "The Aitoff projection is obtained by multiplying the longitude by 1/2, using azimuthal equidistant projection, and then dividing X by 1/2. "
          "Hammer projection is similar but equi-area projection is used instead. "
          "Here you can change this parameter.", 'b');
      param_f(p.miller_parameter, sp+"miller");
      param_f(p.loximuthal_parameter, sp+"loximuthal")
      -> editable(-M_PI/2, M_PI/2, .1, "loximuthal parameter", 
          "Loximuthal is similar to azimuthal equidistant, but based on loxodromes (lines of constant geographic direction) rather than geodesics. "
          "The loximuthal projection maps (the shortest) loxodromes to straight lines of the same length, going through the starting point. "
          "This setting changes the latitude of the starting point.\n\n"
          "In retroazimuthal projections, a point is drawn at such a point that the azimuth *from* that point to the chosen central point is correct. "
          "For example, if you should move east, the point is drawn to the right. This parameter is the latitude of the central point."
          "\n\n(In hyperbolic geometry directions are assigned according to the Lobachevsky coordinates.)", 'b'
          );
      param_f(p.winkel_parameter, sp+"winkel")
      -> editable(-1, 1, .1, "Winkel Tripel mixing", 
        "The Winkel Tripel projection is the average of Aitoff projection and equirectangular projection. Here you can change the proportion.", 'B');
  
      param_b(p.show_hyperboloid_flat, sp+"hyperboloid-flat", true)
      -> editable("show flat", 'b');
  
      param_f(p.skiprope, sp+"mobius", 0)
      -> editable(0, 360, 15, "Möbius transformations", "", 'S')->unit = "°";

      param_b(p.dualfocus_autoscale, sp+"dualfocus_autoscale", 0)
      -> editable("autoscale dual focus", 'A');
      
      addsaver(p.formula, sp+"formula");
      addsaverenum(p.basic_model, sp+"basic model");
      addsaver(p.use_atan, sp+"use_atan");  
  
      param_f(p.spiral_angle, sp+"sang")
      -> editable(0, 360, 15, "spiral angle", "set to 90° for the ring projection", 'x')
      -> unit = "°";
      param_f(p.spiral_x, sp+"spiralx")
      -> editable(-20, 20, 1, "spiral period: x", "", 'x');
      param_f(p.spiral_y, sp+"spiraly")
      -> editable(-20, 20, 1, "spiral period: y", "", 'y');
  
      param_f(p.scale, sp+"scale", 1);
      param_f(p.xposition, sp+"xposition", 0);
      param_f(p.yposition, sp+"yposition", 0);

      param_i(p.back_and_front, sp+"backandfront", 0);

      addsaver(p.alpha, sp+"projection", 1);
      param_custom(p.alpha, sp+"projection", menuitem_projection_distance, 'p')
      ->help_text = "projection distance|Gans Klein Poincare orthographic stereographic";

      param_f(p.camera_angle, pp+"cameraangle", sp+"camera angle", 0);
      addsaver(p.ballproj, sp+"ballproj", 1);      

      param_f(p.ballangle, pp+"ballangle", sp+"ball angle", 20)
      -> editable(0, 90, 5, "camera rotation in 3D models", 
        "Rotate the camera in 3D models (ball model, hyperboloid, and hemisphere). "
        "Note that hyperboloid and hemisphere models are also available in the "
        "Hypersian Rug surfaces menu, but they are rendered differently there -- "
        "by making a flat picture first, then mapping it to a surface. "
        "This makes the output better in some ways, but 3D effects are lost. "
        "Hypersian Rug model also allows more camera freedom.",
        'b')
      -> unit = "°";

      string help =
        "This parameter has a bit different scale depending on the settings:\n"
        "(1) in spherical geometry (with spiral angle=90°, 1 produces a stereographic projection)\n"
        "(2) in hyperbolic geometry, with spiral angle being +90° or -90°\n"
        "(3) in hyperbolic geometry, with other spiral angles (1 makes the bands fit exactly)";
      
      param_f(p.sphere_spiral_multiplier, "sphere_spiral_multiplier")
      -> editable(0, 10, .1, "sphere spiral multiplier", help, 'M')->unit = "°";

      param_f(p.right_spiral_multiplier, "right_spiral_multiplier")
      -> editable(0, 10, .1, "right spiral multiplier", help, 'M')->unit = "°";

      param_f(p.any_spiral_multiplier, "any_spiral_multiplier")
      -> editable(0, 10, .1, "any spiral multiplier", help, 'M')->unit = "°";

      param_f(p.spiral_cone, "spiral_cone")
      -> editable(0, 360, -45, "spiral cone", "", 'C')->unit = "°";
      };
    
    add_all(pconf, "", "");
    add_all(vid.rug_config, "rug_", "rug-");
    }

  auto hookSet = addHook(hooks_configfile, 100, add_model_config);
  }

}