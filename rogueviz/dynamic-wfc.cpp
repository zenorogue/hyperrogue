#include "../hyper.h"

// an implementation of WFC-like constraint satisfaction problem using polytime dynamic programming
// see: https://arxiv.org/abs/2002.09534

// usage: hyper -canvas 0 -dynamic-wfc [constraint] [cutoff] [radius]

// where constraint is one of:
// 0: every cell has 2 yellow neighbors
// 1: every cell's neighborhood has two consistent regions
// 2: every cell's neighborhood has four consistent regions
// 3: landscape: every cell has 1 consistent region of next type and 1 region of prev type

// cutoff is cut off from the disk
// radius is the radius of the disk to generate

namespace hr {

namespace dynamic_wfc {

bool animated = false;

vector<cell*> generate_pd_list(celllister& cl) {
  cell *croot = cl.lst.back();
  
  cellwalker cw(croot, 0);
  while(cl.listed(cw.peek())) cw++;
  while(!cl.listed(cw.peek())) cw++;
  
  vector<cell*> currpath = {};
  cellwalker cw1 = cw;

  vector<cell*> result;
  
  // int steps = 0;
  
  auto push = [&] (cell *c) {
    // println(hlog, "push ", c, " at ", isize(currpath));
    currpath.push_back(c);
    result.push_back(c);
    };

  auto pop = [&] () {
    // println(hlog, "pop ", currpath.back(), " at ", isize(currpath)-1);
    currpath.pop_back();
    };
  
  auto replace = [&] (int pos, cell *c) {
    // println(hlog, "replace ", currpath[pos], " to ", c, " at ", pos, "/", isize(currpath));
    currpath[pos] = c;
    result.push_back(c);
    };
  
  push(croot);
  
  while(true) {
    cw += wstep;
    cw++;
    while(!cl.listed(cw.peek())) cw++;
    cell *cn = cw.at;
    // cn->item = itGold;
    // println(hlog, "at ", cn, " [", steps++, "]");
    int cd = celldistance(cn, croot) + 1;
    if(cd > isize(currpath)) 
      push(cn);
    else {
      if(cd < isize(currpath))
        pop();
      
      if(currpath.back() != cn) {
        int moves = 1;
    
        while(celldistance(currpath[cd-1-moves], cn) > moves) {
          moves++;
          continue;
          }
        
        while(moves > 0) {
          moves--;        
          forCellEx(cx, currpath[cd-2-moves])
            if(celldistance(cx, cn) == moves)
              replace(cd-1-moves, cx);
          }
        }
      }

    /*
    cell *last = nullptr;
    
    for(cell *c: currpath) {
      if(last) print(hlog, " ", celldistance(c, last), " ");
      last = c;
      print(hlog, c);
      }
    println(hlog); */
    
    for(int i=1; i<isize(currpath); i++)
      if(celldistance(currpath[i-1], currpath[i]) != 1)
        println(hlog, "PD error");
    
    if(cw == cw1) break;
    }
  
  return result;
  }

struct ttrans {
  int news;
  int olds;
  float proportion;
  char id;
  };

vector<ttrans> trans;

vector<cell*> ctf;

vector<cell*> global_list;

int wfctype, wfcrad, cutoff;

void wfc_clear() {
  trans.clear();
  ctf.clear();
  global_list.clear();
  }

void wfc_build() {  
  int code_at = trans.back().news;
  int cpos = isize(ctf);
  for(int i=isize(trans)-1; i>=0; i--) {
    auto& tri = trans[i];
    if(tri.news == code_at) {
      // println(hlog, tie(tri.news, tri.olds, tri.proportion, tri.where, tri.id));
      if(hrandf() < tri.proportion) {
        cell *c = ctf[--cpos];
        int id = tri.id;
        
        switch(wfctype) {
          case 0:          
            if(id == 0) c->wall = waNone, c->landparam = 0xFF4040;
            else c->wall = waNone, c->landparam = 0xFFFF40;
            break;
          case 1:
            if(id == 0) c->wall = waNone, c->landparam = 0x4040FF;
            else c->wall = waNone, c->landparam = 0x40FF40;
            break;
          case 2:
            if(id == 0) c->wall = waNone, c->landparam = 0x8080FF;
            else c->wall = waNone, c->landparam = 0x202020;
            break;
          case 3:
            if(id == 0) c->wall = waNone, c->landparam = 0xC08080;
            else if(id == 1) c->wall = waNone, c->landparam = 0x80C080;
            else c->wall = waNone, c->landparam = 0x8080C0;
            break;
          }

        // println(hlog, code_at, " -> ", tri.olds, " [", tri.where, "=", tri.id, "]");
        code_at = tri.olds;
        }
      }
    }
  }

void wfc() {
  int rad = wfcrad;
  vector< vector< vector<cell*> > > new_neighborhoods;
  map<cell*, int> lorder;
  map<cell*, int> multiplicity;
  
  set<cell*> consider;
  
  celllister cl(cwt.at, rad, 999999, nullptr);
  for(cell *c: cl.lst) setdist(c, 7, nullptr);
  auto l = generate_pd_list(cl);
  int ls = isize(l);
  for(int i=0; i<ls; i++) lorder[l[i]] = i;
  println(hlog, "lorder[0] = ", l[0]);

  for(int i=0; i<ls; i++) {
    // int dctr = celldistance(l[i], cwt.at);
    int dfirst = celldistance(l[i], l[0]);
    // int dtotal = celldistance(l[0], cwt.at);
    //if(dtotal == dfirst + dctr && dfirst < cutoff) ;
    //else
    if(dfirst >= cutoff)
      consider.insert(l[i]); 
    }
  
  new_neighborhoods.resize(ls);

  for(cell *c: cl.lst) {
    bool ok = true;
    forCellEx(c1, c) if(!consider.count(c1)) ok = false;
    if(!ok) continue;
    vector<cell*> nn = {c};
    forCellEx(c1, c) nn.push_back(c1);
    for(auto cx: nn) multiplicity[cx]++;
    int max_order = -1;
    for(auto cx: nn) max_order = max(max_order, lorder[cx]);
    
    new_neighborhoods[max_order].push_back(nn);
    }
  
  int typecount = 2;
  if(wfctype == 3) typecount++;
  vector<cell*> inpath;
  
  typedef unsigned long long code_t;
  const int cpo = 2;
  const code_t mask = (1<<cpo)-1;
  
  struct freqdata {
    code_t code;
    int id;
    double prob;
    };
  
  vector<freqdata> freq = { freqdata{0, 0, 1} };
  vector<freqdata> nfreq;
  
  trans.clear();
  
  int nextcode = 1;
  
  int all = 2 << (cpo * 8);
  
  vector<double> mul(all, 0);
  
  for(int a=0; a<all; a++) {
    vector<int> v;
    int a0 = a;
    while(a0) { v.push_back(a0 & mask); a0 >>= cpo; }
    if(isize(v) < 2) continue;
    v.pop_back();
    int cur = v.back();
    v.pop_back();
    int changes = 0;
    for(int i=1; i<isize(v); i++) if(v[i] == 1 && v[i-1] == 0) changes++;
    if(v[0] == 1 && v.back() == 0) changes++;
    int nes = 0;
    for(int vv: v) nes += vv;
    
    switch(wfctype) {
      case 0:
        mul[a] = nes == 2;
        break;
      case 1:
        mul[a] = changes == 1;
        break;
      case 2:
        mul[a] = changes == 2;
        break;
      case 3: {
        int ca=0, cb=0, cc=0, cd=0, ce=0;
        int cur1 = (cur+1) % 3;
        int cur2 = (cur+2) % 3;
        int same = 0;
        int in1 = 0;
        int in2 = 0;
        for(int i=0; i<isize(v); i++) {
          auto va = v[i], vb = v[(i+1)%isize(v)];
          if(va == cur && vb == cur1) ca++;
          if(va == cur && vb == cur2) cb++;
          if(va == cur1 && vb == cur) cc++;
          if(va == cur2 && vb == cur) cd++;
          if(va == cur1 && vb == cur2) ce++;
          if(va == cur2 && vb == cur1) ce++;
          if(va == cur) same++;
          if(va == cur1) in1++;
          if(va == cur2) in2++;
          }
        mul[a] = ca==1 && cb==1 && cc==1 && cd==1 && ce==0; // && in1 >= 2; // && in2 >= 2;
        if(mul[a]) println(hlog, v, cur, " => ", mul[a]);
        break;
        }
      }
    
    // println(hlog, a, ": ", v, " -> ", mul[a]);
    }
  
  /*
  mul[0] = 0;
  for(int a=0; a<8; a++)
    mul[1<<a] = 1;
  */

  long long tfreq = 0;

  if(animated)
  for(int j=0; j<ls; j++)
      for(auto& nns: new_neighborhoods[j]) for(auto c: nns) c->landparam = 0x202020;
  
  for(int i=0; i<ls; i++) if(consider.count(l[i])) {
    tfreq += isize(freq);
    string s = lalign(0, i, "/", ls, ": freqs = ", isize(freq), " inpath ", isize(inpath));
    println(hlog, s);

    if(animated) {
      for(int j=0; j<i; j++)
        for(auto& nns: new_neighborhoods[j]) for(auto c: nns) c->landparam = multiplicity[c] ? 0xFFFF00 : 0x00FF00;
      for(auto& nns: new_neighborhoods[i]) for(auto c: nns) c->landparam = 0xFFFFFF;

      history::progress(s);
      }

    inpath.push_back(l[i]);
  
    int sh = cpo * (isize(inpath)-1);

    for(auto& p: freq) if(p.prob) for(char c=0; c<typecount; c++) {
      nfreq.emplace_back(p);
      nfreq.back().code |= (code_t(c) << sh);
      }
    freq = std::move(nfreq);
    nfreq = {};
    // println(hlog, "inpath = ", inpath);
    // println(hlog, "freqs = ", isize(freq));
    if(isize(freq) > 500000000) {
      println(hlog, "freq overflow");
      exit(3);
      }
    fflush(stdout);
    
    global_list = cl.lst;

    for(auto& nns: new_neighborhoods[i]) {
      // println(hlog, "acknowledge ", nns);
      fflush(stdout);
            
      vector<int> sids;
      for(auto cz: nns) {
        for(int id=0; id<isize(inpath); id++) if(inpath[id] == cz) sids.push_back(id);
        }
      
      for(auto& p: freq) {
        unsigned code = 1;
        for(int v: sids) code = (code << cpo) | ((p.code >> (cpo*v)) & mask);
        p.prob *= mul[code];
        }

      for(auto cz: nns) {
        multiplicity[cz]--;
        if(multiplicity[cz] == 0) {
          ctf.push_back(cz);
          // println(hlog, "remove ", cz, " (#", lorder[cz], ") from ", inpath);
          fflush(stdout);
          int is = isize(inpath)-1;
          int id = 0;
          while(id <= is && inpath[id] != cz) id++;
          if(id > is) { 
            println(hlog, "error");
            exit(4);
            }
          inpath[id] = inpath[is];
          inpath.resize(is);
          for(auto& p: freq) if(p.prob) {
            nfreq.push_back(p);
            int tid = (p.code >> (cpo*id)) & mask;
            code_t& s = nfreq.back().code;
            s &=~ (mask << (cpo*id));
            if(id < is) s |= ((s >> (cpo*is)) & mask) << (cpo*id);
            s &=~ (mask << (cpo*is));
            // s.resize(is);
            // if(id < is) s[id] = p.code[is];
            nfreq.back().id = (nfreq.back().id << cpo) | tid;
            }
          
          freq.clear();
          sort(nfreq.begin(), nfreq.end(), [] (const freqdata& a, const freqdata& b) { return a.code < b.code; });
          for(int i=0; i<isize(nfreq); i++) {
            auto &next = nfreq[i];
            if(i == 0 || nfreq[i].code != nfreq[i-1].code) {
              freq.push_back(nfreq[i]);
              freq.back().id = nextcode++;
              freq.back().prob = 0;
              }
            
            auto& group = freq.back();
            
            group.prob += next.prob;

            ttrans nt;
            nt.news = group.id;
            nt.olds = next.id >> cpo;
            nt.proportion = next.prob / group.prob;
            nt.id = next.id & mask;
            trans.push_back(nt);
            }
          
          nfreq.clear();            
          }
        }
      }
    }
  
  if(isize(freq)) println(hlog, "last freq = ", freq[0].prob);
  println(hlog, "freq size = ", isize(freq));
  println(hlog, "tfreq = ", format("%lld", tfreq));
  println(hlog, "trans size = ", isize(trans));
  println(hlog, "next code = ", nextcode);
  clearMessages();
  
  wfc_build();
  }

bool wfc_handleKey(int sym, int uni) {  
  if(sym == '5') {
    wfc_build();
    return true;
    }
  return false;
  }

void hwrite(hstream& f, const ttrans& t) {
  hwrite(f, t.news);
  hwrite(f, t.olds);
  hwrite(f, t.proportion);
  hwrite(f, t.id);
  }

void save(string s) {
  fhstream f(s, "wb");
  vector<int> indices;
  for(auto cf: ctf)
    for(int i=0; i<isize(global_list); i++)
      if(global_list[i] == cf) indices.push_back(i);
  
  hwrite(f, wfctype);
  hwrite(f, cutoff);
  hwrite(f, wfcrad);
  hwrite(f, indices);
  hwrite(f, trans);
  }

auto wfc_hook = 
  addHook(hooks_args, 100, [] {
    using namespace arg;

    if(0) ;
    else if(argis("-dynamic-wfc")) {
      start_game();
      shift(); wfctype = argi();
      shift(); cutoff = argi();
      shift(); wfcrad = argi();
      wfc();
      }
    else if(argis("-dynamic-wfc-save")) {
      shift(); save(args());
      }
    else if(argis("-wfc-regen-key")) {
      rogueviz::rv_hook(hooks_handleKey, 100, wfc_handleKey);
      }
      
    else return 1;
    return 0;
    });

}
}
