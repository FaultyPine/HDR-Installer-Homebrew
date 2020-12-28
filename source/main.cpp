#include "menu.h"
#include "utils.h"
#include <iostream>
#include <fstream>

bool mainMenuLoop(u64 kDown, Menu*& menu) { // return true when you want to exit the app
    /* return true in order to return to hbmenu */
    if(kDown & KEY_PLUS) return true;

    int selected = menu->selected;
    int num_entries = menu->child_count;

    /* Scroll up */
    if (kDown & KEY_UP) {
        selected = (selected-1) % num_entries;
        if (selected < 0) {
            selected += num_entries;
        }
        menu->selected = selected;
    }
    /* Scroll down */
    else if (kDown & KEY_DOWN) {
        selected = abs((selected+1) % num_entries);
        menu->selected = selected;
    }
    /* Select */
    else if (kDown & KEY_A) {
        /* If our currently selected menu has submenus, enter the submenu that is currently selected */
        if (menu->children.submenus->at(selected)->has_submenus) {
            menu->selected = 0;
            menu = menu->children.submenus->at(selected);
            menu->selected = 0;
        }
        /* If our currently selected menu has no submenus, then we want to do the thing related to that menu */
        else {
            if (menu->children.submenus->at(selected)->handle_menu()) return true; // this returns true when we update ourselves and want to restart
        }
    }
    /* Back out */
    else if (kDown & KEY_B) {
        if (menu->parent != nullptr) {
            menu->selected = 0;
            menu = menu->parent;
            menu->selected = 0;
        }
    }
    /* Launch smash */
    else if (kDown & KEY_X) {
        printf(GREEN "Launching smash... Please be patient, your switch hasn't froze, it's just loading. First time boot of HDR may take a minute or two." RESET);
        appletRequestLaunchApplication(0x01006A800016E000, NULL);
    }

    /* Insult the user & lock them in an infinite loop if they use the Poké Ball Plus controller */
    /*else if (kDown & HidNpadButton_Palma) {
        while (true) {
            printf(RED "Why tf do you have one of these things" RESET);
            consoleUpdate(NULL);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }  
    }*/

    return false;
}

void setup(Menu* addons_menu, Menu* uninstall_menu) {

    /* Make sure important paths exists */
    vector<const char*> paths_to_make = {HDR_INSTALLER_PATH, HDR_ROMFS_PATH};
    for(const char* c: paths_to_make) {
        fs::path installer_path(c);
        if (!fs::exists(c)) {
            fs::create_directories(c);
        }
    }

    /* Remove already existing hdr-addons-list (to ensure we get the right one? Is this even necessary?) */
    const char* hdr_addons_list_path = HDR_INSTALLER_PATH "HDR-Addons-List.txt";
    if (fs::exists(hdr_addons_list_path)) {
        fs::remove(hdr_addons_list_path);
    }

    /* Download addons list file to installer directory */
    if (downloadFile(HDR_ADDONS_LIST_URL, hdr_addons_list_path, false)) {
        /* read addons list file */
        fstream addons_list(HDR_INSTALLER_PATH "/HDR-Addons-List.txt", ios::in);
        if (addons_list.is_open()) {
            string line;
            while(getline(addons_list, line)) {
                /* push each addon onto addons_submenus and uninstall_submenus vectors */

                string url = HDR_RELEASES_URL;
                url += line.c_str();
                url += ".zip";

                Menu* curr_addons_addition = new Menu(line.c_str(), Download, line.c_str(), url.c_str());
                curr_addons_addition->parent = addons_menu;
                Menu* curr_uninstall_addition = new Menu(line.c_str(), Uninstall, line.c_str());
                curr_uninstall_addition->parent = uninstall_menu;

                addons_menu->children.submenus->push_back(curr_addons_addition);
                addons_menu->child_count += 1;
                
                uninstall_menu->children.submenus->push_back(curr_uninstall_addition);
                uninstall_menu->child_count += 1;
            }
        }
        addons_list.close();
    }
}

int main(int argc, char **argv)
{
    socketInitializeDefault();
    console_init();
    console_set_status("\n" RED "X" RESET " to launch smash" MAGENTA "\t\t\t\tHDR Installer Ver. " APP_VERSION WHITE "\t\t\t\t\t" RED "+" RESET " to exit" RESET);
    //nxlinkStdio();    // Redirect stdout and stderr over the network to nxlink

    /* Install HDR */
    string url = HDR_RELEASES_URL;
    url += "HDR-Base.zip";
    Menu install_menu ("Install HDR", Download, "HDR-Base", url.c_str());

    /* Addons */
    vector<Menu*> addons_submenus = { };
    Menu addons_menu("Addons", &addons_submenus);

    /* Uninstall */
    Menu u_hdr_base("HDR-Base", Uninstall, "HDR-Base");
    vector<Menu*> uninstall_submenus = { &u_hdr_base };
    Menu uninstall_menu("Uninstall", &uninstall_submenus);

    /* Update ourselves */
    Menu update_app_menu("Update HDR-Installer", Download, "HDR-Installer", HDR_INSTALLER_URL);

    /* Main Menu */
    vector<Menu*> main_submenus = { &install_menu, &addons_menu, &uninstall_menu, &update_app_menu };
    Menu main_menu("Main Menu", &main_submenus);

    /* Our current menu */
    Menu* current_menu = &main_menu;

    /* Get addons and uhhhh do some other shtuff */
    setup(&addons_menu, &uninstall_menu);

    /* Init hid stuff */
    //padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    //PadState pad;
    //padInitializeDefault(&pad);

    while(appletMainLoop())
    {
        consoleClear();
        //padUpdate(&pad);

        /* Using deprecated HID stuff since devkit's docker image isn't up to date */
        hidScanInput();

        //u64 kDown = padGetButtonsDown(&pad);
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        current_menu->printMenu();
        if (mainMenuLoop(kDown, current_menu)) break;

        consoleUpdate(NULL);
    }
    console_exit();
    socketExit();      // Cleanup
    return 0;
}