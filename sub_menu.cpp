#include "sub_menu.h"

#include "utils.h"

void Menu_1() {
  size_t size_Mb;
  string VolName;
  cout << " enter size of Volume: ";
  cin >> size_Mb;
  cout << " enter Volume Name: ";
  Flush(cin);
  getline(cin, VolName);
  FAT32 fat32(size_Mb, VolName);
  cout << " Create Volume success" << endl;
}

void Menu_2() {}
void Menu_3() {}
void Menu_4() {}
void Menu_5() {}
void Menu_6() {}