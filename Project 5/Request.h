/**
 * @file Request.h
 * @author Ethan Stockbridge (ethanstockbridge@gmail.com)
 * @author Devan Kidd (jjkidd1245@gmail.com)
 * @brief Requests header file
 * @date 2022-04-05
 * 
 */


#ifndef REQUEST_H

#define REQUEST_H


/**
 * @brief Request class. Keeps track of the transaction details requested
 * from an input text file of ints, converted into requests
 * 
 */
class Request
{
private:
    /**
     * @brief Transaction ID, unique
     * 
     */
    unsigned int transaction_ID;
    /**
     * @brief The account which this transaction will take out of
     * 
     */
    unsigned int from_account;
    /**
     * @brief The account which this transaction will put into
     * 
     */
    unsigned int to_account;
    /**
     * @brief The amount of money that will be sent from the "from" to "to"
     * 
     */
    unsigned int amount;
    /**
     * @brief Status of this transaction, as follows:
     * 0 = not started
     * 1 = successfully took out from user1's account , still needs to add to user 2's account
     * 2 = successfully added to user2's account, COMPLETE
     */
    unsigned int status;

public:
    /**
     * @brief Construct a new Request object from the given data
     * 
     * @param ID Transaction ID
     * @param From Account number that will be giving
     * @param To Account number that will be receiving 
     * @param Amt Amount to transfer
     */
    Request(unsigned int ID, unsigned int From, unsigned int To,
        unsigned int Amt) : transaction_ID(ID), from_account(From), to_account(To), 
        amount(Amt), status(0) {}
    /**
     * @brief Set the Status of this object
     * 
     * @param num Status indicator
     */
    void setStatus(unsigned int num);
    /**
     * @brief Get the transaction ID
     * 
     * @return unsigned int Transaction ID
     */
    unsigned int getID();
    /**
     * @brief Get the Amount of this transaction
     * 
     * @return unsigned int Amount
     */
    unsigned int getAmount();
    /**
     * @brief Get the account number that will be sending the money
     * 
     * @return unsigned int "From" ID
     */
    unsigned int getFrom();
    /**
     * @brief Get the account number that will be receiving the money
     * 
     * @return unsigned int "To" ID
     */
    unsigned int getTo();
    /**
     * @brief Get the status of this request
     * 
     * @return unsigned int Status indicator
     */
    unsigned int getStatus();
};

#endif