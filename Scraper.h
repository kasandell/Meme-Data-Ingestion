#ifndef _SCRAPER_H
#define _SCRAPER_H

#include <fstream>
#include <vector>
#include <string>
#include <atomic>
#include <curl/curl.h>
#include <regex>
#include <mutex>


class Scraper
{
    public:
        Scraper();
        Scraper(std::string baseURL, std::string memeType);
        Scraper(Scraper &scraper);
        void scrape();
        void kill();

    private:
        std::string findLastOf(std::regex r);
        std::vector<std::string> findAllOf(std::regex r);
        void writeToFile(std::vector<std::string> links);
        std::string grabPage();
        void createNextLink();
        std::vector<std::string> getLinks();
        static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);

        int itemsFound;


        std::string js;
        std::string curURL;


        std::string baseURL;
        std::string desc;
        
        std::string base;
        std::string params;
        
        std::atomic<bool> killed;


        static int maxResults;
        static std::string baseDir;
        std::mutex fileMtx;
        static std::mutex outMtx;
};

#endif
