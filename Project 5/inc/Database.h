/**
 * @file Database.h
 * @author Ethan Stockbridge (ethanstockbridge@gmail.com)
 * @author Devan Kidd (jjkidd1245@gmail.com)
 * @brief Database class
 * @date 2022-04-05
 * 
 */


#ifndef DATABASE_H

#define DATABASE_H

#include <sqlite3.h>
#include <iostream>
#include <vector>
#include "User.h"


/**
 * @brief Database class used to interface with SQLite
 * 
 */
class Database
{
private:
    /**
     * @brief SQLite3 object to connect us to the database
     * 
     */
    sqlite3* DB;
    /**
     * @brief List of users used to get information from
     * 
     */
    std::vector<User*> userList;
    /**
     * @brief Hashtable for user lookup based on ID
     * 
     */
    User** hashTable;
    /**
     * @brief Mutex lock for threading
     * 
     */
    pthread_mutex_t DBlock = PTHREAD_MUTEX_INITIALIZER; 

public:
    /**
     * @brief Construct a new Database object
     * 
     * @param fName file to import the database from
     */
    Database(std::string fName);
    /**
     * @brief Destroy the Database object
     * 
     */
    ~Database();
    /**
     * @brief Get the user, based on the unique ID given, using hashtable (O(1)) lookup
     * 
     * @param ID ID of desired user
     * @return User* Desired user
     */
    User* getUser(unsigned int ID);
    /**
     * @brief Force sync the database to disk
     * 
     */
    void syncDB();
    /**
     * @brief Print all users and balances in the database
     * 
     */
    void printDB();
};

#endif