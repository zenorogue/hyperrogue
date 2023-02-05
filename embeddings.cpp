// Hyperbolic Rogue -- embeddings
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file embeddings.cpp
 *  \brief Embedding 2D geometries into 3D
 *
 *  This file handles primarily embedding 2D geometries into 3D.
 *
 *  The following coordinate systems are used for embedding of 2D geometries into 3D:
 *
 *  - *base* coordinates are simply the coordinate in the underlying 2D geometry. They support only two dimensions.
 *  - *logical* coordinates: X and Y are in the Beltrami-Klein or gnomonic model, or in horocyclic coordinates for binary-like tilings. Z coordinate is the altitude above the plane.
 *  - *logical_scaled* coordinates: X and Y are scaled (and possibly rotated in the XY plane) in order to match the scale and orientation of the ambient 3D geometry. They are a linear transformation of logical.
 *  - *intermediate* coordinates: they use the same assignment of coordinates as actual, but they are a linear transformation of logical scaled.
 *  - *actual* coordinates: final coordinates in the ambient 3D geometry.
 *
 */

#include "hyper.h"

namespace hr {

EX namespace geom3 {
  #if HDR
  enum eSpatialEmbedding {
    seNone,
    seDefault,
    seLowerCurvature,
    seMuchLowerCurvature,
    seProduct,
    seNil,
    seSol,
    seNIH,
    seSolN,
    seCliffordTorus,
    seProductH,
    seProductS,
    seSL2,
    seCylinderE,
    seCylinderH,
    seCylinderHE,
    seCylinderNil,
    seCylinderHoro,
    seCylinderSL2
    };
  #endif

  EX vector<pair<string, string>> spatial_embedding_options = {
    {"2D engine",       "Use HyperRogue's 2D engine to simulate same curvature. Works well in top-down and third-person perspective. The Hypersian Rug mode can be used to project this to a surface."},
    {"same curvature",  "Embed as an equidistant surface in the 3D version of the same geometry."},
    {"lower curvature", "Embed as a surface in a space of lower curvature."},
    {"much lower curvature", "Embed sphere as a sphere in hyperbolic space."},
    {"product",          "Add one extra dimension in the Euclidean way."},
    {"Nil",              "Embed Euclidean plane into Nil."},
    {"Sol",              "Embed Euclidean or hyperbolic plane into Sol."},
    {"stretched hyperbolic",              "Embed Euclidean or hyperbolic plane into stretched hyperbolic geometry."},
    {"stretched Sol",              "Embed Euclidean or hyperbolic plane into stretched Sol geometry."},
    {"Clifford Torus",              "Embed Euclidean rectangular torus into S3."},
    {"hyperbolic product", "Embed Euclidean or hyperbolic plane in the H2xR product space."},
    {"spherical product", "Embed Euclidean cylinder or spherical plane in the H2xR product space."},
    {"SL(2,R)",           "Embed Euclidean plane in twisted product geometry."},
    {"cylinder",          "Embed Euclidean cylinder in Euclidean space."},
    {"hyperbolic cylinder", "Embed Euclidean cylinder in hyperbolic space."},
    {"product cylinder", "Embed Euclidean cylinder in H2xR space."},
    {"Nil cylinder", "Embed Euclidean cylinder in Nil."},
    {"horocylinder", "Embed Euclidean as a horocylinder in H2xR space."},
    {"SL2 cylinder", "Embed Euclidean as a cylinder in twisted product geometry."},
    };

  EX bool clifford_torus_valid() {
    rug::clifford_torus ct;
    ld h = ct.xh | ct.yh;
    return !(sqhypot_d(2, ct.xh) < 1e-3 || sqhypot_d(2, ct.yh) < 1e-3 || abs(h) > 1e-3);
    }

  EX string why_wrong(eSpatialEmbedding sp) {
    string ans = "";
    if(among(sp, seNil, seCliffordTorus, seProductH, seProductS, seSL2) || any_cylinder(sp)) {
      if(!PURE)
        ans += " pure";
      if(!meuclid)
        ans += " E";
      if((sp == seProductS || any_cylinder(sp)) && !quotient)
        ans += " cyl";
      if(sp == seCliffordTorus && !clifford_torus_valid())
        ans += " torus";
      }
    if(among(sp, seSol, seNIH, seSolN)) {
      if((meuclid && !PURE) && !bt::in()) ans += " pure or bin";
      }
    return ans;
    }

  EX eSpatialEmbedding spatial_embedding = seDefault;
  EX ld euclid_embed_scale = 1;
  EX ld euclid_embed_scale_y = 1;
  EX ld euclid_embed_rotate = 0;
  EX bool auto_configure = true;
  EX bool flat_embedding = false;
  EX bool inverted_embedding = false;

  EX ld euclid_embed_scale_mean() { return euclid_embed_scale * sqrt(euclid_embed_scale_y); }
  EX void set_euclid_embed_scale(ld x) { euclid_embed_scale = x; euclid_embed_scale_y = 1; euclid_embed_rotate = 0; }

  EX bool supports_flat() { return among(spatial_embedding, seDefault, seProductH, seProductS); }
  EX bool supports_invert() { return among(spatial_embedding, seDefault, seLowerCurvature, seMuchLowerCurvature, seNil, seSol, seNIH, seSolN, seProductH, seProductS) || any_cylinder(spatial_embedding); }

  EX vector<geometryinfo> ginf_backup;

  EX eGeometryClass mgclass() {
    return (embedded_plane ? ginf_backup : ginf)[geometry].g.kind;
    }

  EX eGeometryClass ggclass() {
    return (flipped ? ginf_backup : ginf)[geometry].g.kind;
    }

  EX bool any_cylinder(eSpatialEmbedding e) {
    return among(e, seCylinderE, seCylinderH, seCylinderHE, seCylinderHoro, seCylinderNil, seCylinderSL2);
    }

  EX bool in_product() {
    return ggclass() == gcProduct;
    }

  EX bool flipped;
  
  EX geometry_information* unflipped;

  EX void light_flip(bool f) {
    if(f != flipped) {
      if(!flipped) unflipped = cgip;
      swap(ginf[geometry].g, geom3::ginf_backup[geometry].g);
      swap(ginf[geometry].flags, geom3::ginf_backup[geometry].flags);
      if(!flipped) cgip = unflipped;
      flipped = f;
      }
    }
  
  #if HDR
  template<class T> auto in_flipped(const T& f) -> decltype(f()) {
    light_flip(true);
    finalizer ff([] { light_flip(false); });
    return f();
    }

  template<class T> auto in_not_flipped(const T& f) -> decltype(f()) {
    light_flip(false);
    finalizer ff([] { light_flip(true); });
    return f();
    }

  #define IPF(x) geom3::in_flipped([&] { return (x); })
  #endif

  EX void apply_always3() {
    if(!vid.always3 && !ginf_backup.empty()) {
      ginf = ginf_backup;
      ginf_backup.clear();
      }
    if(vid.always3 && ginf_backup.empty()) {
      ginf_backup = ginf;
      for(geometryinfo& gi: ginf) {
        auto &g = gi.g;
        if(vid.always3 && g.gameplay_dimension == 2 && g.graphical_dimension == 2) {
          /* same-in-same by default */
          auto og = g;
          g.graphical_dimension++;
          g.homogeneous_dimension++;
          g.sig[3] = g.sig[2];
          g.sig[2] = g.sig[1];

          bool ieuclid = g.kind == gcEuclid;
          bool isphere = g.kind == gcSphere;
          bool ieuc_or_binary = ieuclid || (gi.flags & qBINARY);

          if(spatial_embedding == seProduct && !ieuclid) g = giProduct, g.sig[2] = og.sig[2];
          if(spatial_embedding == seProductH && ieuclid) g = giProductH;
          if(spatial_embedding == seProductS && ieuclid) g = giProductS;
          if(spatial_embedding == seLowerCurvature) g = (isphere ? giEuclid3 : giHyperb3);
          if(spatial_embedding == seMuchLowerCurvature) g = giHyperb3;
          if(spatial_embedding == seNil && ieuclid) g = giNil;
          if(spatial_embedding == seCliffordTorus && ieuclid) g = giSphere3;
          if(spatial_embedding == seSol && ieuc_or_binary) g = giSol;
          if(spatial_embedding == seNIH && ieuc_or_binary) g = giNIH;
          if(spatial_embedding == seSolN && ieuc_or_binary) g = giSolN;
          if(spatial_embedding == seSL2 && ieuclid) g = giSL2;
          if(spatial_embedding == seCylinderH && ieuclid) g = giHyperb3;
          if(spatial_embedding == seCylinderHE && ieuclid) g = giProductH;
          if(spatial_embedding == seCylinderHoro && ieuclid) g = giProductH;
          if(spatial_embedding == seCylinderNil && ieuclid) g = giNil;
          if(spatial_embedding == seCylinderSL2 && ieuclid) g = giSL2;
          
          g.gameplay_dimension = 2;
          }
        }
      }
    }

  EX void configure_clifford_torus() {
    rug::clifford_torus ct;

    if(hypot_d(2, ct.xh) < 1e-6 || hypot_d(2, ct.yh) < 1e-6) {
      euclid_embed_scale = TAU / 20.;
      euclid_embed_scale_y = 1;
      euclid_embed_rotate = 0;
      vid.depth = 45._deg - 1;
      vid.wall_height = 0.2;
      vid.eye = vid.wall_height / 2 - vid.depth;
      return;
      }

    euclid_embed_scale = TAU / hypot_d(2, ct.xh);
    euclid_embed_scale_y = TAU / hypot_d(2, ct.yh) / euclid_embed_scale;
    euclid_embed_rotate = atan2(ct.xh[1], ct.xh[0]) / degree;

    ld alpha = atan2(ct.xfactor, ct.yfactor);

    vid.depth = alpha - 1;
    vid.wall_height = min(1 / euclid_embed_scale_mean(), (90._deg - alpha) * 0.9);
    vid.eye = vid.wall_height / 2 - vid.depth;
    }

  EX void configure_cylinder() {
    rug::clifford_torus ct;
    hyperpoint vec;
    if(sqhypot_d(2, ct.yh) > 1e-6) vec = ct.yh;
    else if(sqhypot_d(2, ct.xh) > 1e-6) vec = ct.xh;
    else vec = hyperpoint(10, 0, 0, 0);
    
    euclid_embed_scale = TAU / hypot_d(2, vec);
    euclid_embed_scale_y = 1;
    euclid_embed_rotate = atan2(vec[1], vec[0]) / degree;
    }

EX }

