// Hyperbolic Rogue
// Copyright (C) 2011-2017 Zeno and Tehora Rogue, see 'hyper.cpp' for details

// Kohonen's self-organizing networks. 
// This is a part of RogueViz, not a part of HyperRogue.

namespace kohonen {

int cols;

typedef vector<double> kohvec;

struct sample {
  kohvec val;
  string name;
  };

vector<sample> data;

vector<int> samples_shown;

int whattodraw[3];

struct neuron {
  kohvec net;
  cell *where;
  double udist;
  int lpbak;
  int col;
  int samples, csample;
  };

kohvec weights;

vector<neuron> net;

int neuronId(neuron& n) { return &n - &(net[0]); }

void alloc(kohvec& k) { k.resize(cols); }

bool neurons_indexed = false;

int samples;

template<class T> T sqr(T x) { return x*x; }

vector<neuron*> whowon;

void normalize() {
  alloc(weights);
  for(int k=0; k<cols; k++) {
    double sum = 0, sqsum = 0;
    for(sample& s: data)
      sum += s.val[k],
      sqsum += s.val[k] * s.val[k];
    double variance = sqsum/samples - sqr(sum/samples);
    weights[k] = 1 / sqrt(variance);
    }
  }

double vnorm(kohvec& a, kohvec& b) {
  double diff = 0;
  for(int k=0; k<cols; k++) diff += sqr((a[k]-b[k]) * weights[k]);
  return diff;
  }

void loadsamples(const char *fname) {
  normalize();
  FILE *f = fopen(fname, "rt");
  if(!f) return;
  if(fscanf(f, "%d", &cols) != 1) { fclose(f); return; }
  while(true) {
    sample s;
    bool shown = false;
    alloc(s.val);
    if(feof(f)) break;
    for(int i=0; i<cols; i++)
      if(fscanf(f, "%lf", &s.val[i]) != 1) { break; }
    fgetc(f);
    while(true) {
      int c = fgetc(f);
      if(c == -1 || c == 10 || c == 13) break;
      if(c == '!' && s.name == "") shown = true;
      else if(c != 32 && c != 9) s.name += c;
      }
    if(shown) samples_shown.push_back(size(data));
    data.push_back(move(s));
    }
  fclose(f);
  samples = size(data);
  normalize();
  
  vdata.resize(size(samples_shown));
  for(int i=0; i<size(samples_shown); i++) {
    vdata[i].name = data[samples_shown[i]].name;
    vdata[i].cp = dftcolor;
    createViz(i, cwt.c, Id);
    }
  
  storeall();
  }

int t, tmax;

int lpct, mul, maxdist, cells, perdist;
double maxfac;

neuron& winner(int id) {
  double bdiff = 1e20;
  neuron *bcell = NULL;
  for(neuron& n: net) {
    double diff = vnorm(n.net, data[id].val);
    if(diff < bdiff) bdiff = diff, bcell = &n;
    }
  return *bcell;
  }

void setindex(bool b) {
  if(b == neurons_indexed) return;
  neurons_indexed = b;
  if(b) {
    for(neuron& n: net) n.lpbak = n.where->landparam, n.where->landparam = neuronId(n);
    }
  else {
    for(neuron& n: net) n.where->landparam = n.lpbak;
    }
  }

neuron *getNeuron(cell *c) {
  if(!c) return NULL;
  setindex(true);
  if(c->landparam < 0 || c->landparam >= cells) return NULL;
  neuron& ret = net[c->landparam];
  if(ret.where != c) return NULL;
  return &ret;
  }

neuron *getNeuronSlow(cell *c) {
  if(neurons_indexed) return getNeuron(c);
  for(neuron& n: net) if(n.where == c) return &n;
  return NULL;
  }

double maxudist;

neuron *distfrom;

void coloring() {
  setindex(false);
  for(int pid=0; pid<3; pid++) {
    int c = whattodraw[pid];
    vector<double> listing;
    for(neuron& n: net) switch(c) {
      case -4:
        listing.push_back(n.samples);
        break;
        
      case -3:
        if(distfrom) 
          listing.push_back(vnorm(n.net, distfrom->net));
        else
          listing.push_back(0);
        break;
      
      case -2:
        listing.push_back(n.udist);
        break;
      
      case -1: 
        listing.push_back(-n.udist);
        break;
      
      default:
        listing.push_back(n.net[c]);
        break;
      }
    
    double minl = listing[0], maxl = listing[0];
    for(double& d: listing) minl = min(minl, d), maxl = max(maxl, d);
    if(maxl-minl < 1e-3) maxl = minl+1e-3;
    
    for(int i=0; i<cells; i++) 
      part(net[i].where->landparam, pid) = (255 * (listing[i] - minl)) / (maxl - minl);
    }
  }

void analyze() {

  setindex(true);
  
  maxudist = 0;
  for(neuron& n: net) {
    int qty = 0;
    double total = 0;
    forCellEx(c2, n.where) {
      neuron *n2 = getNeuron(c2);
      if(!n2) continue;
      qty++;
      total += sqrt(vnorm(n.net, n2->net));
      }
    n.udist = total / qty;
    maxudist = max(maxudist, n.udist);
    }
    
  whowon.resize(samples);
  
  for(neuron& n: net) n.samples = 0;
  
  for(int id=0; id<size(samples_shown); id++) {
    int s = samples_shown[id];
    auto& w = winner(s);
    whowon[s] = &w;
    w.samples++;
    }
  
  for(int id=0; id<size(samples_shown); id++) {
    int s = samples_shown[id];
    auto& w = *whowon[s];
    vdata[id].m->base = w.where;
    vdata[id].m->at = 
      spin(2*M_PI*w.csample / w.samples) * xpush(.25 * (w.samples-1) / w.samples);
    w.csample++;
    }
  
  shmup::fixStorage();  
  setindex(false);
  coloring();
  }

struct cellcrawler {

