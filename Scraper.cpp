#include "Scraper.h"
#include <iostream>
#include <future>
using std::cout;
using std::endl;


int Scraper::maxResults = 1000;
std::string Scraper::baseDir = "";
std::mutex Scraper::outMtx;

Scraper::Scraper(Scraper &scraper)
{
        this->baseURL = scraper.baseURL;
        this->desc = scraper.desc;
        this->js = scraper.js;
        this->curURL = scraper.curURL;


}

Scraper::Scraper()
{

}

Scraper::Scraper(std::string baseURL, std::string memeType): baseURL(baseURL), desc(memeType)
{
    itemsFound = 0;
    curURL = baseURL + ".json";
}

void Scraper::scrape()
{
    killed = false;
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if(curl)
    {
        while(curURL != "UNF" && !killed)
        {
            if(itemsFound > maxResults)
            {
                return;
            }
            {
                std::unique_lock<std::mutex> lck(outMtx);
                cout<<"URLs found for type: "<<desc<<" - "<<itemsFound<<endl;
            }
            js = grabPage();
            std::vector<std::string> links = getLinks();
            std::async(&Scraper::writeToFile, this, links);

            createNextLink();
        }
    }
    return;
}

void Scraper::writeToFile(std::vector<std::string> links)
{
    std::unique_lock<std::mutex> lck(fileMtx);
    std::string dir = baseDir + desc + std::string(".lnk");
    std::ofstream out(dir.c_str(), std::ios::app);
    if(out.is_open())
    {
        for(auto x: links)
        {
            out << x << endl;
        }
    }
    out.close();

}

std::string Scraper::grabPage()
{
    std::string r;
    
    CURL* curl;
    CURLcode res;
    curl = curl_easy_init();
    if(curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, curURL.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &Scraper::WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &r);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

    }
    return r;
}

void Scraper::createNextLink()
{
    curURL = baseURL;
    curURL +=".json";
    std::regex r{"\"name\": \"(\\w+)\""};
    auto last = findLastOf(r);
    curURL += ("?after=" + last);
}

std::vector<std::string> Scraper::getLinks()
{
    std::regex r{ "\"url\": \"(https?://(?:[a-z0-9\\-]+\\.)+[a-z]{2,6}(?:/[^/#?]+)+\\.(?:jpg|png))\""};
    std::vector<std::string> links = findAllOf(r);
    return links;
}

size_t Scraper::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;

}
std::string Scraper::findLastOf(std::regex r)
{
    std::smatch m;
    std::string ret = "DNF";
    std::string::const_iterator searchStart( js.cbegin());
    while ( regex_search( searchStart, js.cend(), m, r ) )
    {
        ret = std::string(m[1]);
        searchStart += m.position() + m.length();
    }
    return ret;

}

std::vector<std::string> Scraper::findAllOf(std::regex r)
{
    std::vector<std::string> ret;
    std::smatch m;
    std::string::const_iterator searchStart( js.cbegin() );

    while ( regex_search( searchStart, js.cend(), m, r ) )
    {
        ret.push_back(m[1]);
        //imgSet.emplace_back(std::string("https:"+std::string(m[0])));
        itemsFound++;
        searchStart += m.position() + m.length();
    }
    return ret;
}