  #if HDR
  struct embedding_method {
    virtual ld center_z() { return 0; }
    virtual hyperpoint tile_center();
    virtual transmatrix intermediate_to_actual_translation(hyperpoint i) = 0;
    virtual hyperpoint intermediate_to_actual(hyperpoint i) { return intermediate_to_actual_translation(i) * tile_center(); }
    virtual hyperpoint actual_to_intermediate(hyperpoint a) = 0;
    virtual hyperpoint orthogonal_move(const hyperpoint& a, ld z);
    virtual transmatrix map_relative_push(hyperpoint h);
    virtual ld get_logical_z(hyperpoint a) { return (intermediate_to_logical_scaled * actual_to_intermediate(a))[2]; }
    virtual hyperpoint logical_to_actual(hyperpoint l) { return intermediate_to_actual(logical_to_intermediate * l); }
    virtual hyperpoint actual_to_logical(hyperpoint a) { return intermediate_to_logical * actual_to_intermediate(a); }
    virtual hyperpoint base_to_actual(hyperpoint h) = 0;
    virtual transmatrix base_to_actual(const transmatrix &T) = 0;
    virtual hyperpoint actual_to_base(hyperpoint h) = 0;
    virtual transmatrix actual_to_base(const transmatrix &T) = 0;
    virtual hyperpoint normalize_flat(hyperpoint a) { return flatten(normalize(a)); }
    virtual hyperpoint flatten(hyperpoint a);
    virtual void set_radar_transform();
    virtual transmatrix get_lsti() { return Id; }
    virtual transmatrix get_lti() { return logical_scaled_to_intermediate; }
    virtual hyperpoint base_to_logical(hyperpoint h) = 0;
    virtual hyperpoint logical_to_base(hyperpoint h) = 0;
    virtual ld anim_center_z() { return center_z(); }
    virtual hyperpoint anim_tile_center();
    virtual void logical_fix(transmatrix&) = 0;
    
    virtual bool is_euc_in_product() { return false; }
    virtual bool is_product_embedding() { return false; }
    virtual bool is_euc_in_sl2() { return false; }
    virtual bool is_same_in_same() { return false; }
    virtual bool is_sph_in_low() { return false; }
    virtual bool is_hyp_in_solnih() { return false; }
    virtual bool is_euc_in_hyp() { return false; }
    virtual bool is_euc_in_sph() { return false; }
    virtual bool is_euc_in_nil() { return false; }
    virtual bool is_euc_in_noniso() { return false; }
    virtual bool is_in_noniso() { return false; }
    virtual bool is_depth_limited() { return false; }
    virtual bool is_cylinder() { return false; }
    virtual bool no_spin() { return false; }

    /* convert the tangent space in logical coordinates to actual coordinates */
    transmatrix logical_to_intermediate;
  
    /* convert the tangent space in actual coordinates to logical coordinates */
    transmatrix intermediate_to_logical;
  
