# Project 1: Matrix Multiplication

## Background

Matrix Multiplication is used widely in the computer science industry, however one of the downfalls of it's usage is that it has a large time complexity.
For example, to perform the multiplication of matrix ```A(rows = l, columns = m)``` times matrix ```B(rows = m, columns = n)```, one must go through each row in A and multiply each value with the corresponding value in B's column. This results in the matrix, ```C(rows = l, columns = n)```, being calculated in a time complexity of ```O(l*m*n)```. 

## Goal

This cubic time complexity shown above can cause the multiplication to perform slow, especially for large square matricie inputs. The goal of this project is to minimize the time to calculate the result using SIMD functions (in our case, AVX and AVX2) and implimenting cache optimization. 

## Implimentation

### Overview

This project utilizes c++ templated classes, which are named ```Matrix<T>```. This enables the usage of any number type to be accepted. This allows the user to configure additional SIMD functions if desired. Through the usage of these templated objects, "normal" C++ matrix Multiplication can be performed easily regardless of the number type (int, float, etc), which can then be compared to the SIMD instruction output. The numbers themselves have been stored in arrays on the heap, allowing for contiguous memory to be pulled to the cache, increasing cache optimization.

## Installation and Execution

### Build

This project can be built using the command line and calling g++, using the following line:

```g++ -Wall -g *.cpp -o main.o -msse4.1 -mavx```

### Execution

After building the project, you can then run the program by calling ```./main.o```

## Results

### Testing procedure 

The matrix size was varried from 100 to 10000 and tested for each data type. Each trial was tested 10 times and an average was taken. All other computer programs were closed to prevent inconsistent CPU usage.

### Floating point numbers

One of the implimentations coded uses floating point numbers as the type for the object, ```Matrix<T>```. This part utilizes 256 bit register, using AVX SIMD instrucitons.  

Below shows the difference in times between the c++ matrix multiplication, using only normal ```+``` and ```*``` operations, versus using the implimented AVX SIMD instructions.

| Matrix Size | Traditional C++ Time | AVX SIMD Timing |
|-------------|----------------------|-----------------|
| 100         |                      |                 |
| 1000        |                      |                 |
| 10000       |                      |                 |

Additionally, using the #define, ```CACHE_OPTIMIZATION```, the resulting numbers have been observed:

| Matrix Size | Traditional C++ Time | AVX SIMD Timing |
|-------------|----------------------|-----------------|
| 100         |                      |                 |
| 1000        |                      |                 |
| 10000       |                      |                 |

### Fixed point integers

Another implimentation coded uses fixed point unsigned intnumbers as the type for the object, ```Matrix<T>```. This part utilizes 256 bit register, using AVX and AVX2 SIMD instrucitons. 

Similar to above, the following numbers were observed from testing:

| Matrix Size | Traditional C++ Time | AVX SIMD Timing |
|-------------|----------------------|-----------------|
| 100         |                      |                 |
| 1000        |                      |                 |
| 10000       |                      |                 |

Using #define, ```CACHE_OPTIMIZATION```:


| Matrix Size | Traditional C++ Time | AVX SIMD Timing |
|-------------|----------------------|-----------------|
| 100         |                      |                 |
| 1000        |                      |                 |
| 10000       |                      |                 |