/**
 * @file chunk.h
 * @author Ethan Stockbridge (ethanstockbridge@gmail.com)
 * @author Devan Kidd (jjkidd1245@gmail.com)
 * @brief Chunk class header file
 * @date 2022-02-06
 */

#ifndef CHUNK_H_
#define CHUNK_H_

/**
 * @brief Chunk class used for storing chunks of data, compressing them, and then storing 
 * the compressed data, to be then written into the output file later.
 * 
 */
class chunk
{
private:
    /**
     * @brief uncompressed and compressed chunk data
     * 
     */
    char* data;
    char* compressedData;

    /**
     * @brief side of the uncompressed and compressed data
     * 
     */
    int dataSize;
    int compressedDataSize;

    /**
     * @brief Chunk's number representing the n'th chunk read in from the file
     * 
     */
    int id;

public:
    /**
     * @brief Construct a new chunk object
     * 
     * @param mid Chunk's number relative to the number of chunks read in from the file
     * @param mdata Chunk's uncompressed size in bytes
     * @param mdataSize Chunk's data 
     */
    chunk(int mid,char* mdata,int mdataSize) : id(mid),data(mdata),dataSize(mdataSize)
    {
        ;
    }

    /**
     * @brief Compress the data using ZSTD compression algorithms. To be performed by the
     * currently active thread. Store the compressed data into this object
     * 
     */
    void compress()
    {
        // Compress with a thread using zstd
        size_t const cBuffSize = ZSTD_compressBound(this->dataSize);
        char* const cBuff = new char[cBuffSize];
        size_t const cSize = ZSTD_compress(cBuff, cBuffSize, this->data, this->dataSize, COMPRESSION_LEVEL);
        this->compressedData = cBuff;
        this->compressedDataSize = cSize;
    }
    /**
     * @brief Get the chunk's id
     * 
     * @return unsigned int 
     */
    unsigned int getID() const {return id;}

    /**
     * @brief Get the size of the uncompressed data
     * 
     * @return unsigned int 
     */
    unsigned int getDataSize() const {return dataSize;}

    /**
     * @brief Get the size of the compressed data 
     * 
     * @return unsigned int 
     */
    unsigned int getCompressedDataSize() const {return compressedDataSize;}

    /**
     * @brief Get the uncompressed data
     * 
     * @return char* 
     */
    char* getData() const {return data;}

    /**
     * @brief Get the compressed data
     * 
     * @return char* 
     */
    char* getCompressedData() const {return compressedData;}

    /**
     * @brief Comparing operator "<" for std::set. Used to orient the chunks from
     * ID being lowest to highest, so the file can be re-assembled in order
     * 
     * @param right passed in chunk object
     * @return true 
     * @return false 
     */
    bool operator< (const chunk &right) const
    {
        return this->id < right.id;
    }
};

#endif