    /* convert the tangent space in logical coordinates to actual coordinates */
    transmatrix logical_scaled_to_intermediate;
  
    /* convert the tangent space in actual coordinates to logical coordinates */
    transmatrix intermediate_to_logical_scaled;
    
    void prepare_lta();
    void auto_configure();
    };

  #endif

EX geometry_information *swapper;

hyperpoint embedding_method::tile_center() {
  ld z = center_z();
  if(z == 0) return C0;
  return lzpush(z) * C0;
  }

hyperpoint embedding_method::anim_tile_center() {
  ld z = anim_center_z();
  if(z == 0) return C0;
  return lzpush(z) * C0;
  }

transmatrix embedding_method::map_relative_push(hyperpoint a) {
  auto i = actual_to_intermediate(a);
  return intermediate_to_actual_translation(i);  
  }

hyperpoint embedding_method::orthogonal_move(const hyperpoint& a, ld z) {
  auto i = actual_to_intermediate(a);
  auto l = intermediate_to_logical_scaled * i;
  l[2] += z;
  i = logical_scaled_to_intermediate * l;
  return intermediate_to_actual(i);
  }

hyperpoint embedding_method::flatten(hyperpoint a) {
  auto i = actual_to_intermediate(a);
  auto l = intermediate_to_logical * i;
  l[2] = 0; i = logical_to_intermediate * l;
  return intermediate_to_actual(i);
  }

/** dummy 'embedding method' used when no embedding is used (2D engine or 3D map) */

struct emb_none : embedding_method {
  hyperpoint actual_to_intermediate(hyperpoint a) override {
    if(mhybrid) return base_to_logical(a);
    return a;
    }
  hyperpoint intermediate_to_actual(hyperpoint i) override {
    if(mhybrid) return logical_to_base(i);
    return i;
    }
  transmatrix intermediate_to_actual_translation(hyperpoint i) override {
    if(gproduct) i = intermediate_to_actual(i);
    return rgpushxto0(i);
    }
  hyperpoint flatten(hyperpoint a) override {
    if(gproduct) return a / exp(zlevel(a));
    return embedding_method::flatten(a);
    }

  transmatrix base_to_actual(const transmatrix& T) override { return T; }
  hyperpoint base_to_actual(hyperpoint h) override { return h; }
  transmatrix actual_to_base(const transmatrix& T) override { return T; }
  hyperpoint actual_to_base(hyperpoint h) override { return h; }
  hyperpoint orthogonal_move(const hyperpoint& h, ld z) override {
    if(GDIM == 2) return scale_point(h, geom3::scale_at_lev(z));
    if(gproduct) return scale_point(h, exp(z));
    if(sl2) return slr::translate(h) * cpush0(2, z);
    if(nil) return nisot::translate(h) * cpush0(2, z);
    if(translatable) return hpxy3(h[0], h[1], h[2] + z);
    /* copied from emb_same_in_same */
    ld u = 1;
    if(h[2]) z += asin_auto(h[2]), u /= cos_auto(asin_auto(h[2]));
    u *= cos_auto(z);
    return hpxy3(h[0] * u, h[1] * u, sinh(z));
    }
  hyperpoint base_to_logical(hyperpoint h) override {
    if(sn::in() || !bt::in())
      return h;
    #if CAP_BT
    if(bt::in() && !mproduct) return bt::minkowski_to_bt(h);
    #endif
    if(mproduct) {
      ld z = zlevel(h);
      h /= h[2];
      h[2] = z;
      }
    if(sl2) {
      ld z = atan2(h[2], h[3]);
      h = slr::translate(h) * zpush0(-atan2(h[2], h[3]));
      h[0] = h[0] / h[3]; h[1] = h[1] / h[3]; h[2] = z;
      return h;
      }
    return h;
    }
  hyperpoint logical_to_base(hyperpoint h) override {
    if(sn::in() || !bt::in())
      return ultra_normalize(h);
    #if CAP_BT
    if(bt::in() && !mproduct)
      return bt::bt_to_minkowski(h);
    #endif
    if(mproduct) {
      ld z = h[2];
      h[2] = 1;
      flatten(h);
      h *= exp(z);
      }
    if(sl2) {
      ld z = h[2];
      h[2] = 0; h[3] = 1; normalize(h);
      h = slr::translate(h) * zpush0(z);
      return h;
      }
    return h;
    }

  void logical_fix(transmatrix& T) override {
    if(nonisotropic) {
      hyperpoint h = tC0(T);
      transmatrix rot = gpushxto0(h) * T;
      fix_rotation(rot);
      T = rgpushxto0(h) * rot;
      }
    else fixmatrix(T);
    fixelliptic(T);
    }
  };

/** embeddings methods that are not emb_none */

struct emb_actual : embedding_method {

  hyperpoint base_to_logical(hyperpoint h) override {
    if(bt::in()) {
      auto h1 = bt::inverse_horopoint(h);
      h1[2] = 0; h1[3] = 1;
      return h1;
      }
    h /= h[2];
    h[2] = 0; h[3] = 1;
    return h;
    }

  hyperpoint logical_to_base(hyperpoint h) override {
    if(bt::in()) {
      auto h1 = bt::get_horopoint(h);
      h1[3] = 1;
      return h1;
      }
    h[2] = 1; h = normalize(h);
    h[3] = 1;
    return h;
    }

  void logical_fix(transmatrix& T) override {
    hyperpoint a = T * tile_center();
    hyperpoint i0 = actual_to_intermediate(a);
    auto l0 = intermediate_to_logical * i0;
    auto l = l0; l[2] = 0;
    auto i = logical_to_intermediate * l;
    auto rot0= inverse(intermediate_to_actual_translation(i0)) * T ;
    auto rot = intermediate_to_logical_scaled * rot0 * logical_scaled_to_intermediate;
    ld alpha = atan2(rot[0][1], rot[0][0]);
    T = intermediate_to_actual_translation(i) * spin(alpha);
    fixelliptic(T);
    }
  };

/** embed in the 3D variant of the same geometry */

struct emb_same_in_same : emb_actual {
  virtual bool is_same_in_same() override { return true; }
  transmatrix intermediate_to_actual_translation(hyperpoint i) override { return rgpushxto0(logical_to_actual(i)); }
  hyperpoint actual_to_intermediate(hyperpoint a) override { return actual_to_logical(a); }
  hyperpoint orthogonal_move(const hyperpoint& h, ld z) override {
    ld u = 1;
    if(h[2]) z += asin_auto(h[2]), u /= cos_auto(asin_auto(h[2]));
    u *= cos_auto(z);
    return hpxy3(h[0] * u, h[1] * u, sinh(z));
    }
  transmatrix base_to_actual(const transmatrix &T0) override {
    auto T = T0;
    for(int i=0; i<4; i++) T[i][3] = T[i][2], T[i][2] = 0;
    for(int i=0; i<4; i++) T[3][i] = T[2][i], T[i][2] = 0;
    for(int i=0; i<4; i++) T[i][2] = T[2][i] = 0;
    T[2][2] = 1;
    return T;
    }
  transmatrix actual_to_base(const transmatrix &T0) override {
    auto T = T0;
    for(int i=0; i<4; i++) T[i][2] = T[i][3], T[i][3] = 0;
    for(int i=0; i<4; i++) T[2][i] = T[3][i], T[i][3] = 0;
    T[3][3] = 1;
    fixmatrix(T);
    for(int i=0; i<MDIM; i++) for(int j=0; j<MDIM; j++) if(isnan(T[i][j])) return Id;
    return T;
    }    
  hyperpoint base_to_actual(hyperpoint h) override {
    h[3] = h[2]; h[2] = 0;
    return h;
    }
  hyperpoint actual_to_base(hyperpoint h) override {
    h[2] = h[3]; h[3] = 0;
    return h;
    }
  transmatrix map_relative_push(hyperpoint h) override {
    ld z = asin_auto(h[2]);
    ld u = 1 / cos_auto(z);
    auto h1 = hpxy3(h[0] * u, h[1] * u, 0);
    return rgpushxto0(h1) * zpush(z);
    }

