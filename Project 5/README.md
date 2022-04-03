# Project 5: Banking simulator

## Introduction

Through the usage of Sqlite, logging and multithreading, a banking simulator will be created to simulate the many parallel transactions that banks process.

## Prerequisites

Be sure to install sqlite3, so c++ can import this module. The easiest of installations is shown below:   
`sudo apt-get install sqlite3 libsqlite3-dev`

## Usage 

To build:

`g++  -g *.cpp -lsqlite3 -o ./a.out`

To run:

`./a.out`

## Example 1: Resuming banking program after server crash.

### Example Situation
You're running the banking program, connected to a SQL database server, and suddenly, the server goes down. Upon resuming the program, you want to be sure that the program does not commit transactions that have already been committed.

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

Upon running the program, suppose that there are 2 threads

- Thread 1 takes on the task of `100 1 2 3`, or: "transaction #100: client #001 transfers $3 to client #002".
This thread starts it's job, and adds to the transaction.log:  
`100 1 10000 9997`

- Thread 2 takes on the task of `101 4 5 6`, or: "Transaction #101: client #004 transfers $6 to client #005". This thread adds to the transaction log:
`101 3 10000 9994`

Now, the file transactions.log looks like this:

```
100 1 10000 9997
101 4 10000 9994
```

Suppose that these two half-transactions have successfully made it to the database, so that the stored database, `users.db`, is depicted below:

|Client ID | balance|
|---------|-------|
|001 | 9997 |
|002 | 10000|
|004 | 9994 |
|005 | 10000|

Now, suppose that the program stopped working, or the database connection was lost, erroring the running program.  
If we start back up, assumably this program would start back up at performing each item in `requests.txt` again, but this results in changes on the database that have aleady been done (ie: `100 1 10000 9997` AND `101 3 10000 9994`), which is very bad!

To solve this, the program parses through `transactions.log`, and compares these values to the (now connected) database, and sees that we have already actually completed the first half of transactions #100 and #101, because the accounts have amount of 9997 and 9994 respectively.  

So, instead of repeating the already half-done transactions, this program will *repair* logged transactions and fix them on the fly.   
The resulting transactions.log after running again will be shown as below:

```
100 1 10000 9997
100 2 10000 10003
101 4 10000 9994
101 5 10000 10006
```
Note that the first 2 transactions (since they were already complete and committed), stay in the transactions, and then the other half are completed.  And now, after syncing the database, we can see that the database now reads:

|Client ID | balance|
|---------|-------|
|001 | 9997 |
|002 | 10003|
|004 | 9994 |
|005 | 10006|


Now, after the program's repair process, we can see that the output database contents show exactly what is desired. This is able to be done through the usage of log parsing and carefully determining which pieces of data have been updated to the database or not by cross-referencing the existing database. 


### Testing example 1

Testing this is a bit hard, since we cannot guarantee that the threads will choose the transactions as shown in the example. However, we can assume that there is a server failure and start from there, and observe the corrections.  
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

The resulting users.db database is now filled correctly:

```
1|9997
2|10003
4|9994
5|10006
```