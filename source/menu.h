#pragma once
#include <switch.h>
#include <filesystem>
#include <cstring>
#include <vector>
extern "C" {
    #include "console.h"
}
using namespace std;

struct Menu {
    public:
        const char* title;
        Menu* parent; // having a pointer to our parent allows for smoother menu transitions
        union {
            Menu** submenus;
            const char** substrings;
        } children; // union because either all submenus or all substrings
        bool is_strings; // to know which element in the union to use
        size_t child_count; // in order to know how many elements
        // Notice that there is no default constructor here
        //     A default instructor is Menu()
        // This is because we want our menu to be instantiated
        // with data. There is no point in a menu that doesn't even
        // have a title. Now, if you were dynamically creating menus
        // that would be a better alternative, but since we know everything at
        // runtime, this is fine.

        // std::string is pretty bulky and unwieldly for our purposes
        Menu(const char* title);
        // An std::vector would be better if we were able to add
        // entries at runtime. Maybe if you pulled the addons from a server
        // then a std::vector would be ideal. In this case, we can just
        // use static memory.
        Menu(const char* title, Menu** submenus, size_t count);
        Menu(const char* title, const char** substrings, size_t count);

        Menu(const Menu&) = delete; // We don't want to be copying menus
        Menu(Menu&&) = delete; // We don't want to be moving menus

        int selected;
        void printMenu();
};
