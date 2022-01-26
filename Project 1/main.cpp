/**
 * @file main.cpp
 * @author Ethan Stockbridge (ethanstockbridge@gmail.com)
 * @author Devan Kidd (jjkidd1245@gmail.com)
 * @brief Main file for performing matrix multiplication performance beteen 
 * regular c++ matrix multiplication and utilizing intel intrinsics
 * @date 2022-01-25
 */

#include <iostream>
#include <time.h>
#include <chrono>
// For intrinsics functions:
#include <immintrin.h>
#include "matrix.h"

using namespace std;


/**
 * @brief Matrix multiplication using regular C++ basic matrix multiplication
 * 
 * @param A First input matrix
 * @param B Second input matrix
 * @param C Output matrix (A x B = C)
 */
template <typename T>
void matrix_multiplication(Matrix<T> A, Matrix<T> B, Matrix<T> C)
{
    if( (A.getCols() != B.getRows()) || 
        (C.getCols() != B.getCols()) ||
        (C.getRows() != A.getRows()) )
    {
        cerr<<"Error: Cannot perform matrix multiplication because matrix dimensions do not match"<<endl;
        return;
    }
    int i,j,k;
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


/**
 * @brief Matrix multiplication using intel intrinsics instructions for floats
 * 
 * @param A First input matrix
 * @param B Second input matrix
 * @param C Output matrix (A x B = C)
 */
void SIMD_float_matrix_multiplication(Matrix<float>& A, Matrix<float>& B, Matrix<float>& C)
{
    if( (A.getCols() != B.getRows()) || 
        (C.getCols() != B.getCols()) ||
        (C.getRows() != A.getRows()) )
    {
        cerr<<"Error: Cannot perform matrix multiplication because matrix dimensions do not match"<<endl;
        return;
    }
    
    unsigned int i,j,k;
    float ans[8];
    __m256 num1, num2, num3, sum;

    float** matrixA = A.getMatrix();
    float** matrixB = B.getInvertedMatrix();
    float** matrixC = C.getMatrix();

    unsigned int size1 = A.getRows(); //= C.getRows()
    unsigned int size2 = B.getCols(); //= C.getCols()
    unsigned int size3 = A.getCols(); //= B.getRows()

    for (i = 0; i < size1; i++)
    {
        float* arow = matrixA[i];
        for (j = 0; j < size2; j++)
        {
            float* brow = matrixB[j];
            //multiply accumulate flattened column with flattened row
            sum= _mm256_setzero_ps();  //sets sum to zero

            for(k=0; k<size3; k+=8)
            {
                // num1 = _mm256_loadu_ps(arow+k);             // load: num1 = [a[7], a[6], a[5], a[4]. a[3], a[2], a[1], a[0]]
                // num2 = _mm256_loadu_ps(brow+k);             // load: num2 = [b[7], b[6], b[5], b[4], b[3], b[2], b[1], b[0]]
                // num3 = _mm256_dp_ps(num1, num2, 0xFF);      // dot prod hi,low: [a[7]b[7] + a[6]b[6] + ,..., a[3]b[3] + a[2]b[2]+ ,...]
                // Combining the above saves time vs storing into num 1,2:
                num3 = _mm256_dp_ps(_mm256_loadu_ps(arow+k), _mm256_loadu_ps(brow+k), 0xFF);
                sum = _mm256_add_ps(sum, num3);             // performs vertical addition with previous values
            }
            _mm256_storeu_ps(ans, sum); //stores sum to local float
            matrixC[i][j] = ans[0] + ans[4]; //set matrix answer
        }
        
    }
}

/**
 * @brief Matrix multiplication using intel intrinsics instructions for integers
 * 
 * @param A First input matrix
 * @param B Second input matrix
 * @param C Output matrix (A x B = C)
 */
void SIMD_matrix_multiplication(Matrix<unsigned int>& A, Matrix<unsigned int>& B, Matrix<unsigned int>& C)
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
        for (j = 0; j < B.getCols(); j++)
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


/**
 * @brief Main function for executing matrix multiplication and testing the performance
 */
int main()
{
    // Set the seed time for random
    srand (time(NULL));

    // Size for default N x N matrix multiplication:
    unsigned int size = 3;

    cout<<"Testing float matrix:"<<endl;
    Matrix<float> A(size, size, true);
    A.calculateInvert();
    cout<<"Matrix A:"<<endl;
    A.print();

    Matrix<float> B(size, size, true);
    B.calculateInvert();
    cout<<"Matrix B:"<<endl;
    B.print();

    cout<<"A x B ="<<endl;
    Matrix<float>  C(size, size, false);
    auto start1 = std::chrono::high_resolution_clock::now();
    matrix_multiplication(A, B, C);
    auto stop1 = std::chrono::high_resolution_clock::now();
    C.print();

    cout<<"A x B ="<<endl;
    Matrix<float>  C2(size, size, false);
    auto start2 = std::chrono::high_resolution_clock::now();
    SIMD_float_matrix_multiplication(A, B, C2);
    auto stop2 = std::chrono::high_resolution_clock::now();
    C2.print();
    auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(stop1 - start1);
    auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(stop2 - start2);
    
    cout<<"Regular c++ matrix multiplication: "<<duration1.count()<<"us"<<endl;
    cout<<"Utilizing Intrinsics: "<<duration2.count()<<"us"<<endl;

    return 0;
}
