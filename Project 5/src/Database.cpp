/**
 * @file Database.cpp
 * @author Ethan Stockbridge (ethanstockbridge@gmail.com)
 * @author Devan Kidd (jjkidd1245@gmail.com)
 * @brief Database function definitions
 * @date 2022-04-05
 * 
 */

#include "../inc/Database.h"


Database::Database(std::string fName)
{
    int retval = sqlite3_open(fName.c_str(), &(this->DB));
    if(retval)
    {
        std::cerr << "Error opening/creating DB " << sqlite3_errmsg(DB) << std::endl;
    }
    else
    {
        std::cout << "Loaded database from file: " << fName << std::endl;
    }
    sqlite3_stmt* stmt;
    retval = sqlite3_prepare_v2(this->DB, "select * from users", -1, &stmt, nullptr);
    if(retval != SQLITE_OK) {
        std::cout << "ERROR: while compiling sql: " << sqlite3_errmsg(this->DB) << std::endl;
        exit(1);
    }
    int ret_code = 0;
    while((ret_code = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        unsigned int id=sqlite3_column_int(stmt, 0);
        unsigned int balance=sqlite3_column_int(stmt, 1);
        User* newuser = new User(id,balance);
        this->userList.push_back(newuser);
    }
    this->hashTable = new User*[100];
    for(unsigned int i=0; i<this->userList.size(); i++)
    {
        this->hashTable[this->userList[i]->getID()] = this->userList[i];
    }
    sqlite3_finalize(stmt);
}

Database::~Database()
{
    sqlite3_close(this->DB);
    for(int i=0;i<userList.size();i++)
    {
        delete this->userList[i];
    }
    delete [] this->hashTable;
}

User* Database::getUser(unsigned int ID)
{//trivial implimentation but fast. use unique ID to find location of user in a hashtable
    return this->hashTable[ID];
}

void Database::syncDB()
{
    pthread_mutex_lock(&(DBlock));
    for(std::vector<User*>::iterator itr=this->userList.begin();
        itr!=this->userList.end(); itr++)
    {
        User* user1 = *itr;
        std::string query = "update users set balance="+
            std::to_string(user1->getBalance())+" where id="+
            std::to_string(user1->getID());
        char *errMsg = 0;
        if(sqlite3_exec(DB, query.c_str(), NULL, 0, &errMsg) != SQLITE_OK)
        {
            std::cout << "ERROR: while compiling sql: " << errMsg << std::endl;
        }
    }
    pthread_mutex_unlock(&(DBlock));
}

void Database::printDB()
{
    std::cout<<"User ID\t|Balance"<<std::endl;
    std::cout<<"-------\t|-------"<<std::endl;
    sqlite3_stmt* stmt;
    if(sqlite3_prepare_v2(DB, "select * from users", -1, &stmt, nullptr) != SQLITE_OK) {
        std::cout << "ERROR: while compiling sql: " << sqlite3_errmsg(this->DB) << std::endl;
    }
    int ret_code = 0;
    while((ret_code = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        unsigned int id=sqlite3_column_int(stmt, 0);
        unsigned int balance=sqlite3_column_int(stmt, 1);
        std::cout<< sqlite3_column_int(stmt, 0)<<"\t|"<<sqlite3_column_int(stmt, 1)<<std::endl;
    }
    sqlite3_finalize(stmt);
}