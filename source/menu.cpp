#include "menu.h"
#include "utils.h"
#include <elzip/elzip.hpp>

Menu::~Menu() {
    /* deallocate title memory since we allocated in setup() in main.cpp 
    This isn't technically necessary, as menus persist throughout the lifetime of the app,
    but it's good practice and boy oh boy do I need practice */
    delete[] title; 
}

Menu::Menu(const char* _title) {
    title = new char[strlen(_title) + 1];
    memcpy(title, _title, strlen(_title) + 1);
    child_count = 0;
    handle_menu_type = None;
    is_strings = false;
    children.submenus = nullptr;
    parent = nullptr;
    selected = 0;
}

Menu::Menu(const char* _title, vector<Menu*>* submenus) {
    title = new char[strlen(_title) + 1];
    memcpy(title, _title, strlen(_title) + 1);
    child_count = submenus->size();
    handle_menu_type = None;
    is_strings = false;
    children.submenus = submenus;
    for (int i = 0; i < child_count; i++) {
        children.submenus->at(i)->parent = this;
    }
    parent = nullptr;
    selected = 0;
}

Menu::Menu(const char* _title, vector<const char*>* substrings, HandleMenuType menu_type) {
    title = new char[strlen(_title) + 1];
    memcpy(title, _title, strlen(_title) + 1);
    child_count = substrings->size();
    handle_menu_type = menu_type;
    is_strings = true;
    children.substrings = substrings;
    parent = nullptr;
    selected = 0;
}

void Menu::printMenu() {
    printf(GREEN "%s\n\n" RESET, title);
    if (child_count > 0) {
        for (int i = 0; i < child_count; i++) {
            const char* element = is_strings ? children.substrings->at(i) : children.submenus->at(i)->title;
            if (i == selected)
                printf(BLUE "%s\n" RESET, element);
            else
                printf("%s\n", element);
        }
    }
}

void Menu::handle_menu() {
    if (handle_menu_type == Download) {
        /* Keep in mind these will be zips, still gotta figure out unzipping, and then delete the zip afterwards */
        string root_path = "sdmc:/";

        string url = HDR_RELEASES_URL; // base url of the releases page of HDR-Release repo
        url += title; // add the title of our menu (assumed to be the same as the name of the zip file on the releases page we wanna download)
        url += ".zip"; // add extension of that file

        string zip_path = root_path + title;
        string unzipped_path = zip_path;
        zip_path += ".zip";

        if (downloadFile(url.c_str(), zip_path.c_str())) {
            consoleClear();
            printf("\nExtracting download...");
            printf("\n");
            consoleUpdate(NULL);
            elz::extractZip(zip_path, root_path);
            consoleClear();
            fs::remove(zip_path);
        }
        printf(GREEN "Installation complete!" RESET);
        consoleUpdate(NULL);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    else if (handle_menu_type == Uninstall) {
        string _p = HDR_ROMFS_PATH;
        _p += title;
        printf("\n\nUninstalling: ");
        printf(_p.c_str());
        printf("\n\n");
        consoleUpdate(NULL);
        /* remove HDR plugin if we're uninstalling HDR-Base...
        In the future it would be better to have each menu keep a list of relevant paths to plugins & romfs folders */
        if (strcmp(title, "HDR-Base") == 0) {
            fs::remove("sdmc:/atmosphere/contents/01006A800016E000/romfs/skyline/plugins/libHDR.nro");
        }
        fs::path p(_p);
        if (fs::exists(p)) {
            children.substrings->push_back(p.c_str());
            fsdevDeleteDirectoryRecursively(p.c_str());
            printf(GREEN "Uninstalled successfully!" RESET);
        }
        else {
            printf(RED "Attempted to remove path that doesn't exist!\n" RESET);
        }
        consoleUpdate(NULL);
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
}