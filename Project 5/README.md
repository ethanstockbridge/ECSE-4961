# Project 5: Multithread money transaction simulator

## Introduction

Through the usage of SQL database, logging and multithreading, a realtime transaction simulator will be created to simulate the many parallel transactions that banks processs.

## Prerequisites

For operation, a linux machine must be used, as the pthread module is utilized for the method of threading for this project. Additionally, be sure to install sqlite3, so c++ can import this module. This can be done by the following command:  
`sudo apt-get install sqlite3 libsqlite3-dev`

## Usage 

To build:
Go to the folder "Project 5" and do one of the following:
- Recommended: use command `make` on terminal to use the makefile build  
 Multithread build: `make MULTITHREAD`

- `g++  -g ./src/*.cpp -lsqlite3 -pthread -o ./a.out`  
    Multithread build:  
    `g++  -g ./src/*.cpp -lsqlite3 -pthread -DMULTITHREAD -o ./a.out`  

To run (in folder "Project 5"):  
If built with `-DMULTITHREAD`:  
`./a.out <NUM of threads>`  
If built without `-DMULTITHREAD`:  
`/a.out`

## Methodology

The process used for this project was to be able to transfer money from users in volatile memory, and then move this data with a database on the disk every transfer using multiple threads, logging the actions taken. Using a database lets the data be saved every transaction. However, if this was never synced with the disk, then after a power outage, or other error, then the volatile memory changes would not be able to be saved before the machine/program powers off, unable to recover when booted up.  
So to prevent any database inconsistencies, each transfer has 6 basic steps:

```
log write (request id, user1 id, user1 pre balance, user1 post balance)
user1 balance -= amount
database sync user1

log write (request id, user1 id, user2 pre balance, user2 post balance)
user2 balance += amount
database sync user2
```

These steps allow the program to keep track of any changes that were/weren't done to the database in a log file, and also be able to cross reference this log file to the database file if some sort of power outage or system malfunction occurs, so the data is consistent. 

Upon starting the program, it initally checks this log file `transactions.log`. From there, it then loads the database `users.db` and compares which transactions were completed by checking the pre and post balance of each user. Once a correct balance is identified, then we know where the program crashed, so we resume the program at that step. The algorithm impemented for deciding which requests must be re-done scans through the two resources stated above as well as `resources.txt` to split them into 2 new requests lists: requests that must be fixed, and requests that must be completed in full (that have not been done yet). Along the way, the algorithm disards any requests that have already been fulfilled, since we do not want to complete them again.
From there, the program then does all requests that must be fixed using 1 thread, regardless of the amount specified to prevent further possible error. Then after these are completed, the program uses the full amount of threads specfied to complete the remaining requests in `requests.txt` 

## Files

There are 3 main files that are used for in program besides the c++ files:

- `users.db` - This SQL table file specifies the unique user IDs and current balance of all users.
- `requests.txt` - This file specifies which requests must be completed. Each line consists of 4 values: unique request id, user 1 id, user 2 id, and amont. At the end of each request, user 1 will transfer user 2 x amount of money.
- `transactions.log` - This log file specifies which transactions have (supposedly) been processed and pushed to the database, but we will discuss later examples of inconsistencies. Each line consists of 4 items: request id, user id, pre-balance, post-balance

## Testing

To run the pre-configured files, you can go to ./tests/ex# and copy the files to the root folder, "Project 5".  
Ex: If you want to test the **Example 1** below, go to ./tests/ex1 and copy the files to root folder, "Project 5", then run the program using `./a.out` after building (with optional threads)

## Example 1: Resuming banking program after server crash (with previous successful database update)

### Example Situation
You're running the banking program, connected to a SQL database server, and suddenly, the server goes down. Upon resuming the program, you want to be sure that the program does not commit transactions that have already been (fully or half) committed.

### Pre-run: System setup

requests.txt:

```
100 1 2 3
101 4 5 6
```

Users.db:
|Client ID | balance|
|---------|-------|
|001 | 10000|
|002 | 10000|
|004 | 10000|
|005 | 10000|

### Running the program:

Upon running the program, suppose that there are 2 threads:

- Thread 1 takes on the task of `100 1 2 3`, or: "transaction #100: client #001 transfers $3 to client #002".
This thread starts it's job, and adds to the transaction.log:  
`100 1 10000 9997`

- Thread 2 takes on the task of `101 4 5 6`, or: "Transaction #101: client #004 transfers $6 to client #005". This thread adds to the transaction log:
`101 3 10000 9994`

