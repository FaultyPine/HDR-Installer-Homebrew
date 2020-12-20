#include "menu.h"

Menu::Menu() {
    this->menu_title = "";
    this->submenus = {};
    this->body = {};
    this->currently_selected_menu_idx = 0;
}

Menu::Menu(string m_title) {
    this->menu_title = m_title;
    this->submenus = {};
    this->body = {};
    this->currently_selected_menu_idx = 0;
}

Menu::Menu(string m_title, vector<string> m_body) {
    this->menu_title = m_title;
    this->submenus = {};
    this->body = m_body;
    this->currently_selected_menu_idx = 0;
}

Menu::Menu(string m_title, vector<Menu> m_submenus) {
    this->menu_title = m_title;
    this->submenus = m_submenus;
    this->body = {};
    this->currently_selected_menu_idx = 0;
}

void Menu::printMenu() {
    printf(GREEN);
    printf(this->menu_title.c_str());
    printf("\n\n");
    printf(RESET);

    if (this->submenus.size() > 0) {
        for(int i = 0; i < (int)this->submenus.size(); i++) {
            if (i == this->currently_selected_menu_idx) {
                printf(BLUE);
                printf(this->submenus[i].menu_title.c_str());
                printf("\n");
                printf(RESET);
            }
            else {
                printf(this->submenus[i].menu_title.c_str());
                printf("\n");
            }
        }
    }

    if (this->body.size() > 0) {
        printf("\n\n\n\n\n");
        for(int i = 0; i < (int)this->body.size(); i++) {
            printf(this->body[i].c_str());
        }
    }
}
