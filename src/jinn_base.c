#include "system_messages.h"


int print_banner() {
 random = rand(srand()) %10;
 printf(banner[random]);
}

int print_name() {

};

int print_version() {

};

int print_line() {
  int rand = rand(srand()) %4;
  char response
  switch(rand){
    case 0:
      int loops = 8;
      while (loops > 0) {
        sprintf(print_line_short());
        loops--;
      }
      return response;
    break;
    case 1:
     for (int i=0; i<4;i++) {
       sprintf(print_line_long());
     }
    break;
    case 2:
     sprintf(print_line_long(), print_line_short(), print_line_long(), print_line_short(), print_line_long(), print_line_short());
    break;
    case 4:
     sprintf(print_line_short(), print_line_long(), print_line_short(), print_line_long(), print_line_short(),print_line_long());
    break;
  }

};

int print_line_long() {

};

int print_line_short() {

};

int print_break() {

}

int print_usage() {

};
