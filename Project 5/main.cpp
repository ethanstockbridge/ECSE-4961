/**
 * @file main.cpp
 * @author Ethan Stockbridge (ethanstockbridge@gmail.com)
 * @author Devan Kidd (jjkidd1245@gmail.com)
 * @brief Main file
 * @date 2022-04-05
 * 
 */

#include <iostream>
#include <vector>
#include <list>
#include <random>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include "User.h"
#include "Database.h"
#include "Logger.h"
#include "Request.h"
#include "Bank.h"

#define MULTITHREAD

std::list<Request*> requests;
pthread_mutex_t req_lock = PTHREAD_MUTEX_INITIALIZER; 

/**
 * @brief Structure to send the information of the bank object and ID to the thread
 * 
 */
struct threadArgs
{
    unsigned long id;
    Bank* mybank;
};

/**
 * @brief Thread task. Every thread will perform this task.
 * 
 * @param args threadArgs struct
 * @return void* param object
 */
void *thread_func(void* args)
{
    threadArgs* myargs= (threadArgs*)args;
    Bank* mybank = myargs->mybank;
    unsigned long myID = myargs->id;
    while(requests.size()>0)
    {
        Request* myReq = NULL;
        bool gotOne=false;
        pthread_mutex_lock(&(req_lock));
        if(requests.size()>0)
        {
            Request* myReq = requests.front();
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

//forward delcarations
std::vector<Request*> fixRequests(std::list<Request*>& requests, const std::vector<std::vector<int>>& logged, Database* DB);
std::vector<std::vector<int>> readFile(const std::string& fName);


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
    std::cout<<"Loaded contents from file: requests.txt"<<std::endl;

    for(int i=0;i<result.size();i++)
    {
        requests.push_back(new Request(result[i][0], result[i][1], result[i][2], result[i][3]));
    }

    std::vector<std::vector<int>> transactions = readFile("transactions.log");
    std::cout<<"Loaded contents from file: transactions.log"<<std::endl;

    std::cout<<"Banking simulator starting now"<<std::endl;
    
    //go through requests file and see which one(s) or part(s) we already handled and are on the DB already
    std::vector<Request*> repairList = fixRequests(requests, transactions, mydatabase);
    std::cout<<"The program will repair "<<repairList.size()<<" requests now"<<std::endl;


    for(unsigned int i=0;i<repairList.size(); i++)
    {   //repair all individially prior to releasing the requests to the threads
        mybank->transfer(repairList[i]);
    }

    #ifdef MULTITHREAD

    //dispatch workers to process remaining requests after stuff is fixed
    pthread_t threads[NUM_WORKERS];
    int err;

    std::cout << "Using " << NUM_WORKERS << " threads to perform "<<requests.size()<<" transactions" << std::endl;

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

    //delete the stack after threads are done 
    for(int i=0;i<threadVec.size();i++)
    {
        delete threadVec[i];
    }

    #else

    //do the rest of the untouched transactions:
    for(std::list<Request*>::iterator itr = requests.begin(); itr!=requests.end();)
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

/**
 * @brief Read in the transaction or request file. Turns a space-separated file
 * into a vector of vector of ints
 * 
 * @param fName File name
 * @return std::vector<std::vector<int>> Data from the file row<column<int>>
 */
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
    return result;
}

/**
 * @brief Main algorithm to locate and fix issues between the comparison of the transaction
 * log and the database.
 * 
 * @param requests requests as specified in the requests.txt file read in
 * @param logged already logged transactions, as specified in the transactions.log
 * @param DB SQL database of the currently saved database data
 * @return std::vector<Request*> Requests that must be corrected
 */
std::vector<Request*> fixRequests(std::list<Request*>& requests, const std::vector<std::vector<int>>& logged, Database* DB)
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
        std::list<Request*>::iterator itr = requests.begin();
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
        Request* thisrequest = *itr; //current requests we're looking at

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

    //make a vector of just the ones that have to be fixed
    std::vector<Request*> repairList;
    for(std::list<Request*>::iterator itr = requests.begin(); itr!=requests.end();)
    {
        Request* thisrequest = *itr;
        if(thisrequest->getStatus()>0) //requests that were started but not completed
        {
            repairList.push_back(thisrequest);
            itr=requests.erase(itr);
        }
        else
            itr++;
    }

    //return the ones that need to be fixed. the others have been kept/deleted from the 
    //source, passed by ref
    return repairList;
}

