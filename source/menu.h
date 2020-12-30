#pragma once
#include <vector>
#include <string>
using namespace std;

enum HandleMenuType {
    Download,
    Uninstall,
    None
};

struct ModInfo {
    string new_version;
    string current_version;
    string description;
    string name;
    string author;
};

struct Menu {
    public:
        char* title;
        char* dl_name;
        char* url;
        int selected;
        bool has_submenus;
        int child_count;
        HandleMenuType handle_menu_type;
        Menu* parent;

        union {
            vector<Menu*>* submenus;
            vector<const char*>* substrings;
        } children;

        Menu(const char* _title, vector<Menu*>* _submenus);
        Menu(const char* _title, HandleMenuType _handle_menu_type, const char* _dl_name, const char* _url = "");
        ~Menu();

        Menu(const Menu&) = delete; // We don't want to be copying menus
        Menu(Menu&&) = delete; // We don't want to be moving menus

        bool handle_menu();
        void printMenu();
        void set_menu_body(vector<const char*> _body);
};

void parse_addons_list_info_by_line(string line);