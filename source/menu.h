#pragma once
#include <vector>
using namespace std;

enum HandleMenuType {
    Download,
    Uninstall,
    None
};

struct Menu {
    public:
        char* title;
        HandleMenuType handle_menu_type;
        Menu* parent;
        union {
            vector<Menu*>* submenus;
            vector<const char*>* substrings;
        } children;
        bool is_strings;
        int child_count;

        Menu(const char* title);
        Menu(const char* title, vector<Menu*>*);
        Menu(const char* title, vector<const char*>* substrings, HandleMenuType h_m_type);
        ~Menu();

        Menu(const Menu&) = delete; // We don't want to be copying menus
        Menu(Menu&&) = delete; // We don't want to be moving menus

        int selected;
        void handle_menu();
        void printMenu();
};
