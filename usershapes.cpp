// HyperRogue -- user-defined shapes
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file usershapes.cpp
 *  \brief user-defined shapes
 */

#include "hyper.h"
#if CAP_SHAPES

namespace hr {

EX int usershape_changes;

EX array<map<int, usershape*>, mapeditor::USERSHAPEGROUPS> usershapes;
EX void initShape(int sg, int id) {

  if(!usershapes[sg][id]) {
    usershape *us = new usershape;
    usershapes[sg][id] = us;

    for(int i=0; i<USERLAYERS; i++) {
      us->d[i].prio = PPR((sg == 3 ? 1:50) + i);

      us->d[i].rots = 1;
      us->d[i].sym = 0;
      us->d[i].shift = C0;
      us->d[i].spin = Cx1;
      us->d[i].color = 0;
      us->d[i].zlevel = 0;
      }
    }
  }

EX basic_textureinfo user_triangles_texture;

void geometry_information::pushShape(usershapelayer& ds) {

  if(ds.list.empty()) return;
  if(GDIM == 3) last->flags |= POLY_TRIANGLES;

  transmatrix T = rgpushxto0(ds.shift) * rspintox(ds.spin);
  
  int z = GDIM == 3 ? 3 : 1;
  
  for(int r=0; r<ds.rots; r++) {
    for(int i=0; i<isize(ds.list)/z*z; i++)
      hpcpush(T * spin(TAU*r/ds.rots) * ds.list[i]);

    if(ds.sym) {
  
      transmatrix mirrortrans = Id; mirrortrans[1][1] = -1;
      for(int i=isize(ds.list)-1; i>=0; i--)
        hpcpush(T * spin(TAU*r/ds.rots) * mirrortrans * ds.list[i]);
      }
    }
  
  if(GDIM == 2) hpcpush(T * ds.list[0]);

  #if MAXMDIM >= 4 && CAP_GL
  if(GDIM == 3) {
    auto& utt = user_triangles_texture;
    utt.texture_id = floor_textures->renderedTexture;
    ds.texture_offset = isize(utt.tvertices);
    for(int i=0; i<isize(ds.list)-2; i+=3) {
      hyperpoint h = orthogonal_of_C0(ds.list[i], ds.list[i+1], ds.list[i+2]);
      ld rad = hypot_d(3, h);
      ld factor = 0.49 + (0.17 * h[2] + 0.13 * h[1] + 0.20 * h[0]) / rad;
      for(int i=0; i<3; i++)
        utt.tvertices.push_back(glhr::makevertex(-1, factor, 0));
      }
    }
  #endif
  }

void geometry_information::prepare_usershapes() {
  hpc.resize(prehpc);
  last = NULL;
  DEBB(DF_POLY, ("hpc = ", prehpc));

  user_triangles_texture.tvertices.clear();
  
  for(int i=0; i<mapeditor::USERSHAPEGROUPS; i++) for(auto usp: usershapes[i]) {
    auto us = usp.second;
    if(!us) continue;
    for(int l=0; l<USERLAYERS; l++) {
      bshape(ushr[&us->d[l]], us->d[l].prio);
      pushShape(us->d[l]);
      finishshape();
      }
    }
  
  static int qhpc0;
  int qhpc = isize(hpc);
  if(qhpc != qhpc0 && (debugflags & (DF_GEOM | DF_POLY))) {
    println(hlog, "qhpc = ", qhpc0=qhpc, " (", prehpc, "+", qhpc-prehpc, ")");
    println(hlog, "shapes = ", isize(allshapes));
    int inve=0, issi=0, vcon=0, ccon=0;
    for(auto sh: allshapes) {
      if(sh->flags & POLY_INVERSE) inve++;
      if(sh->flags & POLY_ISSIDE) issi++;
      if(sh->flags & POLY_VCONVEX) vcon++;
      if(sh->flags & POLY_CCONVEX) ccon++;
      }
    println(hlog, format("inverse = %d isside = %d vcon = %d ccon = %d", inve, issi, vcon, ccon));
    }
  
  initPolyForGL();
  }

}
#endif
