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
 * @brief Utilize using intrinisics to calculate the mod of a number (a%b)
 * 
 * @param a hashed number
 * @param b mod to perform on the hashed number
 * @return int 
 */
unsigned int Encoder::SIMD_mod(const __m128i& a, const int& b)
{
    //a % b = a - int(float(a)/float(b))*b
    __m128 hashtablesize_1 = _mm_set1_ps(1.0f/b);
    __m128i hashtablesize = _mm_set1_epi32(b);
    
    //ans = int(float(a)/float(b)) = int(float(a) * float(1/b))
    __m128i ans = _mm_cvttps_epi32(_mm_mul_ps(_mm_cvtepi32_ps(a), hashtablesize_1));
    ans = _mm_sub_epi32(a, _mm_mul_epu32(ans, hashtablesize));
    unsigned int totalans[4];
    _mm_storeu_si128((__m128i*)(totalans), ans);
    return totalans[0];
}


/**
 * @brief Obtain the hash of the passed in string item by utilizing an
 * equation that can be evaluated by using math on the char number
 * 
 * @param item Passed in string to find hash of
 * @return int 
 */
unsigned int Encoder::getHash(const std::string& item)
{
    // unsigned int hash = (item.length()*item[0]*item[2] + item[0] + item[1])%this->hashtable_size;

    __m128i a = _mm_set1_epi32(item.length());
    __m128i b = _mm_set1_epi32(item[0]);
    __m128i c = _mm_set1_epi32(item[2]);
    __m128i d = _mm_set1_epi32(item[1]);

    a=_mm_mul_epu32(a,b);
    b=_mm_mul_epu32(a,c);
    a=_mm_add_epi32(a,b);
    a=_mm_add_epi32(a,d);

    unsigned int hash[4];
    _mm_storeu_si128((__m128i*)(hash), a);

    if(hash[0] >= this->hashtable_size)
    {
        hash[0] = SIMD_mod(a,this->hashtable_size);
    }

    return hash[0];
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
element* Encoder::insert(const std::string& item)
{
    if(num_items > hashtable_size*threashold)
    {
        resize();
    }
    unsigned int hash = this->getHash(item);
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
void Encoder::encode(const std::vector<std::string>& items)
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
int Encoder::query(const std::vector<std::string>& input, const std::string& query)
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
        return encodedQuery;
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
    return count;
}

/**
 * @brief Write the encoding to a file.
 * This consists of first, the dictionary and then the encoded column data 
 * 
 * @param fout 
 */
void Encoder::writeEncoded(const std::string& fout)
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