/**
 * @file matrix.h
 * @author Ethan Stockbridge (ethanstockbridge@gmail.com)
 * @author Devan Kidd (jjkidd1245@gmail.com)
 * @brief Matrix class and associated helper functions 
 * @date 2022-01-25
 */


/**
 * @brief Matrix class 
 * 
 * @tparam T Type of data stored in the matrix (ex: float, int)
 */
template <typename T>
class Matrix
{
private:

    /**
     * @brief The curent matrix and inverted matrix
     */
    T** M;
    T** M_inv;
    /**
     * @brief The row and column values of the stored matrix
     */
    unsigned int row; 
    unsigned int col; 

public:
    /**
     * @brief Constructor of the matrix class
     * @param mrow The number of rows to make this matrix
     * @param mcol The number of columns to make this matrix
     * @param randomize Randomize the numbers inside of the matrix
     */
    Matrix(unsigned int mrow, unsigned int mcol, bool randomize);
    /**
     * @brief Destroy the Matrix object
     */
    // ~Matrix();

    // Accessors
    /**
     * @brief Get the matrix's amount of rows
     * 
     * @return unsigned int 
     */
    unsigned int getRows(){return this->row;}
    /**
     * @brief Get the matrix's amount of columns
     * 
     * @return unsigned int 
     */
    unsigned int getCols(){return this->col;}

    // Modifiers
    /**
     * @brief Randomize the values inside of the matrix
     */
    void randomize();
   
    /**
     * @brief Set a value of the matrix
     * 
     * @param row The row desired to be set
     * @param col The column desired to be set
     * @param val The value to set this cell to
     */
    void set(const int row, const int col, const T val)
    {
        this->M[row][col] = val;
    }

    /**
     * @brief Calculate the inverted matrix 
     */
    void calculateInvert();
    /**
     * @brief Print the matrix 
     */
    void print();
    /**
     * @brief Free the allocated space in the matrix object
     */
    void free()
    {
        for(unsigned int i=0;i<row;i++)
        {
            delete[] this->M[i];
        }
        delete[] this->M;
    }
    /**
     * @brief Get the inverted matrix
     * 
     * @return T** This object's inverted matrix
     */
    T** getInvertedMatrix()
    {
        return this->M_inv;
    }
    /**
     * @brief Get the matrix
     * 
     * @return T** This object's matrix
     */
    T** getMatrix()
    {
        return this->M;
    }

    // Opertors
    /**
     * @brief Overloading subscript operator
     * 
     * @param i Number indicating row desired 
     * @return T*& Row of the matrix: M[i]
     */
    T* & operator [](int i)
    {
        return this->M[i];
    }
};
 
// Matrix function implimentation 

template <typename T>
Matrix<T>::Matrix(unsigned int mrow, unsigned int mcol, bool randomize)
{
    this->col = mcol;
    this->row = mrow;
    //initilize the matrix with specified size
    this->M = new T*[row];
    for(unsigned int i=0; i<row; i++)
    {
        this->M[i] = new T[col];
    }
    if(randomize)
    {
        this->randomize();
    }
}

template <typename T>
void Matrix<T>::print()
{
    for(unsigned int i=0; i<this->row; i++)
    {
        for(unsigned int j=0; j<this->col; j++)
        {
            std::cout<<this->M[i][j]<<"\t";
        }
        std::cout<<std::endl;
    }
    std::cout<<std::endl;
}

template <typename T>
void Matrix<T>::calculateInvert()
{
    //create a new matrix to store inverted B in
    M_inv = new T*[this->col];
    for (int i=0;i<this->col;i++)
    {
        M_inv[i] = new T[row];
    }
    //invert values of B
    for (int i=0;i<this->row;i++)
    {
        for(int j=0;j<this->col;j++)
        {
            M_inv[j][i] = M[i][j];
        }
    }
}

template <typename T>
void Matrix<T>::randomize()
{
    for(int i=0; i < this->getRows(); i++)
    {
        for(int j=0; j< this->getCols(); j++)
        {
            this->set(i,j,(T)(rand()));
        }
    }
}