  hyperpoint actual_to_logical(hyperpoint h) override {
    ld z = asin_auto(h[2]);
    ld u = 1 / cos_auto(z);
    auto h1 = hpxy3(h[0] * u, h[1] * u, 0);
    h1[2] = h1[3];
    geom3::light_flip(true);
    h1 = base_to_logical(h1);
    geom3::light_flip(false);
    return h1;
    }

  hyperpoint logical_to_actual(hyperpoint h) override {
    geom3::light_flip(true);
    auto b = logical_to_base(h);
    geom3::light_flip(false);
    b[3] = b[2]; b[2] = 0;
    return orthogonal_move(b, h[2]);
    }

  hyperpoint flatten(hyperpoint h) override {
    ld z = asin_auto(h[2]);
    ld u = 1 / cos_auto(z);
    return hpxy3(h[0] * u, h[1] * u, 0);
    }

  void logical_fix(transmatrix& T) override {
    // optimization
    for(int i=0; i<4; i++) T[i][2] = T[2][i] = i == 2;
    fixmatrix(T);
    fixelliptic(T);
    }
  };

/** embed in the product geometry */

struct emb_product_embedding : emb_actual {
  virtual bool is_product_embedding() override { return true; }
  transmatrix intermediate_to_actual_translation(hyperpoint i) override { return rgpushxto0(logical_to_actual(i)); }
  hyperpoint actual_to_intermediate(hyperpoint a) override { return a; }
  hyperpoint flatten(hyperpoint h) override { h /= exp(zlevel(h)); return h; }
  hyperpoint orthogonal_move(const hyperpoint& h, ld z) override { return h * exp(z); }
  transmatrix base_to_actual(const transmatrix &T) override { return T; }
  transmatrix actual_to_base(const transmatrix &T0) override {
    auto T = T0; fixmatrix(T);
    for(int i=0; i<MDIM; i++) for(int j=0; j<MDIM; j++) if(isnan(T[i][j])) return Id;
    return T;
    }    
  hyperpoint base_to_actual(hyperpoint h) override { return h; }
  hyperpoint actual_to_base(hyperpoint h) override { return flatten(h); }
  transmatrix map_relative_push(hyperpoint h) override { return rgpushxto0(h); }

  hyperpoint actual_to_logical(hyperpoint h) override {
    ld z = zlevel(h);
    h /= exp(z);
    h = base_to_logical(h);
    h[2] = z;
    return h;
    }

  hyperpoint logical_to_actual(hyperpoint h) override {
    return logical_to_base(h) * exp(h[2]);
    }
  };

/** embed Euclidean plane as horosphere */

struct emb_euc_in_hyp : emb_actual {
  bool is_euc_in_hyp() override { return true; }
  hyperpoint actual_to_intermediate(hyperpoint a) override { return deparabolic13(a); }
  transmatrix intermediate_to_actual_translation(hyperpoint i) override { return parabolic13_at(i); }
  transmatrix base_to_actual(const transmatrix &T) override {
    geom3::light_flip(true);
    hyperpoint mov = T * C02;
    transmatrix U = gpushxto0(mov) * T;
    geom3::light_flip(false);
    for(int i=0; i<4; i++) U[i][3] = U[3][i] = i == 3;
    return parabolic13(mov[0], mov[1]) * U;
    }
  hyperpoint base_to_actual(hyperpoint h) override {
    h[3] = h[2]; h[2] = 0; return parabolic13(h[0], h[1]) * C0;
    }
  hyperpoint actual_to_base(hyperpoint h) override { return deparabolic13(h); }
  transmatrix actual_to_base(const transmatrix& T) override { hyperpoint h = deparabolic13(T * C0); return eupush(h[0], h[1]); }
  ld anim_center_z() override { return vid.depth; }
  };

/** sphere into a isotropic space of higher curvature */

struct emb_sphere_in_low : emb_actual {
  bool is_sph_in_low() override { return true; }
  bool is_depth_limited() override { return true; }
  transmatrix intermediate_to_actual_translation(hyperpoint i) override {
    return map_relative_push(logical_to_actual(i)) * zpush(-1);
    }
  hyperpoint actual_to_intermediate(hyperpoint a) override { return actual_to_logical(a); }
  ld center_z() { return 1; }
  transmatrix map_relative_push(hyperpoint a) {
    ld z = hdist0(a);
    geom3::light_flip(true);
    auto h1 = normalize(a);
    transmatrix T = rgpushxto0(h1);
    geom3::light_flip(false);
    return T * zpush(z);
    }
  transmatrix base_to_actual(const transmatrix &T0) override {
    auto T = T0;
    for(int i=0; i<4; i++) T[i][3] = T[3][i] = i == 3;
    return T;
    }
  hyperpoint base_to_actual(hyperpoint h) override {
    if(euclid) h[3] = 1;
    else h *= sinh(1), h[3] = cosh(1);
    return h;
    }
  hyperpoint actual_to_base(hyperpoint h) override { return h; }
  transmatrix actual_to_base(const transmatrix& T) override { return T; }
  ld get_logical_z(hyperpoint a) override { return hdist0(a) - 1; }
  hyperpoint flatten(hyperpoint a) override {
    ld d = hdist0(a);
    if(d == 0) return a;
    a *= sin_auto(1) / sin_auto(d);
    a[3] = cos_auto(1);
    return a;
    }
  hyperpoint orthogonal_move(const hyperpoint& h, ld z) override {
    ld z0 = hdist0(h);
    ld f = sin_auto(z0 + z) / sin_auto(z0);
    hyperpoint hf = h * f;
    hf[3] = cos_auto(z0 + z);
    return hf;
    }
  hyperpoint logical_to_actual(hyperpoint h) override {
    auto z = h[2];
    h[2] = 1;
    geom3::light_flip(true);
    h = normalize(h);
    geom3::light_flip(false);
    h *= sin_auto(1 + z);
    h[3] = cos_auto(1 + z);
    return h;
    }
  hyperpoint actual_to_logical(hyperpoint h) override {
    ld z = get_logical_z(h);
    geom3::light_flip(true);
    h = kleinize(h);
    geom3::light_flip(false);
    h[2] = z; h[3] = 1;
    return h;
    }

