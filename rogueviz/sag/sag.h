// RogueViz -- SAG embedder: main header file
// Copyright (C) 2011-2024 Zeno Rogue, see 'hyper.cpp' for details

#ifndef _SAG_H_
#define _SAG_H_

#include "../rogueviz.h"
#include "../embeddings/embeddings.h"

namespace rogueviz {
namespace sag {

/** SAG is generally active */
static const flagtype SS_GENERAL = 1;

/** the cells are known */
static const flagtype SS_CELLS = 2;

/** the graph is known */
static const flagtype SS_DATA = 4;

/** the graph is visualized */
static const flagtype SS_GRAPH = 8;

/** no cells because we need to call init_snake */
static const flagtype SS_NEED_SNAKE = 16;

/** the graph is weighted */
static const flagtype SS_WEIGHTED = 32;

extern flagtype state;

extern vector<int> sagid;

extern ld yes_for(ld d);

void init();
void after_data();
void clear();
void compute_cost();
void compute_loglik_tab();

extern debugflag debug_sag;
extern debugflag debug_init_sag;

namespace cells {
  extern color_t sag_cellcolor;
  void activate_cellcolor();

  extern int cell_request;
  void init_cells();
  }

void sag_bridging(bool, int);
bool optimized_embedding(int, ld=0);

extern bool twoway, allow_doubles;

enum eSagMethod { smClosest, smLogistic, smMatch };
extern eSagMethod method;
extern rogueviz::embeddings::logistic lgsag, lgsag_pre, best;
extern ld bestcost;

enum eSagmode { sagOff, sagHC, sagSA };
extern eSagmode sagmode;

extern int vizsa_start, vizsa_len;

}
}

#endif
