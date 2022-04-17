/**
 * @file Logger.h
 * @author Ethan Stockbridge (ethanstockbridge@gmail.com)
 * @author Devan Kidd (jjkidd1245@gmail.com)
 * @brief Logger class
 * @date 2022-04-05
 * 
 */


#ifndef LOGGER_H

#define LOGGER_H

#include <iostream>
#include <fstream>


/**
 * @brief Logger class used to write journal of transactions to keep track of any faults
 * 
 */
class Logger
{
private:
    /**
     * @brief Name of the file to log to
     * 
     */
    std::string fName;
    /**
     * @brief Mutex for writing during threading
     * 
     */
    pthread_mutex_t fileLock = PTHREAD_MUTEX_INITIALIZER; 
public:
    /**
     * @brief Construct a new Logger object
     * 
     */
    Logger(){}
    /**
     * @brief Construct a new Logger object given filename
     * 
     * @param filename 
     */
    Logger(std::string filename) : fName(filename) {}
    /**
     * @brief Write new data to the log file 
     * 
     * @param transaction_ID ID of the transaction being performed
     * @param account_no ID of the user performing a transaction
     * @param pre The balance of the user before transaction performed
     * @param post The balance of the user after transaction performed
     */
    void write(unsigned int transaction_ID, unsigned int account_no, 
        unsigned int pre, unsigned int post);
    /**
     * @brief Clear the log file
     * 
     */
    void clear();
};

#endif