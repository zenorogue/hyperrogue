/* a header file for kohonen and embedding */
#ifndef _KOHONEN_H_
#define _KOHONEN_H_

#include "rogueviz.h"

namespace rogueviz {

namespace kohonen {

typedef vector<double> kohvec;

static const flagtype KS_ROGUEVIZ = 1;
static const flagtype KS_NEURONS = 2;
static const flagtype KS_DISPERSION = 4;
static const flagtype KS_SAMPLES = 8;
static const flagtype KS_NEURONS_INI = 16;

extern flagtype state;

extern int tmax;
extern int samples;
extern int t, tmax, lpct, cells;
extern int gaussian;

extern int krad, kqty, kohrestrict;
extern int qpct;
extern int columns;
extern double dispersion_end_at;
extern int dispersion_count;
extern double learning_factor, distmul;
extern double ttpower;
extern int min_group, max_group, columns;

struct neuron {
  kohvec net;
  cell *where;
  double udist;
  int lpbak;
  color_t col;
  int allsamples, drawn_samples, csample, bestsample, max_group_here;
  int debug;
  neuron() { drawn_samples = allsamples = bestsample = 0; max_group_here = max_group; debug = 0; }
  };

struct sample {
  kohvec val;
  string name;
  };

inline void alloc(kohvec& k) { k.resize(columns); }

extern kohvec weights;
extern vector<sample> data;
extern vector<int> samples_to_show;
extern vector<neuron> net;
extern vector<string> colnames;

void initialize_neurons();
void initialize_neurons_initial();
void initialize_dispersion();
void initialize_samples_to_show();
void clear();
void create_neurons();
void analyze();
void step();
void initialize_rv();
void set_neuron_initial();
bool finished();

vector<cell*> gen_neuron_cells();
neuron& winner(int id);

double vdot(const kohvec& a, const kohvec& b);
void vshift(kohvec& a, const kohvec& b, ld i);
double vnorm(kohvec& a, kohvec& b);
}

namespace embeddings {

using kohonen::columns;
using kohonen::kohvec;
using kohonen::alloc;

enum embedding_type { eProjection, eNatural, eLandscape, eSignpost, eHypersian };
extern embedding_type etype;
void mark_signposts(bool full, const vector<cell*>& ac);
void mark_signposts_subg(int a, int b, const vector<cell*>& ac);
void generate_rug(int i, bool close);
void init_landscape(int dimensions);

extern map<cell*, hyperpoint> rug_coordinates;
extern void get_coordinates(kohvec& v, cell *c, cell *c0);

extern vector<cell*> signposts;

}

}


#endif
