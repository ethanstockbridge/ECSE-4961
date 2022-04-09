# ECSE-4961 - RPI Advanced Computer Systems 

## Project 1 - [SIMD matrix multiplication](https://github.com/ethanstockbridge/ECSE-4961/tree/main/Project%201)

This project has implimentation a C/C++ module that carries out high-speed matrix-matrix multiplication by explicitly utilizing x86 SIMD instructions. Matrix-matrix multiplication is one of the most important data processing kernels in numerous real-life applications, e.g., machine learning, computer vision, signal processing, and scientific computing. By utlizing SIMD instructions and cache access optimization, matrix x matrix multiplication throughput can be greatly increased, taking less than 1.6% of the time required for unoptimized c++ matrix multiplication

## Project 2 - [Multithreaded ZSTD compression](https://github.com/ethanstockbridge/ECSE-4961/tree/main/Project%202)

This project implements a C/C++ module that uses multiple threads to compress an input data stream. Compression is be done by calling the ZSTD library, which was open sourced by Facebook and has been widely adopted in the industry. Each 16KB block in the input data stream is compressed by ZSTD individually. All the compressed blocks are then in-order written to one output file. 

## Project 3 - [System performance analysis](https://github.com/ethanstockbridge/ECSE-4961/tree/main/Project%203)

This project has us explore throughput performance and modern memory storage devices. Using Intel Memory Latency Checker and Flexible IO tester (FIO), we observe a clear trade-off between access latency and throughput. As memory/storagre access queues are incrased, we will achieve a higher resource allocation and higher throughput, but at the cost of higher latency and data access time required. 

## Project 4 - [Dictionary encoder](https://github.com/ethanstockbridge/ECSE-4961/tree/main/Project%204)

This project had us impliment a dictionary encoder. To create this, data structures such as hashtables were utilized. This dictionary encoder allowed us to quickly and efficiency encode files. Utilizing SIMD allowed increase throughput. With encoding optimization, the output files could be decreased in size when compared to the input files. 

## Project 5 - Final project - [Multithread transaction simulator](https://github.com/ethanstockbridge/ECSE-4961/tree/main/Project%205)

This final project was open-ended and up to the student. For us, we found it interesting to learn about databases and data logging. This project utilizes SQL and multithreading to achieve a transaction simulator, which uses logging to allow for error detection and correction when there is system malfunction, allowing for the correction of data. 