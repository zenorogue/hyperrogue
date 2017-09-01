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

int whattodraw[3] = {-2,-2,-2};

struct neuron {
  kohvec net;
  cell *where;
  double udist;
  int lpbak;
  int col;
  int samples, csample, bestsample;
  };

vector<string> colnames;

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

void sominit(int);
void uninit(int);

void loadsamples(const char *fname) {
  FILE *f = fopen(fname, "rt");
  if(!f) {
    fprintf(stderr, "Could not load samples\n");
    return;
    }
  if(fscanf(f, "%d", &cols) != 1) { fclose(f); return; }
  while(true) {
    sample s;
    bool shown = false;
    alloc(s.val);
    if(feof(f)) break;
    for(int i=0; i<cols; i++)
      if(fscanf(f, "%lf", &s.val[i]) != 1) { goto bigbreak; }
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
  bigbreak:
  fclose(f);
  samples = size(data);
  normalize();
  colnames.resize(cols);
  for(int i=0; i<cols; i++) colnames[i] = "Column " + its(i);
  uninit(0); sominit(1);
  }
                  
int tmax = 30000;
double distmul = 1;
double learning_factor = .1;

int qpct = 100;

int t, lpct, cells;
double maxdist;

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

bool noshow = false;

void coloring() {
  if(noshow) return;
  setindex(false);
  
  bool besttofind = true;

  for(int pid=0; pid<3; pid++) {
    int c = whattodraw[pid];
    
    if(c == -5) {
      if(besttofind) {
        besttofind = false;
        for(neuron& n: net) {
          double bdiff = 1e20;
          for(int i=0; i<size(samples_shown); i++) {
            double diff = vnorm(n.net, data[samples_shown[i]].val);
            if(diff < bdiff) bdiff = diff, n.bestsample = i;
            }
          }
        }

      for(int i=0; i<cells; i++) 
        part(net[i].where->landparam, pid) = part(vdata[net[i].bestsample].cp.color1, pid+1);
      }

    else {
      vector<double> listing;
      for(neuron& n: net) switch(c) {
        case -4:
          listing.push_back(log(5+n.samples));
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
    
  if(!noshow) {

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
    }
  
  coloring();
  }

// traditionally Gaussian blur is used in the Kohonen algoritm
// but it does not seem to make much sense in hyperbolic geometry
// especially wrapped one.
// GAUSSIAN==1: use the Gaussian blur, on celldistance
// GAUSSIAN==2: use the Gaussian blur, on true distance
// GAUSSIAN==0: simulate the dispersion on our network

int gaussian = 0;

double mydistance(cell *c1, cell *c2) {
  if(gaussian == 2) return hdist(tC0(shmup::ggmatrix(c1)), tC0(shmup::ggmatrix(c2)));
  else return celldistance(c1, c2);
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
    if(gaussian) for(cellcrawlerdata& s: data)
      s.dist = mydistance(s.orig.c, start.c);
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

cellcrawler scc[2]; // hex and non-hex

double dispersion_end_at = 1.5;

double dispersion_precision = .0001;
int dispersion_each = 1;

vector<vector<ld>> dispersion[2]; 

int dispersion_count;

void buildcellcrawler(cell *c) {
  int sccid = c->type != 6;
  
  cellcrawler& cr =  scc[sccid];
  cr.build(cellwalker(c,0));

  if(!gaussian) {
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
    
    for(iter=0; dispersion_count ? true : vmax > vmin * dispersion_end_at; iter++) {
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
    }
  }

bool finished() { return t == 0; }

int krad;

double ttpower = 1;

void sominit(int);

void step() {

  if(t == 0) return;
  sominit(2);
  
  double tt = (t-1.) / tmax;
  tt = pow(tt, ttpower);

  double sigma = maxdist * tt;
  int dispid = int(dispersion_count * tt);

  if(qpct) {
    int pct = (int) ((qpct * (t+.0)) / tmax);
    if(pct != lpct) {
      printf("pct %d lpct %d\n", pct, lpct);
      lpct = pct;
      analyze();
      
      if(gaussian)
        printf("t = %6d/%6d %3d%% sigma=%10.7lf maxudist=%10.7lf\n", t, tmax, pct, sigma, maxudist);
      else
        printf("t = %6d/%6d %3d%% dispid=%5d maxudist=%10.7lf\n", t, tmax, pct, dispid, maxudist);
      }
    }
  int id = hrand(samples);
  neuron& n = winner(id);
  whowon.resize(samples);
  whowon[id] = &n;
    
  /* 
  for(neuron& n2: net) {
    int d = celldistance(n.where, n2.where);
    double nu = learning_factor; 
//  nu *= exp(-t*(double)maxdist/perdist);
//  nu *= exp(-t/t2);
    nu *= exp(-sqr(d/sigma));
    for(int k=0; k<cols; k++)
      n2.net[k] += nu * (irisdata[id][k] - n2.net[k]);
    } */
    
  int sccid = n.where->type != 6;
  cellcrawler& s = scc[sccid];
  s.sprawl(cellwalker(n.where, 0));

  vector<double> fake(1,1);
  auto it = gaussian ? fake.begin() : dispersion[sccid][dispid].begin();

  for(auto& sd: s.data) {
    neuron *n2 = getNeuron(sd.target.c);
    if(!n2) continue;
    double nu = learning_factor;
    
    if(gaussian)
      nu *= exp(-sqr(sd.dist/sigma));
    else
      nu *= *(it++);

    for(int k=0; k<cols; k++)
      n2->net[k] += nu * (data[id].val[k] - n2->net[k]);
    }
  
  t--;
  if(t == 0) analyze();
  }

int initdiv = 1;

int inited = 0;

void uninit(int initto) {
  if(inited > initto) inited = initto;
  }

void showsample(int id) {
  for(int ii: samples_shown) 
    if(ii == id) 
      return;
  int i = vdata.size();
  samples_shown.push_back(id);
  vdata.emplace_back();
  auto& v = vdata.back();
  v.name = data[id].name;
  v.cp = dftcolor;
  createViz(i, cwt.c, Id);
  v.m->store();
  }

void showsample(string s) {
  if(s == "") return;
  for(int i=0; i<samples; i++) {
    if(s[0] != '*' && data[i].name == s)
      showsample(i);
    if(s[0] == '*' && data[i].name.find(s.substr(1)) != string::npos)
      showsample(i);
    }
  }

void showbestsamples() {
  vector<int> samplesbak;
  for(auto& n: net) 
    if(n.samples)
      showsample(n.bestsample);
  analyze();
  for(auto& n: net) n.samples = 0;
  for(int i=0; i<samples; i++) 
    if(whowon[i])
      whowon[i]->samples++;
  }
  
void sominit(int initto) {

  if(inited < 1 && initto >= 1) {
    inited = 1;
    if(!samples) {
      fprintf(stderr, "Error: SOM without samples\n");
      exit(1);
      }
    
    init(); kind = kKohonen;
    
    /* if(geometry != gQuotient1) {
      targetGeometry = gQuotient1;
      restartGame('g');
      }
    if(!purehepta) restartGame('7'); */
    
    printf("Initializing SOM (1)\n");
  
    vector<cell*> allcells;
    
    if(krad) {
      celllister cl(cwt.c, krad, 1000000, NULL);
      allcells = cl.lst;
      }
    else allcells = currentmap->allcells();
  
    cells = size(allcells);
    net.resize(cells);
    for(int i=0; i<cells; i++) net[i].where = allcells[i], allcells[i]->landparam = i;
    for(int i=0; i<cells; i++) {
      net[i].where->land = laCanvas;
      alloc(net[i].net);
  
      for(int k=0; k<cols; k++)
      for(int z=0; z<initdiv; z++)
        net[i].net[k] += data[hrand(samples)].val[k] / initdiv;
      }
      
    for(neuron& n: net) for(int d=BARLEV; d>=7; d--) setdist(n.where, d, NULL);
  
    printf("samples = %d (%d) cells = %d\n", samples, size(samples_shown), cells);

    if(!noshow) {
      vdata.resize(size(samples_shown));
      for(int i=0; i<size(samples_shown); i++) {
        vdata[i].name = data[samples_shown[i]].name;
        vdata[i].cp = dftcolor;
        createViz(i, cwt.c, Id);
        }
      
      storeall();
      }

    analyze();
    }
  
  if(inited < 2 && initto >= 2) {
    inited = 2;

    printf("Initializing SOM (2)\n");

    if(gaussian) {
      printf("dist = %lf\n", mydistance(net[0].where, net[1].where));
      cell *c1 = net[cells/2].where;
      vector<double> mapdist;
      for(neuron &n2: net) mapdist.push_back(mydistance(c1,n2.where));
      sort(mapdist.begin(), mapdist.end());
      maxdist = mapdist[size(mapdist)*5/6] * distmul;
      printf("maxdist = %lf\n", maxdist);
      }
      
    dispersion_count = 0;  
    cell *c1 = currentmap->gamestart();
    cell *c2 = createMov(c1, 0);
    buildcellcrawler(c1);
    if(c1->type != c2->type) buildcellcrawler(c2);
  
    lpct = -46130;
    }
  }

void describe(cell *c) {
  if(cmode & sm::HELP) return;
  neuron *n = getNeuronSlow(c);
  if(!n) return;
  help += "cell number: " + its(neuronId(*n)) + " (" + its(n->samples) + ")\n";
  help += "parameters:"; for(int k=0; k<cols; k++) help += " " + fts(n->net[k]); 
  help += ", u-matrix = " + fts(n->udist);
  help += "\n";
  vector<pair<double, int>> v;
  for(int s=0; s<samples; s++) if(whowon[s] == n) v.emplace_back(vnorm(n->net, data[s].val), s);
  random_shuffle(v.begin(), v.end());
  sort(v.begin(), v.end(), [] (auto a, auto b) { return a.first < b.first; });
  
  for(int i=0; i<size(v) && i<20; i++) {
    int s = v[i].second;
    help += "sample "+its(s)+":"; 
    for(int k=0; k<cols; k++) help += " " + fts(data[s].val[k]); 
    help += " "; help += data[s].name; help += "\n";
    }
  }

namespace levelline {

  struct levelline {
    int column, qty;
    unsigned int color;
    vector<double> values;
    bool modified;
    };
  
  vector<levelline> levellines;
  
  bool on;
  
  void create() {
    int xlalpha = int(pow(ld(.5), ggamma) * 255);
    for(int i=0; i<cols; i++) {
      levellines.emplace_back();
      levelline& lv = levellines.back();
      lv.column = i;    
      lv.color = ((hrandpos() & 0xFFFFFF) << 8) | xlalpha;
      lv.qty = 0;
      }
    }
  
  void build() {
    if(levellines.size() == 0) create();
    on = false;
    for(auto& lv: levellines) {
      if(!lv.qty) { lv.values.clear(); continue; }
      on = true;
      if(!lv.modified) continue;
      lv.modified = false;
      vector<double> sample;
      for(int j=0; j<=1024; j++) sample.push_back(data[hrand(samples)].val[lv.column]);
      sort(sample.begin(), sample.end());
      lv.values.clear();
      lv.values.push_back(-1e10);
      for(int j=0; j<=1024; j+=1024 >> (lv.qty)) lv.values.push_back(sample[j]);
      lv.values.push_back(1e10);
      }
    }
  
  void draw() {
    if(!on) return;
    for(auto& g: gmatrix) {
      cell *c1 = g.first;
      transmatrix T = g.second;
      neuron *n1 = getNeuron(c1);
      if(!n1) continue;
      for(int i=0; i<c1->type; i++) {
        cell *c2 = c1->mov[i];
        if(!c2) continue;
        cell *c3 = c1->mov[i ? i-1 : c1->type-1];
        if(!c3) continue;
  
        if(!gmatrix.count(c2)) continue;
        if(!gmatrix.count(c3)) continue;
        double d2 = hdist(tC0(T), tC0(gmatrix[c2]));
        double d3 = hdist(tC0(T), tC0(gmatrix[c3]));
        
        neuron *n2 = getNeuron(c2);
        if(!n2) continue;
        neuron *n3 = getNeuron(c3);
        if(!n3) continue;
              
        for(auto& l: levellines) {
          auto val1 = n1->net[l.column];
          auto val2 = n2->net[l.column];
          auto val3 = n3->net[l.column];
          auto v1 = lower_bound(l.values.begin(), l.values.end(), val1);
          auto v2 = lower_bound(l.values.begin(), l.values.end(), val2);
          auto v3 = lower_bound(l.values.begin(), l.values.end(), val3);
          auto draw = [&] () {
            auto vmid = *v1;
            queueline(
              tC0(T * ddspin(c1,i) * xpush(d2 * (vmid-val1) / (val2-val1))), 
              tC0(T * ddspin(c1,i-1) * xpush(d3 * (vmid-val1) / (val3-val1))), 
              l.color);
            };
          while(v1 < v2 && v1 < v3) {
            draw();
            v1++;
            }
          while(v1 > v2 && v1 > v3) {
            v1--;
            draw();
            }
          }
        }      
      }
    setindex(false);
    }

  void show() {
    if(levellines.size() == 0) create();
    gamescreen(0);
    cmode = vid.xres > vid.yres * 1.4 ? sm::SIDE : 0;
    dialog::init("level lines");
    char nx = 'a';
    for(auto &l : levellines) {
      dialog::addSelItem(colnames[l.column], its(l.qty), nx++);
      dialog::lastItem().colorv = l.color >> 8;
      }
    dialog::addItem("exit menu", '0');
    dialog::addItem("shift+letter to change color", 0);
    dialog::display();
    keyhandler = [] (int sym, int uni) {
      dialog::handleNavigation(sym, uni);
      if(uni >= 'a' && uni - 'a' + size(levellines)) {
        auto& l = levellines[uni - 'a'];
        dialog::editNumber(l.qty, 0, 10, 1, 0, colnames[l.column], 
          XLAT("Controls the number of level lines."));
        dialog::reaction = [&l] () {
          l.modified = true;
          build();
          };
        }
      else if(uni >= 'A' && uni - 'A' + size(levellines)) {
        auto& l = levellines[uni - 'A'];
        dialog::openColorDialog(l.color, NULL);
        }
      else if(doexiton(sym, uni)) popScreen();
      };
    }

  
  }

void ksave(const char *fname) {
  sominit(1);
  FILE *f = fopen(fname, "wt");
  if(!f) {
    fprintf(stderr, "Could not save the network\n");
    return;
    }
  fprintf(f, "%d %d\n", cells, t);
  for(neuron& n: net) {
    for(int k=0; k<cols; k++)
      fprintf(f, "%.9lf ", n.net[k]);
    fprintf(f, "\n");
    }
  fclose(f);
  }

void kload(const char *fname) {
  sominit(1);
  int xcells;
  FILE *f = fopen(fname, "rt");
  if(!f) {
    fprintf(stderr, "Could not load the network\n");
    return;
    }
  if(fscanf(f, "%d%d\n", &xcells, &t) != 2) return;
  if(xcells != cells) {
    fprintf(stderr, "Error: bad number of cells\n");
    exit(1);
    }
  for(neuron& n: net) {
    for(int k=0; k<cols; k++) if(fscanf(f, "%lf", &n.net[k]) != 1) return;
    }
  fclose(f);
  analyze();
  }

void ksavew(const char *fname) {
  sominit(1);
  FILE *f = fopen(fname, "wt");
  if(!f) {
    fprintf(stderr, "Could not save the weights\n");
    return;
    }
  for(int i=0; i<cols; i++)
    fprintf(f, "%s=%.9lf\n", colnames[i].c_str(), weights[i]);
  fclose(f);
  }

void kloadw(const char *fname) {
  sominit(1);
  FILE *f = fopen(fname, "rt");
  if(!f) {
    fprintf(stderr, "Could not load the weights\n");
    return;
    }
  for(int i=0; i<cols; i++) {
    string s1, s2;
    char kind = 0;
    while(true) {
      int c = fgetc(f);
      if(c == 10 || c == 13 || c == -1) {
        if(s1 == "" && !kind && c != -1) continue;
        if(s1 != "") colnames[i] = s1;
        if(kind == '=') weights[i] = atof(s2.c_str());
        if(kind == '*') weights[i] *= atof(s2.c_str());
        if(kind == '/') weights[i] /= atof(s2.c_str());
        if(c == -1) break;
        goto nexti;
        }
      else if(c == '=' || c == '/' || c == '*') kind = c;
      else (kind?s2:s1) += c;
      }
    nexti: ;
    }
  fclose(f);
  analyze();
  }


unsigned lastprogress;
void progress(string s) {
  if(SDL_GetTicks() >= lastprogress + (noGUI ? 500 : 100)) {
    if(noGUI)
      printf("%s\n", s.c_str());
    else {
      clearMessages();
      addMessage(s);
      mainloopiter();
      }
    lastprogress = SDL_GetTicks();
    } 
  }

void kclassify(const char *fname_classify) {

  sominit(1);
  vector<double> bdiffs(samples, 1e20);
  vector<int> bids(samples, 0);

  printf("Classifying...\n");

  for(neuron& n: net) n.samples = 0;

  for(int s=0; s<samples; s++) {
    for(int n=0; n<cells; n++) {
      double diff = vnorm(net[n].net, data[s].val);
      if(diff < bdiffs[s]) bdiffs[s] = diff, bids[s] = n, whowon[s] = &net[n];
      }
    if(!(s % 128))
      progress("Classifying: " + its(s) + "/" + its(samples));
    }
  
  vector<double> bdiffn(cells, 1e20);

  printf("Finding samples...\n");

  for(int s=0; s<samples; s++) {
    int n = bids[s];
    double diff = bdiffs[s];
    if(diff < bdiffn[n]) bdiffn[n] = diff, net[n].bestsample = s;
    }

  for(int s=0; s<samples; s++) net[bids[s]].samples++;
  
  if(fname_classify != NULL) {  
    printf("Listing classification...\n");  
    FILE *f = fopen(fname_classify, "wt");  
    if(!f) {
      printf("Failed to open file\n");
      }
    else {
      for(int s=0; s<samples; s++)
        fprintf(f, "%s;%d\n", data[s].name.c_str(), bids[s]);
      fclose(f);
      }
    }
  coloring();
  }

void klistsamples(const char *fname_samples, bool best, bool colorformat) {
  if(fname_samples != NULL) { 
    printf("Listing samples...\n");
    FILE *f = fopen(fname_samples, "wt");  
    if(!f) {
      printf("Failed to open file\n");
      }
    else {
      auto klistsample = [f, colorformat] (int id, int neu) {
        if(colorformat) {
          fprintf(f, "%s;+#%d\n", data[id].name.c_str(), neu);
          }
        else {
          for(int k=0; k<cols; k++)
            fprintf(f, "%.4lf ", data[id].val[k]);
          fprintf(f, "!%s\n", data[id].name.c_str());
          }
        };
      if(!colorformat) fprintf(f, "%d\n", cols);
      if(best)
        for(int n=0; n<cells; n++) {
          if(!net[n].samples) { if(!colorformat) fprintf(f, "\n"); continue; }
          klistsample(net[n].bestsample, n);
          }
      else
        for(int i=0; i<size(samples_shown); i++) 
          klistsample(samples_shown[i], neuronId(*(whowon[i])));  
      fclose(f);
      }
    }
  }

void neurondisttable(const char *fname) {
  FILE *f = fopen(fname, "wt");
  if(!f) {
    printf("Could not open file: %s\n", fname);
    return;
    }
  int neurons = size(net);
  fprintf(f, "%d\n", neurons);
  for(int i=0; i<neurons; i++) {
    for(int j=0; j<neurons; j++) fprintf(f, "%3d", celldistance(net[i].where, net[j].where));
    // todo: build the table correctly for gaussian=2
    fprintf(f, "\n");
    }
  fclose(f);
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
    else if(whattodraw[i] == -5) c = "best sample's color";
    else if(whattodraw[i] == -6) c = "sample names to colors";
    else c = colnames[whattodraw[i]];
    dialog::addSelItem(XLAT("coloring (%1)", parts[i]), c, '1'+i);
    }
  dialog::addItem("coloring (all)", '0');
  dialog::addItem("level lines", '4');
  }

bool handleMenu(int sym, int uni) {
  if(uni >= '1' && uni <= '3') {
    int i = uni - '1';
    whattodraw[i]++;
    if(whattodraw[i] == cols) whattodraw[i] = -5;
    coloring();
    return true;
    }
  if(uni == '0') {
    for(char x: {'1','2','3'}) handleMenu(x, x);
    return true;
    }
  if(uni == '4') {
    pushScreen(levelline::show);
    return true;
    }
  return false;
  }

int readArgs() {
  using namespace arg;

  // #1: load the samples
  
  if(argis("-som")) {
    PHASE(3);
    shift(); kohonen::loadsamples(args());
    }

  // #2: set parameters

  else if(argis("-somkrad")) {
    gaussian = 0; uninit(0);
    }
  else if(argis("-somsim")) {
    gaussian = 0; uninit(1);
    }
  else if(argis("-somcgauss")) {
    gaussian = 1; uninit(1);
    }
  else if(argis("-somggauss")) {
    gaussian = 2; uninit(1);
    }
  else if(argis("-sompct")) {
    shift(); qpct = argi();
    }
  else if(argis("-sompower")) {
    shift(); ttpower = argf();
    }
  else if(argis("-somparam")) {
    shift(); (gaussian ? distmul : dispersion_end_at) = argf();
    if(dispersion_end_at <= 1) {
      fprintf(stderr, "Dispersion parameter illegal\n");
      dispersion_end_at = 1.5;
      }
    uninit(1);
    }
  else if(argis("-sominitdiv")) {
    shift(); initdiv = argi(); uninit(0);
    }
  else if(argis("-somtmax")) {
    shift(); t = (t*1./tmax) * argi();
    tmax = argi();
    }
  else if(argis("-somlearn")) {
    // this one can be changed at any moment
    shift(); learning_factor = argf();
    }

  else if(argis("-somrun")) {
    t = tmax; sominit(1);
    }

  // #3: load the neuron data (usually without #2)
  else if(argis("-somload")) {
    PHASE(3);
    shift(); kohonen::kload(args());
    }

  // #4: run, stop etc.
  else if(argis("-somrunto")) {
    int i = argi();
    shift(); while(t > i) {
      if(t % 128 == 0) progress("Steps left: " + its(t));
      kohonen::step();
      }
    }
  else if(argis("-somstop")) {
    t = 0;
    }
  else if(argis("-somnoshow")) {
    noshow = true;
    }
  else if(argis("-somfinish")) {
    while(!finished()) {
      kohonen::step();
      if(t % 128 == 0) progress("Steps left: " + its(t));
      }
    }

  // #5 save data, classify etc.
  else if(argis("-somsave")) {
    PHASE(3);
    shift(); kohonen::ksave(args());
    }
  else if(argis("-somsavew")) {
    PHASE(3);
    shift(); kohonen::ksavew(args());
    }
  else if(argis("-somloadw")) {
    PHASE(3);
    shift(); kohonen::kloadw(args());
    }
  else if(argis("-somclassify")) {
    PHASE(3);
    shift(); kohonen::kclassify(args());
    }
  else if(argis("-somlistshown")) {
    PHASE(3);
    shift(); kohonen::klistsamples(args(), false, false);
    }
  else if(argis("-somlistbest")) {
    PHASE(3);
    shift(); kohonen::klistsamples(args(), true, false);
    }
  else if(argis("-somlistbestc")) {
    PHASE(3);
    shift(); kohonen::klistsamples(args(), true, true);
    }
  else if(argis("-somndist")) {
    PHASE(3);
    shift(); kohonen::neurondisttable(args());
    }
  else if(argis("-somshowbest")) {
    showbestsamples();
    }

  else return 1;
  return 0;
  }

auto hooks = addHook(hooks_args, 100, readArgs) + addHook(hooks_frame, 50, levelline::draw);
}

void mark(cell *c) {
  using namespace kohonen;
  distfrom = getNeuronSlow(c);
  coloring();
  }

