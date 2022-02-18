/**
 * @file main.cpp
 * @author Ethan Stockbridge (ethanstockbridge@gmail.com)
 * @author Devan Kidd (jjkidd1245@gmail.com)
 * @brief Main file for performing c++ ZSTD compression on a single file using
 * pthread multithreading
 * @date 2022-02-06
 */

#include <iostream>
#include <fstream>
#include <pthread.h>
#include "zstd.h"       // Assume already installed 
#include <chrono> // For timing

#include <vector>
#include <set>

/**
 * @brief Compression level for ZSTD usage. Default 50
 * 
 */
#ifndef COMPRESSION_LEVEL
#define COMPRESSION_LEVEL 50
#endif

#include "chunk.h"      //Data storage class

/**
 * @brief Size (in bytes) of each chunk to be compressed. Default 16KB
 * 
 */
#ifndef CHUNK_SIZE
#define CHUNK_SIZE 16*1024 // 16KB
#endif

/**
 * @brief keep only MAX_RAW_CHUNKS amount of chunks available at any given time.
 * This reduces the amount of RAM used by reading in more chunks as needed 
 * instead of reading in the entire input file all at once.
 * 
 */
unsigned int MAX_RAW_CHUNKS = 0;

// use a thread lock so multiple workers can work from the same bin 
pthread_mutex_t raw_lock; 
pthread_mutex_t compressed_lock; 

//flags to indicate status of main thread 
bool reading_complete = false;
bool writing_complete = false;

/**
 * @brief data storage of raw data and compressed data from worker threads
 * 
 */
std::set<chunk> compressed; //one chunk per thread
std::vector<chunk> raw; //one chunk per thread


// Forward declaration
void *threadCompress(void *id);
void manageChunks(const char* inFilename, const char* outFilename);


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

    if (argc!=4) {
        std::cout<<"Error: Incorrect arguments"<<std::endl;
        std::cout<<"Usage: "<<exeName<<" <NUM WORKERS> <INPUT FILE> <OUTPUT FILE>"<<std::endl;;
        return 1;
    }

    const char* numWorkers = argv[1];
    unsigned int NUM_WORKERS = std::stoi(argv[1]);
    const char* inFilename = argv[2];
    const char* outFilename = argv[3];

    //allocate 10*number of worker threads of raw chunks available to all workers
    MAX_RAW_CHUNKS = NUM_WORKERS*10;
    
    //dispatch workers to process chunks:
    pthread_t threads[NUM_WORKERS];
    int err;
    long i;

    std::cout << "Using " << NUM_WORKERS << " threads for compression" << std::endl;

    for( i = 0; i < NUM_WORKERS; i++ ) {
        // Create new thread:
        err = pthread_create(&threads[i], NULL, threadCompress, (void*)i);
        if (err)
        {
            std::cout << "Error: Unable to create thread," << err << std::endl;
            exit(-1);
        }
    }

    //start reading in the file as chunks and write them when workers are done:
    auto start = std::chrono::high_resolution_clock::now();
    manageChunks(inFilename, outFilename); 
    auto stop = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    std::cout<<"Compression took "<<duration.count()*1e-6<<"s\n"<<std::endl;
    return 0;
}


/**
 * @brief Worker thread's function. Monitor the raw data vector and take from it when
 * data appears. Then compress and add to the compressed set to keep data in-order.
 * 
 * @param id Thread's numerical ID
 */
void *threadCompress(void *id)
{
    long myID = (long)id;
    // std::cout << "Thread #" << myID << " starting"<< std::endl;

    while(!writing_complete)
    {
        //check for availble chunks
        if(raw.size()>0)
        { //we found one (possibly). lock thread and try to get it if not already taken
            pthread_mutex_lock(&(raw_lock));
            if(raw.size()>0)
            {
                // std::cout<<"thread "<<id<<" grabbed chunk"<<std::endl;
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
    unsigned long readSize = 0; //total amount of bytes read in
    unsigned long writeSize=0; //total amount of bytes written
    uint chunkWriteOrder=0; //amount of chunks written
    uint chunksRead=0; //amount of CHUNK_SIZE chunks read in
    uint chunkSize; //current size of chunk (may differ from CHUNK_SIZE for last chunk)

    std::ifstream fin(inFilename);
    std::ofstream fout(outFilename);

    //get total file size
    const auto begin = fin.tellg();
    fin.seekg (0, std::ios::end);
    const auto end = fin.tellg();
    const auto totalFileSize = (end-begin);
    fin.seekg (0, std::ios::beg); //move back to beginning of file

    if(totalFileSize==0)
    {
        std::cout << "Error: Input file size cannot be zero" << std::endl;
        exit(1);
    }

    std::cout << "Input file '" << inFilename << "' size: " << totalFileSize << " Bytes" << std::endl;


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
                std::cout << "Finishing up writing compressed output file now..." << "\r";
                std::cout.flush();
            }
            else
            {   //successfully read in a chunk
                std::cout << "\33[2K" << "Progress: " << (int)((100*readSize)/totalFileSize) << "%\r";
                std::cout.flush();
                readSize+=chunkSize;
                //lock to prevent incorrect read
                pthread_mutex_lock(&(raw_lock));
                raw.push_back(chunk(chunksRead,someData,chunkSize));
                pthread_mutex_unlock(&(raw_lock));
                chunksRead++;
            }
        }
        if(!writing_complete)
        { 
            //writing not complete, check if next in-order chunk has been compressed yet
            if((compressed.begin())->getID() == chunkWriteOrder)
            {   //next, in-order chunk was added, so write to file now.
                fout.write(compressed.begin()->getCompressedData(),compressed.begin()->getCompressedDataSize());
                writeSize+=compressed.begin()->getCompressedDataSize();
                delete[] compressed.begin()->getData();
                delete[] compressed.begin()->getCompressedData();
                pthread_mutex_lock(&(compressed_lock));
                compressed.erase(compressed.begin()); //erase from list
                pthread_mutex_unlock(&(compressed_lock));
                chunkWriteOrder+=1;
            }
            if(reading_complete && chunkWriteOrder==chunksRead)
            {   //we reached the last chunk to be written. everything is done now, we may exit
                //clear line "finishing up writing ..." and output file name and size
                std::cout << "\33[2K" << "Output file '" << outFilename << "' size: " << writeSize << " Bytes" << std::endl;
                std::cout.flush();
                writing_complete=true; 
            }
        }
    }
}
