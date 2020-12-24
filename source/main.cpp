#include "menu.h"
#include "utils.h"
#include <iostream>
#include <fstream>

const char* standard_download_msg_start = "Downloading...";
const char* standard_uninstall_msg_start = "Uninstalling...";
vector<const char*> download_msg_vec{standard_download_msg_start};
vector<const char*> uninstall_msg_vec{standard_uninstall_msg_start};

void mainMenuLoop(u64 kDown, Menu*& menu) {
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
        if (!menu->is_strings && num_entries > 0 && selected >= 0 && selected < num_entries && menu->children.submenus->at(selected)->child_count > 0) {
            menu->selected = 0;
            menu = menu->children.submenus->at(selected);
            menu->selected = 0;
            if (menu->is_strings) {
                /* If our currently selected menu has no submenus, then we want to do the thing related to that menu */
                if (strcmp(menu->title, "HDR-Installer") == 0) {
                    downloadFile(HDR_INSTALLER_URL, "sdmc:/switch/HDR_Installer.nro");
                }
                else {
                    menu->handle_menu();
                }
                menu = menu->parent;
            }
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
        printf(GREEN "Launching smash..." RESET);
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

    /* Download addons list file to installer directory */
    if (downloadFile(HDR_ADDONS_LIST_URL, HDR_INSTALLER_PATH "HDR-Addons-List.txt")) {
        /* read addons list file */
        fstream addons_list(HDR_INSTALLER_PATH "/HDR-Addons-List.txt", ios::in);
        if (addons_list.is_open()) {
            string line;
            while(getline(addons_list, line)) {
                /* push each addon onto addons_submenus and uninstall_submenus vectors */

                Menu* curr_addons_addition = new Menu(line.c_str(), &download_msg_vec, Download);
                curr_addons_addition->parent = addons_menu;
                Menu* curr_uninstall_addition = new Menu(line.c_str(), &uninstall_msg_vec, Uninstall);
                curr_uninstall_addition->parent = uninstall_menu;

                Menu* addons_menuptr = curr_addons_addition;
                addons_menu->children.submenus->push_back(addons_menuptr);
                addons_menu->child_count += 1;
                
                Menu* uninstall_menuptr = curr_uninstall_addition;
                uninstall_menu->children.submenus->push_back(uninstall_menuptr);
                uninstall_menu->child_count += 1;
            }
        }
        addons_list.close();
    }
}

int main(int argc, char **argv)
{
    socketInitializeDefault();
    //nxlinkStdio();    // Redirect stdout and stderr over the network to nxlink

    /* Install HDR */
    Menu hdr_base("HDR-Base", &download_msg_vec, Download);
    vector<Menu*> install_submenus = { &hdr_base };
    Menu install_menu ("Install HDR", &install_submenus);

    /* Addons */
    vector<Menu*> addons_submenus = { };
    Menu addons_menu("Addons", &addons_submenus);

    /* Uninstall */
    Menu u_hdr_base("HDR-Base", &uninstall_msg_vec, Uninstall);
    vector<Menu*> uninstall_submenus = { &u_hdr_base };
    Menu uninstall_menu("Uninstall", &uninstall_submenus);

    /* Update ourselves */
    Menu update_app("HDR-Installer", &download_msg_vec, None);
    vector<Menu*> update_app_submenu = { &update_app };
    Menu update_app_menu("Update HDR-Installer", &update_app_submenu);

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

    console_init();
    console_set_status("\n" MAGENTA "\t\t\t\t\t\t\t\t\tHDR Installer Ver. " APP_VERSION WHITE "\t\t\t\tPress + to exit" RESET);

    while(appletMainLoop())
    {
        consoleClear();
        //padUpdate(&pad);
        hidScanInput();

        //u64 kDown = padGetButtonsDown(&pad);
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if(kDown & KEY_PLUS) break; // break in order to return to hbmenu

        current_menu->printMenu();
        mainMenuLoop(kDown, current_menu);

        printf(WHITE "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nPress X to launch smash" RESET);

        consoleUpdate(NULL);
    }
    console_exit();
    socketExit();      // Cleanup
    return 0;
}