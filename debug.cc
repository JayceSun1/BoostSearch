#include "searcher.hpp"
#include <iostream>
#include <string>
#include <cstdio>
// #include <unistd.h>

const std::string input = "/home/ssj/Boost/boost-search/data/output/raw.txt";

int main()
{
    // for test
    ns_searcher::Searcher *search = new ns_searcher::Searcher();
    //std::cout << "search over--------------" << std::endl;
    //sleep(5);
    search->InitSearcher(input);

   // std::cout << "init over--------------" << std::endl;
   // sleep(5);

    std::string query;
    std::string json_string;
    char buffer[1024];
    while (true)
    {
        std::cout << "Please Enter Your Search Query# ";
        fgets(buffer, sizeof(buffer) - 1, stdin);
        buffer[strlen(buffer) - 1] = 0;
        query = buffer;
        // std::cin >> query;
        search->Search(query, &json_string);
        std::cout << json_string << std::endl;
    }
    return 0;
}