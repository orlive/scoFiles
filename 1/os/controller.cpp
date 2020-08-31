#include "controller.h"
extern "C" {
#include "os.h"
}

#define D { FILE* out=fopen("/tmp/log.log","at"); fprintf(out,"%s/%d\n",__FILE__,__LINE__); fflush(out); fclose(out); }
#define D2(a,b) { FILE* out=fopen("/tmp/log.log","at"); fprintf(out,"%s/%d  %s:%d\n",__FILE__,__LINE__,a,b); fflush(out); fclose(out); }

void checkSize() {
  int oldX = getmaxx(winh);
  int oldY = getmaxy(winh);
  int x,y;

  while (1) {
    x = getmaxx(winh);
    y = getmaxy(winh);


    if ( oldX != x || oldY != y ) {
      D2("x:",x);
      D2("y:",y);
      wresize(win[0],y-4,x/2);
      wresize(win[1],y-4,x/2);
      mvwin(win[0],0,0);
      mvwin(win[1],0,x/2);
      //win[0]  = newwin( y-4 , x/2 , 0 , 0   );
      //win[1]  = newwin( y-4 , x/2 , 0 , x/2 );
      mvwin(func,y-4,0);
      wresize(func,4,x);
      //func    = newwin(4,x,y-4,0);
 
      //os_wininit();
      os_refresh_all();
      D2("func-y:",getcury(func));
    }

    std::this_thread::sleep_for( std::chrono::seconds(1) );

    oldX = x;
    oldY = y;
  }
}

void controllerInit() {
  D;
  std::thread *checkThread = new std::thread(checkSize);
  D;
}

