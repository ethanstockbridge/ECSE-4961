#include <iostream>
#include <fstream>
#include <pthread.h>
#include "zstd.h"       // Assume installed 
#include <vector>
#include <set>
#include <unistd.h>


/**
 * @brief Number of threads to be created to comrpess data
 * 
 */
#define NUM_WORKERS 3

/**
 * @brief Size (in bytes) of each chunk to be compressed
 * 
 */
#define CHUNK_SIZE 16000 //16000bytes = 16kb

/**
 * @brief keep only MAX_RAW_CHUNKS amount of chunks available at any given time
 * this greatly reduces the amount of ram used. read in more as needed
 * 
 */
#define MAX_RAW_CHUNKS NUM_WORKERS*2


//use a thread lock so multiple workers can work from the same bins of jobs
pthread_mutex_t raw_lock; 
pthread_mutex_t compressed_lock; 


//flags to indicate status of work
bool reading_complete = false;
bool writing_complete = false;


/**
 * @brief prototype class for data storage
 * 
 */
class chunk
{
private:
    char* data; //data of chunk
    int dataSize; //size of data
    int compressedDataSize; //size of data
    char* compressedData; //size of data
    int id; //nth chunk's number

public:
    chunk(int mid,char* mdata,int mdataSize) : id(mid),data(mdata),dataSize(mdataSize)
    {
        ;
    }
    void compress()
    {
        // Compress with a thread using zstd
        size_t const cBuffSize = ZSTD_compressBound(this->dataSize);
        void* const cBuff = malloc(cBuffSize);
        size_t const cSize = ZSTD_compress(cBuff, cBuffSize, this->data, this->dataSize, 1);
        this->compressedData = (char*)cBuff;
        this->compressedDataSize = cSize;
    }
    unsigned int getID() const {return id;}
    unsigned int getDataSize() const {return dataSize;}
    char* getData() const {return data;}
    unsigned int getCompressedDataSize() const {return compressedDataSize;}
    char* getCompressedData() const {return compressedData;}
    bool operator< (const chunk &right) const
    {
        return this->id < right.id;
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
    std::cout << "Thread #" << myID << " starting"<< std::endl;

    while(!writing_complete)
    {
        //check for availble chunks
        if(raw.size()>0)
        { //we found one (possibly). lock thread and try to get it if not already taken
            pthread_mutex_lock(&(raw_lock));
            if(raw.size()>0)
            {
                // start compression on that chunk
                chunk mychunk = raw[0];
                raw.erase(raw.begin());
                pthread_mutex_unlock(&(raw_lock));

                mychunk.compress();

                //save to global set to be written
                pthread_mutex_lock(&(compressed_lock));
                compressed.insert(mychunk); //insert uncompressed just for testing
                pthread_mutex_unlock(&(compressed_lock));
            }
            else
            {
                pthread_mutex_unlock(&(raw_lock));
            }
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
void manageChunks(const char* inFilename, const char* outFilename)
{
    unsigned int fsize = 0; //total size of read in file

    std::ifstream fin(inFilename);
    std::ofstream fout(outFilename);
    uint chunksRead=0;
    uint chunkSize;
    uint chunksWritten=0;

    while(!reading_complete || !writing_complete)// && writing_complete))
    {
        if(!reading_complete && raw.size()<MAX_RAW_CHUNKS)
        {
            // grab new chunk if there are still chunks to be read
            // and we arent at max chunk size yet
            char* someData = new char[CHUNK_SIZE];
            fin.read(someData, CHUNK_SIZE);
            chunkSize = fin.gcount();
            if(chunkSize==0)
            {
                //no more chunks to be read in from the file, stop reading
                reading_complete=true;
                std::cout<<"READ COMPLETE"<<std::endl;
            }
            else
            {   //successfully read in a chunk
                // std::cout<<"Read in "<<chunkSize<<" bytes from file"<<std::endl;
                fsize+=chunkSize;
                pthread_mutex_lock(&(raw_lock));
                raw.push_back(chunk(chunksRead,someData,chunkSize));
                pthread_mutex_unlock(&(raw_lock));
                chunksRead++;
            }
        }
        if(!writing_complete)
        { 
            //writing not complete, check for next chunk if it has been compressed yet
            if((compressed.begin())->getID() == chunksWritten)
            {   //next, in-order chunk was added, so write to file now.
                fout.write(compressed.begin()->getCompressedData(),compressed.begin()->getCompressedDataSize());
                // fout.write(compressed.begin()->data,compressed.begin()->dataSize);
                // std::cout<<"Wrote "<<compressed.begin()->getDataSize()<<" bytes to file"<<std::endl;
                delete compressed.begin()->getData();
                delete compressed.begin()->getCompressedData();
                pthread_mutex_lock(&(compressed_lock));
                compressed.erase(compressed.begin()); //erase from list
                pthread_mutex_unlock(&(compressed_lock));
                chunksWritten+=1;
            }
            if(reading_complete && chunksWritten==chunksRead)
            {   //we reached the last chunk to be written. everything is done now.
                std::cout<<"WRITE COMPLETE"<<std::endl;
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

    if (argc!=3) {
        std::cout<<"Error: Incorrect arguments"<<std::endl;
        std::cout<<"Usage: "<<exeName<<" <INPUT FILE> <OUTPUT FILE>"<<std::endl;;
        return 1;
    }

    const char* inFilename = argv[1];
    const char* outFilename = argv[2];
    
    //dispatch workers to process chunks:
    pthread_t threads[NUM_WORKERS];
    int err;
    long i;

    for( i = 0; i < NUM_WORKERS; i++ ) {
        // Create new thread:
        err = pthread_create(&threads[i], NULL, threadTask, (void*)i);
        if (err) {
            std::cout << "Error:unable to create thread," << err << std::endl;
            exit(-1);
        }
    }

    //start reading in the file as chunks and write them when workers are done:
    manageChunks(inFilename, outFilename); 

    return 0;
}

