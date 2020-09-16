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
      euclidean_spin = pispin * iso_inverse(cview().T * master_relative(centerover, true));
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
    if(prod) {
      if(pm == mdPerspective) return true;
      if(among(pm, mdBall, mdHemisphere)) return false;
      return PIU(model_available(pm));
      }
    if(sl2) return pm == mdGeodesic;
    if(nonisotropic) return among(pm, mdDisk, mdPerspective, mdHorocyclic, mdGeodesic, mdEquidistant, mdFisheye);
    if(pm == mdGeodesic && !sol) return false;
    if(sphere && (pm == mdHalfplane || pm == mdBall))
      return false;
    if(GDIM == 2 && pm == mdPerspective) return false;
    if(GDIM == 2 && pm == mdEquivolume) return false;
    if(GDIM == 3 && among(pm, mdBall, mdHyperboloid, mdFormula, mdPolygonal, mdRotatedHyperboles, mdSpiral, mdHemisphere)) return false;
    if(pm == mdCentralInversion && !euclid) return false;
    return true;
    }    
  
  EX bool has_orientation(eModel m) {
    if(m == mdHorocyclic)
      return hyperbolic;
    return
      among(m, mdHalfplane, mdPolynomial, mdPolygonal, mdTwoPoint, mdJoukowsky, mdJoukowskyInverted, mdSpiral, mdSimulatedPerspective, mdTwoHybrid, mdHorocyclic, mdAxial, mdAntiAxial, mdQuadrant,
        mdWerner, mdAitoff, mdHammer, mdLoximuthal, mdWinkelTripel) || mdBandAny();
    }

  /** @brief returns the broken coordinate, or zero */
  EX int get_broken_coord(eModel m) {
    if(m == mdWerner) return 1;
    if(sphere) return (mdinf[m].flags & mf::broken) ? 2 : 0;
    return 0;
    }
  
  EX bool is_perspective(eModel m) {
    return among(m, mdPerspective, mdGeodesic);
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
    if(among(m, mdPerspective, mdHyperboloid, mdEquidistant)) return false;
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
      if(among(m, mdEquidistant, mdFisheye, mdHorocyclic)) return XLAT(mdinf[m].name_hyperbolic);
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
      ) + "\n\n" + parser_help()
      );
    #if CAP_QUEUE && CAP_CURVE
    dialog::extra_options = [] () {
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
    gamescreen(0);
    dialog::init(XLAT("models & projections"));
    #if CAP_RUG
    USING_NATIVE_GEOMETRY_IN_RUG;
    #endif

    for(int i=0; i<mdGUARD; i++) {
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
      
  void edit_stretch() {
    dialog::editNumber(vpconf.stretch, 0, 10, .1, 1, XLAT("vertical stretch"), 
      "Vertical stretch factor."
      );
    dialog::extra_options = [] () {
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
      };
    }

  EX void model_menu() {
    cmode = sm::SIDE | sm::MAYDARK | sm::CENTER;
    gamescreen(0);
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
    
    // if(vpmodel == mdBand && sphere)
    if(!in_perspective()) {
      dialog::addSelItem(XLAT("scale factor"), fts(vpconf.scale), 'z');
      dialog::add_action(editScale);
      }
    
    if(abs(pconf.alpha-1) > 1e-3 && vpmodel != mdBall && vpmodel != mdHyperboloid && vpmodel != mdHemisphere && vpmodel != mdDisk) {
      dialog::addBreak(50);
      dialog::addInfo("NOTE: this works 'correctly' only if the Poincaré model/stereographic projection is used.");
      dialog::addBreak(50);
      }
    
    if(among(vpmodel, mdDisk, mdBall, mdHyperboloid, mdRotatedHyperboles)) {
      dialog::addSelItem(XLAT("projection distance"), fts(vpconf.alpha) + " (" + current_proj_name() + ")", 'p');
      dialog::add_action(projectionDialog);
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

  if(GDIM == 3 && vpmodel != mdPerspective) {
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

    if(vpmodel == mdBall) {
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
    
    if(is_3d(vpconf) && GDIM == 2) {
      dialog::addSelItem(XLAT("camera rotation in 3D models"), fts(vpconf.ballangle) + "°", 'b');
      dialog::add_action(config_camera_rotation);
      }
    
    if(vpmodel == mdHyperboloid) {
      dialog::addSelItem(XLAT("maximum z coordinate to show"), fts(vpconf.top_z), 'l');
      dialog::add_action([](){
        dialog::editNumber(vpconf.top_z, 1, 20, 0.25, 4, XLAT("maximum z coordinate to show"), "");
        });
      }
    
    if(has_transition(vpmodel)) {
      dialog::addSelItem(XLAT("model transition"), fts(vpconf.model_transition), 't');
      dialog::add_action([]() {
        dialog::editNumber(vpconf.model_transition, 0, 1, 0.1, 1, XLAT("model transition"), 
          "You can change this parameter for a transition from another model to this one."
          );
        });
      }

    if(among(vpmodel, mdJoukowsky, mdJoukowskyInverted, mdSpiral) && GDIM == 2) {
      dialog::addSelItem(XLAT("Möbius transformations"), fts(vpconf.skiprope) + "°", 'S');
      dialog::add_action([](){
        dialog::editNumber(vpconf.skiprope, 0, 360, 15, 0, XLAT("Möbius transformations"), "");
        });
      }
    
    if(vpmodel == mdHemisphere && euclid) {
      dialog::addSelItem(XLAT("parameter"), fts(vpconf.euclid_to_sphere), 'l');
      dialog::add_action([] () {
        dialog::editNumber(vpconf.euclid_to_sphere, 0, 10, .1, 1, XLAT("parameter"), 
          "Stereographic projection to a sphere. Choose the radius of the sphere."
          );
        dialog::scaleLog();
        });
      }
      
    if(among(vpmodel, mdTwoPoint, mdSimulatedPerspective, mdTwoHybrid)) {
      dialog::addSelItem(XLAT("parameter"), fts(vpconf.twopoint_param), 'b');
      dialog::add_action([vpmodel](){
        dialog::editNumber(vpconf.twopoint_param, 1e-3, 10, .1, 1, XLAT("parameter"), 
          s0 + (vpmodel == mdTwoPoint ?
          "This model maps the world so that the distances from two points "
          "are kept. " : "") + "This parameter gives the distance from the two points to "
          "the center."
          );
        dialog::scaleLog();
        });
      }
    
    if(vpmodel == mdFisheye) {
      dialog::addSelItem(XLAT("parameter"), fts(vpconf.fisheye_param), 'b');
      dialog::add_action([](){
        dialog::editNumber(vpconf.fisheye_param, 1e-3, 10, .1, 1, XLAT("parameter"), 
          "Size of the fish eye."
          );
        dialog::scaleLog();
        });
      }
    
    if(vpmodel == mdCollignon) {
      dialog::addSelItem(XLAT("parameter"), fts(vpconf.collignon_parameter) + (vpconf.collignon_reflected ? " (r)" : ""), 'b');
      dialog::add_action([](){
        dialog::editNumber(vpconf.collignon_parameter, -1, 1, .1, 1, XLAT("parameter"), 
          ""
          );
        dialog::extra_options = [] {
          dialog::addBoolItem_action(XLAT("reflect"), vpconf.collignon_reflected, 'R');
          };
        });
      }
    
    if(vpmodel == mdMiller) {
      dialog::addSelItem(XLAT("parameter"), fts(vpconf.miller_parameter), 'b');
      dialog::add_action([](){
        dialog::editNumber(vpconf.miller_parameter, -1, 1, .1, 4/5., XLAT("parameter"), 
          "The Miller projection is obtained by multiplying the latitude by 4/5, using Mercator projection, and then multiplying Y by 5/4. "
          "Here you can change this parameter."
          );
        });
      }
    
    if(vpmodel == mdLoximuthal) {
      dialog::addSelItem(XLAT("parameter"), fts(vpconf.loximuthal_parameter), 'b');
      dialog::add_action([](){
        dialog::editNumber(vpconf.loximuthal_parameter, -M_PI/2, M_PI/2, .1, 0, XLAT("parameter"), 
          "This model is similar to azimuthal equidistant, but based on loxodromes (lines of constant geographic direction) rather than geodesics. "
          "The loximuthal projection maps (the shortest) loxodromes to straight lines of the same length, going through the starting point. "
          "This setting changes the latitude of the starting point." 
          );
        });
      }

    if(among(vpmodel, mdAitoff, mdHammer, mdWinkelTripel)) {
      dialog::addSelItem(XLAT("parameter"), fts(vpconf.aitoff_parameter), 'b');
      dialog::add_action([](){
        dialog::editNumber(vpconf.aitoff_parameter, -1, 1, .1, 1/2., XLAT("parameter"), 
          "The Aitoff projection is obtained by multiplying the longitude by 1/2, using azimuthal equidistant projection, and then multiplying X by 1/2. "
          "Hammer projection is similar but equi-area projection is used instead. "
          "Here you can change this parameter."
          );
        });
      }
    
    if(vpmodel == mdWinkelTripel) {
      dialog::addSelItem(XLAT("mixing proportion"), fts(vpconf.winkel_parameter), 'B');
      dialog::add_action([](){
        dialog::editNumber(vpconf.winkel_parameter, -1, 1, .1, 1, XLAT("parameter"), 
          "The Winkel Tripel projection is the average of Aitoff projection and equirectangular projection. Here you can change the proportion."
          );
        });
      }
    
    if(vpmodel == mdSpiral && !euclid) {
      dialog::addSelItem(XLAT("spiral angle"), fts(vpconf.spiral_angle) + "°", 'x');
      dialog::add_action([](){
        dialog::editNumber(vpconf.spiral_angle, 0, 360, 15, 0, XLAT("spiral angle"), 
          XLAT("set to 90° for the ring projection")
          );
        });

      ld& which =
        sphere ? vpconf.sphere_spiral_multiplier :
        ring_not_spiral ? vpconf.right_spiral_multiplier :
        vpconf.any_spiral_multiplier;
                
      dialog::addSelItem(XLAT("spiral multiplier"), fts(which) + "°", 'M');
      dialog::add_action([&which](){
        dialog::editNumber(which, 0, 10, -.1, 1, XLAT("spiral multiplier"), 
          XLAT(
            "This parameter has a bit different scale depending on the settings:\n"
            "(1) in spherical geometry (with spiral angle=90°, 1 produces a stereographic projection)\n"
            "(2) in hyperbolic geometry, with spiral angle being +90° or -90°\n"
            "(3) in hyperbolic geometry, with other spiral angles (1 makes the bands fit exactly)"
            )
          );
        });

      dialog::addSelItem(XLAT("spiral cone"), fts(vpconf.spiral_cone) + "°", 'C');
      dialog::add_action([](){
        dialog::editNumber(vpconf.spiral_cone, 0, 360, -45, 360, XLAT("spiral cone"), "");
        });
      }

    if(vpmodel == mdSpiral && euclid) {
      dialog::addSelItem(XLAT("spiral period: x"), fts(vpconf.spiral_x), 'x');
      dialog::add_action([](){
        dialog::editNumber(vpconf.spiral_x, -20, 20, 1, 10, XLAT("spiral period: x"), "");
        });
      dialog::addSelItem(XLAT("spiral period: y"), fts(vpconf.spiral_y), 'y');
      dialog::add_action([](){
        dialog::editNumber(vpconf.spiral_y, -20, 20, 1, 10, XLAT("spiral period: y"), "");
        });
      if(euclid && quotient) {
        dialog::addSelItem(XLAT("match the period"), its(spiral_id), 'n');
        dialog::add_action(match_torus_period);
        }
      }

    dialog::addSelItem(XLAT("vertical stretch"), fts(vpconf.stretch), 's');
    dialog::add_action(edit_stretch);
    
    if(product_model(vpmodel)) {
      dialog::addSelItem(XLAT("product Z stretch"), fts(vpconf.product_z_scale), 'Z');
      dialog::add_action([] {
        dialog::editNumber(vpconf.product_z_scale, 0.1, 10, 0.1, 1, XLAT("product Z stretch"), "");        
        dialog::scaleLog();
        });
      }

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

  #if CAP_COMMANDLINE
  
  eModel read_model(const string& ss) {
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
  }

}