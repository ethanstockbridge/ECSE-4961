/**
 * @file User.cpp
 * @author Ethan Stockbridge (ethanstockbridge@gmail.com)
 * @author Devan Kidd (jjkidd1245@gmail.com)
 * @brief User function definitions
 * @date 2022-03-30
 * 
 */

#include "User.h"

void User::setBalance(unsigned int amount)
{
    this->balance=amount;
}

unsigned int User::getID()
{
    return this->ID;
}

unsigned int User::getBalance()
{
    return this->balance;
}