Since transaction logs are written right before each account balance change in the database, the file transactions.log now looks like this:
```
100 1 10000 9997
101 4 10000 9994
```

Suppose that these two half-transactions have **successfully** updated the database, so that the stored database, `users.db`, is shown below:

|Client ID | balance|
|---------|-------|
|001 | 9997 |
|002 | 10000|
|004 | 9994 |
|005 | 10000|

Now, suppose that the program stopped working at this moment, or the database connection was lost, erroring this running program.  
If we start the program now, without further consideration to order, assumably this program would start back up at performing each item in `requests.txt` again from the top, but this results in changes on the database that have aleady been done (ie: `100 1 10000 9997` AND `101 3 10000 9994`), which is very bad! Ultimately, this would result in user 1 losing an extra $3, and user 4 losing an extra $6. 

To solve this, upon starting, the program parses through `transactions.log`, and compares these values to the (now connected) database `users.db`, and sees that we have already actually completed the first half of transactions #100 and #101, because the accounts do have amount of 9997 and 9994 respectively in the database.  

So, instead of repeating the already half-done transactions, this program will *repair* these uncompleted logged requests and fix them on the fly.   
The resulting transactions.log after running again will be shown as below:

```
100 1 10000 9997
100 2 10000 10003
101 4 10000 9994
101 5 10000 10006
```

Note that the first 2 transactions (since they were already complete and committed), stay in the transactions, and then the other half are completed. And now, after syncing the database, we can see that the database now reads:

|Client ID | balance|
|---------|-------|
|001 | 9997 |
|002 | 10003|
|004 | 9994 |
|005 | 10006|


We can now see that the output database contents show exactly what is desired. This is able to be done through the usage of log parsing and carefully determining which pieces of data have been updated to the database or not by cross-referencing the existing database. 


### Testing example 1

Testing this is a bit hard, since we cannot guarantee that given threads will choose the transactions as shown in the example. However, we can assume that there is a server failure and start from there, and observe the corrections.  
By filling `requests.txt` with:
```
100 1 2 3
101 4 5 6
```
and transactions.log with:
```
100 1 10000 9997
101 4 10000 9994
```
and setting up users.db as shown below:
```
1|9997
2|10000
4|9994
5|10000
```

We can then run the program and observe that the output is as shown and explained above:

As noted by program output:

```The program will repair 2 requests now```

The resulting users.db database is now filled correctly after running:

```
1|9997
2|10003
4|9994
5|10006
```

## Example 2: Resuming banking program after server crash (with previous unsuccessful database update)

Assume the same situation as the previous example, but now, the first half of the transaction did not go through (this could be due to a database connection error, or similar). This means that thread 1 and thread 2 were unsuccessful at carrying out the first subtraction of money from user 1 and 4 respectively. After the program faults, the database is as shown:

Users.db:
```
1|10000
2|10000
4|10000
5|10000
```

However, conflictingly, the transactions file shows these balance changes, since logging is done **before** database changes:

transactions.log:
```
100 1 10000 9997
101 4 10000 9994
```

Unlike last time, the real value in the database is **not** 9997 and 9994 respectively as shown above. Upon the program resuming, we must be able to check this to determine the situation. Using the ability to parse the log file, the program can figure out that there was a fault between when the log file was written to, and the database trying to update these values. It then resumes from the memory point when user 1 had $10000 and when user 4 had $10000, as specified in the SQL database. 

So, after running the program again, the SQL database will be as expected, with all users having correct balances. The transactions.log will be as shown below:

```
100 1 10000 9997
101 4 10000 9994
100 1 10000 9997
101 4 10000 9994
100 2 10000 10003
101 5 10000 10006
```

Additionally, there are 6 lines. Note that the first 2 were never actually committed into the database, so they must be done over again, totalling in an additional 4 lines to complete the operation of 2 requests. 

### Testing example 2

This is similar to the previous example, except now the database must not match the log file.
Use the same inputs from example 1, except set up the users.db as shown below:

```
1|10000
2|10000
4|10000
5|10000
```

Now, running the program, you can see that the users.db has been updated correctly, and the transactions.log now has 6 lines displaying all of the logged transactions (committed correctly or not)

## Concusion

Through using the methods of logging, database usage, and multithreading, it has become quite clear to see that this process of keeping a database consistent across systems is not an easy task. However, with logging, it is a lot easier to keep track of which requests have been (partially, or fully) completed. Using the log file to cross reference the database is a good way to ensure that changes have been correctly saved to disk, so that this data is consistent across systems. Additionally, implimenting multithreading showed that we must use thread locks across many items, such as file writing, and user data.  
