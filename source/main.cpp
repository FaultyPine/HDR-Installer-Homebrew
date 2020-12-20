#include <switch.h>
#include "menu.h"
using namespace std;

const char* standard_download_msg_start = "Beginning download...";
const char* standard_uninstall_msg_start = "Beginning uninstallation...";

// Install HDR
static Menu hdr_base("Install HDR-Base", &standard_download_msg_start, 1);
static Menu* install_submenus[1] = { &hdr_base };
static Menu install_menu("Install HDR", install_submenus, 1);

// Addons
static Menu hdr_graphics("HDR-Graphics", &standard_download_msg_start, 1);
static Menu theme_song("Main Menu Theme Song", &standard_download_msg_start, 1);
static Menu hdr_tips("HDR-Tips", &standard_download_msg_start, 1);
static Menu song_pack("Bassnaut's Song Pack", &standard_download_msg_start, 1);
static Menu* addons_submenus[4] = { &hdr_graphics, &theme_song, &hdr_tips, &song_pack };
static Menu addons_menu("Addons", addons_submenus, 4);

// Uninstall
static Menu u_hdr_base("Uninstall HDR-Base", &standard_uninstall_msg_start, 1);
static Menu u_all_addons("Uninstall All Addons", &standard_uninstall_msg_start, 1);
static Menu u_hdr_graphics("Uninstall HDR-Graphics", &standard_uninstall_msg_start, 1);
static Menu u_theme_song("Uninstall Main Menu Theme Song", &standard_uninstall_msg_start, 1);
static Menu u_hdr_tips("Uninstall HDR-Tips", &standard_uninstall_msg_start, 1);
static Menu u_song_pack("Uninstall Bassnaut\'s Song Pack", &standard_uninstall_msg_start, 1);
static Menu* uninstall_submenus[6] = { &u_hdr_base, &u_all_addons, &u_hdr_graphics, &u_theme_song, &u_hdr_tips, &u_song_pack };
static Menu uninstall_menu("Uninstall", uninstall_submenus, 6);

static Menu* main_submenus[3] = { &install_menu, &addons_menu, &uninstall_menu };
static Menu main_menu("Main Menu", main_submenus, 3);

void mainMenuLoop(u64 kDown, Menu*& menu) {
    int selected = menu->selected;
    int num_entries = menu->child_count;
    //printf("\nSelected: %i\n", selected);
    //printf("\nselected menu selection: %i\n", menu.submenus[selected].currently_selected_menu_idx);

    /* Scroll up */
    if (kDown & KEY_LSTICK_UP) {
        selected = (selected-1) % num_entries;
        if (selected < 0) {
            selected += num_entries;
        }
        menu->selected = selected;
    }
    /* Scroll down */
    else if (kDown & KEY_LSTICK_DOWN) {
        selected = abs((selected+1) % num_entries);
        menu->selected = selected;
    }
    /* Select */
    else if (kDown & KEY_A) {
        if (!menu->is_strings && num_entries > 0 && selected >= 0 && selected < num_entries && menu->children.submenus[selected]->child_count > 0) {
            menu->selected = 0;
            menu = menu->children.submenus[selected];
            menu->selected = 0;
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
}


int main(int argc, char **argv)
{
    console_init();
    console_set_status("\n" MAGENTA "\t\t\t\t\t\t\t\t\t\tHDR Installer" WHITE "\t\t\t\t\t\tPress + to exit" RESET);

    //socketInitializeDefault();              // Initialize sockets
    //nxlinkStdio();                          // Redirect stdout and stderr over the network to nxlink

    Menu* current_menu = &main_menu;

    while(appletMainLoop())
    {
        consoleClear();
        hidScanInput();

        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if(kDown & KEY_PLUS) break; // break in order to return to hbmenu

        current_menu->printMenu();
        mainMenuLoop(kDown, current_menu);
        printf(WHITE "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nPress X to launch smash" RESET);

        consoleUpdate(NULL);
    }
    console_exit();
    //socketExit();      // Cleanup
    return 0;
}