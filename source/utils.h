#pragma once
#include "switch.h"
#include <filesystem>
#include <sys/select.h>
#include <curl/curl.h>
#include <stdio.h>
#include <filesystem>
#include <cstring>
#include <vector>
#include <math.h>
#include <thread>
#include <chrono>  
#include <algorithm>
extern "C" {
    #include "console.h"
}

using namespace std;
namespace fs = std::filesystem;

#define APP_VERSION           "1.2.0"

#define HDR_INSTALLER_PATH    "sdmc:/switch/HDR_Installer/"
#define HDR_ROMFS_PATH        "sdmc:/ultimate/mods/"
#define SKYLINE_PLUGINS_PATH  "sdmc:/atmosphere/contents/01006A800016E000/romfs/skyline/plugins/"

#define HDR_RELEASES_URL      "https://github.com/FaultyPine/HDR-Release/releases/download/beta/"
#define HDR_ADDONS_LIST_URL   "https://github.com/FaultyPine/HDR-Release/releases/download/beta/HDR-Addons-List.txt"
#define HDR_INSTALLER_URL     "https://github.com/FaultyPine/HDR-Installer-Homebrew/releases/download/Release/HDR_Installer.nro"

#define NUM_PROGRESS_CHARS 50


void pauseForText(int seconds);

int indexOfCharVec(vector<char*> vector, char *data);

bool downloadFile(const char* url, const char* path, bool print_progress);