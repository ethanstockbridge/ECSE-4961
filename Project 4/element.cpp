/**
 * @file element.cpp
 * @author Ethan Stockbridge (ethanstockbridge@gmail.com)
 * @author Devan Kidd (jjkidd1245@gmail.com)
 * @brief Element class function definitions
 * @date 2022-03-07
 * 
 */

#include <string>
#include "element.h"

/**
 * @brief Construct a new element object
 * 
 * @param item Decoded data
 * @param id Encoded ID
 */
element::element(std::string item, int id)
{
    data=item;
    encodedID=id;
    count=1;
}

/**
 * @brief Return whether the decoded element is the same as the passed in item
 * 
 * @return true Decoded element is the same as the passed in item
 */
bool element::operator==(std::string item)
{
    return this->data==item;
}

/**
 * @brief Incriment the count of the item, representing how many times it was found
 * in the input column
 * 
 * @return int 
 */
int element::operator++(int)
{
    count+=1;
    return count;
}

/**
 * @brief Return the count of the element
 * 
 * @return int Count
 */
int element::getCount()
{
    return count;
}

/**
 * @brief Set the encoded ID of this element
 * 
 * @param encoded Specified ID to set this element to
 */
void element::setEncode(int encoded)
{
    this->encodedID=encoded;
}

/**
 * @brief Return the encoded ID
 * 
 * @return int Encoded ID
 */
int element::getEncoded()
{
    return encodedID;
}

/**
 * @brief Return the decoded data
 * 
 * @return std::string Decoded data
 */
std::string element::getDecoded()
{
    return data;
}
