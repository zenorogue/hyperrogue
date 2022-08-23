// general utilities
#include <sys/time.h>

namespace dhrg {

struct progressbar : indenter_finish {
  string name;
  static const int PBSIZE = 64;
  int step = -1, total, drawat = 0, count = -1;
  
  void operator ++ (int) { 
    step++;
    while(step >= drawat && total) {
      count++;
      drawat = (total * (count+1)) / PBSIZE;
      fprintf(stderr, "%s [", get_stamp().c_str());
      for(int k=0; k<count; k++) fprintf(stderr, "#");
      for(int k=count; k<64; k++) fprintf(stderr, "-");
      fprintf(stderr, "] %s\x1b[K\r", name.c_str());
      fflush(stderr);
      }
    }
  
  ~progressbar() {
    fprintf(stderr, "\x1b[K");
    }

  progressbar(int t, string n) : indenter_finish(n) { hlog.indentation -= 2; println(hlog, name); hlog.indentation += 2; total = t; (*this)++; }
  };

}
