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
    T** M_inv;
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
        }
    }
    void set(int row, int col, T val)
    {
        this->M[row][col] = val;
    }
    unsigned int getRows(){return this->row;}
    unsigned int getCols(){return this->col;}
    void calculateInvert()
    {
        //create a new matrix to store inverted B in
        M_inv = new T*[col];
        for (int i=0;i<row;i++)
        {
            M_inv[i] = new T[row];
        }
        //invert values of B
        for (int i=0;i<row;i++)
        {
            for(int j=0;j<col;j++)
            {
                M_inv[j][i] = M[i][j];
            }
        }
    }
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
    T** getInvertedMatrix()
    {
        return this->M_inv;
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
        //load inverted values
        this->calculateInvert(); 
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
        //load inverted values
        this->calculateInvert();
    }
    ~int_matrix(){this->free();}
};

//test function for normal c matrix multiplication
template <typename T>
void matrix_multiplication(T** A, T** B, T** C, int size)
{
    int i,j,k;
    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
        {
            C[i][j] = 0;
            for (k = 0; k < size; k++)
            {
                C[i][j] += A[i][k]*B[k][j];
            }
        }
    }
}


//test function for intrinsics matrix multiplication
void SIMD_matrix_multiplication(float_matrix& A, float_matrix& B, float_matrix& C)
{
    if( (A.getCols() != B.getRows()) || 
        (C.getCols() != B.getCols()) ||
        (C.getRows() != A.getRows()) )
    {
        cerr<<"Error, Matrix dimensions do not match"<<endl;
        return;
    }
    unsigned int i,j,k;
    float ans[8];
    __m256 num1, num2, num3, sum;
    float** matrixA = A.getMatrix();
    float** matrixB = B.getInvertedMatrix();
    float** matrixC = C.getMatrix();
    int size = A.getRows();

    for (i = 0; i < size; i++)
    {
        float* arow = matrixA[i];
        for (j = 0; j < size; j++)
        {
            float* brow = matrixB[j];
            //multiply accumulate flattened column with flattened row
            sum= _mm256_setzero_ps();  //sets sum to zero

            for(k=0; k<size; k+=8)
            {
                // num1 = _mm256_loadu_ps(arow+k);             // load: num1 = [a[7], a[6], a[5], a[4]. a[3], a[2], a[1], a[0]]
                // num2 = _mm256_loadu_ps(brow+k);             // load: num2 = [b[7], b[6], b[5], b[4], b[3], b[2], b[1], b[0]]
                // num3 = _mm256_dp_ps(num1, num2, 0xFF);      // dot prod hi,low: [a[7]b[7] + a[6]b[6] + ,..., a[3]b[3] + a[2]b[2]+ ,...]
                // combining saves time vs storing into num 1,2:
                num3 = _mm256_dp_ps(_mm256_loadu_ps(arow+k), _mm256_loadu_ps(brow+k), 0xFF);      // dot prod hi,low: [a[7]b[7] + a[6]b[6] + ,..., a[3]b[3] + a[2]b[2]+ ,...]
                sum = _mm256_add_ps(sum, num3);             // performs vertical addition with previous values
            }
            _mm256_storeu_ps(ans, sum); //stores sum to local float
            matrixC[i][j] = ans[0] + ans[4]; //set matrix answer
        }
    }
}


void SIMD_matrix_multiplication(int_matrix& A, int_matrix& B, int_matrix& C)
{
    int i,j,k;
    if( (A.getCols() != B.getRows()) || 
        (C.getCols() != B.getCols()) ||
        (C.getRows() != A.getRows()) )
    {
        cerr<<"Error, Matrix dimensions do not match"<<endl;
        return;
    }
    __m128i num1, num2, num3, sum;
    for (i = 0; i < A.getRows(); i++)
    {
        for (j = 0; j < B.getRows(); j++)
        {
            //multiply accumulate flattened column with flattened row
            sum= _mm_setzero_si128();  //sets sum to zero

            for(k=0; k<A.getCols(); k+=4)
            {
                num1 = _mm_load_si128((__m128i*)(A[i]+k));            // load: num1 = [a[3], a[2], a[1], a[0]]
                num2 = _mm_load_si128((__m128i*)(B.getInvertedMatrix()[j]+k));        // load: num2 = [b[3], b[2], b[1], b[0]]
                num3 = _mm_mullo_epi32(num1, num2);                   // multiply, store low bits: num3 = [a[3]*b[3],  a[2]*b[2],  a[1]*b[1],  a[0]*b[0]]
                sum = _mm_add_epi32(sum, num3);                     // performs vertical addition with previous values 
            }
            sum= _mm_hadd_epi32(sum, sum); //horizontally adds sums into sum; sum = [prev[3] + prev[2] , prev[1]+prev[0] , prev[3] + prev[2] , prev[1]+prev[0]]
            sum= _mm_hadd_epi32(sum, sum); //horizontally adds into sum; sum[0] = [prev[3] + prev[2] + prev[1] + prev[0] , ...]
            C[i][j] = _mm_extract_epi32(sum, 0); //sum from above is in sum[0], save to matrix
        }
    }
}

//main function
int main()
{
    srand (time(NULL));

    unsigned int size = 3;

    cout<<"Testing float matrix:"<<endl;
    float_matrix A(size, size);
    A.randomize();
    A.calculateInvert();
    cout<<"Matrix A:"<<endl;
    A.print();

    float_matrix B(size, size);
    B.randomize();
    cout<<"Matrix B:"<<endl;
    B.print();

    cout<<"A x B ="<<endl;
    float_matrix C(size,size);
    auto start1 = high_resolution_clock::now();
    matrix_multiplication(A.getMatrix(), B.getMatrix(), C.getMatrix(), size);
    auto stop1 = high_resolution_clock::now();
    C.print();

    cout<<"A x B ="<<endl;
    float_matrix C2(size,size);
    auto start2 = high_resolution_clock::now();
    SIMD_matrix_multiplication(A, B, C2);
    auto stop2 = high_resolution_clock::now();
    C2.print();
    auto duration1 = duration_cast<microseconds>(stop1 - start1);
    auto duration2 = duration_cast<microseconds>(stop2 - start2);
    cout<<"Regular c++ matrix multiplication: "<<duration1.count()<<"us"<<endl;
    cout<<"Utilizing Intrinsics: "<<duration2.count()<<"us"<<endl;

    return 0;
}
