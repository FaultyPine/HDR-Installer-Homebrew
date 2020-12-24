#include "utils.h"


void pauseForText() {
    consoleUpdate(NULL);
    std::this_thread::sleep_for(std::chrono::seconds(4));
}

void print_progress(size_t progress, size_t max) {
    size_t prog_chars;
    if (max == 0) prog_chars = NUM_PROGRESS_CHARS;
    else prog_chars = ((float) progress / max) * NUM_PROGRESS_CHARS;

    printf("\n\n\n");
    printf("Downloading...\n\n");
    if (prog_chars < NUM_PROGRESS_CHARS) printf(YELLOW);
    else printf(GREEN);

    printf("[");
    for (size_t i = 0; i < prog_chars; i++)
        printf("=");

    if (prog_chars < NUM_PROGRESS_CHARS) printf(">");
    else printf("=");

    for (size_t i = 0; i < NUM_PROGRESS_CHARS - prog_chars; i++)
        printf(" ");

    printf("]\t%lu/%lu\n" RESET, progress, max);
}

/*
Args:
ptr: pointer set with CURLOPT_XFERINFODATA, not used by libcurl (is only passed along -> ignore it)
TotalToDownload: total # of bytes expected to be downloaded
NowDownloaded: # of bytes downloaded so far
TotalToUpload: # of bytes expected to be uploaded
NowUploaded: # of bytes uploaded
*/
int download_progress(void* ptr, double TotalToDownload, double NowDownloaded, 
                    double TotalToUpload, double NowUploaded)
{
    consoleClear();
    print_progress( (int)((NowDownloaded/TotalToDownload)*100.0), 100 ); // percent out of 100
    consoleUpdate(NULL);

    // if you don't return 0, the transfer will be aborted
    return 0; 
}

bool downloadFile(const char* url, const char* path) {
    bool ret = true;
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    FILE *dest;
    curl = curl_easy_init();
    if (curl) {
        dest = fopen(path, "wb");

        curl_easy_setopt(curl, CURLOPT_URL, url);

        curl_easy_setopt(curl, CURLOPT_USERAGENT, "PiNE");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, download_progress);

        curl_easy_setopt(curl, CURLOPT_WRITEDATA, dest);

        consoleUpdate(NULL);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            printf("curl_easy_perform failed: %s\n", curl_easy_strerror(res));
            ret = false;
        }

        fclose(dest);
        curl_easy_cleanup(curl);
    }
    else {
        printf("Curl init failed!");
        consoleUpdate(NULL);
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    curl_global_cleanup();
    return ret;
}