/**
 * @file Logger.cpp
 * @author Ethan Stockbridge (ethanstockbridge@gmail.com)
 * @author Devan Kidd (jjkidd1245@gmail.com)
 * @brief Logger function definitions
 * @date 2022-04-05
 * 
 */

#include "../inc/Logger.h"

void Logger::write(unsigned int transaction_ID, unsigned int account_no, unsigned int pre, unsigned int post)
{
    //lock
    pthread_mutex_lock(&(fileLock));
    std::ofstream loggerFile;
    loggerFile.open(fName, std::fstream::app);
    loggerFile << std::to_string(transaction_ID) << " " << std::to_string(account_no)<<" "
    <<std::to_string(pre)<<" "<<std::to_string(post)<<std::endl;
    loggerFile.close(); //flush stream to disk
    pthread_mutex_unlock(&(fileLock));
    //unlock
}

void Logger::clear()
{
    std::ofstream loggerFile;
    loggerFile.open(fName);
    loggerFile.close(); //flush stream to disk
}