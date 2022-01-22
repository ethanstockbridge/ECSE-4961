//Cpp imports
#include <iostream>
#include <time.h>
#include <chrono>
//intrinsic includes
#include <immintrin.h>

using namespace std::chrono;
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
        this->col = mcol;
        this->row = mrow;
        //initilize the matrix with specified size
        this->M = new T*[row];
        for(unsigned int i=0; i<row; i++)
        {
            this->M[i] = new T[col];
            for(unsigned int j=0; j<this->col; j++)
            {
            	this->M[i][j] = 0;
            }
        }
    }
    void set(int row, int col, T val)
    {
        this->M[row][col] = val;
    }
    unsigned int getRows(){return this->row;}
    unsigned int getCols(){return this->col;}

    void print()
    {
        for(unsigned int i=0; i<row; i++)
        {
            for(unsigned int j=0; j<col; j++)
            {
                cout<<this->M[i][j]<<"\t";
            }
            cout<<endl;
        }
        cout<<endl;
    }
    void free()
    {
        for(unsigned int i=0;i<row;i++)
        {
            delete[] this->M[i];
        }
        delete[] this->M;
    }
    T** getMatrix()
    {
        return this->M;
    }
    T* & operator [](int i)
    {
        return this->M[i];
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
    ~float_matrix(){this->free();}
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
    ~int_matrix(){this->free();}
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

//test function for intrinsics matrix multiplication
template <typename T>
void SIMD_matrix_multiplication(Matrix<T> A, Matrix<T> B, Matrix<T>& C)
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
            //flatten each column of B
            T b_col[B.getCols()];
            for (k = 0; k < B.getCols(); k++)
            {
                b_col[k] = B[k][j];
            }
            //multiply accumulate flattened column with flattened row

            __m128 num1, num2, num3, num4;

            num4= _mm_setzero_ps();  //sets sum to zero

            for(k=0; k<A.getCols(); k+=4)
            {
                num1 = _mm_loadu_ps(A[i]+k);   //loads unaligned array a into num1  num1= a[3]  a[2]  a[1]  a[0]
                num2 = _mm_loadu_ps(b_col+k);   //loads unaligned array b into num2  num2= b[3]   b[2]   b[1]  b[0]
                num3 = _mm_mul_ps(num1, num2);  //performs multiplication   num3 = a[3]*b[3]  a[2]*b[2]  a[1]*b[1]  a[0]*b[0]
                num3 = _mm_hadd_ps(num3, num3); //performs horizontal addition
                                                //num3=  a[3]*b[3]+ a[2]*b[2]  a[1]*b[1]+a[0]*b[0]  a[3]*b[3]+ a[2]*b[2]  a[1]*b[1]+a[0]*b[0]
                num4 = _mm_add_ps(num4, num3);  //performs vertical addition
            }
            num4= _mm_hadd_ps(num4, num4);
            _mm_store_ss(C[i]+j,num4);
        }
    }
}



int main()
{
    srand (time(NULL));

    unsigned int size = 3;

    float_matrix first(size,size);
    first.randomize();
    cout<<"Matrix A:"<<endl;
    first.print();

    float_matrix second(size,size);
    second.randomize();
    cout<<"Matrix B:"<<endl;
    second.print();

    cout<<"A x B ="<<endl;
    float_matrix result(size,size);
    auto start1 = high_resolution_clock::now();
    matrix_multiplication(first, second, result);
    auto stop1 = high_resolution_clock::now();
    result.print();

    cout<<"A x B ="<<endl;
    float_matrix result2(size,size);
    auto start2 = high_resolution_clock::now();
    SIMD_matrix_multiplication(first, second, result2);
    auto stop2 = high_resolution_clock::now();
    result2.print();

    auto duration1 = duration_cast<microseconds>(stop1 - start1);
    auto duration2 = duration_cast<microseconds>(stop2 - start2);
    cout<<"Time 1: "<< duration1.count()<< "ns Time 2: "<<duration2.count()<<"ns"<<endl;
    return 0;
}
