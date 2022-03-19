/**
 * @file element.h
 * @author Ethan Stockbridge (ethanstockbridge@gmail.com)
 * @author Devan Kidd (jjkidd1245@gmail.com)
 * @brief Element class to classify the contents of the input file and keep track of
 * encoded ID, count and original decoded data
 * @date 2022-03-07
 * 
 */

class element
{
private:
    int count; //Number of times this data occurs in the column
    int encodedID;  //Encoded ID
    std::string data; //Decoded data
public:
    // Constructor
    element(std::string item, int id);

    // Setters
    void setEncode(const int& encoded);

    // Getters
    int getCount();
    int getEncoded();
    const std::string& getDecoded();

    // Operators
    bool operator==(const std::string& item);
    int operator++(int);
};

