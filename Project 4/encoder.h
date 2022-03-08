/**
 * @file encoder.h
 * @author Ethan Stockbridge (ethanstockbridge@gmail.com)
 * @author Devan Kidd (jjkidd1245@gmail.com)
 * @brief Encoder class to contain all of the encoder and query functionality
 * @date 2022-03-07
 * 
 */

#include <list>
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include "element.h"

/**
 * @brief Encoder class utilizing a hashtable. Encoding is performed by 
 * getting the hash of the input to index the hashtable. Decoding uses
 * the encoded ID to directly index the data from the decoder list. Hashtable will
 * resize at threshold to reduce collisions 
 * 
 */
class Encoder
{
private:
    std::list<element*>* hashtable; //Use hashtable as an encoder
    std::list<element*> decoder;    //Use list as decoder
    std::list<element*> input;      //Keep a history of the input column
    
    int num_items=0;
    int hashtable_size; 
    float threashold = 0.5; //resize when half full to increase efficiency of hashtable

    int getHash(std::string item);
    void resize();
    element* insert(std::string item);

public:
    //Constructor
    Encoder();

    //Destructor
    ~Encoder();
    
    //Main functions
    void encode(std::vector<std::string> items);
    void optimizeEncoding();
    void query(std::vector<std::string> input, std::string query);
    void writeEncoded(std::string fout);
};