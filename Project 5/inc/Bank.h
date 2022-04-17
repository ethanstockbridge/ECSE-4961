/**
 * @file Bank.h
 * @author Ethan Stockbridge (ethanstockbridge@gmail.com)
 * @author Devan Kidd (jjkidd1245@gmail.com)
 * @brief Bank class
 * @date 2022-04-05
 * 
 */


#ifndef BANK_H

#define BANK_H

#include "Database.h"
#include "Logger.h"
#include "Request.h"


/**
 * @brief Bank class to manage user's transactions and record to the database, and logger
 * 
 */
class Bank
{
private:
    /**
     * @brief Database and logger for this bank's transactions
     * 
     */
    Database* myDB;
    Logger* myLog;
public:
    /**
     * @brief Construct a new Bank object
     * 
     * @param importDB Database to get/write the user information to
     * @param importLog Log to write the transactions to
     */
    Bank(Database* importDB, Logger* importLog);

    /**
     * @brief Perform the request, req
     * 
     * @param req Request to be performed
     * @return true Success
     * @return false Failure (possibly that user1 had no money)
     */
    bool transfer(Request* req);
};

#endif