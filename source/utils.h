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
extern "C" {
    #include "console.h"
}

using namespace std;
namespace fs = std::filesystem;

#define APP_VERSION           "1.0.1"

#define HDR_INSTALLER_PATH    "sdmc:/switch/HDR_Installer/"
#define HDR_ROMFS_PATH        "sdmc:/ultimate/mods/"

#define HDR_RELEASES_URL      "https://github.com/FaultyPine/HDR-Release/releases/download/beta/"
#define HDR_ADDONS_LIST_URL   "https://github.com/FaultyPine/HDR-Release/releases/download/beta/HDR-Addons-List.txt"
#define HDR_INSTALLER_URL     "https://github.com/FaultyPine/HDR-Installer-Homebrew/releases/download/beta/HDR_Installer.nro"

#define NUM_PROGRESS_CHARS 50


void pauseForText();
//void removeRecursive(std::filesystem::path path);
bool downloadFile(const char* url, const char* path);