  void logical_fix(transmatrix& T) override {
    fix4(T);
    fixmatrix(T);
    fixelliptic(T);
    }
  };

/** abstract class for embeddings of Euclidean plane; these embeddings are not isotropic */

struct emb_euclid_noniso : emb_actual {
  bool is_euc_in_noniso() override { return true; }
  bool is_in_noniso() override { return true; }
  transmatrix base_to_actual(const transmatrix &T) override {
    auto T0 = T;
    hyperpoint h = get_column(T0, 2);
    h[2] = 0; h[3] = 1;
    return intermediate_to_actual_translation( logical_to_intermediate * h);
    }
  hyperpoint base_to_actual(hyperpoint h) override {
    h[2] = 0; h[3] = 1;
    return intermediate_to_actual_translation( logical_to_intermediate * h ) * tile_center();
    }
  hyperpoint actual_to_base(hyperpoint h) override {
    hyperpoint h1 = intermediate_to_logical * actual_to_intermediate(h);
    h1[2] = 1; h1[3] = 0;
    return h1;
    }
  transmatrix actual_to_base(const transmatrix& T) override { hyperpoint h = actual_to_base(T * tile_center()); return eupush(h[0], h[1]);  }

  transmatrix get_lti() override {
    transmatrix lti = Id;
    lti[0][0] *= geom3::euclid_embed_scale;
    lti[1][1] *= geom3::euclid_embed_scale * geom3::euclid_embed_scale_y;
    return logical_scaled_to_intermediate * cspin(0, 1, geom3::euclid_embed_rotate * degree) * lti;
    }
  };

struct emb_euc_in_product : emb_euclid_noniso {
  bool is_euc_in_product() override { return true; }
  bool no_spin() override { return true; }
  hyperpoint actual_to_intermediate(hyperpoint a) override {
    ld bz = zlevel(a);
    auto h1 = a / exp(bz);
    ld by = asin_auto(h1[1]);
    ld bx = atan2_auto(h1[0], h1[2]);
    return hyperpoint(bx, by, bz, 1);
    }
  transmatrix get_lsti() override { return cspin90(2, 1); }
  transmatrix intermediate_to_actual_translation(hyperpoint i) {
    return zpush(i[2]) * xpush(i[0]) * ypush(i[1]);
    }
  };

struct emb_euc_in_sl2 : emb_euclid_noniso {

  transmatrix esl2_zpush(ld z) { return cspin(2, 3, z) * cspin(0, 1, z); }

  hyperpoint intermediate_to_actual(hyperpoint i) override {
    return esl2_zpush(i[2]) * xpush(i[0]) * ypush0(i[1]);
    }

  transmatrix intermediate_to_actual_translation(hyperpoint i) override {
    return esl2_zpush(i[2]) * xpush(i[0]) * ypush(i[1]);
    }

  hyperpoint actual_to_intermediate(hyperpoint h) override {
    ld a1 = (h[0] * h[3] - h[1] * h[2]) / (-h[2] * h[2] - h[1] * h[1] -h[0] * h[0] - h[3] * h[3]);
    // a1 is S*sqrt(1+S*S) / (1+2*S*S), where S = sinh(-x) and C = cosh(-x); U is S*S
    ld a = a1 * a1;
    ld b = 4 * a - 1;
    ld U = sqrt(.25 - a/b) - .5;
    ld S = sqrt(U) * (a1 > 0 ? 1 : -1);
    ld x = -asinh(S);
    h = lorentz(0, 3, -x) * lorentz(1, 2, x) * h;
    ld y = h[3]*h[3] > h[2]*h[2] ? atanh(h[1] / h[3]) : atanh(h[0] / h[2]);
    h = lorentz(0, 2, -y) * lorentz(1, 3, -y) * h;
    ld z = atan2(h[2], h[3]);
    return hyperpoint(x, y, z, 0);
    }

