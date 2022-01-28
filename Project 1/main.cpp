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
#include <chrono> // For timing
#include <immintrin.h> // For SIMD functions

#include "matrix.h"

// Optimization flag from argument in main
bool CACHE_OPTIMIZATION = false;

/**
 * @brief Matrix multiplication using regular C++ basic matrix multiplication
 * 
 * @param A First input matrix
 * @param B Second input matrix
 * @param C Output matrix (A x B = C)
 */
template <typename T>
void matrix_multiplication(Matrix<T>& A, Matrix<T>& B, Matrix<T>& C)
{
    if( (A.getCols() != B.getRows()) || 
        (C.getCols() != B.getCols()) ||
        (C.getRows() != A.getRows()) )
    {
        std::cerr<<"Error: Cannot perform matrix multiplication because matrix dimensions do not match"<<std::endl;
        return;
    }
    int i,j,k;
    for (i = 0; i < A.getRows(); i++)
    {
        for (j = 0; j < B.getCols(); j++)
        {
            if(CACHE_OPTIMIZATION)
            {
                T* a = A[i];
                T* b = B[j];
                for (k = 0; k < A.getCols(); k++)
                {
                    C[i][j] += a[k]*b[k];
                }
            }
            else
            {
                C[i][j] = 0;
                for (k = 0; k < A.getCols(); k++)
                {
                    C[i][j] += T(A[i][k])*T(B[k][j]);
                }
            }
        }
    }
}


/**
 * @brief Matrix multiplication using AVX instructions for floats
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
        std::cerr<<"Error: Matrix dimensions do not match"<<std::endl;
        return;
    }
    
    unsigned int i,j,k;
    float ans[8];
    __m256 num1, num2, num3, sum;

    float** matrixA = A.getMatrix();
    float** matrixB = B.getMatrix();
    float** matrixC = C.getMatrix();

    const unsigned int size1 = A.getRows(); //= C.getRows()
    const unsigned int size2 = B.getCols(); //= C.getCols()
    const unsigned int size3 = A.getCols(); //= B.getRows()

    const float* arow;
    const float* bcol;

    for (i = 0; i < size1; i++)
    {
        arow = matrixA[i];
        for (j = 0; j < size2; j++)
        {
            if(CACHE_OPTIMIZATION)
                bcol = matrixB[j];
            else
                bcol = B.getCol(j);

            sum= _mm256_setzero_ps();  //sets sum to zero

            //multiply accumulate flattened column with flattened row
            for(k=0; k<size3; k+=8)
            {
                // num1 = _mm256_loadu_ps(arow+k);             // load: num1 = [a[7], a[6], a[5], a[4]. a[3], a[2], a[1], a[0]]
                // num2 = _mm256_loadu_ps(bcol+k);             // load: num2 = [b[7], b[6], b[5], b[4], b[3], b[2], b[1], b[0]]
                // num3 = _mm256_dp_ps(num1, num2, 0xFF);      // dot prod hi,low: [a[7]b[7]+..., ..., a[3]b[3]+..., ...]
                // Combining the above saves time vs storing into num 1,2:
                num3 = _mm256_dp_ps(_mm256_loadu_ps(arow+k), _mm256_loadu_ps(bcol+k), 0xFF);
                sum = _mm256_add_ps(sum, num3);             // performs vertical addition with previous values
            }
            _mm256_storeu_ps(ans, sum); //stores sum to local float
            matrixC[i][j] = (ans[0] + ans[4]); //set matrix to lower half + upper half answer
        }
    }
}


/**
 * @brief Matrix multiplication using AVX / AVX2 instructions for integers
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
        std::cerr<<"Error, Matrix dimensions do not match"<<std::endl;
        return;
    }

    unsigned int** matrixA = A.getMatrix();
    unsigned int** matrixB = B.getMatrix();
    unsigned int** matrixC = C.getMatrix();

    const int size1 = A.getRows(); //= C.getRows()
    const int size2 = B.getCols(); //= C.getCols()
    const int size3 = A.getCols(); //= B.getRows()

    const unsigned int* arow;
    const unsigned int* bcol;

    // __m256i num1, num2, num3
    __m256i sum;
    int extract[8];

    for (i = 0; i < size1; i++)
    {
        arow = matrixA[i];
        for (j = 0; j < size2; j++)
        {
            if(CACHE_OPTIMIZATION)
                bcol = matrixB[j];
            else
                bcol = B.getCol(j);

            sum= _mm256_setzero_si256();  //sets sum to zero

            //multiply accumulate flattened column with flattened row
            for(k=0; k < size3; k+=8)
            {                                                               // lower half is described. same occurs for upper half:
                // num1 = _mm256_loadu_si256((__m256i*)&arow[k+8]);         // load: num1 = [a[3], a[2], a[1], a[0]]
                // num2 = _mm256_loadu_si256((__m256i*)&bcol[k+8]);         // load: num2 = [b[3], b[2], b[1], b[0]]
                // num1 = _mm256_mullo_epi32(num1, num2);                   // multiply, store low bits: num3 = [a[3]*b[3],  a[2]*b[2],  a[1]*b[1],  a[0]*b[0]]
                // sum = _mm256_add_epi32(sum, num1);                       // performs vertical addition with previous values  sum = [prev[3], prev[2], prev[1], prev[0]]
                
                // Combine into 1 line for maximum efficiency (don't have to save into variables):
                sum = _mm256_add_epi32(sum, 
                        _mm256_mullo_epi32(_mm256_loadu_si256((__m256i*)&arow[k]), 
                                           _mm256_loadu_si256((__m256i*)&bcol[k])));
            }
            sum= _mm256_hadd_epi32(sum, sum); //horizontally sum; sum = [prev[3]+prev[2] , prev[1]+prev[0] , prev[3]+prev[2] , prev[1]+prev[0]]
            sum= _mm256_hadd_epi32(sum, sum); //horizontally sum; sum = [prev[3] + prev[2] + prev[1] + prev[0] , ...]
            _mm256_store_si256((__m256i*)extract, sum);
            matrixC[i][j] = extract[0] + extract[4]; //save extracted values to matrix
        }
    }
}


/**
 * @brief Test function to check whether two matricies have the same contents
 * @note Matricies may not be equal after matrix multiplication due to precision limit of intrinsic instructions,
 * especially when dealing with floats, rather than matrix multiplication syntax mathematical errors.
 * 
 * @tparam T type of matrix to compare
 * @param A First matrix
 * @param B Second matrix
 * @return true Matrix contents match
 * @return false Matrix contents do not match
 */
