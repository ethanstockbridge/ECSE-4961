#include <iostream>
#include <fstream>
#include <pthread.h>
#include "zstd.h"       // Assume installed 
#include <vector>
#include <set>


/**
 * @brief Number of threads to be created to comrpess data
 * 
 */
#define NUM_WORKERS 3

/**
 * @brief Size (in bytes) of each chunk to be compressed
 * 
 */
#define CHUNK_SIZE 4 //16000bytes = 16kb

/**
 * @brief keep only MAX_RAW_CHUNKS amount of chunks available at any given time
 * this greatly reduces the amount of ram used. read in more as needed
 * 
 */
#define MAX_RAW_CHUNKS 10


//use a thread lock so multiple workers dont compress the same raw data
pthread_mutex_t raw_lock; 

//flags to indicate status of work
bool reading_complete, writing_complete = false;


/**
 * @brief prototype class for data storage
 * 
 */
class chunk
{
// private:

public:
    char* data; //data of chunk
    int dataSize; //size of data

    int ord; //nth chunk's number
    chunk(int mord,char* mdata,int mdataSize) : ord(mord),data(mdata),dataSize(mdataSize)
    {
        ;
    }
    bool operator< (const chunk &right) const
    {
        return ord < right.ord;
    }
};


/**
 * @brief data storage of raw data and compressed data from worker threads
 * 
 */
std::set<chunk> compressed; //one chunk per thread
std::vector<chunk> raw; //one chunk per thread


/**
 * @brief Worker thread's function. Monitor the raw data vector and take from it when
 * data appears. Then compress and add to the compressed set to keep data in-order.
 * 
 * @param id Thread's numerical ID
 */
void *threadTask(void *id)
{
    long myID = (long)id;
    std::cout << "Thread " << myID << " starting now"<< std::endl;

    while(!writing_complete)
    {
        //check for more chunks
        pthread_mutex_lock(&(raw_lock));
        if(raw.size()>0)
        { //we found one. start compression on that chunk
            chunk mychunk = raw[0];
            raw.erase(raw.begin());
            pthread_mutex_unlock(&(raw_lock));
            // THREAD COMPRESS DATA
            compressed.insert(mychunk); //insert uncompressed just for testing
        }
        else{
            pthread_mutex_unlock(&(raw_lock));
        }
    }

    return id;
}

/**
 * @brief Function to manage the chunks. This reads in chunks of data from the input file
 * and has up to MAX_RAW_CHUNKS available at any given time. When compressed chunks are 
 * sensed, this then writes them to the output file. 
 * Basically, keeps the worker threads fed at all times and puts away their results in order.
 * 
 * @param fname Input file name
 */
void manageChunks(const char* fname)
{
    unsigned int fsize = 0; //total size of read in file

    std::ifstream fin(fname);
    std::ofstream fout("./b.txt");
    uint chunkID=0;
    uint count;
    uint writeOrder=0;

    while(!(reading_complete))// && writing_complete))
    {
        if(!reading_complete && raw.size()<MAX_RAW_CHUNKS)
        {//grab new chunk if there are still chunks to be read
        // and we arent at max chunk size yet
            char* someData = new char[CHUNK_SIZE];
            fin.read(someData, CHUNK_SIZE);
            count = fin.gcount();
            if(count==0)
            {   //no more chunks to be read in from the file, stop reading
                reading_complete=true;
            }
            else
            {   //successfully read in a chunk
                std::cout<<"Read in "<<count<<" bytes from file"<<std::endl;
                fsize+=count;
                pthread_mutex_lock(&(raw_lock));
                raw.push_back(chunk(chunkID,someData,count));
                pthread_mutex_unlock(&(raw_lock));
                chunkID++;
            }
        }
        if(!writing_complete)
        {  //writing not complete, check for next chunk if it has been compressed yet
            if((compressed.begin())->ord == writeOrder)
            {   //next, in-order chunk was added, so write to file now.
                fout.write(compressed.begin()->data,compressed.begin()->dataSize);
                std::cout<<"Wrote "<<compressed.begin()->dataSize<<" bytes to file"<<std::endl;
                delete compressed.begin()->data;
                compressed.erase(compressed.begin()); //erase from list
                writeOrder+=1;
            }
            if(reading_complete && writeOrder==chunkID)
            {   //we reached the last chunk to be written. everything is done now.
                writing_complete=true; 
            }
        }
    }
}


/**
 * @brief Main function, responsible for creating worker threads and starting the main 
 * chunk manager. 
 * 
 * @param argc Input argument count
 * @param argv Input arguments (char)
 * @return int 
 */
int main(int argc, const char** argv)
{
    const char* exeName = argv[0];

    if (argc!=2) {
        std::cout<<"Error: Incorrect arguments"<<std::endl;
        std::cout<<"Usage: "<<exeName<<" <FILE>"<<std::endl;;
        return 1;
    }

    const char* inFilename = argv[1];

    //dispatch workers to process chunks:
    pthread_t threads[NUM_WORKERS];
    int err;
    long i;

    for( i = 0; i < NUM_WORKERS; i++ ) {
        std::cout << "creating thread: " << i << std::endl;
        err = pthread_create(&threads[i], NULL, threadTask, (void*)i);

        if (err) {
            std::cout << "Error:unable to create thread," << err << std::endl;
            exit(-1);
        }
    }

    //start reading in the file as chunks and write them when workers are done:
    manageChunks(inFilename); 
    return 0;
}

