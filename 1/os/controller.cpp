#include "controller.h"
extern "C" {
#include "os.h"
#include "os_dir.h"
#include "os_be.h"
#include "log.h"
}

void checkSize() {
  int oldX = getmaxx(stdscr);
  int oldY = getmaxy(stdscr);
  int x,y;

  while (1) {
    x = getmaxx(stdscr);
    y = getmaxy(stdscr);

    if ( oldX != x || oldY != y ) {
      logPrintf(__FILE__,__LINE__,"x:%d y:%d",x,y);

      for ( int window=0 ; window<2 ; window++ ) {
        wresize(win[window],y-4,x/2);
        mvwin(win[window],0,(window==0)?0:x/2 );
        wclear(win[window]);
        drucke_dateien(window);
        show_status(window);
        wrefresh(win[window]);
      }

      mvwin(func,y-4,0);
      wresize(func,4,x);
      drucke_button_all(func);

      os_refresh_all();
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

