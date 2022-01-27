/**
 * @file matrix.h
 * @author Ethan Stockbridge (ethanstockbridge@gmail.com)
 * @author Devan Kidd (jjkidd1245@gmail.com)
 * @brief Matrix class and associated helper functions 
 * @date 2022-01-25
 */

#define ROW_WIDTH 8

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
     * @param randomize Randomize the numbers inside of the matrix
     */
    Matrix(unsigned int mrow, unsigned int mcol, bool randomize);
    /**
     * @brief Destroy the Matrix object
     */
    ~Matrix();

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

    T* getCol(int r) const
    {
        T* c[this->realCol];
        for(int i=0;i<this->realCol;i++)
        {
            c[i] = &(this->M[r][i]);
        }
        return *c;
    }

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
    void print() const;
    /**
     * @brief Free the allocated space in the matrix object
     */
    void free();
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
     * @return T*& Row "i" of the matrix
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
    this->row = mrow;
    this->col = mcol;
    // "real" represents the data stored, which is adjusted for intrinsic instructions
    // which require contiguous values of multiples, ROW_WIDTH.
    this->realRow = mrow + ROW_WIDTH - mrow%ROW_WIDTH;
    this->realCol = mcol + ROW_WIDTH - mcol%ROW_WIDTH;
    // Initilize the matrix with specified size
    this->M = new T*[this->realRow];
    for(unsigned int i=0; i<this->realRow; i++)
    {
        this->M[i] = new T[this->realCol];
    }
    this->fill(randomize);
    //Get the inverted matrix for future calculations 
    this->calculateInvert();
}

template <typename T>
Matrix<T>::~Matrix()
{
    this->free();
}

template <typename T>
void Matrix<T>::print() const
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
    for(unsigned int i=0;i<this->realCol;i++)
    {
        delete[] this->M_inv[i];
    }
    delete[] this->M_inv;
}

template <typename T>
void Matrix<T>::calculateInvert()
{
    //create a new matrix to store inverted B in
    M_inv = new T*[this->realCol];
    for (int i=0;i<this->realCol;i++)
    {
        M_inv[i] = new T[realRow];
    }
    //invert values of matrix
    for (int i=0;i<this->realRow;i++)
    {
        for(int j=0;j<this->realCol;j++)
        {
            M_inv[j][i] = M[i][j];
        }
    }
}

template <typename T>
void Matrix<T>::fill(bool randomize)
{
    for(int i=0; i < this->realRow; i++)
    {
        for(int j=0; j< this->realCol; j++)
        {
            if(randomize && (j < this->col && i < this->row))
            { //fill desired space with random numbers
                this->M[i][j] = (T)(i+j)/2;
            }
            else
            { //fill other space with zeros
                this->M[i][j] = 0;
            }
        }
    }
}
