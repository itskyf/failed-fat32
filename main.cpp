
#include <iostream>

#include "console_menu.h"
#include "ext2.h"
#include "utils.h"

using std::cin;
using std::cout;
using std::endl;
using std::getline;
using std::string;

void CreateProcess();

int main(int argc, char *argv[]) {
  int mainC = 1;
  // while (mainC = MainMenu()) {
  //     switch (mainC) {
  //       case MainEnum::create:
  //         CreateProcess();
  //         break;
  //       case MainEnum::open:
  //         break;
  //     }
  //   }
  // TODO min size
  ext2 fs(1, "test.next2");
  return 0;
}

void CreateProcess() {
  string fName = "";
  cout << "New file path: ";
  getline(cin, fName);
}