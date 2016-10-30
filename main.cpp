#include <iostream>
#include "Scraper.h"
#include "Ingester.h"

using namespace std;
int main()
{
    //Scraper scraper("https://www.reddit.com/r/pepethefrog/", "pepe");
    //scraper.scrape();
    Ingester ingest("hi.txt");
    ingest.ingest();

    return 0;
}
