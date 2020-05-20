#include <iostream>

#include "FAT32.h"
#include "sub_menu.h"
using namespace std;

int main() {
  int select = -1;
  cout << " Chon mot trong cac tinh nang sau:\n1. Tao volume\n2. Xem danh sach "
          "File trong Volume\n3. Dat mat khau cho File trong Volume\n4. Export "
          "File trong Volume ra ngoai\n5. Import File vao Volume\n6. Xoa File "
          "trong Volume\n";
  cout << "enter your choice: ";
  cin >> select;
  while (select != 0) {
    switch (select) {
      case 0:
        break;
      case 1:
        Menu_1();
        break;
      case 2:
        break;
      case 3:
        break;
      case 4:
        break;
      case 5:
        break;
      case 6:
        break;
    }
  }
}