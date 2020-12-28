#include "menu.h"
#include "utils.h"
#include <map>
#include <elzip/elzip.hpp>

std::map<string, ModInfo> installed_mods = {};

Menu::~Menu() {
    /* deallocate title memory since we allocated in setup() in main.cpp 
    This isn't technically necessary, as menus persist throughout the lifetime of the app,
    but it's good practice and boy oh boy do I need practice */
    delete[] title;
    if (!has_submenus) {
        delete[] dl_name;
        delete[] url;
    }
}

Menu::Menu(char* _title, vector<Menu*>* submenus) {
    /* Deep copying here because sometimes these menus get initialized in places where the pointer to the title would
    go out of scope and get dropped. (see main.cpp setup() where i pass in line.c_str()) */
    title = new char[strlen(_title) + 1];
    memcpy(title, _title, strlen(_title) + 1);

    dl_name = (char*)"";
    url = (char*)"";

    children.submenus = submenus;
    child_count = children.submenus->size();
    handle_menu_type = None;
    has_submenus = true;
    for (int i = 0; i < child_count; i++) {
        children.submenus->at(i)->parent = this;
    }
    parent = nullptr;
    selected = 0;
}

Menu::Menu(const char* _title, HandleMenuType _handle_menu_type, const char* _dl_name, const char* _url) {
    title = new char[strlen(_title) + 1];
    memcpy(title, _title, strlen(_title) + 1);

    dl_name = new char[strlen(_dl_name) + 1];
    memcpy(dl_name, _dl_name, strlen(_dl_name) + 1);

    url = new char[strlen(_url) + 1];
    memcpy(url, _url, strlen(_url) + 1);

    string p = HDR_ROMFS_PATH;
    p += dl_name;

    if (fs::exists(p) && !installed_mods.contains(dl_name)) {
        auto modinfo = &installed_mods[dl_name];
        modinfo->name = dl_name;
    }

    child_count = 0;
    handle_menu_type = _handle_menu_type;
    has_submenus = false;
    children.substrings = {};
    parent = nullptr;
    selected = 0;
}

void Menu::printMenu() {
    printf(GREEN "\n\n%s\n\n\n" RESET, title);
    if (child_count > 0) {
        for (int i = 0; i < child_count; i++) {
            const char* element = has_submenus ? children.submenus->at(i)->title : children.substrings->at(i);

            /* If current menu has submenus, print each submenu's title */
            if (has_submenus) {
                /* Check if the current menu iteration is "installed" */
                bool is_installed_menu = false;

                if (installed_mods.contains(children.submenus->at(i)->dl_name)) { // dl_name is in installed_mods
                    is_installed_menu = true;
                }

                /* Behavior for the currently "selected" menu */
                if (i == selected) {
                    printf(GREEN "\n--> " RESET);
                    printf(is_installed_menu ? CYAN : WHITE );
                    printf("| %s |\n" RESET, element);
                }
                /* Behavior for other menus that aren't currently "selected" */
                else {
                    printf(is_installed_menu ? BLUE : RESET );
                    printf("\n%s\n" RESET, element);
                }

            }

            /* If menu doesn't have submenus, it might have a body... print that here */
            else {
                printf(WHITE "\n%s\n" RESET, element);
            }

        }
    }
}

bool Menu::handle_menu() {
    /* disable screen dimming and auto sleep. */
    appletSetMediaPlaybackState(true);

    if (handle_menu_type == Download) {

        /* I'm fine special casing this lol.
        Download latest version of the app, then "restart" */
        if (strcmp(dl_name, "HDR-Installer") == 0) {
            downloadFile(HDR_INSTALLER_URL, "sdmc:/switch/HDR_Installer.nro", true);
            printf(GREEN "\nUpdate complete!\n\nRestarting app...." RESET);
            pauseForText(1);
            envSetNextLoad("sdmc:/switch/HDR_Installer.nro", "sdmc:/switch/HDR_Installer.nro");
            return true;
        }

        string prev_installation_path = HDR_ROMFS_PATH;
        prev_installation_path += dl_name;
        if (fs::exists(prev_installation_path)) {
            printf("\n\nRemoving previous installation of %s before downloading...\n", dl_name);
            pauseForText(1);
            fsdevDeleteDirectoryRecursively(prev_installation_path.c_str());
            
            /* If dl_name was already installed, remove it */
            if (installed_mods.contains(dl_name)) {
                installed_mods.erase(dl_name);
            }
        }

        string zip_path = "sdmc:/";
        zip_path += dl_name;
        zip_path += ".zip";

        if (downloadFile(url, zip_path.c_str(), true)) {
            consoleClear();
            printf("\n\n\nExtracting download...");
            printf("\n");
            consoleUpdate(NULL);
            elz::extractZip(zip_path, "sdmc:/");
            consoleClear();
            fs::remove(zip_path);

            /* If dl_name was NOT already installed, add it */
            if (!installed_mods.contains(dl_name)) {
                auto modinfo = &installed_mods[dl_name];
                modinfo->name = dl_name;
            }

            printf(GREEN "\n\n\nInstallation complete!" RESET);
            consoleUpdate(NULL);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        else {
            printf(RED "\n\nDownload failed.\n\n" RESET);
            fs::remove(zip_path);
            pauseForText(3);
        }
    }
    else if (handle_menu_type == Uninstall) {
        string p = HDR_ROMFS_PATH;
        p += dl_name;
        printf("\n\nUninstalling ");
        consoleUpdate(NULL);

        /* remove HDR plugin if we're uninstalling HDR-Base...
        In the fuuuuuture I could keep a list of relevant paths to delete when uninstalling, but I'd rather
        not encourage people to make skyline plugin addons to HDR since that may introduce U.B./weird crashes.
        So since this is literally the only plugin we need to worry about, I'm perfectly fine special-casing it */
        const char* hdr_base_plugin_path = SKYLINE_PLUGINS_PATH "libHDR.nro";
        if (strcmp(dl_name, "HDR-Base") == 0 && fs::exists(hdr_base_plugin_path)) {
            fs::remove(hdr_base_plugin_path);
        }

        /* Remove relevant romfs folder */
        if (fs::exists(p)) {
            printf(dl_name);
            printf("\n\n");
            consoleUpdate(NULL);
            fsdevDeleteDirectoryRecursively(p.c_str());

            /* If dl_name was already installed, remove it */
            if (installed_mods.contains(dl_name)) {
                installed_mods.erase(dl_name);
            }

            printf(GREEN "Uninstalled successfully!\n" RESET);
        }
        else {
            printf(RED "Attempted to remove path that doesn't exist!\n" RESET);
        }
        pauseForText(3);
    }
    else if (handle_menu_type != None) {
        printf(RED "\n\nMenu type unidentified... something went wrong.\n\n" RESET);
        pauseForText(3);
    }
    appletSetMediaPlaybackState(false);
    return false;
}

void Menu::set_menu_body(vector<const char*> body) {
    for (const char* s: body) {
        children.substrings->push_back(s);
        child_count += 1;
    }
}