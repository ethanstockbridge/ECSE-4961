/**
 * @file encoder.cpp
 * @author Ethan Stockbridge (ethanstockbridge@gmail.com)
 * @author Devan Kidd (jjkidd1245@gmail.com)
 * @brief Encoder class function definitions
 * @date 2022-03-07
 * 
 */

#include <immintrin.h> // For SIMD functions
#include <sstream>
#include "encoder.h"

/**
 * @brief Helper function to sort the decoder list when optimizing the encodedID
 * 
 * @param a Element A
 * @param b Element B
 * @return true A appears more times in the file than B
 */
bool ptrSort(element* a, element* b)
{
    return a->getCount() > b->getCount();
}

/**
 * @brief Construct for the encoder object. Initialize the hashtable with 1000 spaces
 * 
 */
Encoder::Encoder()
{
    this->hashtable_size=1000;
    this->hashtable = new std::list<element*>[hashtable_size];
}

/**
 * @brief Destroy the Encoder object, free the hashtable and all stored objects
 * 
 */
Encoder::~Encoder()
{
    for(std::list<element*>::iterator itr=decoder.begin()
        ;itr!=decoder.end(); itr++)
    {
        delete *itr;
    }
    delete[] this->hashtable;
}

/**
 * @brief Obtain the hash of the passed in string item by utilizing a mathematical
 * equation that can be evaluated using intrinsics instructions on the char number
 * 
 * @param item Passed in string to find hash of
 * @return int 
 */
unsigned int Encoder::getHash(std::string item)
{
    short nums[8] = {item[2],short(item.length()),item[3],short(item.length()),
                    item[1],item[1],item[1],item[1]};
    __m128i a = _mm_load_si128((__m128i const*)nums);
    __m128i sum = _mm_add_epi16(a,_mm_srli_epi16(a,4));
    _mm_store_si128((__m128i*)(nums), sum);
    return int(nums[0]*nums[1]*nums[2]+nums[3])%hashtable_size;
}

/**
 * @brief Resize the hash table if the current amount of elements is greater than 
 * hashtable_size*threashold to avoid additional unneeded collisions, improving 
 * throughput
 * 
 */
void Encoder::resize()
{
    int oldsize = hashtable_size;
    //increase new hashtable size by 2x
    this->hashtable_size = oldsize*2;
    std::list<element*>* oldtable = this->hashtable;
    this->hashtable = new std::list<element*>[this->hashtable_size];
    //rehash all from current decoder to new hashtable
    for(std::list<element*>::iterator itr=decoder.begin()
        ;itr!=decoder.end(); itr++)
    { 
        int hash = getHash((*itr)->getDecoded());
        this->hashtable[hash].push_back(*itr);
    }
    //free old table
    delete[] oldtable;
}

/**
 * @brief Insert the new item into the hashtable if not already there. Increase it's 
 * count if existant in the hashtable already.
 * 
 * @param item Passed in string item
 * @return element* Found/newly made element that corresponds to the std::string
 */
element* Encoder::insert(std::string item)
{
    if(num_items > hashtable_size*threashold)
    {
        resize();
    }
    int hash = getHash(item);
    for(std::list<element*>::iterator itr=hashtable[hash].begin()
        ;itr!=hashtable[hash].end(); itr++)
    {
        if(**itr==item)
        { //incriment the count of the element
            (**itr)++;
            return *itr;
        }
    }
    element* newentry = new element(item, decoder.size());
    decoder.push_back(newentry);
    hashtable[hash].push_back(newentry);
    this->num_items+=1;
    return newentry;
}

/**
 * @brief Optimize the output encoding by sorting the decoder. Having the items which
 * appear the most times, with the lowest encoding, uses up the least amount of data 
 * bits when encoded.
 * 
 */
void Encoder::optimizeEncoding()
{
    decoder.sort(ptrSort); //most to least "count"
    int encodedID = 0;
    for(std::list<element*>::iterator itr=decoder.begin()
        ;itr!=decoder.end(); itr++)
    {
        (*itr)->setEncode(encodedID);
        encodedID+=1;
    }
}

/**
 * @brief Encode all of the data from the column
 * 
 * @param items Separated column data
 */
void Encoder::encode(std::vector<std::string> items)
{
    for(unsigned int i=0;i<items.size();i++)
    {
        this->input.push_back(this->insert(items[i]));
    }
}

/**
 * @brief Find out whether a string is existant in the encoded data. If it is, 
 * count the amount of times it appears in the encoded data.
 * 
 * @param input Encoded column dictionary + encoded data
 * @param query String to be queried
 */
void Encoder::query(std::vector<std::string> input, std::string query)
{
    unsigned int i=1;
    int encodedQuery=-1;
    // Go through the dictionary in the encoded file to search for query
    for(;i<input.size();i++)
    {
        // Stop checking when we start data
        if(input[i]=="data:")
        {
            i++;
            break;
        }
        if(input[i]==query)
        { //found encoding, equal to it's numerical position
            encodedQuery=i-1;
        }
    }
    // Query was not in the dictionary, quit now
    if(encodedQuery == -1)
    {
        std::cout<<"Query: "<<query<<" could not be found in the dictionary"<<std::endl;
        exit(0);
    }
    // Count encoded data if == to encoded query
    int count=0;
    for(;i<input.size();i++)
    {
        if(std::stoi(input[i]) == encodedQuery)
        {
            count+=1;
        }
    }
    std::cout<<"Query: "<<query<<" was found "<<count<<" times in the dictionary"<<std::endl;
}

/**
 * @brief Write the encoding to a file.
 * This consists of first, the dictionary and then the encoded column data 
 * 
 * @param fout 
 */
void Encoder::writeEncoded(std::string fout)
{
    std::stringstream outfile;
    //write dictionary to top of file:
    outfile << "dictionary:" << std::endl;
    for(std::list<element*>::iterator itr=decoder.begin()
        ;itr!=decoder.end(); itr++)
    {
        outfile << (*itr)->getDecoded() << std::endl;
    }
    //write data after:
    outfile << "data:" << std::endl;
    for(std::list<element*>::iterator itr=input.begin()
        ;itr!=input.end(); itr++)
    {
        outfile << (*itr)->getEncoded() << std::endl;
    }
    //write stream to files
    std::ofstream ofile(fout);
    ofile<<outfile.str();
}