template <typename T>
bool same(Matrix<T>& A, Matrix<T>& B)
{
    for(int i=0; i<A.getRows(); i++)
    {
        for(int j=0; j<A.getCols(); j++)
        {
            if(T(A[i][j]) != T(B[i][j]))
            {
                return false;
            }
        }
    }
    return true;
}


/**
 * @brief Main function for comparing the performance of float type using 
 * SIMD instructions compared to standard C++ performance
 * 
 * @param size Size of the NxN matrix to be tested
 */
void test_float(unsigned int size)
{
    std::cout<<"Testing float matrix-matrix multiplication:"<<std::endl;
    
    Matrix<float> A(size, size, true);
    std::cout<<"Matrix A:"<<std::endl;
    A.print();

    Matrix<float> B(size, size, true);
    std::cout<<"Matrix B:"<<std::endl;
    B.print();

    //invert entire matrix before multiplication if cache optimization is allowed
    if(CACHE_OPTIMIZATION)
        B.invert(); 

    std::cout<<"Standard A x B ="<<std::endl;
    Matrix<float>  C(size, size, false);
    auto start1 = std::chrono::high_resolution_clock::now();
    matrix_multiplication(A, B, C);
    auto stop1 = std::chrono::high_resolution_clock::now();
    C.print();

    std::cout<<"SIMD A x B ="<<std::endl;
    Matrix<float>  C2(size, size, false);
    auto start2 = std::chrono::high_resolution_clock::now();
    SIMD_float_matrix_multiplication(A, B, C2);
    auto stop2 = std::chrono::high_resolution_clock::now();
    C2.print();
    auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(stop1 - start1);
    auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(stop2 - start2);

    std::cout<<"Regular c++ matrix multiplication: "<<duration1.count()<<"us"<<std::endl;
    std::cout<<"SIMD matrix multiplication: "<<duration2.count()<<"us\n"<<std::endl;
}

/**
 * @brief Main function for comparing the performance of unsigned int type using 
 * SIMD instructions compared to standard C++ performance
 * 
 * @param size Size of the NxN matrix to be tested
 */
void test_int(unsigned int size)
{
	std::cout<<"Testing int matrix-matrix multiplication:"<<std::endl;
    
    Matrix<unsigned int> A(size, size, true);
    std::cout<<"Matrix A:"<<std::endl;
    A.print();

    Matrix<unsigned int> B(size, size, true);
    std::cout<<"Matrix B:"<<std::endl;
    B.print();

    std::cout<<"Standard A x B ="<<std::endl;
    Matrix<unsigned int>  C(size, size, false);
    auto start1 = std::chrono::high_resolution_clock::now();
    matrix_multiplication(A, B, C);
    auto stop1 = std::chrono::high_resolution_clock::now();
    C.print();

    //invert entire matrix before multiplication if cache optimization is allowed
    if(CACHE_OPTIMIZATION)
        B.invert(); 

    std::cout<<"SIMD A x B ="<<std::endl;
    Matrix<unsigned int>  C2(size, size, false);
    auto start2 = std::chrono::high_resolution_clock::now();
    SIMD_matrix_multiplication(A, B, C2);
    auto stop2 = std::chrono::high_resolution_clock::now();
    C2.print();
    auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(stop1 - start1);
    auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(stop2 - start2);

    std::cout<<"Regular c++ matrix multiplication: "<<duration1.count()<<"us"<<std::endl;
    std::cout<<"SIMD matrix multiplication: "<<duration2.count()<<"us\n"<<std::endl;
}


int main(int argc, char* argv[])
{
    // Set the seed time for random
    srand (time(NULL));

    // Size for N x N matrix multiplication:
    unsigned int size;
    if(argc>1 && argc<4)
    {
        size = std::stoi(argv[1]);
        if(std::string(argv[2]) == "CACHE_OPTIMIZATION")
        {
            CACHE_OPTIMIZATION = true;
        }
        if(std::string(argv[2]) == "NO_CACHE_OPTIMIZATION")
        {
            CACHE_OPTIMIZATION = false;
        }
    }
    else
    {
        size = 5;
        CACHE_OPTIMIZATION = true;
    }

    std::cout<<"Using matrix size of: "<<size<<std::endl;

    if(CACHE_OPTIMIZATION)
        std::cout<<"Using cache optimization"<<std::endl;
    else
        std::cout<<"Not using cache optimization"<<std::endl;
    std::cout<<std::endl;

    test_float(size);
    test_int(size);
}