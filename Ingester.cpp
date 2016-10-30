#include "Ingester.h"
#include <fstream>
#include <iostream>
#include <future>

std::mutex outMtx;

Ingester::Ingester()
{

}

Ingester::Ingester(std::string fName)
{
    std::ifstream inp(fName);
    if(inp.is_open())
    {
        std::string ln;
        while(std::getline(inp, ln))
        {
            auto x = parseLine(ln);
            this->memes.push_back(x);
        }
    }
    else{
        throw(FileOpenError());
    }
}

Ingester::Ingester(std::vector<std::pair< std::string, std::string> > memes): memes(memes)
{

}

std::pair<std::string, std::string> Ingester::parseLine(const std::string& inp)
{
    std::size_t sPos = inp.find(" ");
    std::string s1;
    std::string s2;
    s1 = inp.substr(0, sPos);
    s2 = inp.substr(sPos+1);
    return std::pair<std::string, std::string>(s1, s2);
}


void Ingester::ingest()
{
    std::vector<std::future<bool> > results;
    for(auto &x: memes)
    {
        results.emplace_back(
                pool.enqueue([&x](){
                        {
                            std::unique_lock<std::mutex> lck(outMtx);
                            std::cout<<"In lambda: " << x.first<<", "<<x.second<<std::endl;
                        }
                        Scraper s(x.first, x.second);
                        s.scrape();
                        return true;
                    })
                );
    }
    for(auto&& res: results)
    {
        std::cout<<"No issues"<<std::endl;
        res.get();
        std::cout<<"Issues"<<std::endl;
    }
}
