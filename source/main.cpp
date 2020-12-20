#include <switch.h>
#include "menu.h"
using namespace std;

const string standard_download_msg_start = "Beginning download...";
const string standard_uninstall_msg_start = "Beginning uninstallation...";

Menu install_menu = 
Menu(
    "Install HDR",
    {
        Menu("Install HDR-Base", {Menu(standard_download_msg_start)})
    }
);

Menu addons_menu = 
Menu(
    "Addons",
    {
        Menu("HDR-Graphics", {Menu(standard_download_msg_start)}),
        Menu("Main Menu Theme Song", {Menu(standard_download_msg_start)}),
        Menu("HDR-Tips", {Menu(standard_download_msg_start)}),
        Menu("Bassnaut's Song Pack", {Menu(standard_download_msg_start)})
    }
);

Menu uninstall_menu = 
Menu(
    "Uninstall HDR",
    {
        Menu("Uninstall HDR-Base", {Menu(standard_uninstall_msg_start)}),
        Menu("Uninstall All Addons", {Menu(standard_uninstall_msg_start)}),
        Menu("Uninstall HDR-Graphics", {Menu(standard_uninstall_msg_start)}),
        Menu("Uninstall Main Menu Theme Song", {Menu(standard_uninstall_msg_start)}),
        Menu("Uninstall HDR-Tips", {Menu(standard_uninstall_msg_start)}),
        Menu("Uninstall Bassnaut's Song Pack", {Menu(standard_uninstall_msg_start)})
    }
);

Menu main_menu = 
Menu(
    "Main Menu", 
    {
        install_menu,
        addons_menu, 
        uninstall_menu
    }
);

void mainMenuLoop(u64 kDown, Menu &menu) {
    int selected = menu.currently_selected_menu_idx;
    int num_submenus = menu.submenus.size();
    //printf("\nSelected: %i\n", selected);
    //printf("\nselected menu selection: %i\n", menu.submenus[selected].currently_selected_menu_idx);

    /* Scroll up */
    if (kDown & KEY_LSTICK_UP) {
        selected = (selected-1) % num_submenus;
        if (selected < 0) {
            selected += num_submenus;
        }
        menu.currently_selected_menu_idx = selected;
    }
    /* Scroll down */
    else if (kDown & KEY_LSTICK_DOWN) {
        selected = abs((selected+1) % num_submenus);
        menu.currently_selected_menu_idx = selected;
    }
    /* Select */
    else if (kDown & KEY_A) {
        if (num_submenus > 0 && selected >= 0 && selected < num_submenus && menu.submenus[selected].submenus.size() > 0) {
            menu.currently_selected_menu_idx = 0;
            menu = menu.submenus[selected];
            menu.currently_selected_menu_idx = 0;
        }
    }
    /* Back out */
    else if (kDown & KEY_B) {
        menu.currently_selected_menu_idx = 0;
        menu = main_menu;
        menu.currently_selected_menu_idx = 0;
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

    Menu current_menu = main_menu;

    while(appletMainLoop())
    {
        consoleClear();
        hidScanInput();

        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if(kDown & KEY_PLUS) break; // break in order to return to hbmenu

        current_menu.printMenu();
        mainMenuLoop(kDown, current_menu);
        printf(WHITE "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nPress X to launch smash" RESET);

        consoleUpdate(NULL);
    }
    console_exit();
    //socketExit();      // Cleanup
    return 0;
}