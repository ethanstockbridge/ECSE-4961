/**
 * @file Bank.cpp
 * @author Ethan Stockbridge (ethanstockbridge@gmail.com)
 * @author Devan Kidd (jjkidd1245@gmail.com)
 * @brief Bank function definitions
 * @date 2022-04-05
 * 
 */

#include "Bank.h"

Bank::Bank(Database* importDB, Logger* importLog)
{
    this->myDB = importDB;
    this->myLog = importLog;
}

bool Bank::transfer(Request* req)
{

    User* user1 = myDB->getUser(req->getFrom());
    User* user2 = myDB->getUser(req->getTo());

    // user1->lock();
    // DEADLOCK ?
    // user2->lock();

    unsigned int amount = req->getAmount();

    if(req->getStatus()==0)
    {
        if(user1->getBalance()<0)
        {
            return false;
        }
        //write to journal 
        myLog->write(req->getID(), user1->getID(), user1->getBalance(), user1->getBalance()-amount);
        //set balance of user1
        user1->setBalance(user1->getBalance()-amount);
        this->myDB->syncDB();
    }

    //write to journal 
    myLog->write(req->getID(), user2->getID(), user2->getBalance(), user2->getBalance()+amount);
    //set balance of user2
    user2->setBalance(user2->getBalance()+amount);
    this->myDB->syncDB();


    return true;
}