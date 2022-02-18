# Project 2: ZSTD Compression Multithreading

## Background

Zstandard, or ZSTD, is a fast lossless compression algorithm developed by Yann Collet at Facebook. The goal of a compression algorithm is to take input data, use an algorithm, and create output data that represents this input data, in less bits. The higher the compression, the longer this algorithm will take. This library was originally implimented in C, but for our project we utilize the library in C++.  
With multithreading, when the CPU is doing operations such as large mathematical calculations or I/O intensive task like reading or writing memory, other threads (tasks) can be run co-currently while the CPU is waiting on the first task. Using multithreading, we can accelerate the operation of our program. The multithreading library we use is the linux verison, called `pthread`. 

## Goal

Compression algorithms perform intensive algorithms to achieve a high compression on the output file, which may take a long time to complete. To accelerate this, we will be using seperate threads to split the work and accelerate the compression. 

## Methodology

This program uses linux's `pthread` library to impliment threading into our C++ program. A configurable amount of threads ("workers") will be launched from the main thread. Each "worker" thread will perform the ZSTD compression on chunks from the file (`default = 16kB`.) The main thread will read in an initial amount of data from the file. Every time a worker thread takes a chunk, more data will be read in. This method allows more efficient usage of RAM, since the program does not need to keep the whole input file in memory upon starting. The main thread is also responsible for writing the resulting data every time compression is complete. In summary, the progam will read in the input file, multiple worker threads will compress chunks, and then this data will be written to the output file.


## Build process

This project will not work on windows due to the pthread dependancy, so be sure to build on a linux OS. It is also required that you already have the ZSTD library installed.  
To compile, use the following line:  

```g++  -g *.cpp -pthread -lzstd -o ./main.o```

### Additional optional build options
```-DCOMPRESSION_LEVEL=N``` (default 50) tag refers to the compression level to be used for ZSTD compression.  
```-DCHUNK_SIZE=M``` (default 16KB) tag refers to the size of each chunk that will be taken from the input file, to then be compressed and added to the output file.

## Execution

After building the project, you can then run the program by calling  
```./main.o <thread count> <input file> <output file>```  

## Example:
Testing on input file `input.zip`, outputting as `output.zst`, using 3 threads for compression:  
```./main.o 3 input.zip output.zst```  

## Results

### Testing procedure 

The worker count was varied from 1 to 10 and project execution was timed to compress a specified input file. Each trial was tested multiple times to ensure consistency and an average of all samples was taken. All other computer programs were closed to prevent inconsistent CPU usage.

All testing was performed on: Intel i7-8750h 6 core, 12 logical processor CPU

### Input 1: [Silesia Corpus](http://sun.aei.polsl.pl/~sdeor/index.php?page=silesia)

Build specifications:   `COMPRESSION_LEVEL: 50, CHUNK_SIZE: 16KB`

This data set is composed of multiple well-known files supplied by the [Silesia Corpus](http://sun.aei.polsl.pl/~sdeor/index.php?page=silesia), totaling in at ~202MB with 12 files. An uncompressed ZIP file was created from the collection and used as input for this test. The execution times were recorded as follows:

| Worker threads | Program run time | 
|----------------|------------------|
| 1              |     65.025s     |
| 2              |     33.827s     |
| 3              |     23.174s     |
| 4              |     18.704s     |
| 5              |     16.534s     |
| 6              |     14.979s     |
| 8              |     12.746s     |
| 10             |     11.671s     |

### Input 2: 256MB random byte file

Build specifications: `COMPRESSION_LEVEL: 50, CHUNK_SIZE: 16KB`

This input file is composed of 256MB of random bytes, made with the program, ```file_gen.py```. Below shows the different times (in seconds) of the ZSTD compression on the 256MB input file with varying workers.

| Worker threads | Program run time | 
|----------------|------------------|
| 1              |     15.362s     |
| 2              |     8.705s     |
| 3              |     6.230s     |
| 4              |     5.293s     |
| 5              |     4.693s     |
| 6              |     4.040s     |
| 8              |     3.395s     |
| 10             |     3.148s     |

## Conclusion

As noted by the results above, utilizing threading to perform compression on chunks of the input file greatly increased throughput of the overall file compression. Using more worker threads allowed the compression to take as little as 18% of the original time to compress. Multithreading allows the CPU to procress more commands while other threads may be doing seperate I/O tasks such as reading/writing from memory. This process increases paralleism, allowing more work to be done by each thread task, decreasing the amount of time required to compress a single file, as we have seen in the results.