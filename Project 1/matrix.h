/**
 * @file matrix.h
 * @author Ethan Stockbridge (ethanstockbridge@gmail.com)
 * @author Devan Kidd (jjkidd1245@gmail.com)
 * @brief Matrix class definitions 
 * @date 2022-01-25
 */
#ifndef MATRIX_H
#define MATRIX_H

/**
 * @brief set row width to be a multiple of 16 since we are using AVX instructions,
 * these load data in chunks of 8/16
 */
#define ROW_WIDTH 16

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
     * @brief The curent matrix
     */
    T** M;
    /**
     * @brief The row and column values of the stored matrix
     */
    unsigned int row, col; 
    /**
     * @brief The real size of the col (increase width to a multiple of ROW_WIDTH
     * to allow for intrinsics to not access uninitialized memory)
     */
    unsigned int realRow, realCol;

public:
    /**
     * @brief Constructor of the matrix class
     * @param mrow The number of rows to make this matrix
     * @param mcol The number of columns to make this matrix
     * @param randomize If true, randomize the numbers inside of the matrix.
     * Otherwise, initialize them all to 0.
     */
    Matrix(unsigned int mrow, unsigned int mcol, bool randomize);
    /**
     * @brief Destroy the Matrix object
     */
    ~Matrix();

    // Accessors
    /**
     * @brief Get the matrix's number of rows
     * 
     * @return unsigned int 
     */
    unsigned int getRows() const { return this->row; }
    /**
     * @brief Get the matrix's number of columns
     * 
     * @return unsigned int 
     */
    unsigned int getCols() const { return this->col; }


    // Modifiers
    /**
     * @brief Fill the values inside of the matrix
     * 
     * @param randomize If true, randomize the contents. If false, set to 0.
     */
    void fill(bool randomize);
    /**
     * @brief Set a value of the matrix
     * 
     * @param row The row desired to be set
     * @param col The column desired to be set
     * @param val The value to set this cell to
     */
    void set(const int row, const int col, const T val) const
    {
        this->M[row][col] = val;
    }
    /**
     * @brief Calculate the inverted matrix and set it to our matrix
     */
    void invert();
    /**
     * @brief Print the matrix 
     */
    void print();
    /**
     * @brief Free the allocated space in the matrix object
     */
    void free();
    /**
     * @brief Get the matrix
     * 
     * @return T** This object's matrix
     */
    T** getMatrix() const { return this->M; }

    // Opertors
    /**
     * @brief Overloading subscript operator
     * 
     * @param i Number indicating row desired 
     * @return T*& Row "i" of the matrix
     */
    T* & operator [](int i) const { return this->M[i]; }
};
 
// Matrix function implimentation 

template <typename T>
Matrix<T>::Matrix(unsigned int mrow, unsigned int mcol, bool randomize)
{
    this->row = mrow;
    this->col = mcol;
    // "real" represents the data stored, which is adjusted for intrinsic instructions
    // which require contiguous values of multiples, ROW_WIDTH.
    this->realRow = this->row + ROW_WIDTH - this->row%ROW_WIDTH;
    this->realCol = this->col + ROW_WIDTH - this->col%ROW_WIDTH;
    // Initilize the matrix with specified size
    this->M = new T*[this->realRow];
    for(unsigned int i=0; i<this->realRow; i++)
    {
        this->M[i] = new T[this->realCol];
    }
    this->fill(randomize);
}

template <typename T>
Matrix<T>::~Matrix()
{
    this->free();
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
void Matrix<T>::free()
{
    for(unsigned int i=0;i<this->realRow;i++)
    {
        delete[] this->M[i];
    }
    delete[] this->M;
}

template <typename T>
void Matrix<T>::invert()
{
    //create a new matrix to store inverted B in
    T** tmp = new T*[this->realCol];
    for (unsigned int i=0;i<this->realCol;i++)
    {
        tmp[i] = new T[realRow];
    }
    //invert values of matrix
    for (unsigned int i=0;i<this->realRow;i++)
    {
        for(unsigned int j=0;j<this->realCol;j++)
        {
            tmp[j][i] = M[i][j];
        }
    }
    //delete old matrix
    this->free();
    //set new to inverted
    M = tmp;
}

template <typename T>
void Matrix<T>::fill(bool randomize)
{
    for(unsigned int i=0; i < this->realRow; i++)
    {
        for(unsigned int j=0; j< this->realCol; j++)
        {
            if(randomize && (j < this->col && i < this->row))
            { //fill desired space with random numbers
                this->M[i][j] = (T)(rand());
            }
            else
            { //fill other space with zeros
                this->M[i][j] = 0;
            }
        }
    }
}

#endif