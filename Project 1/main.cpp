#include <iostream>
#include <time.h>

using namespace std;


//prototype basic matrix class containing matrix data
template <typename T>
class Matrix
{
    T** M; //our matrix
    unsigned int col; 
    unsigned int row; 

public:
    Matrix(unsigned int mrow, unsigned int mcol)
    {
        col = mcol;
        row = mrow;
        //initilize the matrix with specified size
        M = new T*[row];
        for(unsigned int i=0; i<row; i++)
        {
            M[i] = new T[col];
        }
    }
    void set(int row, int col, T val)
    {
        M[row][col] = val;
    }
    unsigned int getRows(){return row;}
    unsigned int getCols(){return col;}

    void print()
    {
        for(unsigned int i=0; i<row; i++)
        {
            for(unsigned int j=0; j<col; j++)
            {
                cout<<M[i][j]<<"\t";
            }
            cout<<endl;
        }
        cout<<endl;
    }
    ~Matrix()
    {
        for(unsigned int i=0;i<row;i++)
        {
            delete M[i];
        }
        delete M;
    }
    T* & operator [](int i)
    {
        return M[i];
    }
};


// extends matrix class using float logic
class float_matrix: public Matrix<float>
{
public:
    //call parent constructor
    float_matrix(unsigned int mrow, unsigned int mcol): Matrix(mrow, mcol){}
    void randomize()
    {//randomize using float randoms
        for(int i=0; i < this->getRows(); i++)
        {
            for(int j=0; j< this->getCols(); j++)
            {
                this->set(i,j,rand()%10+float(rand()%1000)/1000);
            }
        }
    }
};

// extends matrix class using int logic
class int_matrix: public Matrix<int>
{
public:
    //call parent constructor
    int_matrix(unsigned int mrow, unsigned int mcol): Matrix(mrow, mcol){}
    void randomize()
    {//randomize using integer randoms
        for(int i=0; i < this->getRows(); i++)
        {
            for(int j=0; j< this->getCols(); j++)
            {
                this->set(i,j,rand()%10);
            }
        }
    }
};

//test function for normal c matrix multiplication
template <typename T>
void matrix_multiplication(Matrix<T> A, Matrix<T> B, Matrix<T>& C)
{
    int i,j,k;
    if( (A.getCols() != B.getRows()) || 
    	(C.getCols() != B.getCols()) ||
    	(C.getRows() != A.getRows()) )
    {
        cerr<<"Error, Matrix dimensions do not match"<<endl;
    	return;
    }
    for (i = 0; i < A.getRows(); i++)
    {
        for (j = 0; j < B.getCols(); j++)
        {
            C[i][j] = 0;
            for (k = 0; k < A.getCols(); k++)
            {
                C[i][j] += A[i][k]*B[k][j];
            }
        }
    }
}


int main()
{
    srand (time(NULL));

    int_matrix first(2,4);
    first.randomize();
    cout<<"Matrix A:"<<endl;
    first.print();

    int_matrix second(4,3);
    second.randomize();
    cout<<"Matrix B:"<<endl;
    second.print();


    cout<<"A x B ="<<endl;
    int_matrix result(2,3);
    matrix_multiplication(first, second, result);
    result.print();
    return 0;
}