  bool is_euc_in_sl2() override { return true; }
  bool no_spin() override { return true; }
  transmatrix get_lsti() override { return cspin90(2, 1); }
  };

/* for both seCylinderH and seCylinderE. Possibly actually works for CliffordTorus too */
struct emb_euc_cylinder : emb_euclid_noniso {
  bool is_cylinder() override { return true; }
  ld center_z() override { return 1; }
  bool is_depth_limited() override { return true; }
  transmatrix get_lsti() override { return cspin90(0, 1); }
  hyperpoint actual_to_intermediate(hyperpoint a) override { 
    ld z0 = asin_auto(hypot(a[1], a[2]));
    ld x0 = a[0];
    if(z0 == 0) return hyperpoint(x0, 0, 0, 1);
    x0 = asin_auto(x0 / cos_auto(z0));
    ld y0 = z0 ? atan2(a[1], a[2]) : 0;
    return point31(x0, y0, z0-1);
    }
  transmatrix intermediate_to_actual_translation(hyperpoint i) override {
    return xpush(i[0]) * cspin(1, 2, i[1]) * zpush(i[2]);
    }
  };

struct emb_euc_cylinder_he : emb_euc_cylinder {
  bool no_spin() override { return true; }
  transmatrix get_lsti() override { return cspin90(0, 2) * cspin90(0, 1); }
  hyperpoint actual_to_intermediate(hyperpoint a) override {
    ld z0 = zlevel(a);
    a /= exp(z0);
    ld y0 = atan2(a[1], a[0]);
    ld x0 = asin_auto(hypot(a[0], a[1]));
    return hyperpoint(x0-1, y0, z0, 1);
    }
  transmatrix intermediate_to_actual_translation(hyperpoint i) override {
    return zpush(i[2]) * cspin(1, 0, i[1]) * xpush(i[0]);
    }
  };

struct emb_euc_cylinder_nil : emb_euc_cylinder {
  transmatrix get_lsti() override { return cspin90(0, 2) * cspin90(0, 1); }
  hyperpoint actual_to_intermediate(hyperpoint a) override {
    ld y0 = atan2(a[1], a[0]);
    ld x0 = hypot(a[0], a[1]);
    return hyperpoint(x0-1, y0, a[2], 1);
    }
  transmatrix intermediate_to_actual_translation(hyperpoint i) override {
    return zpush(i[2]) * cspin(1, 0, i[1]) * xpush(i[0]);
    }
  transmatrix get_lti() override {
    ld depth = 0; // for now?
    ld alpha = (1 + depth) / 2.;
    ld c = pow(1 + alpha * alpha, -0.5);
    transmatrix U = Id;
    U[1][1] = (alpha*alpha+1) * c;
    U[0][1] = alpha * c;
    return logical_scaled_to_intermediate * U * intermediate_to_logical_scaled * emb_euc_cylinder::get_lti();
    }
  };

struct emb_euc_cylinder_horo : emb_euc_cylinder {
  ld center_z() override { return 0; }
  bool no_spin() override { return true; }
  hyperpoint actual_to_intermediate(hyperpoint a) override {
    ld z0 = zlevel(a);
    a /= exp(z0);
    auto hy = deparabolic13(a);
    hy[2] = z0;
    return hy;
    }
  transmatrix intermediate_to_actual_translation(hyperpoint i) override {
    return zpush(i[2]) * parabolic1(i[1]) * xpush(i[0]);
    }
  transmatrix get_lsti() override {
    return cspin90(0, 2);
    }
  };

struct emb_euc_cylinder_sl2 : emb_euc_cylinder {
  bool no_spin() override { return true; }
  hyperpoint actual_to_intermediate(hyperpoint a) override {
    hyperpoint i = point31(0, 0, 0);
    i[2] = atan2(a[2], a[3]);
    a = cspin(1, 0, i[2]) * cspin(3, 2, i[2]) * a;
    i[1] = (a[0] || a[1]) ? atan2(a[0], a[1]) : 0;
    a = cspin(1, 0, i[1]) * a;
    i[0] = asinh(a[0]);
    return i;
    }
  transmatrix intermediate_to_actual_translation(hyperpoint i) override {
    return cspin(2, 3, i[2]) * cspin(0, 1, i[2] + i[1]) * xpush(i[0]);
    }
  transmatrix get_lsti() override {
    return cspin90(0, 2) * cspin90(0, 1);
    }
  };

struct emb_euc_in_sph : emb_euclid_noniso {
  bool is_euc_in_sph() override { return true; }
  ld center_z() override { return 1; }
  hyperpoint actual_to_intermediate(hyperpoint a) override { 
    ld tx = hypot(a[0], a[2]);
    ld ty = hypot(a[1], a[3]);
    ld x0 = atan2(a[0], a[2]);
    ld y0 = atan2(a[1], a[3]);
    ld z0 = atan2(tx, ty);
    return hyperpoint(x0, y0, z0-1, 1);
    }
  transmatrix intermediate_to_actual_translation(hyperpoint i) override {
    return cspin(0, 2, i[0]) * cspin(1, 3, i[1]) * cspin(2, 3, i[2]);
    }
  };

struct emb_euc_in_nil : emb_euclid_noniso {
  bool is_euc_in_nil() override { return true; }
  hyperpoint actual_to_intermediate(hyperpoint a) override { a[2] -= a[0] * a[1]; return a; }
  transmatrix intermediate_to_actual_translation(hyperpoint i) override { i[2] += i[0] * i[1]; return rgpushxto0(i); }
  transmatrix get_lsti() override { return cspin90(2, 1); }
  };

struct emb_euc_in_solnih : emb_euclid_noniso {
  hyperpoint actual_to_intermediate(hyperpoint a) override { return a; }
  transmatrix intermediate_to_actual_translation(hyperpoint i) override { return rgpushxto0(i); }
  };

struct emb_hyp_in_solnih : emb_actual {
  bool is_hyp_in_solnih() override { return true; }
  bool is_in_noniso() override { return true; }
  transmatrix intermediate_to_actual_translation(hyperpoint i) override {
    if(cgclass == gcSol) i[0] *= exp(-i[2]);
    if(cgclass == gcSolN) i[0] *= pow(2, -i[2]);
    if(cgclass == gcNIH) i[0] *= pow(2, i[2]);
    return rgpushxto0(i);
    }
  hyperpoint actual_to_intermediate(hyperpoint a) override {
    if(cgclass == gcSol) a[0] *= exp(a[2]);
    if(cgclass == gcSolN) a[0] *= pow(2, a[2]);
    if(cgclass == gcNIH) a[0] *= pow(2, -a[2]);
    return a;
    }
  transmatrix base_to_actual(const transmatrix &T) override {
    auto T1 = T;
    auto h = get_column(T1, 2);
    return rgpushxto0(base_to_actual(h));
    }
  hyperpoint base_to_actual(hyperpoint h) override { 
    // copied from deparabolic13
    h /= (1 + h[2]);
    h[0] -= 1;
    h /= sqhypot_d(2, h);
    h[0] += .5;
    ld hx = log(2) + log(-h[0]);
    if(cgclass == gcNIH) hx /= log(3);
    if(cgclass == gcSolN) hx /= log(3);
    ld hy = h[1] * 2;
    return point31(0, -hy, hx);
    }
  transmatrix actual_to_base(const transmatrix& T) override {
    hyperpoint h = T * C0;
    auto f = geom3::flipped;
    geom3::light_flip(true);
    transmatrix b = parabolic1(h[1]) * xpush(h[2]);
    geom3::light_flip(f);
    return b;
    }
  hyperpoint actual_to_base(hyperpoint h) override {
    auto f = geom3::flipped;
    geom3::light_flip(true);
    hyperpoint b = parabolic1(h[1]) * xpush0(h[2]);
    geom3::light_flip(f);
    return b;
    }
  transmatrix get_lsti() override { return cspin90(0, 1) * cspin90(1, 2) * cspin90(0, 1); }
  hyperpoint orthogonal_move(const hyperpoint& a, ld z) override { return nisot::translate(a) * cpush0(0, z); }
  };

/* the remaining methods */
/*=======================*/

void embedding_method::prepare_lta() {
  bool b = geom3::flipped;
  if(b) geom3::light_flip(false);
  
  logical_scaled_to_intermediate = get_lsti();
  intermediate_to_logical_scaled = inverse(logical_scaled_to_intermediate);
  logical_to_intermediate = get_lti();
  intermediate_to_logical = inverse(logical_to_intermediate);
  if(MDIM == 3) {
    // just in case
    for(int i=0; i<4; i++)
      intermediate_to_logical_scaled[i][3] = intermediate_to_logical_scaled[3][i] = intermediate_to_logical[3][i] = intermediate_to_logical[i][3] = i == 3;
    }
  if(b) geom3::light_flip(true);
  }

/** pick the embedding_method for the current setting */
EX unique_ptr<embedding_method> make_embed() {
  
  embedding_method *emb1;
  using namespace geom3;
  
  if(!embedded_plane)
    emb1 = new emb_none;
  else if(any_cylinder(spatial_embedding) && mgclass() == gcEuclid)
    emb1 = spatial_embedding == seCylinderHE ? new emb_euc_cylinder_he :
           spatial_embedding == seCylinderHoro ? new emb_euc_cylinder_horo :
           spatial_embedding == seCylinderNil ? new emb_euc_cylinder_nil :
           spatial_embedding == seCylinderSL2 ? new emb_euc_cylinder_sl2 :
           new emb_euc_cylinder;
  else if(mgclass() == ggclass())
    emb1 = new emb_same_in_same;
  else if(mgclass() == gcSphere && among(ggclass(), gcHyperbolic, gcEuclid))
    emb1 = new emb_sphere_in_low;
  else if(mgclass() == gcEuclid && ggclass() == gcSphere)
    emb1 = new emb_euc_in_sph;
  else if(mgclass() == gcEuclid && ggclass() == gcSL2)
    emb1 = new emb_euc_in_sl2;
  else if(mgclass() == gcHyperbolic && among(ggclass(), gcSol, gcNIH, gcSolN))
    emb1 = new emb_hyp_in_solnih;
  else if(mgclass() == gcEuclid && ggclass() == gcProduct)
    emb1 = new emb_euc_in_product;
  else if(ggclass() == gcProduct)
    emb1 = new emb_product_embedding;
  else if(mgclass() == gcEuclid && ggclass() == gcNil)
    emb1 = new emb_euc_in_nil;
  else if(mgclass() == gcEuclid && ggclass() == gcHyperbolic)
    emb1 = new emb_euc_in_hyp;
  else if(mgclass() == gcEuclid && among(ggclass(), gcSol, gcNIH, gcSolN))
    emb1 = new emb_euc_in_solnih;
  else
    throw hr_exception("unknown embedding");
  
  unique_ptr<embedding_method> emb(emb1);
  
  emb->prepare_lta();
  return emb;
  }

EX hyperpoint orthogonal_move(hyperpoint h, ld z ) { return cgi.emb->orthogonal_move(h, z); }

EX transmatrix unswap_spin(transmatrix T) {
  return cgi.emb->intermediate_to_logical_scaled * T * cgi.emb->logical_scaled_to_intermediate;
  }

/** rotate by alpha degrees in the XY plane */
EX transmatrix spin(ld alpha) {
  if(cgi.emb->no_spin()) return Id;
  return cgi.emb->logical_scaled_to_intermediate * cspin(0, 1, alpha) * cgi.emb->intermediate_to_logical_scaled;
  }

/** rotate by 90 degrees in the XY plane */
EX transmatrix spin90() {
  if(cgi.emb->no_spin()) return Id;
  return cgi.emb->logical_scaled_to_intermediate * cspin90(0, 1) * cgi.emb->intermediate_to_logical_scaled;
  }

/** rotate by 180 degrees in the XY plane */
EX transmatrix spin180() {
  if(cgi.emb->no_spin()) return Id;
  return cgi.emb->logical_scaled_to_intermediate * cspin180(0, 1) * cgi.emb->intermediate_to_logical_scaled;
  }

/** rotate by 270 degrees in the XY plane */
EX transmatrix spin270() {
  if(cgi.emb->no_spin()) return Id;
  return cgi.emb->logical_scaled_to_intermediate * cspin90(1, 0) * cgi.emb->intermediate_to_logical_scaled;
  }

EX transmatrix lzpush(ld z) {
  auto &lti = cgi.emb->logical_scaled_to_intermediate;
  if(lti[0][2]) return cpush(0, lti[0][2] * z);
  if(lti[1][2]) return cpush(1, lti[1][2] * z);
  return cpush(2, z);
  }

EX transmatrix lxpush(ld alpha) {
  if(embedded_plane) {
    geom3::light_flip(true);
    auto t = cpush(0, alpha);
    geom3::light_flip(false);
    return cgi.emb->base_to_actual(t);
    }
  return cpush(0, alpha);
  }

EX hyperpoint lxpush0(ld x) { return lxpush(x) * tile_center(); }

EX transmatrix lspintox(const hyperpoint& H) {
  if(cgi.emb->no_spin()) return Id;
  if(embedded_plane) {
    hyperpoint H1 = cgi.emb->intermediate_to_logical_scaled * H;
    return cgi.emb->logical_scaled_to_intermediate * spintoc(H1, 0, 1) * cgi.emb->intermediate_to_logical_scaled;
    }
  if(WDIM == 2 || gproduct) return spintoc(H, 0, 1);
  transmatrix T1 = spintoc(H, 0, 1);
  return spintoc(T1*H, 0, 2) * T1;
  }

EX transmatrix lrspintox(const hyperpoint& H) {
  if(cgi.emb->no_spin()) return Id;
  if(embedded_plane) {
    hyperpoint H1 = cgi.emb->intermediate_to_logical_scaled * H;
    return cgi.emb->logical_scaled_to_intermediate * rspintoc(H1, 0, 1) * cgi.emb->intermediate_to_logical_scaled;
    }
  if(WDIM == 2 || gproduct) return rspintoc(H, 0, 1);
  transmatrix T1 = spintoc(H, 0, 1);
  return rspintoc(H, 0, 1) * rspintoc(T1*H, 0, 2);
  }

/** tangent vector in logical direction Z */
EX hyperpoint lztangent(ld z) {
  return cgi.emb->logical_to_intermediate * ctangent(2, z);
  }

EX hyperpoint tile_center() { return cgi.emb->tile_center(); }

EX hyperpoint lspinpush0(ld alpha, ld x) {
  bool f = embedded_plane;
  if(f) geom3::light_flip(true);
  if(embedded_plane) throw hr_exception("still embedded plane");
  hyperpoint h = xspinpush0(alpha, x);
  if(f) geom3::light_flip(false);
  if(f) return cgi.emb->base_to_actual(h);
  return h;
  }

EX hyperpoint xspinpush0(ld alpha, ld x) {
  if(embedded_plane) return lspinpush0(alpha, x);
  if(sl2) return slr::polar(x, -alpha, 0);
  hyperpoint h = Hypc;
  h[LDIM] = cos_auto(x);
  h[0] = sin_auto(x) * cos(alpha);
  h[1] = sin_auto(x) * -sin(alpha);
  return h;
  }

EX transmatrix xspinpush(ld dir, ld dist) {
  if(embedded_plane) {
    geom3::light_flip(true);
    transmatrix T = cspin(0, 1, dir) * xpush(dist) * cspin(0, 1, -dir);
    geom3::light_flip(false);
    return cgi.emb->base_to_actual(T);
    }
  else if(euclid)
    return eupush(cos(dir) * dist, -sin(dir) * dist);
  else
    return spin(dir) * xpush(dist) * spin(-dir);
  }

EX const transmatrix& lmirror() {
  if(cgi.emb->is_euc_in_product()) return Id;
  if(cgi.emb->is_cylinder() && nil) return Id;
  if(cgi.emb->logical_to_intermediate[2][1]) return MirrorZ;
  if(cgi.emb->is_hyp_in_solnih()) return MirrorY;
  return Mirror;
  }

void embedding_method::set_radar_transform() {
  auto& rt = current_display->radar_transform;
  auto& rtp = current_display->radar_transform_post;

  if(!embedded_plane) { rt = rtp = Id; return; }
  transmatrix U = actual_view_transform * View;
  auto a = inverse(U) * C0;
  auto l = actual_to_intermediate(a);
  l = intermediate_to_logical * l;
  auto l0 = l;
  l[2] = 0;
  l = logical_to_intermediate * l;
  rt = inverse(intermediate_to_actual_translation(l)) * inverse(U);
  transmatrix T = View * intermediate_to_actual_translation(logical_to_intermediate * l0);
  if(gproduct) T = NLP * T;
  T = intermediate_to_logical_scaled * T * logical_scaled_to_intermediate;
  if(cgi.emb->is_euc_in_noniso()) T = cspin(1, 0, geom3::euclid_embed_rotate * degree) * T;
  if(cgi.emb->is_hyp_in_solnih()) T = T * MirrorY;
  rtp = cspin(0, 1, atan2(T[0][1], T[0][0]));
  if(cgi.emb->is_hyp_in_solnih()) rtp = MirrorX * cspin90(0, 1) * rtp;
  }

EX void swapmatrix(transmatrix& T) {
  if(embedded_plane) T = swapper->emb->base_to_actual(T);
  else T = swapper->emb->actual_to_base(T);
  }

EX void swappoint(hyperpoint& h) {
  if(embedded_plane) h = swapper->emb->base_to_actual(h);
  else h = swapper->emb->actual_to_base(h);
  }

void embedding_method::auto_configure() {
  using namespace geom3;
  ld ms = min<ld>(cgi.scalefactor, 1);
  vid.depth = ms;
  vid.wall_height = 1.5 * ms;
  if(sphere && msphere) {
    vid.depth = 30 * degree;
    vid.wall_height = 60 * degree;
    }
  vid.human_wall_ratio = 0.8;
  if(mgclass() == gcEuclid && allowIncreasedSight() && vid.use_smart_range == 0) {
    genrange_bonus = gamerange_bonus = sightrange_bonus = cgi.base_distlimit * 3/2;
    }
  vid.camera = 0;
  vid.eye = 0;
  if(is_sph_in_low() || is_cylinder()) {
    vid.depth = 0;
    vid.wall_height = -1;
    vid.eye = -0.5;
    if(inverted_embedding) {
      vid.wall_height = is_cylinder() ? 0.6 : 1.4;
      vid.eye = is_cylinder() ? 0.5 : 0.2;
      vid.depth = is_cylinder() ? 0 : 0.5;
      }
    }
  if(supports_flat() && flat_embedding) {
    vid.eye += vid.depth / 2;
    vid.depth = 0;
    }
  if(spatial_embedding == seDefault && !flat_embedding && inverted_embedding) {
    vid.eye += vid.depth * 1.5;
    vid.depth *= -1;
    }
  if((is_euc_in_hyp() || is_euc_in_noniso()) && inverted_embedding && !is_cylinder()) {
    vid.wall_height *= -1;
    vid.eye = -2 * vid.depth;
    }
  if(is_euc_in_nil() || is_euc_in_sl2()) {
    vid.depth = 0;
    vid.eye = vid.wall_height / 2;
    }
  if(is_euc_in_hyp() && spatial_embedding == seMuchLowerCurvature) {
    vid.eye = inverted_embedding ? -vid.depth : vid.depth;
    vid.depth = 0;
    }
  if(msphere && spatial_embedding == seProduct) {
    vid.depth = 0;
    vid.wall_height = 2;
    vid.eye = 2;
    }
  if(pmodel == mdDisk) pmodel = nonisotropic ? mdGeodesic : mdPerspective;
  if(cgflags & qIDEAL && vid.texture_step < 32)
    vid.texture_step = 32;
#if CAP_RACING
  racing::player_relative = true;
#endif
  if(hyperbolic && is_same_in_same() && spatial_embedding == seLowerCurvature) {
    vid.eye += vid.depth;
    vid.depth *= 2;
    if(inverted_embedding) {
      vid.eye = 1;
      vid.depth *= -1;
      vid.wall_height *= -1;
      }
    }
  if(hyperbolic && is_same_in_same() && spatial_embedding == seMuchLowerCurvature) {
    vid.eye += vid.depth;
    vid.depth *= 3;
    if(inverted_embedding) {
      vid.eye = 2;
      vid.depth *= -1;
      vid.wall_height *= -1;
      }
    }
  if(spatial_embedding == seCliffordTorus) configure_clifford_torus();
  if(spatial_embedding == seProductS) configure_cylinder();
  if(spatial_embedding == seCylinderE) configure_cylinder();
  if(spatial_embedding == seCylinderH) configure_cylinder();
  if(spatial_embedding == seCylinderHE) configure_cylinder();
  if(spatial_embedding == seCylinderSL2) configure_cylinder();
  if(spatial_embedding == seCylinderNil) configure_cylinder();
  }

EX void invoke_embed(geom3::eSpatialEmbedding se) {
  if(GDIM == 3) { if(geom3::auto_configure) geom3::switch_fpp(); else geom3::switch_always3(); }
  if(in_tpp()) geom3::switch_tpp();
  if(se != geom3::seNone) {
    geom3::spatial_embedding = se;
    if(geom3::auto_configure) geom3::switch_fpp(); else geom3::switch_always3();
    delete_sky();
    resetGL();
    }
  }

geom3::eSpatialEmbedding embed_by_name(string ss) {
  using namespace geom3;
  auto& seo = spatial_embedding_options;
  for(int i=0; i<isize(seo); i++) if(seo[i].first == ss) return eSpatialEmbedding(i);
  bool numeric = true;
  for(char c: ss) if(c < '0' || c > '9') numeric = false;
  if(numeric) return eSpatialEmbedding(atoi(ss.c_str()));
  for(int i=0; i<isize(seo); i++) if(appears(seo[i].first, ss)) return eSpatialEmbedding(i);
  for(int i=0; i<isize(seo); i++) if(appears(seo[i].second, ss)) return eSpatialEmbedding(i);
  return seNone;
  }

auto ah_embed = arg::add2("-seo", [] { arg::shift(); invoke_embed(embed_by_name(arg::args())); });

}
