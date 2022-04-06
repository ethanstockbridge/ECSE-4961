/**
 * @file Requests.cpp
 * @author Ethan Stockbridge (ethanstockbridge@gmail.com)
 * @author Devan Kidd (jjkidd1245@gmail.com)
 * @brief Requests function definitions
 * @date 2022-04-05
 * 
 */

#include "Request.h"

void Request::setStatus(unsigned int num)
{
    this->status=num;
}

unsigned int Request::getID()
{
    return this->transaction_ID;
}
unsigned int Request::getAmount()
{
    return this->amount;
}

unsigned int Request::getFrom()
{
    return this->from_account;
}

unsigned int Request::getTo()
{
    return this->to_account;
}

unsigned int Request::getStatus()
{
    return this->status;
}
