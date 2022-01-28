# Project 1: SIMD Matrix Multiplication

## Background

Matrix-Matrix Multiplication is used widely in the computer science industry, however one of the downfalls of it's usage is that it has a large time complexity.
For example, to perform the multiplication of matrix ```A(rows = l, columns = m)``` with matrix ```B(rows = m, columns = n)```, one must go through each row in A and multiply each value with the corresponding value in B's column. This results in the matrix, ```C(rows = l, columns = n)```, being calculated in a time complexity of ```O(l*m*n)```.

## Goal

This cubic time complexity shown above can cause matrix multiplication to perform slow, especially for large square matricie inputs. The goal of this project is to minimize the time to calculate the result using SIMD functions (in our case, AVX and AVX2) and increasing cache optimization. 

## Implimentation

### Overview

This project utilizes c++ templated classes, which are named ```Matrix<T>```. This enables the usage of any number type to be accepted. This allows the user to configure additional SIMD functions if desired. Through the usage of these templated objects, "normal" C++ matrix Multiplication can be performed easily regardless of the number type (int, float, etc), which can then be compared to the SIMD instruction output. The numbers themselves have been stored in arrays on the heap, allowing for contiguous memory to be pulled to the cache, increasing cache optimization.

### Methodology

To increase throughput of matrix multiplication, SIMD functions were used. Specifically, AVX and AVX2 instructions with a 256 bit type were utilized for this project. This allows for operations across multiple numbers in 1 instruction (such as multiplying 8 numbers at a time), greatly increasing efficiency. Another goal of this project was to increase cache efficiency. One of the large contributors of increasing cache efficiency was that the inverse of the second matrix (```B```) was found before any calculations began. This allows the corresponding matrix multiplication functions to bring contigious chunks of memory into the cache, rather than having to collect the values individually which wastes time.

## Installation and Execution

### Build

This project can be built using the command line and calling g++, using the following line:

```g++ -Wall -g *.cpp -o main.o -mavx2```

### Execution

After building the project, you can then run the program by calling  
```./main.o <optional: size, default = 5> <optional: [NO_]CACHE_OPTIMIZATION default = enabled>```.

```size``` referrs to the size of the matrix to be tested. The optional tag, ```CACHE_OPTIMIZATION``` will optimize cache if entered after ```size```. 

### Examples:

5 x 5 matrix multiplication using cache optimization:  
```./main.o```  
3 x 3 matrix multiplication using cache optimization:  
```./main.o 3 CACHE_OPTIMIZATION```  
4 x 4 matrix multiplication not using cache optimization:  
```./main.o 4 NO_CACHE_OPTIMIZATION```  

## Results

### Testing procedure 

The matrix size was varried from 500 to 10000 and tested for each data type. Each trial was tested multiple times to ensure consistency and an average of all samples was taken. All other computer programs were closed to prevent inconsistent CPU usage.

### Floating point numbers

One of the implimentations coded uses floating point numbers as the type for the object, ```Matrix<T>```. This part utilizes 256 bit register, using AVX SIMD instrucitons.  

Below shows the difference in times (in seconds) between the c++ matrix multiplication, using only normal ```+``` and ```*``` operations, versus using the implimented AVX SIMD instructions, with cache optimization disbaled.

| Matrix Size | Traditional C++ Time | AVX SIMD Time |
|-------------|----------------------|---------------|
| 500         |         1.175        |      0.306    |
| 1000        |        11.887        |      2.5001   |
| 2000        |       141.234        |     19.8353   |
| 5000        |      2252.233        |    307.2744   |
| 10000       |     18208.311        |   2502.9834   |

With ```CACHE_OPTIMIZATION``` enabled:

| Matrix Size | Traditional C++ Time | AVX SIMD Time |
|-------------|----------------------|---------------|
| 500         |         0.841        |     0.0686    |
| 1000        |         6.710        |     0.5352    |
| 2000        |        53.808        |     4.832     |
| 5000        |       831.994        |    75.740     |
| 10000       |      5415.301        |   619.376     |

### Fixed point integers

Another implimentation coded uses fixed point unsigned intnumbers as the type for the object, ```Matrix<T>```. This part utilizes 256 bit register, using AVX and AVX2 SIMD instrucitons.

Similar to above, the following numbers were observed from testing:

| Matrix Size | Traditional C++ Time | AVX SIMD Time |
|-------------|----------------------|---------------|
| 500         |          1.115       |      0.306    |
| 1000        |         11.641       |      2.441    |
| 2000        |        143.550       |     19.549    |
| 5000        |       2283.341       |    302.485    |
| 10000       |      19867.093       |    2457.95    |

With ```CACHE_OPTIMIZATION``` enabled:

| Matrix Size | Traditional C++ Time | AVX SIMD Time |
|-------------|----------------------|---------------|
| 500         |         0.844        |      0.064    |
| 1000        |         6.707        |      0.495    |
| 2000        |        53.800        |      4.520    |
| 5000        |       622.461        |     71.077    |
| 10000       |      5041.334        |    583.660    |

## Conclusion

As noted by the results above, utilizing SIMD instructions greatly increased throughput of matrix multiplication. These instructions allow the operation across multiple values, allowing things such as multiplication and addition to perform much faster. Additionally, cache efficiency decreased the overall program runtime greatly in both cases: traditional C++ matrix multiplication and AVX SIMD matrix multiplication. Loading contigious memory chunks into the cache (such as storing a matrix as its inverse, so an entire row can be pulled into the cache) results in less downtime trying to collect the values from scattered memory (iterating down a column collecting individual values). With both topics, we can note that utilizing both of these methods can increase program performance by up to 96.6%.
