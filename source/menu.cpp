#include "menu.h"
#include "utils.h"
#include <map>
#include <elzip/elzip.hpp>

/* Ideally this would be a member of the menu struct.... but it was initially used for something else and then repurposed sooooo ¯\_(ツ)_/¯ */
std::map<string, ModInfo> mods_info_map = {};

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

Menu::Menu(const char* _title, vector<Menu*>* submenus) {
    /* Deep copying here because sometimes these menus get initialized in places where the pointer to the title would
    go out of scope and get dropped. (see main.cpp in setup() where I pass in a char* (.c_str()) to a "new Menu(...) constructor") */
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

    /* Initialize mod info for current menu. This menu has no submenus, so it has some behavior that depends on modinfo to operate */
    if (!mods_info_map.contains(dl_name)) {
        auto modinfo = &mods_info_map[dl_name];
        modinfo->name = dl_name;
        modinfo->current_version = get_current_mod_version(dl_name);
        modinfo->new_version = "0.0.0";
        modinfo->description = "No description";
        modinfo->author = "No author";
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
        string description = "";
        string author = "";
        for (int i = 0; i < child_count; i++) {
            const char* element = has_submenus ? children.submenus->at(i)->title : children.substrings->at(i);
            /* If current menu has submenus, print each submenu's title */
            if (has_submenus) {
                ModInfo* modinfo = nullptr;
                string version_string = "";

                /* If we have the modinfo for the current menu iteration */
                bool is_installed_menu = false;
                if (mods_info_map.contains(children.submenus->at(i)->dl_name)) {
                    modinfo = &mods_info_map[children.submenus->at(i)->dl_name];
                    string p = HDR_ROMFS_PATH;
                    p += children.submenus->at(i)->dl_name;
                    /* If the current menu iteration is installed, and has (non-default) data for both current_version and new_version, then we want to display version info */
                    if (fs::exists(p)) {
                        is_installed_menu = true;
                        if (strcmp(modinfo->current_version.c_str(), "0.0.0") != 0) {
                            version_string = " [Ver. " + modinfo->current_version + "]";
                            if (modinfo->current_version != modinfo->new_version && strcmp(modinfo->new_version.c_str(), "0.0.0") != 0 && strcmp(title, "Uninstall") != 0) {
                                version_string = " Update available! (Ver. " + modinfo->current_version + " -> " + modinfo->new_version + ")\n";
                            }
                        }
                    }
                }

                /* Behavior for the currently "selected" menu */
                if (i == selected) {
                    printf(GREEN "\n\n--> " RESET);
                    printf(is_installed_menu ? CYAN : WHITE );
                    printf("| %s |" RESET, element);
                    printf(version_string.c_str());
                    /* If modinfo isn't nullptr (default init value), then it's a no-submenus-menu, 
                    and we should set descrip/author variables here to the relevant ones from mods_info_map */
                    if (modinfo != nullptr) {
                        description = modinfo->description;
                        author = modinfo->author;
                        author.insert(0, "By: ");
                    }
                }
                /* Behavior for other menus that aren't currently "selected" */
                else {
                    printf(is_installed_menu ? BLUE : RESET );
                    printf("\n\n%s" RESET, element);
                    printf(version_string.c_str());
                }
            }
            /* If menu doesn't have submenus, it might have a body... */
            else {
                printf(WHITE "\n%s\n" RESET, element);
            }

        }
        /* If the currently selected menu doesn't have submenus, and it's not the "update app" menu thing, print the description and author */
        if (!children.submenus->at(selected)->has_submenus && strcmp(children.submenus->at(selected)->dl_name, "HDR-Installer") != 0 && description != "" && author != "") {
            printf(WHITE "\n\n\n\n\n\n\n%s\n\n%s" RESET, description.c_str(), author.c_str());
        }
    }
}

