#pragma once
#include <switch.h>
#include <filesystem>
#include <cstring>
#include <vector>
extern "C" {
    #include "console.h"
}
using namespace std;

class Menu {
    public:
        Menu();
        Menu(string);
        Menu(string, vector<string>);
        Menu(string, vector<Menu>);

        string menu_title;
        vector<Menu> submenus;
        vector<string> body;
        int currently_selected_menu_idx;
        void printMenu();
};
