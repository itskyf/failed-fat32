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
  cout << "Main menu" << endl;
  cout << "1. Create new" << endl;
  cout << "2. Open" << endl;
  cout << "0. Quit" << endl;
  return InputValid(0, MainEnum::last);
}