  struct cellcrawlerdata {
    cellwalker orig;
    int from, spin, dist;
    cellwalker target;
    cellcrawlerdata(const cellwalker& o, int fr, int sp) : orig(o), from(fr), spin(sp) {}
    };
  
  vector<cellcrawlerdata> data;
  
  void store(const cellwalker& o, int from, int spin) {
    if(eq(o.c->aitmp, sval)) return;
    o.c->aitmp = sval;
    data.emplace_back(o, from, spin);
    }
  
  void build(const cellwalker& start) {
    sval++;
    data.clear();
    store(start, 0, 0);
    for(int i=0; i<size(data); i++) {
      cellwalker cw0 = data[i].orig;
      for(int j=0; j<cw0.c->type; j++) {
        cellwalker cw = cw0;
        cwspin(cw, j); cwstep(cw);
        if(!getNeuron(cw.c)) continue;
        store(cw, i, j);
        }
      }
    for(cellcrawlerdata& s: data)
      s.dist = celldistance(s.orig.c, start.c);
    }
  
  void sprawl(const cellwalker& start) {
    data[0].target = start;
    
    for(int i=1; i<size(data); i++) {
      cellcrawlerdata& s = data[i];
      s.target = data[s.from].target;
      if(!s.target.c) continue;
      cwspin(s.target, s.spin);
      if(cwstepcreates(s.target)) s.target.c = NULL;
      else cwstep(s.target);
      }
    }
  };

// traditionally Gaussian blur is used in the Kohonen algoritm
// but it does not seem to make much sense in hyperbolic geometry
// especially wrapped one.
// GAUSSIAN==1: use the Gaussian blur
// GAUSSIAN==0: simulate the dispersion on our network

#ifndef GAUSSIAN
#define GAUSSIAN 0
#endif

cellcrawler scc[2]; // hex and non-hex

#if GAUSSIAN==0
double dispersion_precision = .0001;
int dispersion_each = 1;

vector<vector<ld>> dispersion[2]; 

int dispersion_count;
#endif

void buildcellcrawler(cell *c) {
  int sccid = c->type != 6;
  
  cellcrawler& cr =  scc[sccid];
  cr.build(cellwalker(c,0));

#if GAUSSIAN==0
  vector<ld> curtemp;
  vector<ld> newtemp;
  vector<int> qty;
  vector<pair<ld*, ld*> > pairs;
  int N = size(net);
  
  curtemp.resize(N, 0);
  newtemp.resize(N, 0);
  qty.resize(N, 0);

  for(int i=0; i<N; i++) 
  forCellEx(c2, net[i].where) {
    neuron *nj = getNeuron(c2);
    if(nj) {
      pairs.emplace_back(&curtemp[i], &newtemp[neuronId(*nj)]);
      qty[i]++;
      }
    }
  
  curtemp[neuronId(*getNeuron(c))] = 1;

  ld vmin = 0, vmax = 1;
  int iter;
  
  auto &d = dispersion[sccid];
  
  d.clear();

  printf("Building dispersion...\n");
  
  for(iter=0; dispersion_count ? true : vmax > vmin * 1.5; iter++) {
    if(iter % dispersion_each == 0) {
      d.emplace_back(N);
      auto& dispvec = d.back();
      for(int i=0; i<N; i++) dispvec[i] = curtemp[neuronId(*getNeuron(cr.data[i].orig.c))] / vmax;
      if(size(d) == dispersion_count) break;
      }
    double df = dispersion_precision * (iter+1);
    double df0 = df / ceil(df);
    for(int i=0; i<df; i++) {
      for(auto& p: pairs) 
        *p.second += *p.first;
      for(int i=0; i<N; i++) {
        curtemp[i] += (newtemp[i] / qty[i] - curtemp[i]) * df0;
        newtemp[i] = 0;
        }
      }
    vmin = vmax = curtemp[0];
    for(int i=0; i<N; i++) 
      if(curtemp[i] < vmin) vmin = curtemp[i];
      else if(curtemp[i] > vmax) vmax = curtemp[i];
    }

  dispersion_count = size(d);
  printf("Dispersion count = %d\n", dispersion_count);
#endif
  }

bool finished() { return t == 0; }

void step() {

  if(t == 0) return;

#if GAUSSIAN==1
  double sigma = maxdist * t / (perdist*(double) mul);
#else
  int dispid = int(dispersion_count * (t-1.) / tmax);
#endif

//  double sigma = maxdist * exp(-t / t1);
  int pct = (int) (100 * ((t*(double) mul) / perdist));
  if(pct != lpct) {
    lpct = pct;
    analyze();
#if GAUSSIAN==1
    printf("t = %6d/%2dx%6d pct = %3d sigma=%10.7lf maxudist=%10.7lf\n", t, mul, perdist, pct, sigma, maxudist);
#else
    printf("t = %6d/%2dx%6d pct = %3d dispid=%5d maxudist=%10.7lf\n", t, mul, perdist, pct, dispid, maxudist);
#endif
    }
  int id = hrand(samples);
  neuron& n = winner(id);
  whowon[id] = &n;
  
  
  /* 
  for(neuron& n2: net) {
    int d = celldistance(n.where, n2.where);
    double nu = maxfac; 
//  nu *= exp(-t*(double)maxdist/perdist);
//  nu *= exp(-t/t2);
    nu *= exp(-sqr(d/sigma));
    for(int k=0; k<cols; k++)
      n2.net[k] += nu * (irisdata[id][k] - n2.net[k]);
    } */
    
  int sccid = n.where->type != 6;
  cellcrawler& s = scc[sccid];
  s.sprawl(cellwalker(n.where, 0));

#if GAUSSIAN==0
  auto it = dispersion[sccid][dispid].begin();
#endif
  for(auto& sd: s.data) {
    neuron *n2 = getNeuron(sd.target.c);
    if(!n2) continue;
    double nu = maxfac;
#if GAUSSIAN==0
    nu *= *(it++);
#else
    nu *= exp(-sqr(sd.dist/sigma));
#endif
    for(int k=0; k<cols; k++)
      n2->net[k] += nu * (data[id].val[k] - n2->net[k]);
    }
  
  t--;
  if(t == 0) analyze();
  }

void run(const char *fname, int _perdist, double _maxfac) {
  perdist = _perdist;
  maxfac = _maxfac;
  init(); kind = kKohonen;
  
  loadsamples(fname);

  /* if(geometry != gQuotient1) {
    targetGeometry = gQuotient1;
    restartGame('g');
    }
  if(!purehepta) restartGame('7'); */

#define Z 1

  vector<cell*>& allcells = currentmap->allcells();
  cells = size(allcells);
  net.resize(cells);
  for(int i=0; i<cells; i++) net[i].where = allcells[i], allcells[i]->landparam = i;
  for(int i=0; i<cells; i++) {
    net[i].where->land = laCanvas;
    alloc(net[i].net);

    for(int k=0; k<cols; k++)
    for(int z=0; z<Z; z++)
      net[i].net[k] += data[hrand(samples)].val[k] / Z;
    }
    
  for(neuron& n: net) for(int d=BARLEV; d>=7; d--) setdist(n.where, d, NULL);

  cell *c1 = net[cells/2].where;
  vector<int> mapdist;
  for(neuron &n2: net) mapdist.push_back(celldistance(c1,n2.where));
  sort(mapdist.begin(), mapdist.end());
  maxdist = mapdist[size(mapdist)*5/6];

  printf("samples = %d cells = %d maxdist = %d\n", samples, cells, maxdist);
    
#if GAUSSIAN==0
  dispersion_count = 0;  
#endif
  c1 = currentmap->gamestart();
  cell *c2 = createMov(c1, 0);
  buildcellcrawler(c1);
  if(c1->type != c2->type) buildcellcrawler(c2);

  lpct = -46130;  
  mul = 1;
  tmax = t = perdist*mul;
  step();
  for(int i=0; i<3; i++) whattodraw[i] = -2;
  analyze();
  }
  
void describe(cell *c) {
  if(cmode & sm::HELP) return;
  neuron *n = getNeuronSlow(c);
  if(!n) return;
  help += "cell number: " + its(neuronId(*n)) + "\n";
  help += "parameters:"; for(int k=0; k<cols; k++) help += " " + fts(n->net[k]); 
  help += ", u-matrix = " + fts(n->udist);
  help += "\n";
  int qty = 0;
  for(int s=0; s<samples; s++) if(whowon[s] == n) {
    help += "sample "+its(s)+":"; 
    for(int k=0; k<cols; k++) help += " " + fts(data[s].val[k]); 
    help += " "; help += data[s].name; help += "\n";
    qty++; if(qty >= 20) break;
    }
  }

void ksave(const char *fname) {
  FILE *f = fopen(fname, "wt");
  fprintf(f, "%d %d\n", cells, t);
  for(neuron& n: net) {
    for(int k=0; k<cols; k++)
      fprintf(f, "%.4lf ", n.net[k]);
    fprintf(f, "\n");
    }
  fclose(f);
  }

void kload(const char *fname) {
  int xcells;
  FILE *f = fopen(fname, "rt");
  if(!f) return;
  if(fscanf(f, "%d%d\n", &xcells, &t) != 2) return;
  if(xcells != cells) {
    printf("Error: bad number of cells\n");
    exit(1);
    }
  for(neuron& n: net) {
    for(int k=0; k<cols; k++) if(fscanf(f, "%lf", &n.net[k]) != 1) return;
    }
  fclose(f);
  analyze();
  }

void kclassify(const char *fname) {
  for(neuron& n: net) n.samples = 0;

  FILE *f = fopen(fname, "wt");  
  for(int id=0; id<size(data); id++) {
    auto& w = winner(id);
    w.samples++;
    if(id % 100000 == 0) printf("%d/%d\n", id, size(data));
    fprintf(f, "%s;%d\n", data[id].name.c_str(), neuronId(w));
    }
  fclose(f);
  coloring();
  }

void steps() {
  if(!kohonen::finished()) {
    unsigned int t = SDL_GetTicks();
    while(SDL_GetTicks() < t+20) kohonen::step();
    setindex(false);
    }
  }

void showMenu() {
  string parts[3] = {"red", "green", "blue"};
  for(int i=0; i<3; i++) {
    string c;
    if(whattodraw[i] == -1) c = "u-matrix";
    else if(whattodraw[i] == -2) c = "u-matrix reversed";
    else if(whattodraw[i] == -3) c = "distance from marked ('m')";
    else if(whattodraw[i] == -4) c = "number of samples";
    else c = "column " + its(whattodraw[i]);
    dialog::addSelItem(XLAT("coloring (%1)", parts[i]), c, '1'+i);
    }
  }

bool handleMenu(int sym, int uni) {
  if(uni >= '1' && uni <= '3') {
    int i = uni - '1';
    whattodraw[i]++;
    if(whattodraw[i] == cols) whattodraw[i] = -4;
    coloring();
    return true;
    }
  if(uni == '0') {
    for(char x: {'1','2','3'}) handleMenu(x, x);
    return true;
    }
  return false;
  }

}

void mark(cell *c) {
  using namespace kohonen;
  distfrom = getNeuronSlow(c);
  coloring();
  }
