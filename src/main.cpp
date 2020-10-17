#include "Browser.h"
#include <iostream>

int main(int argc, char** argv) { //slz 06032020
  if (!argv[1]) {
    std::cout << "you need to provide a file name!!!!" << std::endl;
    exit(0);
  }  
  Browser browser(argv[1]);
  browser.Run();
  return 0;
}
