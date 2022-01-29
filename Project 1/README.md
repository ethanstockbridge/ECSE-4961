# Project 1: SIMD Matrix Multiplication

## Background

Matrix-Matrix Multiplication is used widely in the computer science industry, however one of the downfalls of its calculation is that it has a large time complexity.  
To perform the multiplication of matrix ```A(rows = l, columns = m)``` with matrix ```B(rows = m, columns = n)```, one must go through each row in A and multiply each value with the corresponding value in B's column, add these values up, and store them in matrix C. This results in the formation of matrix, ```C(rows = l, columns = n)```, being calculated in a time complexity of ```O(l*m*n)```.

## Goal

This cubic time complexity described above can cause matrix multiplication to perform slowly, especially for large square matrix inputs. The goal of this project is to minimize the time to calculate the result using SIMD functions (in our case, AVX and AVX2) and increasing cache optimization. 

## Implementation

### Overview

This project utilizes c++ template classes. To simplify the process, a class named ```Matrix<T>``` was created to store all of the matrix information. This enables the usage of any number type to be accepted. Through the usage of these template objects, "normal" C++ matrix Multiplication can be performed uniformly regardless of the number type (int, float, etc.), which can then be compared to the SIMD instruction performance. The numbers themselves have been stored in arrays on the heap, allowing for contiguous memory to be pulled to the cache, increasing cache optimization.

### Methodology

To increase throughput of matrix multiplication, SIMD functions were used. Specifically, AVX and AVX2 instructions with a 256 bit type were utilized for this project. This allows for operations across multiple numbers in 1 instruction (such as multiplying 8 numbers at a time), greatly increasing efficiency. Another goal of this project was to increase cache efficiency. One of the large contributors of increasing cache efficiency was that the inverse of the second matrix (```B```) was found before any calculations began. This allows the corresponding matrix multiplication functions to bring contigious chunks of memory into the cache, row by row, rather than having to collect the values individually which wastes time.

## Installation and Execution

### Build

This project can be built using the command line and calling g++, using the following line:

```g++ -Wall -g *.cpp -o main.o -mavx2 -<optional: DCACHE_OPTIMIZATION> -<optional: DVERBOSE>```  

The DCACHE_OPTIMIZATION tag refers to enabling code that optimizes the cache as described above.  
The DVERBOSE tag refers to printing out the matrix A, B and resulting matrix C. This is useful for viewing small matrix, but should not be used for large matrix, as the size will be too big to display nicely.  

### Execution

After building the project, you can then run the program by calling  
```./main.o <optional: size, default = 5>```  

Where ```size``` refers to the size of the matrix to be tested

### Examples:

Default testing 5 x 5 matrix multiplication:  
```./main.o```  
Testing 100 x 100 matrix multiplication:  
```./main.o 100```  

## Results

### Testing procedure 

The matrix size was varied from 500 to 10000 and tested for each data type (```short int```, ```float```). Each trial was tested multiple times to ensure consistency and an average of all samples was taken. All other computer programs were closed to prevent inconsistent CPU usage.

### Floating point numbers

One of the implementations coded uses 4 byte ```float``` numbers as the type for the object, ```Matrix<T>```. This part utilizes 256 bit register functions, using AVX SIMD instructions.  

Below shows the different times (in seconds) between the c++ matrix multiplication, using only normal ```+``` and ```*``` operations, versus using the implemented AVX SIMD instructions, with cache optimization disabled:

| Matrix Size | Traditional C++ Time | AVX SIMD Time |
|-------------|----------------------|---------------|
| 500         |        1.168s        |     0.605s    |
| 1000        |       11.949s        |     6.194s    |
| 2000        |      138.076s        |    78.750s    |
| 5000        |     2256.578s        |   1298.320s   |
| 10000       |    18981.448s        |  10996.880s   |

With build flag ```-DCACHE_OPTIMIZATION```:

| Matrix Size | Traditional C++ Time | AVX SIMD Time |
|-------------|----------------------|---------------|
| 500         |       0.627s         |    0.068s     |
| 1000        |       4.945s         |    0.545s     |
| 2000        |      39.897s         |    4.949s     |
| 5000        |     621.379s         |   76.920s     |
| 10000       |    4971.780s         |  615.880s     |

### Fixed point integers

Another implementation uses 2 byte fixed point ```short int``` numbers as the type for the object, ```Matrix<T>```. This part utilizes 256 bit register, using AVX and AVX2 SIMD instructions.

Similar to above, the following numbers were recorded from testing:

| Matrix Size | Traditional C++ Time | AVX SIMD Time |
|-------------|----------------------|---------------|
| 500         |        1.145s        |      0.559s   |
| 1000        |        9.538s        |      4.664s   |
| 2000        |      134.879s        |     67.335s   |
| 5000        |     2184.755s        |   1149.710s   |
| 10000       |    17678.748s         |   9299.502s   |

With build flag ```-DCACHE_OPTIMIZATION```:

| Matrix Size | Traditional C++ Time | AVX SIMD Time |
|-------------|----------------------|---------------|
| 500         |         0.631s       |     0.034s    |
| 1000        |         4.981s       |     0.250s    |
| 2000        |        40.003s       |     2.192s    |
| 5000        |       622.003s       |    35.653s    |
| 10000       |      4986.963s       |   280.840s    |

## Conclusion

As noted by the results above, utilizing SIMD instructions greatly increased throughput of matrix multiplication. These instructions allow the operation across multiple values, allowing arithmetic such as multiplication and addition to perform much faster. It is seen that even without cache optimization, the calculations utilizing SIMD instructions to perform matrix multiplication were able to complete 2 times faster than the traditional C++ matrix multiplication. Implementing cache optimization was able to decrease the overall program runtime greatly in both cases as shown above in the results. Loading contiguous data chunks into the cache (such as storing a matrix as its inverse, so an entire row can be pulled into the cache) results in less downtime trying to collect the singular values from scattered memory (such as iterating down a column, collecting individual values). Utilizing both of these methods can increase program performance by up to 98.4%.
