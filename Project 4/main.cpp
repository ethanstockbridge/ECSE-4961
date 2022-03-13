/**
 * @file main.cpp
 * @author Ethan Stockbridge (ethanstockbridge@gmail.com)
 * @author Devan Kidd (jjkidd1245@gmail.com)
 * @brief Main file for performing dictionary encoding and query
 *  on a file with column based data
 * @date 2022-03-07
 * 
 */

#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <string>
#include <chrono> // For timing
#include "encoder.h"

/**
 * @brief Main function to perform dictionary encoding or query
 * 
 * @param argc Amount of passed in arguments
 * @param argv Program parameters
 */
int main(int argc, const char** argv)
{
    const char* exeName = argv[0];

    Encoder myEncoder = Encoder();

    if (argc!=4) {
        std::cout<<"Error: Incorrect arguments"<<std::endl;
        std::cout<<"Usages: "<<std::endl;
        std::cout<<exeName<<" ENCODE inputfile.txt outputfile.txt"<<std::endl;
        std::cout<<exeName<<" QUERY inputfile.txt query"<<std::endl;
        return 1;
    }

    if(std::string(argv[1]) == "ENCODE")
    {
        std::ifstream infile(argv[2]);
        if (infile.fail())
        {
            std::cerr<<"Error operning file "<<argv[2]<<std::endl;
            exit(1);
        }

        std::cout<<"Reading file contents..."<<std::endl;
        std::vector<std::string> input;
        std::string line;
        while(infile >> line)
        {
            input.push_back(line);
        }

        auto start = std::chrono::high_resolution_clock::now();
        
        std::cout<<"Encoding file contents..."<<std::endl;
        myEncoder.encode(input);

        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        std::cout<<"Encoding process took "<<duration.count()*1e-6<<"s"<<std::endl;
        
        #ifdef OPTIMIZE_ENCODING
        std::cout<<"Optimizing encoding..."<<std::endl;
        start = std::chrono::high_resolution_clock::now();
        myEncoder.optimizeEncoding();
        stop = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        std::cout<<"Optimization process took "<<duration.count()*1e-6<<"s"<<std::endl;
        #endif

        std::cout<<"Writing resulting file..."<<std::endl;
        start = std::chrono::high_resolution_clock::now();
        myEncoder.writeEncoded(std::string(argv[3]));
        stop = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        std::cout<<"File writing process took "<<duration.count()*1e-6<<"s"<<std::endl;
    
    }

    if(std::string(argv[1]) == "QUERY")
    {
        std::ifstream infile(argv[2]);
        if (infile.fail())
        {
            std::cerr<<"Error operning file "<<argv[2]<<std::endl;
            exit(1);
        }

        // read in the dictionary and data from input file
        std::vector<std::string> input;
        std::string line;
        while(infile >> line)
        {
            input.push_back(line);
        }

        std::string query = std::string(argv[3]);

        auto start = std::chrono::high_resolution_clock::now();
        int count = myEncoder.query(input, query);
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        std::cout<<"Query took "<<duration.count()*1e-6<<"s"<<std::endl;
        if(count==-1)
            std::cout<<"Query: "<<query<<" could not be found in the dictionary"<<std::endl;
        else
            std::cout<<"Query: "<<query<<" was found "<<count<<" times in the dictionary"<<std::endl;
    }
    return 0;
}
