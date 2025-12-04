#ifndef _DHRG_H_
#define _DHRG_H_
#define DHRGVER "7.1"
#include "../rogueviz.h"
#include "../embeddings/embeddings.h"

#define LONG_BRACKETS

namespace dhrg {

using namespace hr;

typedef long long ll;

#ifndef BOXSIZE
static constexpr int BOXSIZE = 32;
#endif
static constexpr int MAXDIST = (2*BOXSIZE);
static constexpr int SETS = 4;

struct segment;

inline cell *croot() { return currentmap->gamestart(); }

extern int M;
extern vector<struct mycell*> vertices;

extern vector<ld> disttable0, disttable1;

void memoryInfo();

void dhrg_init();
bool dhrg_animate(int sym, int uni);

extern int iterations;

void clear();

void graph_from_rv();
}
#endif
