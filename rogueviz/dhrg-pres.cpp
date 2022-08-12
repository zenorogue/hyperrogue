// -noscr -slides DHRG rv_latex=1 -slide-textoff -title c000

#include "rogueviz.h"

#define RVPATH HYPERPATH "rogueviz/"

namespace dhrg {
  void graphv(std::string s);
  extern double graph_R;
  extern int N;
  extern int iterations;
  void fixedges();
  void unsnap();
  bool dhrg_animate(int sym, int uni);
  void rvcoords();
  void clear();
  
  void prepare_pairs();
  std::vector<int> path(int src);
  int get_actual(int src);
  void prepare_goal(int goal);
  }

namespace hr {

using namespace rogueviz::pres;

int find_vertex(string name) { 
  int id = 0;
  for(auto& v: rogueviz::vdata) {
    if(v.name == name) return id;
    id++;
    }
  return -1;
  }

void zoom_to(cell *c, int steps) {
  cwt.at = c;
  for(int i=0; i<steps; i++) {
    cell *next = cwt.at;
    forCellEx(c1, cwt.at) if(celldist(c1) < celldist(next)) next = c1;
    cwt.at = next;
    }
  playermoved = true;
  cwtV = ggmatrix(cwt.at);
  }

vector<int> path_to_draw;

int me, them;

void greedy_test() {
  me = find_vertex("Eryk_Kopczynski");
  auto me2 = find_vertex("Erich_Grädel");
  dhrg::prepare_pairs();  
  dhrg::rvcoords();
  them = find_vertex(dhrg::iterations ? "Florian_Willich" : "Stéphane_Chrétien");
  for(int goal=0; goal<dhrg::N; goal++) {
    dhrg::prepare_goal(goal);
    auto p = dhrg::path(me2);
    if(p.back() == goal) {
      println(hlog, "actual = ", dhrg::get_actual(me), " p = ", p, " ~ ", rogueviz::vdata[p.back()].name);
      }
    if(goal == them) {
      path_to_draw = {me};
      for(auto who: p) path_to_draw.push_back(who);
      }
    }
  }

void launch_sea() {
  enable_canvas_backup('0');  
  start_game();
  dhrg::graphv("rogueviz/dhrg-data/sea-ppl");      
  resetview();
  }

void gamedata(hr::gamedata* gd) { 
  if(true) {
    // gd->store(search_for); // assuming 1 player!
    gd->store(rogueviz::vdata);
    gd->store(rogueviz::labeler);
    gd->store(rogueviz::legend);
    gd->store(rogueviz::edgeinfos);
    gd->store(rogueviz::edgetypes);
    // gd->store(rogueviz::relmatrices);
    }
  }

void draw_huge_circle_to(shiftmatrix& M, ld rad, ld& last, ld next) {
  hyperpoint l = xspinpush0(last, rad);
  hyperpoint sl;
  applymodel(M*l, sl);
  hyperpoint n = xspinpush0(next, rad);
  hyperpoint sn;
  applymodel(M*n, sn);
  if(sqhypot_d(2, sl-sn) < 0.02) {
    last = next;
    curvepoint(n);
    }
  else {
    draw_huge_circle_to(M, rad, last, (last+next)/2);
    draw_huge_circle_to(M, rad, last, next);
    }
  }

void draw_huge_circle(shiftmatrix& M, ld rad, color_t line, color_t fill) {
  ld last_angle = 0;  
  /* we do every 5 to avoid the case where all the basic points are far away from the part we see, */
  /* and thus draw_huge_circle_to fail. Could be improved */
  for(int i=0; i<=360; i += 5)
    draw_huge_circle_to(M, rad, last_angle, i * degree);
  queuecurve(M, line, fill, PPR::FLOOR);
  }

shiftpoint lastpoint;
void prettylineto(const shiftpoint& pt) {
  hyperpoint sh;
  applymodel(lastpoint, sh);
  hyperpoint sn;
  applymodel(pt, sn);
  hyperpoint sm;
  applymodel(mid(lastpoint, pt), sm);
  ld dist = abs(((sm - sh) ^ (sn - sh))[2]) / hypot_d(2, sn - sh);
  if(dist < 0.001) {
    lastpoint = pt;
    curvepoint(unshift(pt));
    }
  else {
    prettylineto(mid(lastpoint, pt));
    prettylineto(pt);
    }
  }

void prettylineto_easy(const shiftpoint& pt, int lev) {
  if(lev == 0) {
    lastpoint = pt;
    curvepoint(unshift(pt));
    }
  else {
    prettylineto_easy(mid(lastpoint, pt), lev-1);
    prettylineto_easy(pt, lev-1);
    }
  }

bool snapped = false;
bool showgrid = false;

color_t dhrg_grid = 0x00FF00A0;

void graph_visuals(presmode mode) {
  if(mode == pmStart) {
    slide_backup(patterns::canvasback, 0xFF00);
    slide_backup(canvas_default_wall, waInvisibleFloor);
    slide_backup(rogueviz::showlabels, true);
    slide_backup(rogueviz::ggamma, 2);
    tour::slide_backup(vid.use_smart_range, 2);
    tour::slide_backup(vid.smart_range_detail, 8);
    tour::slide_backup(smooth_scrolling, true);
    tour::slide_backup(stdgridcolor, dhrg_grid);
    tour::slide_backup(vid.grid, !showgrid);
    tour::slide_backup(mapeditor::drawplayer, false);
    tour::slide_backup(vid.axes, 0);
    tour::slide_backup(no_find_player, true);
    tour::slide_backup(rogueviz::highlight_target, false);
    tour::slide_backup(draw_centerover, false);
    
    rogueviz::rv_hook(hooks_drawcell, 100, [] (cell *c, const shiftmatrix& V) {
      if(showgrid) {
        c->wall = waNone;
        c->landparam = 0x306030;
        }
      else {
        c->wall = waInvisibleFloor;
        c->landparam = 0x3060030;
        }
      return false;
      });

    rogueviz::rv_hook(hooks_handleKey, 101, [] (int sym, int uni) {
      if((cmode & sm::NORMAL) && uni == 'r') {
        stop_game();
        dhrg::clear();
        launch_sea();
        return true;
        }
      return false;
      });

    slide_backup(distance_from, dfStart);
    }
  }

void swap_snap() {
  snapped = !snapped;
  if(snapped) { for(auto& v: rogueviz::vdata) v.m->at = Id; dhrg::fixedges(); }
  if(!snapped) dhrg::unsnap();
  }

void dhrg_hooks() {
  rogueviz::rv_hook(hooks_handleKey, 100, dhrg::dhrg_animate);
  rogueviz::graph_rv_hooks();
  }

void view_treelike(presmode mode) {
  if(mode == pmStart) {
    stop_game();
    set_variation(eVariation::pure);
    tour::slide_backup(vid.creature_scale, 0.5);
    start_game();
    tour::slide_backup(viewdists, true);
    using linepatterns::patTree;
    tour::slide_backup(patTree.color, 0xFFFFFFFF);
    tour::slide_backup(patTree.multiplier, 5);
    tour::slide_backup(mapeditor::drawplayer, false);
    tour::slide_backup(number_coding, ncType);
    tour::slide_backup(distance_from, dfStart);
    tour::slide_backup(no_find_player, true);
    tour::slide_backup(smooth_scrolling, true);
    tour::slide_backup(vid.axes, 0);
    tour::slide_backup(show_distance_lists, false);
    tour::slide_backup(draw_centerover, false);
    }
  }

vector<cell*> cpath;

void prepare_cpath() {
  vector<cell*> possible;
  for(cell *c: dcal) if(celldist(c) == 2) possible.push_back(c);
  println(hlog, "1 possible size = ", isize(possible));
  if(possible.empty()) return;
  cell *lca = hrand_elt(possible);
  possible.clear();
  for(cell *c: dcal) if(celldist(c) == 5 && celldistance(c, lca) == 3) possible.push_back(c);
  println(hlog, "2 possible size = ", isize(possible));
  if(possible.empty()) return;
  cell *start = hrand_elt(possible);
  cell *final = hrand_elt(possible);
  cpath = {start};
  while(cpath.back() != final) {
    forCellEx(c1, cpath.back()) 
      if(celldistance(c1, final) < celldistance(cpath.back(), final) && celldist(c1) <celldist(cpath.back())) {
        cpath.push_back(c1);
        goto next;
        }
    forCellEx(c1, cpath.back()) 
      if(celldistance(c1, final) < celldistance(cpath.back(), final)) {
        cpath.push_back(c1);
        goto next;
        }
    next: ;
    }
  println(hlog, "cpath = ", cpath);
  }

string defs = 
  "\\def\\map{m}"
  "\\def\\VofH{V}"
  "\\def\\dist{\\delta}"
  "\\def\\ra{\\rightarrow}"
  "\\def\\bbH{\\mathbb{H}}"
  "\\def\\bbE{\\mathbb{E}}"
  "\\renewcommand{\\rmdefault}{\\sfdefault}\\sf"
  ;

slide dhrg_slides[] = {
  {"Title Page", 10, LEGAL::ANY | QUICKSKIP | QUICKGEO | NOTITLE, "", 
    [] (presmode mode) {
      white_screen(mode);
      graph_visuals(mode);      
      uses_game(mode, "sea_graph", launch_sea, dhrg_hooks);
      if(mode == pmStart) {
        slide_backup(nohud, false);
        slide_backup(pconf.scale, .65);
        slide_backup(bright, true);
        slide_backup(modelcolor, 0xC0C0C0FF);
        slide_backup(stdgridcolor, 0xFFFFFFFF);
        slide_backup(vid.linewidth, vid.linewidth * 3);
        }
      add_stat(mode, [] {
        tour::slide_backup(nohelp, true);
        gamescreen();
        dialog::init();
        tour::slide_backup(no_find_player, true);
        dialog::addTitle("Discrete Hyperbolic Random Graph Model", dialog::dialogcolor, 150);
        dialog::addBreak(1600);
        dialog::addTitle(" ", dialog::dialogcolor, 150);
        dialog::addItem("paper on arXiv",  '5');
        dialog::display();
        return true;
        });
      if(mode == pmKey) open_url("https://arxiv.org/abs/2109.11772");
      }
    },
  
  {"Animated Hyperbolic Plane", 10, LEGAL::ANY | QUICKGEO | NOTITLE, "Scroll the map to learn how hyperbolic geometry works.", 
    [] (presmode mode) {
      setCanvas(mode, 'F');
      if(mode == pmStart) {
        stop_game();
        tour::slide_backup(smooth_scrolling, true);
        tour::slide_backup(colortables['F'][0], 0x10F010);
        tour::slide_backup(colortables['F'][1], 0x104010);
        tour::slide_backup(patterns::canvasback, 0x10F010);
        tour::slide_backup(vid.use_smart_range, 2);
        tour::slide_backup(vid.smart_range_detail, 5);
        tour::slide_backup(mapeditor::drawplayer, false);
        tour::slide_backup(no_find_player, true);
        tour::slide_backup(draw_centerover, false);
        tour::slide_backup(vid.axes, 0);
        start_game();
        }        
      // mine_slide(mode, geom_pentagos, cl_pentagons, chessboard_assigner);
      no_other_hud(mode);
      }
    },  
  
  {"Hyperbolic Geometry is Tree-Like", 999, LEGAL::NONE | QUICKGEO, 
    "Hyperbolic geometry has a tree-like structure, and expands exponentially.\n\nThis tree-like structure has been used in applications, for visualizing and modeling tree-like hierarchical structures.\n\nPress '5' to show the expansion.\n\n"
    ,
    [] (presmode mode) {
      setCanvas(mode, '0');
      view_treelike(mode);
      // mine_slide(mode, geom_pentagos, cl_pentagons, chessboard_assigner);
      if(mode == pmFrame) clearMessages();
      if(mode == pmKey) show_distance_lists = !show_distance_lists;
      no_other_hud(mode);
      add_temporary_hook(mode, hooks_prestats, 250, [] { 
        if(viewdists && show_distance_lists) 
          get_expansion().view_distances_dialog();
        return false;
        });
      add_temporary_hook(mode, dialog::hooks_display_dialog, 110, [] () {
        if((cmode && sm::EXPANSION) && show_distance_lists) {
          vector<dialog::item> it = std::move(dialog::items);
          dialog::items.clear();
          for(auto& d: it) {
            if(d.key == 'S' || d.key == 'C') continue;
            if(d.body == "" && d.value == "") continue;
            string latexbody = d.body + d.value;
            auto rep = [&] (string s, string t) {
              while(latexbody.find(s) != string::npos)
                latexbody.replace(latexbody.find(s), isize(s), t);
              };
            rep("Θ", "\\Theta");
            rep("...", "\\ldots");
            rep("ᵈ", "^d");
            rep(") (", ")\\ (");
            if(latexbody.find(": ") != string::npos) {
              latexbody = "\\makebox[6em]{" + latexbody + "}";
              rep(": ", ":\\hfill");
              }
            dialog_may_latex(defs + "$" + latexbody + "$", d.body, d.color, 100);
            }
          }
        });
      }
    },

  {"Scale-Free Networks", 999, LEGAL::NONE | QUICKGEO | NOTITLE, "One kind of such a tree-like hierarchical structure is scale-free networks.",
    [] (presmode mode) {
      latex_slide(mode, defs + R"=(

   {\color{remph}Scale-free networks} \hfill \rule{0cm}{0cm}
  
\begin{itemize}
  \item social networks, also ubiquitious in nature and technology
  \item power-law distribution of vertex degrees: $P(X>x) \sim x^{-(\beta-1)}$
  \item high clustering coefficient: if $v$ and $w$ are connected to $u$, they are likely to be also connected
  \item how to model them mathematically?
  \end{itemize}

    )=");
    }},

  {"Hyperbolic Random Graph model", 999, LEGAL::NONE | QUICKGEO | NOTITLE, "Hyperbolic Random Graph model is a theoretical model appropriate for modeling scale-free networks. It exhibits the basic properties (while simpler models do not).",
    [] (presmode mode) {
      latex_slide(mode, defs+R"=(

   \rule{0cm}{0cm} \hfill {\color{remph}Hyperbolic Random Graph model (HRG)} \hfill \rule{0cm}{0cm}
  
   \rule{0cm}{0cm} \hfill {\small (Krioukov, Papadopoulos, Kitsak, Boguñá 2010)} \hfill \rule{0cm}{0cm}

\begin{itemize}
  \item parameters: $n$ (number of vertices), $R$ (radius), $T$ (temperature), $\alpha$
  \item Each vertex $v \in \VofH = \{1,\ldots,n\}$ is independently randomly assigned a point $\map(v)$ in a {\color{remph} hyperbolic disk} of radius $R$
  \item the density of the distribution of $r_v \in [0,R]$ is given by $f(r) = { {\alpha \sinh(\alpha r)} \over {\cosh(\alpha R)-1}}$.
  \item $(v,w)$ connected with probability $p(\dist(\map(v),\map(w)))$,
  \item $p(d) = {1 \over 1+e^{(d-R)/2T}}$.
  \end{itemize}

    )=");
    }},

  {"HRG embedding", 999, LEGAL::NONE | QUICKGEO | NOTITLE, "We can also embed a real-world scale-free network into the hyperbolic plane. Again, we want to do this so that the points which are embedded close to each other are likely to be connected.",
    [] (presmode mode) {
      latex_slide(mode, defs+R"=(
   {\color{remph}MLE HRG embedding problem:}
   
   Given a graph $(V,E)$, find $\map: V \ra \bbH^2$.
   
   \vskip 5mm
   
   Quality of the embedding = log-likelihood $\sum_{v,w} \log p'(v,w)$
   
   \vskip 5mm

   where $p'(v,w)=p(\dist(\map(v),\map(w))$ if the edge exists, $1-p$ otherwise
   
   \vskip 1cm

   {\color{remph}BFKL embedder (Bläsius, Friedrich, Krohmer, Laue 2016)}
   
   A HRG embedding algorithm working in ${\tilde O}(|V|)$
   )="); }},

  {"Embedded Graph: SEA 2022 and coauthorships", 20, LEGAL::HYPERBOLIC | QUICKGEO,
    "The following slide is a visualization of the SEA 2022 graph.",

    [] (presmode mode) {

      if(mode == pmStart) showgrid = false;
      graph_visuals(mode);

      uses_game(mode, "sea_graph", launch_sea, dhrg_hooks);

      rogueviz::rv_hook(hooks_frame, 100, [] {      
        draw_huge_circle(ggmatrix(currentmap->gamestart()), dhrg::graph_R, 0xFFFFFFFF, 0x101010FF);
        });
      
      if(mode == pmKey) {
        int id = find_vertex("Thomas_Bläsius");
        auto& v = rogueviz::vdata[id];
        zoom_to(v.m->base, 5);
        println(hlog, "TB found!");
        }
      no_other_hud(mode);
      }
    },

  {"Embedded Graph in a grid", 20, LEGAL::HYPERBOLIC | QUICKGEO,
    "Our idea is to consider Discrete Hyperbolic Random Graph model, that is, where the nodes of the network are mapped to tiles, not the points of the hyperbolic plane. Press '5' to show the grid.",

    [] (presmode mode) {

      if(mode == pmStart) showgrid = true;
      graph_visuals(mode);
      
      uses_game(mode, "sea_graph", launch_sea, dhrg_hooks);

      if(mode == pmKey) vid.grid = !(showgrid = !showgrid);
      no_other_hud(mode);
      }
    },

  {"Discrete Hyperbolic Random Graph", 20, LEGAL::HYPERBOLIC | QUICKGEO,
    "Press a key ('5') to convert our network to DHRG (and back).",

    [] (presmode mode) {

      if(mode == pmStart) showgrid = true;

      graph_visuals(mode);
      
      uses_game(mode, "sea_graph", launch_sea, dhrg_hooks);

      if(mode == pmKey) swap_snap();
      no_other_hud(mode);
      }
    },

  {"HRG vs DHRG", 999, LEGAL::NONE | QUICKGEO | NOTITLE, "Why would this be a good idea?",
    [] (presmode mode) {
      latex_slide(mode, defs+R"=(
   {\color{remph}DHRG vs HRG}   
   \begin{itemize}   
   \item distances are large ($>10$ tiles) so the loss of precision is small
   \item tessellation distances are good approximations of continuous distances \\
     ($\bbH^2$: variance $O(d)$, $\bbE^2$: variance $O(d^2)$)
   \item combinatorial representation avoids the numerical issues
   \item easier to visualize/think about (discrete algorithms, automata theory)
   \end{itemize}
   )="); }},

  {"Basic Algorithmic Properties", 999, LEGAL::NONE | QUICKGEO | NOTITLE, "Basic algorithmic properties of hyperbolic tessellations... press '5' to show some shortest distances.",
    [] (presmode mode) {
      setCanvas(mode, '0');
      view_treelike(mode);
      if(mode == pmStart) tour::slide_backup(number_coding, ncNone);
      if(mode == pmStart) tour::slide_backup(vid.creature_scale, 1);
      if(mode == pmStart) cpath.clear();
      if(mode == pmKey) prepare_cpath();
      rogueviz::rv_hook(hooks_frame, 100, [] {
        if(cpath.size()) {
          static bool drawn = true;
          if(drawn) println(hlog, "drawn"), drawn = false;
          vid.linewidth *= 6;
          lastpoint = tC0(ggmatrix(cpath[0]));
          for(cell *c: cpath) prettylineto_easy(tC0(ggmatrix(c)), 4);
          queuecurve(shiftless(Id), 0xFF00C0, 0, PPR::LINE);
          vid.linewidth /= 6;
          for(cell *c: cpath)
            queuepolyat(ggmatrix(c), cgi.shDisk, 0xFF00FF, PPR::LINE);
          queuepolyat(ggmatrix(cpath[0]), cgi.shHugeDisk, 0xFF00FF, PPR::LINE);
          queuepolyat(ggmatrix(cpath.back()), cgi.shHugeDisk, 0xFF00FF, PPR::LINE);
          }
        });      
      latex_slide(mode, defs+R"=(
   {\color{remph}Hyperbolic Tessellations: 
   
   basic algorithmic properties}   
   \begin{itemize}
   \item we can represent tiles as objects
   
   (generated lazily)
   \item tiles adjacent to $t$ can be found
   
   in amortized time $O(1)$
   \item just like in a tree,
   
   distance $d$ between $t$ and $u$
   
   can be found in time $O(d)$
   \end{itemize}
   )=", sm::SIDE, 90); }},

  {"Distance Tally Counter", 999, LEGAL::NONE | QUICKGEO | NOTITLE, "Computing the distance between TWO tiles in O(d) is maybe not very impressive... but we can do it en masse!",
    [] (presmode mode) {
      latex_slide(mode, defs+R"=(
   {\color{remph}Distance Tally Counter} 

    Data structure with the following operations:
   \begin{itemize}
   \item {\sc Init}, which initializes the multiset $A$ to empty
   \item {\sc Add}($u$,$x$), which adds tile $u$ to the multiset $A$ with multiplicity $x$ (which can be negative)
   \item {\sc Count}($t$), which returns an array $T$ such that $T[d]$ is the number of elements of $A$ in distance $d$ from tile $t$
   \end{itemize}
   
   For our hyperbolic tessellations,
   there is an implementation of distance tally counter where all the operations are executed in $O(R^2)$,
   where $R$ is the maximum distance from the central tile.
   )="); }},

  {"Applications of DTC", 999, LEGAL::NONE | QUICKGEO | NOTITLE, "So what are the applications of such algorithms?",
    [] (presmode mode) {
      latex_slide(mode, defs+R"=(
   {\color{remph}Applications of the Distance Tally Counter:} 

   \begin{itemize}
   \item compute the log-likelihood in time $O(n R^2+m R)$
   \item after preprocessing, computing the log-likelihood assuming other values of parameters in $O(R)$
   \item local search: improve the embedding by moving vertices in a way which improves the log-likelihood, in time $O(R^2n + Rm)$ per iteration
   \end{itemize}
   )="); }},

  {"Local Search", 20, LEGAL::HYPERBOLIC | QUICKGEO,
    "Press a key ('5') to perform local search.",

    [] (presmode mode) {

      if(mode == pmStart) showgrid = true;

      graph_visuals(mode);
      
      uses_game(mode, "sea_graph", launch_sea, dhrg_hooks);

      no_other_hud(mode);
      }
    },

  {"Experimental setup", 30, LEGAL::NONE | NOTITLE, "The setup of our experiments on the DHRG model.", 
    [] (presmode mode) {
      latex_slide(mode, defs+R"=(
   {\color{remph}Our experimental setup:} 

   \begin{itemize}
   \item {\bf Embedding:} we use the BFKL embedder to map a network to the hyperbolic plane (HRG model)
   \item {\bf Discretization:} we convert the HRG embedding to a DHRG embedding
   \item {\bf Local search:} we apply the local search algorithm (20 iterations)
   \item {\bf De-discretization:} we convert the DHRG embedding back to a HRG embedding
   \end{itemize}
   )="); }},

  {"Real-world networks", 30, LEGAL::NONE | NOTITLE, "Our experiments on the real-world networks.", 
    [] (presmode mode) {
      latex_slide(mode, defs+R"=(
\centering
%\addtolength{\tabcolsep}{-5pt}
\resizebox{\linewidth}{!}{
\begin{tabular}{|l|rrrr|l|rrrr|rrrrrr|}
\hline
name & $n$ & $m$ & $R$ & $\alpha$ & grid & -$L_2$ & $D$ & $L$ & $DD$ & MB & $t_m$ [s] & $t_l$ [s] & $t_e$ [s] & $t_c$ [s] & $t_b$ [s]\\
\hline
Fb & 4309 & 88234 & 12.57 & 0.755 & $G_{710}$ & 176131 &  1.04 &  0.93 &  0.97 & 40 & 0.196 &  0.03 & 10 & 0.35 & 0.048\\
Fb & 4309 & 88234 & 12.57 & 0.755 & $G_{810}$ & 176131 &  1.07 &  0.92 &  0.98 & 54 & 0.183 &  0.03 & 8 & 0.5 & 0.048\\
F09 & 74946 & 537972 & 20.90 & 0.855 & $G_{710}$ & 3954627 &  1.04 &  0.86 &  0.90 & 2010 & 5.432 &  1.16 & 222 & 131 & 0.896\\
F09 & 74946 & 537972 & 20.90 & 0.855 & $G_{810}$ & 3954627 &  1.06 &  0.84 &  0.90 & 1866 & 4.634 &  0.81 & 176 & 128 & 0.896\\
Sd & 77352 & 327431 & 26.00 & 0.610 & $G_{710}$ & 2091651 &  1.25 &  0.72 &  0.92 & 2659 & 5.326 &  1.05 & 201 & 130 & 0.292\\
Sd & 77352 & 327431 & 26.00 & 0.610 & $G_{810}$ & 2091651 &  1.27 &  0.71 &  0.92 & 2253 & 4.618 &  0.78 & 158 & 126 & 0.292\\
Am & 334863 & 925872 & 24.11 & 0.995 & $G_{710}$ & 6957174 &  1.04 &  0.86 &  0.91 & 5677 & 23.34 &  5.40 & 721 & 2690 & 1.444\\
Am & 334863 & 925872 & 24.11 & 0.995 & $G_{810}$ & 6957174 &  1.04 &  0.85 &  0.90 & 4868 & 19.76 &  3.92 & 576 & 2811 & 1.444\\
F11 & 405270 & 2345813 & 26.34 & 0.715 & $G_{710}$ & 20028756 &  1.22 &  0.76 &  0.93 & 9995 & 30.36 &  7.36 & 1349 & 3715 & 5.216\\
F11 & 405270 & 2345813 & 26.34 & 0.715 & $G_{810}$ & 20028756 &  1.22 &  0.76 &  0.93 & 8940 & 25.84 &  5.38 & 1113 & 3636 & 5.216\\
Go & 855804 & 4291354 & 26.06 & 0.865 & $G_{710}$ & 22762281 &  1.30 &  0.75 &  0.98 & 18226 & 64.75 & 16.05 & 2363 & 16618 & 3.560\\
Go & 855804 & 4291354 & 26.06 & 0.865 & $G_{810}$ & 22762281 &  1.32 &  0.75 &  0.99 & 15314 & 54.31 & 10.93 & 1823 & 15818 & 3.560\\
Pa & 3764118 & 16511741 & 28.74 & 0.995 & $G_{810}$ & --- & --- &  0.90 & --- & 66396 & 250.6 & 73.65 & 9335 & --- & 41.24\\
\hline
\end{tabular}} % L3 (Patents) = 208618134
\vskip 2em
\small
Facebook (Fb), Slashdot (Sd), Amazon (Am), Google (Go), and Patents (Pa) networks from SNAP database.
Loglikelihood ratios after discretization (D), local search (L) and de-discretization (DD).
Times as $t_m$ (converting HRG to DHRG), $t_l$ (computing log-likelihood),
$t_e$ (local search), $t_c$ (computing loglikelihood in $O(n^2)$). 
%#; F09 and F11 are GitHub networks. MB is the amount of memory in megabytes, and time is in seconds.
   )="); }},

  {"Changing the tessellation", 30, LEGAL::NONE | NOTITLE, "How does this change when we change the tessellation?", 
    [] (presmode mode) {
      latex_slide(mode, defs+R"=(
\begin{center}
%\resizebox{\linewidth}{!}
{
\begin{tabular}{|l|rrr|rr|rrr|}
\hline
grid      & $L_3$   & $L_5$   & $L_7$   & MB    & \#it & $t_m$ [s] & $t_l$ [s] & $t_e$ [s] \\
\hline                
$G_{810}$ & -187738 & -172018 & -172585 & 46    & 37   & 0.180 & 0.027 & 14.17 \\
$G_{710}$ & -182721 & -170074 & -170873 & 40    & 29   & 0.194 & 0.030 & 12.13 \\
$G_{711}$ & -179125 & -167991 & -168445 & 61    & 23   & 0.281 & 0.058 & 17.82 \\
$G_{720}$ & -179977 & -168105 & -168817 & 98    & 71   & 1.025 & 0.094 & 91.87 \\
$G_{721}$ & -178108 & -167407 & -167824 & 146   & 99*  & 1.359 & 0.208 & 282.0 \\
$G_{753}$ & -177254 & -166889 & -167648 & 1050  & 99*  & 4.446 & 3.059 & 4999  \\
$B_{2}  $ & -180354 & -168055 & -168338 & 47    & 15   & 1.278 & 0.037 & 17.17 \\
$B_{1.1}$ & -180112 & -169019 & -168134 & 54    & 11   & 1.513 & 0.041 & 4.362 \\
$B_{1.0}$ & -179554 & -168717 & -168214 & 53    & 59   & 1.555 & 0.042 & 8.830 \\
$B_{0.9}$ & -179500 & -168973 & -168282 & 56    & 45   & 1.607 & 0.042 & 22.56 \\
$B_{0.5}$ & -179742 & -168906 & -168017 & 62    & 7    & 2.158 & 0.046 & 6.182 \\
$\{5,4\}$ & -195952 & -173641 & -175671 & 38    & 20   & 0.159 & 0.024 & 5.700 \\
\hline                                                   
\end{tabular}}
\vskip 1em
$\begin{smallmatrix} \includegraphics[width=.10\textwidth]{../rogueviz/dhrg-data/tes/tiling-hep.pdf} \\ G_{710} \end{smallmatrix}$
$\begin{smallmatrix} \includegraphics[width=.10\textwidth]{../rogueviz/dhrg-data/tes/tiling-oct.pdf} \\ G_{810} \end{smallmatrix}$
$\begin{smallmatrix} \includegraphics[width=.10\textwidth]{../rogueviz/dhrg-data/tes/tiling-711.pdf} \\ G_{711} \end{smallmatrix}$
$\begin{smallmatrix} \includegraphics[width=.10\textwidth]{../rogueviz/dhrg-data/tes/tiling-720.pdf} \\ G_{720} \end{smallmatrix}$
$\begin{smallmatrix} \includegraphics[width=.10\textwidth]{../rogueviz/dhrg-data/tes/tiling-721.pdf} \\ G_{721} \end{smallmatrix}$
$\begin{smallmatrix} \includegraphics[width=.10\textwidth]{../rogueviz/dhrg-data/tes/tiling-bin.pdf} \\ B_{1.0} \end{smallmatrix}$
$\begin{smallmatrix} \includegraphics[width=.10\textwidth]{../rogueviz/dhrg-data/tes/tiling-54.pdf} \\ \{5,4\} \end{smallmatrix}$

Experimental results on the Facebook network ($L_2=-176131$).
\end{center}
   )="); }},

  {"Simulated graphs", 30, LEGAL::NONE | NOTITLE, "Experiments on simulated graphs.", 
    [] (presmode mode) {
      latex_slide(mode, defs+R"=(
We generate 1000 graph for every value of $n$ and $T$, $\alpha=0.75$, $R=2\log(n)-1.$
\begin{center}
\raisebox{4em}{$T=0.1$}\hfill \includegraphics[width=.8\linewidth]{../rogueviz/dhrg-data/erc_eerc_gz_3x1.pdf}

\raisebox{4em}{$T=0.7$}\hfill \includegraphics[width=.8\linewidth]{../rogueviz/dhrg-data/temp_3x1.pdf}

\small
Density of BFKL/ground truth (black) and de-discretized/ground truth (blue)
\end{center}
   )="); }},

  {"Greedy Routing", 20, LEGAL::HYPERBOLIC | QUICKGEO,
    "Press a key ('5') to try greedy routing. We have to find a path to another node (that we are not directly connected to). In greedy routing, we use our geometric graph: we go to the node which is the closest to the goal, then the node which is the closest from there, and so on. We may not always succeed, but we hope to succeed.",

    [] (presmode mode) {

      static int gphase = 0;
      if(mode == pmStart) showgrid = true;
      graph_visuals(mode);
      uses_game(mode, "sea_graph", launch_sea, dhrg_hooks);
      if(mode == pmKey) {
        gphase++;
        if(gphase == 1) greedy_test();
        if(gphase == 2 && !gmatrix.count(rogueviz::vdata[me].m->base)) gphase = 1;
        if(gphase == 1) zoom_to(rogueviz::vdata[me].m->base, 1);
        if(gphase == 2) rogueviz::search_for = them;
        if(gphase == 4) gphase = 0;
        if(gphase == 0) rogueviz::search_for = -1;
        }
      rogueviz::rv_hook(hooks_frame, 100, [] {
        if(gphase < 3) return;
        auto pt = [&] (int i) { 
          auto &m = rogueviz::vdata[i].m;
          return ggmatrix(m->base) * m->at * C0;
          };
        vid.linewidth *= 6;
        curvepoint(unshift(lastpoint = pt(path_to_draw[0])));
        for(int i=1; i<isize(path_to_draw); i++)
          prettylineto(pt(path_to_draw[i]));
        
        queuecurve(shiftless(Id), 0xFFD500C0, 0, PPR::WALL);
        vid.linewidth /= 6;
        });      
      no_other_hud(mode);
      }
    },

  {"Greedy routing: results", 30, LEGAL::NONE | NOTITLE, "Hyperbolic random graphs turn out to be great for greedy routing! We have also checked how good the DHRG model is according to this measure.", 
    [] (presmode mode) {
      latex_slide(mode, defs+R"=(
{\color{remph} Internet (Boguñá, Papadopoulos, Krioukov 2010)}
\begin{itemize}
\item Success rate about 97\% (HRG distances)
\item Success rate about 14\% (geographic distances)
\item Robust with respect to link removals
\end{itemize}

{\color{remph} Our results on synthetic graphs}
\begin{itemize}
\item For the HRG embedding, success rate about 93\%
\item Usually reduced by discretization by about 3\% ($G_{710}$) or 1\% ($G_{711}$)
\item Usually improved by local search
\item Three steps: for $G_{711}$ and large graphs (15000 nodes) and $T=0.1$, improves in 87\% cases by 0.32\% on average
\item Less good for $G_{710}$ (32\%), smaller graphs (70\% for n=1000), higher $T$ (65\% for $T=0.7$)
\end{itemize}

   )="); }},

  {"Thanks for your attention!", 123, LEGAL::ANY | QUICKSKIP | FINALSLIDE | NOTITLE, 
    "Thanks for watching!",
  
    [] (presmode mode) {
      graph_visuals(mode);      
      uses_game(mode, "sea_graph", launch_sea, dhrg_hooks);
      if(mode == pmStart) slide_backup(nohud, false);
      add_stat(mode, [] {
        dialog::init();        
        dialog::addTitle("Thanks for your attention!", 0xFF00, 200);
        dialog::display();
        return true;
        });
      }
    }
  };
  
int dhrg_phooks = 
  0 +
  addHook(hooks_gamedata, 100, gamedata) +
  addHook_slideshows(100, [] (tour::ss::slideshow_callback cb) {
    cb(XLAT("Discrete Hyperbolic Random Graph (DHRG)"), &dhrg_slides[0], 'd');
    });
 
}
