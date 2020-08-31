#include "controller.h"
extern "C" {
#include "os.h"
#include "os_dir.h"
#include "os_be.h"
#include "log.h"
}

void checkSize() {
  int oldX = getmaxx(winh);
  int oldY = getmaxy(winh);
  int x,y;

  while (1) {
    x = getmaxx(winh);
    y = getmaxy(winh);

    if ( oldX != x || oldY != y ) {
      logPrintf(__FILE__,__LINE__,"x:%d y:%d",x,y);
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
      wclear(win[0]); drucke_dateien(0); show_status(0); wrefresh(win[0]);
      wclear(win[1]); drucke_dateien(1); show_status(1); wrefresh(win[1]);
      drucke_button_all(func);
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

