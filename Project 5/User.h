/**
 * @file User.h
 * @author Ethan Stockbridge (ethanstockbridge@gmail.com)
 * @author Devan Kidd (jjkidd1245@gmail.com)
 * @brief User class
 * @date 2022-04-05
 * 
 */


#ifndef USER_H

#define USER_H


/**
 * @brief C++ User class that keeps track of in-memory User ID and balance
 * 
 */
class User
{

private:

    /**
     * @brief Private variables: User ID and 
     * 
     */
    unsigned int ID;
    unsigned int balance;

public:

    /**
     * @brief Construct a new User object
     * 
     * @param myID User's ID
     * @param myBalance User's current balance
     */
    User(unsigned int myID, unsigned int myBalance) : ID(myID), balance(myBalance) {}

    /**
     * @brief Set the balance of this user
     * 
     * @param amount Balance
     */
    void setBalance(unsigned int amount);

    /**
     * @brief Get the ID of this user
     * 
     * @return unsigned int User ID
     */
    unsigned int getID();

    /**
     * @brief Get the balance of this user
     * 
     * @return unsigned int User Balance
     */
    unsigned int getBalance();
};

#endif