# Project 2: ZSTD Compression Multithreading

## Background

Zstandard, or ZSTD, is a fast lossless compression algorithm developed by Yann Collet at Facebook. The goal of a compression algorithm is to take input data, use an algorithm, and create output data that represents this input data, in less bits. The higher the compression, the longer this algorithm will take. This library was originally implimented in C, but for our project we utilize the library in C++.  
With multithreading, when the CPU is doing operations such as large mathematical calculations or I/O intensive task like reading or writing memory, other threads (tasks) can be run co-currently while the CPU is waiting on the first task. Using multithreading, we can accelerate the operation of our program. The multithreading library we use is the linux verison, called `pthread`. 

## Goal

Compression algorithms perform intensive algorithms to achieve a high compression on the output file, which may take a long time to complete. To accelerate this, we will be using seperate threads to split the work and accelerate the compression. 

## Implementation

### Methodology

This program uses `pthread` as the library to impliment threading into our C++ program. A configurable amount of threads ("workers") will be launched from the main thread. Each "worker" thread will perform the ZSTD compression on chunks from the file (`default = 16kB`.) The main thread will read in a configurable amount of limited data from the file. Every time a worker thread takes a chunk, more data will be read in. This method allows a smaller usage of ram, since the program does not need to keep the whole input file in memory. The main thread is also responsible for writing the resulting data every time compression is complete. All together, the progam will read in the input file, workers will compress chunks, and then this data will be written to the output data.


## Installation and Execution

### Build

This project can be built using the command line and calling g++, using the following line:

```g++  -g *.cpp -pthread -lzstd -o ./main.o -DNUM_WORKERS=<worker thread count>```  

The DNUM_WORKERS tag refers to the amount of worker threads that will be created to perform compression on the chunks.

### Execution

After building the project, you can then run the program by calling  
```./main.o <input file> <output file>```  

### Example:

Default testing 5 x 5 matrix multiplication:  
```./main.o input.zip output.zst```  

## Results

### Testing procedure 

The worker count was varied from 1 to 10 and tested to find the amount of time taken to compress a specified input file. Each trial was tested multiple times to ensure consistency and an average of all samples was taken. All other computer programs were closed to prevent inconsistent CPU usage.

All testing was performed on: Intel i7-8750h 6 core, 12 logical processors

### Input: 256MB random byte file

Build specifications:   `COMPRESSION_LEVEL: 50, CHUNK_SIZE: 16KB`

This input file is composed of 256MB of random bytes.
Below shows the different times (in seconds) between the ZSTD compression on the 256MB input file.

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

As noted by the results above, utilizing threading to perform compression on only chunks of the input file greatly increased throughput of ZSTD compression. This allows the CPU to procress more commands while some of the threads are doing seperate tasks such as reading/writing from memory. Enabling threading increases paralleism in the work able to be done by separate thread tasks, thus improving the amount of time required to compress a single file.