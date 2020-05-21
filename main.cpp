#include <iostream>

#include "FAT32.h"
#include "sub_menu.h"
using namespace std;

int main() {
  FAT32 *fs = nullptr;
  int select = -1;
  cout << " 1. Create\n2.Open\n";
  cin >> select;
  switch (select) {
    case 0:
      break;
    case 1:
      Menu_1(fs);
      break;
    case 2:
      Menu_2(fs);
      break;
  }
  if (fs) delete fs;
}