/**
 * @file main.cpp
 * @author Ethan Stockbridge (ethanstockbridge@gmail.com)
 * @author Devan Kidd (jjkidd1245@gmail.com)
 * @brief Main file
 * @date 2022-03-30
 * 
 */

#include <sqlite3.h>
#include <iostream>
#include <vector>
#include <list>
#include <random>
#include <fstream>
#include <sstream>
#include <map>
#include <set>

// #define MULTITHREAD

/**
 * @brief C++ User class that keeps track of in-memory user ID and balance
 * 
 */
class user
{
private:
    unsigned int ID;
    unsigned int balance;
public:
    user(unsigned int myID, unsigned int myBalance) : ID(myID), balance(myBalance) {}
    void setBalance(unsigned int amount)
    {
        this->balance=amount;
    }
    unsigned int getID()
    {
        return this->ID;
    }
    unsigned int getBalance()
    {
        return this->balance;
    }
};


/**
 * @brief Database class used to interface with SQLite
 * 
 */
class Database
{
private:
    sqlite3* DB;
    std::vector<user*> userList;
    user** hashTable;
    pthread_mutex_t DBlock = PTHREAD_MUTEX_INITIALIZER; 

public:
    Database(std::string fName)
    {
        int retval = sqlite3_open(fName.c_str(), &(this->DB));
        if(retval)
        {
            std::cerr << "Error open DB " << sqlite3_errmsg(DB) << std::endl;
        }
        else
        {
            std::cout << "Loaded database from file: " << fName << std::endl;
        }
        sqlite3_stmt* stmt;
        retval = sqlite3_prepare_v2(this->DB, "select * from users", -1, &stmt, nullptr);
        if(retval != SQLITE_OK) {
            std::cout << "ERROR: while compiling sql: " << sqlite3_errmsg(this->DB) << std::endl;
        }
        int ret_code = 0;
        while((ret_code = sqlite3_step(stmt)) == SQLITE_ROW)
        {
            unsigned int id=sqlite3_column_int(stmt, 0);
            unsigned int balance=sqlite3_column_int(stmt, 1);
            user* newuser = new user(id,balance);
            this->userList.push_back(newuser);
        }
        this->hashTable = new user*[100];
        for(unsigned int i=0; i<this->userList.size(); i++)
        {
            this->hashTable[this->userList[i]->getID()] = this->userList[i];
        }
        sqlite3_finalize(stmt);
    }
    ~Database()
    {
        sqlite3_close(this->DB);
        for(int i=0;i<userList.size();i++)
        {
            delete this->userList[i];
        }
        delete [] this->hashTable;
    }
    user* getUser(unsigned int ID)
    {//trivial implimentation but fast. use unique ID to find location of user in a hashtable
        return this->hashTable[ID];
    }
    void syncDB()
    {
        pthread_mutex_lock(&(DBlock));
        for(std::vector<user*>::iterator itr=this->userList.begin();
            itr!=this->userList.end(); itr++)
        {
            user* user1 = *itr;
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
    void printDB()
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
};


/**
 * @brief Logger class used to write journal of transactions to keep track of any faults
 * 
 */
class Logger
{
private:
    std::string fName;
    pthread_mutex_t fileLock = PTHREAD_MUTEX_INITIALIZER; 
public:
    Logger(){}
    Logger(std::string filename) : fName(filename) {}
    void initialize(std::string filename) {this->fName=filename;}
    void write(unsigned int transaction_ID, unsigned int account_no, unsigned int pre, unsigned int post)
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
    void clear()
    {
        std::ofstream loggerFile;
        loggerFile.open(fName);
        loggerFile.close(); //flush stream to disk
    }
};


class request
{
private:
    unsigned int transaction_ID;
    unsigned int from_account;
    unsigned int to_account;
    unsigned int amount;
    unsigned int status;
    // 0 = not started
    // 1 = successfully took out from user1's account , still needs to add to user 2's account
    // 2 = successfully added to user2's account, COMPLETE
public:
    request(unsigned int ID, unsigned int From, unsigned int To,
        unsigned int Amt) : transaction_ID(ID), from_account(From), to_account(To), 
        amount(Amt), status(0) {}
    void setStatus(unsigned int num){this->status=num;}
    unsigned int getID(){return this->transaction_ID;}
    unsigned int getAmount(){return this->amount;}
    unsigned int getFrom(){return this->from_account;}
    unsigned int getTo(){return this->to_account;}
    unsigned int getStatus(){return this->status;}
};


/**
 * @brief Bank class to manage user's transactions and record to the database, and logger
 * 
 */
class Bank
{
private:
    Database* myDB;
    Logger* myLog;
public:
    Bank(Database* importDB, Logger* importLog)
    {
        this->myDB = importDB;
        this->myLog = importLog;
    }
    bool transfer(request* req)
    {

        user* user1 = myDB->getUser(req->getFrom());
        user* user2 = myDB->getUser(req->getTo());

        // user1->lock();
        // DEADLOCK 
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
};


std::vector<std::vector<int>> readFile(const std::string& fName)
{
    std::vector<std::vector<int>> result;
    std::ifstream file;
    file.open(fName);
    std::string line;
    std::vector<std::string> lines;
    while(getline (file,line))
    {
        int vals[5]={-1,-1,-1,-1,-1};
        int itr=0;
        for(int i=0; i<line.size();i++)
        {
            if(i==0)
                vals[itr]=0;
            if(line[i]=='\n' || line[i]=='\r')
            {
                continue;
            }
            if(line[i]==' ')
            {
                itr++;
                vals[itr]=0;
            }
            else
            {
                vals[itr]*=10;
                vals[itr]+=(int)line[i]-48;
            }
        }
        itr=0;
        std::vector<int> oneRes;
        while(1)
        {
            if(vals[itr]==-1)
                break;
            oneRes.push_back(vals[itr]);
            itr++;
        }
        result.push_back(oneRes);
    }
    std::cout<<"Loaded contents from file: "<<fName<<std::endl;
    return result;
}

std::ostream& operator<<(std::ostream& os, request* req)
{
    os<<"ID: "<<req->getID()<<" Status: "<<req->getStatus()<<std::endl;
    os<<"From: #"<<req->getFrom()<<" To: #"<<req->getTo()<<std::endl;
    os<<"Amount: $"<<req->getAmount()<<std::endl;
    return os;
}

//delete complete and update the ones that are not complete;
std::vector<request*> fixRequests(std::list<request*>& requests, const std::vector<std::vector<int>>& logged, Database* DB)
{

    std::set<unsigned int> user_OK;

    for(int i=logged.size()-1; i>=0; i--)
    {
        //get numbers for all needed info
        unsigned int transactionID= logged[i][0];
        unsigned int specifiedUser = logged[i][1];
        unsigned int pre_price = logged[i][2];
        unsigned int post_price = logged[i][3];

        //find the request corresponding to this transcation in the requests file
        std::list<request*>::iterator itr = requests.begin();
        for(; itr!=requests.end();itr++)
        {
            if((*itr)->getID()==transactionID)
            {
                break;
            }
        }
        if(itr==requests.end())
        {//request was already deleted previously, go to next log
        //(suppose we are looking at the "from" and have already deleted from the "to)
            continue;
        }
        request* thisrequest = *itr; //current requests we're looking at

        //get the user's balance from the database
        unsigned int DB_balance = DB->getUser(specifiedUser)->getBalance();

        //this is for the user receiving money
        bool on_to = (specifiedUser==thisrequest->getTo());

        if(user_OK.find(specifiedUser)!=user_OK.end())
        {//we already confirmed this user is correct up till this point now
            if(on_to)
            {
                //this means that the "to" user got payment already, meaning the "from" user already
                //got deducted, and thus is up to date
                user_OK.insert(thisrequest->getFrom());
                delete thisrequest;
                requests.erase(itr);
            }
            else
            {//this is the "from" user
            //this confirms that the transaction at least has this user taking out money correctly
                thisrequest->setStatus(1);
            }
        }
        else
        {
            if(on_to)
            {//to user
                if(post_price == DB_balance)
                {//definitely ok to delete this since we already did it. also,
                    //all transactions before this done for the "To" and "From" user are also ok
                    //now have imunity for all older transaction records:
                    user_OK.insert(thisrequest->getTo());
                    user_OK.insert(thisrequest->getFrom());
                    //erase this one cuz its already done
                    delete thisrequest;
                    requests.erase(itr);
                }
                if(pre_price == DB_balance)
                {//the transaction record is here for the transfer to the recipient but it didnt go through
                //meaning that the widthdrawl was fine
                    thisrequest->setStatus(1);
                    //now have imunity for all older transaction records
                    user_OK.insert(thisrequest->getFrom());
                }
            }
            else
            {//"from" user
                if(post_price == DB_balance)
                {
                    thisrequest->setStatus(1);
                    //now have imunity for all older transaction records
                    user_OK.insert(thisrequest->getFrom());
                }
                else
                {//"from" user didnt pay yet, this request was not even started yet
                    thisrequest->setStatus(0);
                }
            }
        }
    }

    std::vector<request*> repairList;
    for(std::list<request*>::iterator itr = requests.begin(); itr!=requests.end();)
    {
        request* thisrequest = *itr;
        if(thisrequest->getStatus()>0)
        {
            repairList.push_back(thisrequest);
            itr=requests.erase(itr);
        }
        else
            itr++;
    }

    std::cout<<"The program will repair "<<repairList.size()<<" requests now"<<std::endl;
    return repairList;
}

std::list<request*> requests;
pthread_mutex_t req_lock = PTHREAD_MUTEX_INITIALIZER; 

struct threadArgs
{
    unsigned long id;
    Bank* mybank;
};

void *thread_func(void* args)
{
    threadArgs* myargs= (threadArgs*)args;
    Bank* mybank = myargs->mybank;
    unsigned long myID = myargs->id;
    while(requests.size()>0)
    {
        request* myReq = NULL;
        bool gotOne=false;
        pthread_mutex_lock(&(req_lock));
        if(requests.size()>0)
        {
            request* myReq = requests.front();
            requests.pop_front();
            pthread_mutex_unlock(&(req_lock));
            gotOne=true;
            std::cout<<"Starting transaction #"<<myReq->getID()<<" Thread: #"<<myID<<std::endl;
            mybank->transfer(myReq);
            std::cout<<"Finished transaction #"<<myReq->getID()<<" Thread: #"<<myID<<std::endl;
            delete myReq;
        }
        if(!gotOne)
            pthread_mutex_unlock(&(req_lock));
    }
    return args;
}


/**
 * @brief Main function, performs banking simulation
 * 
 */
int main()
{
    Database* mydatabase = new Database("users.db");
    Logger* myLog = new Logger("transactions.log");
    std::cout<<"Initial user balances:"<<std::endl;
    mydatabase->printDB();
    Bank* mybank = new Bank(mydatabase, myLog);

    unsigned int NUM_WORKERS = 3;

    std::vector<std::vector<int>> result = readFile("requests.txt");

    for(int i=0;i<result.size();i++)
    {
        requests.push_back(new request(result[i][0], result[i][1], result[i][2], result[i][3]));
    }

    std::vector<std::vector<int>> transactions = readFile("transactions.log");

    std::cout<<"Banking simulator starting now"<<std::endl;
    
    //go through requests file and see which one(s) or part(s) we already handled and are on the DB already
    std::vector<request*> repairList = fixRequests(requests, transactions, mydatabase);

    for(unsigned int i=0;i<repairList.size(); i++)
    {   //repair all individially prior to releasing the requests to the threads
        mybank->transfer(repairList[i]);
    }

    #ifdef MULTITHREAD

    //dispatch workers to process remaining requests after stuff is fixed
    pthread_t threads[NUM_WORKERS];
    int err;

    std::cout << "Using " << NUM_WORKERS << " threads for compression" << std::endl;

    std::vector<threadArgs*> threadVec;

    for(unsigned long i = 0; i < NUM_WORKERS; i++) {
        // Create new thread:
        threadArgs* threadargs = new threadArgs;
        threadargs->mybank = mybank;
        threadargs->id = i;
        threadVec.push_back(threadargs);
        err = pthread_create(&threads[i], NULL, &thread_func, (void*)threadargs);
        if (err)
        {
            std::cout << "Error: Unable to create thread," << err << std::endl;
            exit(-1);
        }
    }

    for(unsigned long i = 0; i < NUM_WORKERS; i++) {
        pthread_join(threads[i], NULL);
    }

    for(int i=0;i<threadVec.size();i++)
    {
        delete threadVec[i];
    }

    #else

    //do the rest of the OK transactions:
    for(std::list<request*>::iterator itr = requests.begin(); itr!=requests.end();)
    {
        mybank->transfer(*itr);
        delete *itr;
        itr = requests.erase(itr);
    }

    #endif
    
    delete mydatabase;
    delete myLog;
    delete mybank;
    
    return 0;
}