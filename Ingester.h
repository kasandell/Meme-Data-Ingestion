#ifndef _INGESTER_H
#define _INGESTER_H

#include <vector>
#include <string>
#include "Scraper.h"
#include "ThreadPool.h"
using std::string;
using std::vector;

class Ingester
{
    public:
        Ingester();
        Ingester(std::string fName);
        Ingester(std::vector<std::pair< std::string, std::string> > memes);
        void ingest();

        
        class FileOpenError{};
    private:
        ThreadPool pool;
        std::pair<std::string, std::string> parseLine(const std::string& inp);
        std::vector<std::pair<std::string, std::string> > memes;
        

};




#endif
