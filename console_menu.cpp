#include "console_menu.h"

#include <iostream>
#include <limits>

#include "utils.h"
using std::cin;
using std::cout;
using std::endl;

int InputValid(int l = 0, int h = INT_MAX) {
  int n = 0;
  cout << "Select: ";
  while (!(cin >> n) || (n < l) || n > h) {
    cin.clear();
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    cout << "Invalid input. Please try again: ";
  }
  Flush(cin);
  return n;
}

int MainMenu() {
  const char* msg = R"(Main menu
1. Create new
2. Open file
0. Quit)";
  cout << msg << endl;
  return InputValid(0, MainEnum::last);
}
