// RogueViz -- SAG embedder: main header file
// Copyright (C) 2011-2024 Zeno Rogue, see 'hyper.cpp' for details

#ifndef _SAG_H_
#define _SAG_H_

#include "../rogueviz.h"

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

void init();
void clear();

}
}

#endif
