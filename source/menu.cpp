#include "menu.h"

Menu::Menu(const char* _title) {
    title = _title;
    child_count = 0;
    is_strings = false;
    children.submenus = nullptr;
    parent = nullptr;
    selected = 0;
}

Menu::Menu(const char* _title, Menu** submenus, size_t count) {
    title = _title;
    child_count = count;
    is_strings = false;
    children.submenus = submenus;
    for (size_t i = 0; i < count; i++) {
        children.submenus[i]->parent = this;
    }
    parent = nullptr;
    selected = 0;
}

Menu::Menu(const char* _title, const char** substrings, size_t count) {
    title = _title;
    child_count = count;
    is_strings = true;
    children.substrings = substrings;
    parent = nullptr;
    selected = 0;
}

void Menu::printMenu() {
    printf(GREEN "%s\n\n" RESET, title);

    if (child_count > 0) {
        for (size_t i = 0; i < child_count; i++) {
            const char* element = is_strings ? children.substrings[i] : children.submenus[i]->title;
            if (i == selected)
                printf(BLUE "%s\n" RESET, element);
            else
                printf("%s\n", element);
        }
    }
}