bool Menu::handle_menu() {
    /* disable screen dimming and auto sleep. */
    appletSetMediaPlaybackState(true);
    /* Disables short-press of the home button (only works in application mode) */
    appletBeginBlockingHomeButton(0);

    /* Handle menus realted to downloading */
    if (handle_menu_type == Download) {

        /* I'm fine special casing this lol.
        Download latest version of the app, then "restart" */
        if (strcmp(dl_name, "HDR-Installer") == 0) {
            downloadFile(HDR_INSTALLER_URL, "sdmc:/switch/HDR_Installer.nro", true);
            printf(GREEN "\nUpdate complete!\n\nRestarting app...." RESET);
            pauseForText(1);
            /* I know there are "restart" funcs in libnx, but they only work in application mode. 
            Using this allows the restart to also work in applet mode */
            envSetNextLoad("sdmc:/switch/HDR_Installer.nro", "sdmc:/switch/HDR_Installer.nro");
            appletSetMediaPlaybackState(false);
            appletEndBlockingHomeButton();
            return true;
        }

        /* Remove previous installation of romfs mod before downloading...
        This circumvents the situation where an update would delete a file, but download the new files wouldn't delete that file. */
        string prev_installation_path = HDR_ROMFS_PATH;
        prev_installation_path += dl_name;
        if (fs::exists(prev_installation_path)) {
            printf("\n\nRemoving previous installation of %s before downloading\n", dl_name);
            pauseForText(1);
            fsdevDeleteDirectoryRecursively(prev_installation_path.c_str());
        }

        /* dl_name w/ a .zip extension at sd card root */
        string zip_path = "sdmc:/";
        zip_path += dl_name;
        zip_path += ".zip";

        /* Download file, and if it downloads proplerly, extract it to sd card root. 
        This assumes the mod will have a folder structure mimicing the sd card root, which is a reasonable assumption, 
        and one that I will ensure stays consistent through PR reviews on the HDR-Releases repo */
        if (downloadFile(url, zip_path.c_str(), true)) {
            consoleClear();
            printf("\n\n\nExtracting download...");
            printf("\n");
            consoleUpdate(NULL);
            elz::extractZip(zip_path, "sdmc:/");
            consoleClear();
            fs::remove(zip_path);

            if (mods_info_map.contains(dl_name)) {
                auto mod_info = &mods_info_map[dl_name];
                mod_info->current_version = get_current_mod_version(dl_name);
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
    /* Handle menus related to uninstalling */
    else if (handle_menu_type == Uninstall) {
        string p = HDR_ROMFS_PATH;
        p += dl_name;
        printf("\n\n\nUninstalling ");
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

            printf(GREEN "\nUninstalled successfully!\n" RESET);
        }
        else {
            printf(RED "\n\nAttempted to remove path that doesn't exist!\n" RESET);
        }
        pauseForText(3);
    }
    else if (handle_menu_type != None) {
        printf(RED "\n\nMenu type unidentified... something went wrong.\n\n" RESET);
        pauseForText(3);
    }
    appletSetMediaPlaybackState(false);
    appletEndBlockingHomeButton();
    return false;
}

/* Set the substrings field of our menu, properly dealing with child_count for every pushed char* */
void Menu::set_menu_body(vector<const char*> body) {
    for (const char* s: body) {
        children.substrings->push_back(s);
        child_count += 1;
    }
}

void parse_addons_list_info_by_line(string line) {
    /* line will look like this:
    HDR-Base :: 9.0.4 :: HDR Dev Team :: The premiere Ultimate gameplay mod...
    dl_name ^    ver ^     author ^       description ^ */

    /* parse string for name, version and description */
    string dl_name = line.substr(0, line.find(" ::"));
    int version_string_length = nthSubstr(2, line, "::") - (line.find("::") + 3);
    string version = line.substr(line.find("::") + 3, version_string_length-1 ); // from first occurance of "::" to the second occurance
    int author_string_length = nthSubstr(3, line, "::") - (nthSubstr(2, line, "::") + 3);
    string author = line.substr(nthSubstr(2, line, "::") + 3, author_string_length-1); // from second occurance of "::" to the third occurance
    string description = line.substr(nthSubstr(3, line, "::") + 3); // from third occurance of "::" to end of string

    string p = HDR_ROMFS_PATH;
    p += dl_name;

    /* If the path to the mod exists, and it's in our mods_info_map, update it with new_version, descrip, and author */
    if (mods_info_map.contains(dl_name)) {
        auto modinfo = &mods_info_map[dl_name];
        modinfo->new_version = version;
        modinfo->description = description;
        modinfo->author = author;
    }
}