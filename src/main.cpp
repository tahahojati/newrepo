#include "ncurses_display.h"
#include "system.h"
#include "line_parser.h"
#include <iostream>

int main() {
  // LineParser lp;
  // lp.setModeFirstToken();
  // while(true) {
  //   lp.readLine(std::cin);
  //   std::cout << lp.getKey() << lp.getValueCount() << std::endl;
  //   for(int i = 0 ; i < lp.getValueCount() ; ++i){
  //     std::cout << lp.getValueAt(i) << " ";
  //   }
  // }
  System system;
  NCursesDisplay::Display(system);
}