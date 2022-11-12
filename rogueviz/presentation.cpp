#ifndef PRESENTATION_CPP
#define PRESENTATION_CPP

#include "../rogueviz/rogueviz.h"

namespace rogueviz {

#if CAP_RVSLIDES
namespace pres {

/* maks graphs in presentations */
grapher::grapher(ld _minx, ld _miny, ld _maxx, ld _maxy) : minx(_minx), miny(_miny), maxx(_maxx), maxy(_maxy) {
  auto& cd = *current_display;
  
  ld xpixels = 2 * min(cd.xcenter - cd.xmin, cd.xmax - cd.xcenter);
  ld ypixels = 2 * min(cd.ycenter - cd.ymin, cd.ymax - cd.ycenter);
  
  ld sca = min(abs(xpixels / (maxx-minx)), abs(ypixels / (maxy-miny)));
  
  ld medx = (minx + maxx) / 2;
  ld medy = (miny + maxy) / 2;

  hyperpoint zero = atscreenpos(cd.xcenter - sca * medx, cd.ycenter + sca * medy, 1) * C0;

  hyperpoint zero10 = atscreenpos(cd.xcenter - sca * medx + sca, cd.ycenter + sca * medy, 1) * C0;
  hyperpoint zero01 = atscreenpos(cd.xcenter - sca * medx, cd.ycenter + sca * medy - sca, 1) * C0;
  
  T = shiftless(Id);
  T.T[LDIM] = zero;
  T.T[0] = zero10 - zero;
  T.T[1] = zero01 - zero;
  
  T.T = transpose(T.T);
  }

void grapher::line(hyperpoint h1, hyperpoint h2, color_t col) {
    curvepoint(h1);
    curvepoint(h2);
    queuecurve(T, col, 0, PPR::LINE).flags |= POLY_FORCEWIDE;
    }
  
void grapher::arrow(hyperpoint h1, hyperpoint h2, ld sca, color_t col) {
  line(h1, h2, col);
  if(!sca) return;
  hyperpoint h = h2 - h1;
  ld siz = hypot_d(2, h);
  h *= sca / siz;
  curvepoint(h2);
  curvepoint(h2 - spin(15._deg) * h);
  curvepoint(h2 - spin(-15._deg) * h);
  curvepoint(h2);
  queuecurve(T, col, col, PPR::LINE);
  }
  
shiftmatrix grapher::pos(ld x, ld y, ld sca) {
  transmatrix P = Id;
  P[0][0] = sca;
  P[1][1] = sca;
  P[0][LDIM] = x;
  P[1][LDIM] = y;
  return T * P;
  }

hyperpoint p2(ld x, ld y) { return LDIM == 2 ? point3(x, y, 1) : point31(x, y, 0); }

/* temporary hooks */

using namespace hr::tour;

void add_stat(presmode mode, const bool_reaction_t& stat) {
  add_temporary_hook(mode, hooks_prestats, 200, stat);
  }

void no_other_hud(presmode mode) {
  add_temporary_hook(mode, hooks_prestats, 300, [] { return true; });
  clearMessages();
  }

/** disable all the HyperRogue game stuff */
void non_game_slide(presmode mode) {
  if(mode == pmStart) {
    tour::slide_backup(game_keys_scroll, true);
    tour::slide_backup(mapeditor::drawplayer, false);
    tour::slide_backup(no_find_player, true);
    tour::slide_backup(playermoved, false);
    tour::slide_backup(vid.axes, 0);
    tour::slide_backup(vid.drawmousecircle, false);
    tour::slide_backup(draw_centerover, false);
    }
  no_other_hud(mode);
  }

void non_game_slide_scroll(presmode mode) {
  non_game_slide(mode);
  slide_backup(smooth_scrolling, true);
  }

void white_screen(presmode mode, color_t col) {
  if(mode == pmStart) {
    tour::slide_backup(backcolor, col);
    tour::slide_backup(ringcolor, color_t(0));
    tour::slide_backup<color_t>(dialog::dialogcolor, 0);
    tour::slide_backup<color_t>(forecolor, 0);
    tour::slide_backup<color_t>(bordcolor, 0xFFFFFFFF);
    tour::slide_backup(vid.aurastr, 0);
    }
  }

void empty_screen(presmode mode, color_t col) {
  if(mode == pmStart) {
    tour::slide_backup(nomap, true);
    }
  white_screen(mode, col);
  }

void slide_error(presmode mode, string s) {
  empty_screen(mode, 0x400000);
  add_stat(mode, [s] {
    dialog::init();
    dialog::addTitle(s, 0xFF0000, 150);
    dialog::display();
    return true;
    });
  }

map<string, texture::texture_data> textures;

void draw_texture(texture::texture_data& tex, ld dx, ld dy, ld scale1) {
  static vector<glhr::textured_vertex> rtver(4);
  
  int fs = inHighQual ? 0 : 2 * vid.fsize;
  
  ld tx = tex.tx;
  ld ty = tex.ty;
  ld scalex = (vid.xres/2 - fs) / (current_display->radius * tx);
  ld scaley = (vid.yres/2 - fs) / (current_display->radius * ty);
  ld scale = min(scalex, scaley);
  scale *= 2;

  for(int i=0; i<4; i++) {
    ld cx[4] = {1,0,0,1};
    ld cy[4] = {1,1,0,0};
    rtver[i].texture[0] = (tex.base_x + (cx[i] ? tex.strx : 0.)) / tex.twidth;
    rtver[i].texture[1] = (tex.base_y + (cy[i] ? tex.stry : 0.)) / tex.theight;
    rtver[i].coords[0] = (cx[i]*2-1) * scale * tx * scale1 + dx;
    rtver[i].coords[1] = (cy[i]*2-1) * scale * ty * scale1 + dy;
    rtver[i].coords[2] = 1;
    rtver[i].coords[3] = 1;
    }
  
  glhr::be_textured();
  current_display->set_projection(0, false);
  glBindTexture(GL_TEXTURE_2D, tex.textureid);
  glhr::color2(0xFFFFFFFF);
  glhr::id_modelview();
  current_display->set_mask(0);
  glhr::prepare(rtver);
  glhr::set_depthtest(false);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  }

void sub_picture(string s, flagtype flags, ld dx, ld dy, ld scale) {
  if(!textures.count(s)) {
    auto& tex = textures[s];
    println(hlog, "rt = ", tex.readtexture(s));
    println(hlog, "gl = ", tex.loadTextureGL());
    }
  auto& tex = textures[s];
  flat_model_enabler fme;
  draw_texture(tex, dx, dy, scale);
  }
  
void show_picture(presmode mode, string s, flagtype flags) {
  if(mode == pmStartAll) {
    auto& tex = textures[s];
    println(hlog, "rt = ", tex.readtexture(s));
    println(hlog, "gl = ", tex.loadTextureGL());
    }
  add_stat(mode, [s, flags] { sub_picture(s, flags); return false; });
  }

string latex_packages =
  "\\usepackage{amsmath}\n"
  "\\usepackage{amssymb}\n"
  "\\usepackage{amsfonts}\n"
  "\\usepackage{varwidth}\n"
  "\\usepackage{amsfonts}\n"
  "\\usepackage{enumitem}\n"
  "\\usepackage[utf8]{inputenc}\n"
  "\\usepackage[T1]{fontenc}\n"
  "\\usepackage{color}\n"
  "\\usepackage{graphicx}\n"
  "\\definecolor{remph}{rgb}{0,0.5,0}\n"
  "\\renewcommand{\\labelitemi}{{\\color{remph}$\\blacktriangleright$}}\n";

string latex_cachename(string s, flagtype flags) {
  unsigned hash = 0;
  for(char c: latex_packages + s) hash = (hash << 3) ^ hash ^ c ^ flags;
  return format("latex-cache/%08X.png", hash);
  }

/* note: you pdftopng from the xpdf package for this to work! */
string gen_latex(presmode mode, string s, int res, flagtype flags) {
  string filename = latex_cachename(s, flags);
  if(mode == pmStartAll) {
    if(!file_exists(filename)) {
      hr::ignore(system("mkdir latex-cache"));
      FILE *f = fopen("latex-cache/rogueviz-latex.tex", "w");
      fprintf(f,
        "\\documentclass[border=2pt]{standalone}\n"
        "%s"
        "\\begin{document}\n"
        "\\begin{varwidth}{\\linewidth}\n"
        "%s"
        "\\end{varwidth}\n"
        "\\end{document}\n", latex_packages.c_str(), s.c_str());
      fclose(f);
      hr::ignore(system("cd latex-cache; pdflatex rogueviz-latex.tex"));
      string pngline = 
        (flags & LATEX_COLOR) ? 
          "cd latex-cache; pdftopng -alpha -r " + its(res) + " rogueviz-latex.pdf t"
        : "cd latex-cache; pdftopng -r " + its(res) + " rogueviz-latex.pdf t";
      println(hlog, "calling: ", pngline);
      hr::ignore(system(pngline.c_str()));
      rename("latex-cache/t-000001.png", filename.c_str());
      }
    }
  return filename;
  }

void show_latex(presmode mode, string s) {
  show_picture(mode, gen_latex(mode, s, 2400, 0));
  }

void dialog_add_latex(string s, color_t col, int size, flagtype flags) {
  string fn = gen_latex(pmStart, s, 600, flags);
  if(!textures.count(fn)) {
    gen_latex(pmStartAll, s, 600, flags);
    auto& tex = textures[fn];
    tex.original = true;
    tex.twidth = 4096;
    println(hlog, "rt = ", tex.readtexture(fn));
    if(!(flags & LATEX_COLOR))
    for(int y=0; y<tex.theight; y++)
    for(int x=0; x<tex.twidth; x++) {
      auto& pix = tex.get_texture_pixel(x, y);
      if(y <= tex.base_y || y >= tex.base_y + tex.stry || x <= tex.base_x || x >= tex.base_x + tex.strx) { pix = 0; continue; }
      int dark = 255 - part(pix, 1);
      pix = 0xFFFFFF + (dark << 24);
      }
    println(hlog, "gl = ", tex.loadTextureGL());
    println(hlog, "fn is ", fn);
    }
  dialog::addCustom(size, [s, fn, col] {
    auto& tex = textures[fn];
    flat_model_enabler fme;

    ld tx = tex.tx;
    ld ty = tex.ty;
    int size = dialog::tothei - dialog::top;
    ld scale = size / 116. / 2;

    static vector<glhr::textured_vertex> rtver(4);
    for(int i=0; i<4; i++) {
      ld cx[4] = {1,0,0,1};
      ld cy[4] = {1,1,0,0};
      rtver[i].texture[0] = (tex.base_x + (cx[i] ? tex.strx : 0.)) / tex.twidth;
      rtver[i].texture[1] = (tex.base_y + (cy[i] ? tex.stry : 0.)) / tex.theight;
      ld x = dialog::dcenter + (cx[i]*2-1) * scale * tx;
      ld y = (dialog::top + dialog::tothei)/2 + (cy[i]*2-1) * scale * ty;
      rtver[i].coords = glhr::pointtogl( atscreenpos(x, y, 1) * C0 );
      }

    glhr::be_textured();
    current_display->set_projection(0, false);
    glBindTexture(GL_TEXTURE_2D, tex.textureid);
    glhr::color2(col);
    glhr::id_modelview();
    current_display->set_mask(0);
    glhr::prepare(rtver);
    glhr::set_depthtest(false);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    });
  }

/** possible values: 
 *  0 = never display latex
 *  1 = if a file exists in the cache, use it
 *  2 = otherwise, use pdflatex and xpdf to generate it
 */
int rv_latex = 1;

map<string, bool> file_exists_cache;

void dialog_may_latex(string latex, string normal, color_t col, int size, flagtype flags) {
  bool use_latex = rv_latex == 2;
  if(rv_latex == 1) {  
    string filename = latex_cachename(latex, flags);
    if(!file_exists_cache.count(filename)) file_exists_cache[filename] = file_exists(filename);
    if(file_exists_cache[filename]) use_latex = true;
    }
  if(use_latex) {
    if(flags & LATEX_COLOR) col = 0xFFFFFFFF;
    dialog_add_latex(latex, (col << 8) | 0xFF, size * 3/2, flags);
    }
  else {
    dialog::addInfo(normal, col);
    dialog::items.back().scale = size;
    }
  }

int video_start = 0;

void read_all(int fd, void *buf, int cnt) {
  char *cbuf = (char*) buf;
  while(cnt > 0) {
    int qt = read(fd, cbuf, cnt);
    if(qt <= 0) break;
    cbuf += qt;
    cnt -= qt;
    }
  }

/* note: this loads the whole animation uncompressed into memory, so it is suitable only for short presentations */
void show_animation(presmode mode, string s, int sx, int sy, int frames, int fps) {
#if CAP_VIDEO
  if(mode == pmStartAll || mode == pmStart) {
    /* load only once */
    if(textures.count(s + "@0")) return;
    /* actually load */
    array<int, 2> tab;
    if(pipe(&tab[0])) {
      addMessage(format("Error: %s", strerror(errno)));
      return;
      }

    int pid = fork();
    fflush(stdout);

    fprintf(stderr, "pipe is %d:%d\n", tab[0], tab[1]);

    if(pid == 0) {
      fprintf(stderr, "in child\n");
     fprintf(stderr, "making fformat\n");
      string fformat = "ffmpeg -y -i " + s + " -f rawvideo -pix_fmt bgra /dev/fd/" + its(tab[1]);    
      int sys = system(fformat.c_str());
      ::close(tab[0]);
      fprintf(stderr, "system call returned %d: %s\n", sys, strerror(errno));
      ::close(tab[1]);
      exit(0);
      }
      
    ::close(tab[1]);
    for(int i=0; i<frames; i++) {
      auto& tex = textures[s + "@" + its(i)];
      tex.strx = tex.tx = sx;
      tex.stry = tex.ty = sy;
      tex.twidth = next_p2(tex.tx);
      tex.theight = next_p2(tex.ty);
      tex.base_x = tex.base_y = 0;
      tex.texture_pixels.resize(tex.twidth * tex.theight);
      
      for(int y=0; y<sy; y++) {
        read_all(tab[0], &tex.texture_pixels[tex.twidth * y], 4 * sx);
        }
      
      println(hlog, "load frame ", i, " = ", tex.loadTextureGL(), " color = ", tex.texture_pixels[0]);
      // tex.loadTextureGL();
      }
    
    ::close(tab[0]);
    println(hlog, "waiting");
    wait(nullptr);
    println(hlog, "waited");
    video_start = ticks;
    }
  add_stat(mode, [s, frames, fps] {
    int f = (ticks - video_start) / 1000. * fps;
    f %= frames;
    auto& tex = textures[s + "@" + its(f)];
    flat_model_enabler fme;    
    draw_texture(tex);
    return false;
    });
#endif
  }

void choose_presentation() {
  cmode = sm::NOSCR;
  gamescreen();

  getcstat = ' ';
  
  dialog::init(XLAT("presentations"), 0xFFD500);

  ss::for_all_slideshows([] (string title, slide *sl, char ch) {
    dialog::addItem(title, ch);
    dialog::add_action([sl] { 
      tour::slides = sl;
      if(!tour::texts) nomenukey = true;
      popScreenAll();
      tour::start();
      if(!tour::on) tour::start();
      });
    });
    
  dialog::addBreak(100);
  
  dialog::addBoolItem_action(XLAT("enable/disable texts"), tour::texts, '7');
  
  dialog::display();
  }

int phooks =
  0
  + addHook(hooks_configfile, 100, [] {
    param_i(rv_latex, "rv_latex");
    })
  + addHook(dialog::hooks_display_dialog, 100, [] () {
    if(current_screen_cfunction() == showStartMenu) { 
      dialog::addBreak(100);
      dialog::addBigItem(XLAT("RogueViz demos"), 'd');
      dialog::add_action([] () { pushScreen(choose_presentation); });
      }
    });

void use_angledir(presmode mode, bool reset) {
  if(mode == pmStart && reset)
    angle = 0, dir = -1;
  add_temporary_hook(mode, shmup::hooks_turn, 200, [] (int i) {
    angle += dir * i / 500.;
    if(angle > 90._deg) angle = 90._deg;
    if(angle < 0) angle = 0;
    return false;
    });
  
  if(mode == pmKey) dir = -dir;
  }

void compare_projections(presmode mode, eModel a, eModel b) {
  static function<void()> w;
  if(mode == pmStart) {
    w = wrap_drawfullmap;
    tour::slide_backup(wrap_drawfullmap, w);
    wrap_drawfullmap = [a, b] {
      if(1) {
        dynamicval<ld> xmin(current_display->xmin, 0);
        dynamicval<ld> xmax(current_display->xmax, 0.49);
        dynamicval<eModel> pm(pmodel, a);
        calcparam();
        w();
        current_display->xmin = .51;
        current_display->xmax = 1;
        pmodel = b;
        calcparam();
        w();
        }
      calcparam();
      };
    }
  }

/* default RogueViz tour */

vector<slide> rvslides_mixed;
vector<slide> rvslides_data;
extern vector<slide> rvslides_default;

void add_end(vector<slide>& s) {
  s.emplace_back(
    slide{"THE END", 99, LEGAL::NONE | FINALSLIDE,
    "Press '5' to leave the presentation.",
    [] (presmode mode) {
      if(mode == pmStart) firstland = specialland = laIce;
      if(mode == 4) restart_game(rg::tour);
      }
    });
  }
  
slide *gen_rvtour_data() {
  rvslides_data = rvslides_default;

  callhooks(hooks_build_rvtour, "data", rvslides_data);
  add_end(rvslides_data);

  return &rvslides_data[0];
  }

slide *gen_rvtour_mixed() {

  rvslides_mixed.emplace_back(slide{
    "RogueViz", 999, LEGAL::ANY,
    "This presentation is mostly composed from various unsorted demos, mostly posted on Twitter and YouTube. Press Enter to continue, ESC to look at other functions of this presentation.",
    [] (presmode mode) {
      slide_url(mode, 'y', "YouTube link", "https://www.youtube.com/user/ZenoTheRogue");
      slide_url(mode, 't', "Twitter link", "https://twitter.com/zenorogue/");
      }
    });
  
  callhooks(hooks_build_rvtour, "mixed", rvslides_mixed); 

  add_end(rvslides_mixed);

  return &rvslides_mixed[0];
  }

vector<slide> rvslides_default = {
    {"intro", 999, LEGAL::ANY, 
      "Hyperbolic space is great "
      "for visualizing some kinds of data because of the vast amount "
      "of space.\n\n"
      "Press '5' to switch to the standard HyperRogue tutorial. "
      "Press ESC to look at other functions of this presentation."
      ,
      [] (presmode mode) {
        slidecommand = "the standard presentation";
        if(mode == pmStartAll) firstland = specialland = laPalace;
        if(mode == 4) {
          tour::slides = default_slides;
          while(tour::on) restart_game(rg::tour);
          firstland = specialland = laIce;
          tour::start();
          }
        }
      },
    {"straight lines in the Palace", 999, LEGAL::ANY, 
      "One simple slide about HyperRogue. Press '5' to show some hyperbolic straight lines.",
      [] (presmode mode) {
       using namespace linepatterns;
       slidecommand = "toggle the Palace lines";
       if(mode == 4) patPalace.color = (patPalace.color == 0xFFD500FF ? 0 : 0xFFD500FF);
       if(mode == 3) patPalace.color = 0xFFD50000;
        }
      },
  };

map<string, gamedata> switch_gd;

EX void switch_game(string from, string to) {
  if(game_active) switch_gd[from].storegame();
  if(switch_gd.count(to)) {
    switch_gd[to].restoregame();
    switch_gd.erase(to);
    }
  }

/** have a separate 'game' that lives between several slides */
void uses_game(presmode mode, string name, reaction_t launcher, reaction_t restore) {
  if(mode == pmStart) {
    switch_game("main", name);
    if(!game_active) launcher();
    else restore();
    restorers.push_back([name] { switch_game(name, "main"); });
    }
  }

void latex_slide(presmode mode, string s, flagtype flags, int size) {
  empty_screen(mode);
  add_stat(mode, [=] {
    tour::slide_backup(no_find_player, true);
    if(flags & sm::SIDE) {
      cmode |= sm::SIDE;
      dynamicval<bool> db(nomap, (flags & sm::NOSCR));
      dynamicval<color_t> dc(modelcolor, nomap ? 0 : 0xFF);
      dynamicval<color_t> dc2(bordcolor, 0);
      gamescreen();
      callhooks(hooks_latex_slide);
      }
    else
      gamescreen();
    dialog::init();
    dialog_may_latex(
      s,
      "(LaTeX is off)",
      dialog::dialogcolor, size, LATEX_COLOR
      );
    dialog::display();
    return true;
    });
  no_other_hud(mode);
  }

int pres_hooks = 
  addHook(hooks_slide, 100, [] (int mode) {
    if(currentslide == 0 && slides == default_slides) {
      slidecommand = "RogueViz presentation";
      if(mode == 1)
        help += 
          "\n\nYour version of HyperRogue is compiled with RogueViz. "
          "Press '5' to switch to the RogueViz slides. Watching the "
          "common HyperRogue tutorial first is useful too, "
          "as an introduction to hyperbolic geometry.";         
      if(mode == 4) {
        while(tour::on) restart_game(rg::tour);
        pushScreen(choose_presentation);
        }
      }
    }) +
  addHook(dialog::hooks_display_dialog, 100, [] () {
    if(current_screen_cfunction() == showGameMenu) {
      dialog::addItem(XLAT("RogueViz demos"), 'd'); 
      dialog::add_action_push(choose_presentation);
      }
    }) +
  addHook_slideshows(300, [] (tour::ss::slideshow_callback cb) {
    if(rogueviz::pres::rvslides_data.empty()) pres::gen_rvtour_data();
    cb(XLAT("non-Euclidean geometry in data analysis"), &pres::rvslides_data[0], 'd');

    if(rogueviz::pres::rvslides_mixed.empty()) pres::gen_rvtour_mixed();

    cb(XLAT("unsorted RogueViz demos"), &pres::rvslides_mixed[0], 'u');
    }) +
  0;

void launch_slideshow_by_name(string s) {
  ss::for_all_slideshows([s] (string title, slide *sl, char ch) {
    println(hlog, "comparing ", s, " to ", title);
    if(s.size() == 1 ? s[0] == ch : appears(title, s)) {
      tour::slides = sl;
      nomenukey = true;
      popScreenAll();
      tour::start();
      if(!tour::on) tour::start();
      }
    });
  }

int runslide =
  addHook(hooks_resetGL, 100, [] {
    textures.clear();
    })
+ arg::add3("-slides", [] {
  arg::shift(); launch_slideshow_by_name(arg::args());
  }) + arg::add3("-slide-textoff", [] {
    tour::texts = false;
    }) + arg::add3("-slide", [] {
  arg::shift(); launch_slideshow_by_name(arg::args());
  presentation(pmStop);
  arg::shift(); string s = arg::args();
  int i;
  currentslide = -1;
  for(i=0; (i==0 || !(slides[i-1].flags & FINALSLIDE)); i++) {
    if(appears(slides[i].name, s)) {
      currentslide = i;
      break;
      }
    }
  if(currentslide == -1) {
    int j = atoi(s.c_str());
    if(j >= 0 && j < i) currentslide = j;
    else currentslide = 0;
    }
  presentation(pmStart);
  })
  + arg::add3("-presangle", [] {
    arg::shift_arg_formula(angle);
    dir = 0;
    })
  ;
}
#endif
}

#endif
