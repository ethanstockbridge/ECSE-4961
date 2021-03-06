# Project 3: Memory and Storage Performance Profiling
### Introduction
MLC is a software package from Intel that is used to measure memory latncies and bandwidth and how they change with an increasing load on the system. FIO is a software package that thests the I/O of a system with many different variables to change. Both of these programs will be used to perfrom experiments on our memory and storage devices. As the data will show, as the memory or storage acess queue depth is increased, the storage devices will achieve higher resource utilization and higher throughput, but the latency will be longer.

### Environment
|           | CPU                  | RAM                                       | SSD                                     |
| :-----    | :-------------:      | :-------------:                           | :----:                                  |
| System 1  | Ryzen 5 3600X        | 16G DDR4, Part #: CMK16GX4M2B3200C16W     |    Samsung 970 Evo 1TB M.2 NVMe         |
| System 2  | Intel Core i7 8550U  | 8G DDR4, Par #: HMAA51S6AMR6N-UH          |     Samsung 256G NVMe PCIe Internal SSD |

### Results
#### MLC Testing

##### System 1 Results (Desktop)
The command or file used to run the tests will be displayed at the top of each table.

[Read vs. Write Intensity]
[100% Read]

`mlc --loaded_latency -R`
| Inject Delay    | Latency (ns)    | Bandwidth (MB/sec) |
| :-------------:    | :-------------:     |:----:     |
|        0          |       615          |    35010   |
|       2          |          607        |     35429   |
|       8          |         546        |    36267    |
|       15          |         536         |    36406    | 
|         50        |      463           |    36362    |
|       100          |      172           |   35401    | 
|       200          |        132         |    22616   | 
|       300          |      126            |   16421   |
|       400          |      120            |   12731    |
|       500          |          118        |    10406   |
|         700       |         113          |  7707     |
|         1000       |         110          |   5634    |
|         1300       |         112         |     4458  |
|         1700       |         108          |     3593  |
|         2500       |         107          |     2648  |
|         3500       |         106          |     2069  |
|         5000       |         106          |     1630  |
|         9000       |         107          |     1169  |
|         20000       |         106         |     860  |

[1 Read, 1 Write]

`mlc --loaded_latency -W5`
| Inject Delay    | Latency (ns)    | Bandwidth (MB/sec) |
| :-------------:    | :-------------:     |:----:     |
|        0          |       448          |    24537   |
|       2          |          471        |     24601   |
|       8          |         404        |    25096    |
|       15          |         354         |    25617    | 
|         50        |      386           |    25304    |
|       100          |      355           |   25588    | 
|       200          |        416         |    25147   | 
|       300          |      375            |   25927   |
|       400          |      223            |   23569    |
|       500          |          190        |    19318   |
|         700       |         153          |  14238     |
|         1000       |         138          |   10195    |
|         1300       |         123         |     8220  |
|         1700       |         122          |     6432  |
|         2500       |         117          |     4619  |
|         3500       |         113          |     3486  |
|         5000       |         113          |     2610  |
|         9000       |         112          |     1708  |
|         20000       |         112         |     1085  |

[100% Write]

`mlc --loaded_latency -W6`
| Inject Delay    | Latency (ns)    | Bandwidth (MB/sec) |
| :-------------:    | :-------------:     |:----:     |
|        0          |       662          |    25444   |
|       2          |          682        |     25444   |
|       8          |         790        |    25247    |
|       15          |         635         |    25435    | 
|         50        |      141           |    23496    |
|       100          |      119           |   9797    | 
|       200          |        116         |    6616   | 
|       300          |      115            |   4466   |
|       400          |      113            |   3590    |
|       500          |          112        |    3013   |
|         700       |         109          |  2362     |
|         1000       |         108          |   1856    |
|         1300       |         109         |     1559  |
|         1700       |         112          |     1315  |
|         2500       |         108          |     1103  |
|         3500       |         109          |     948  |
|         5000       |         108          |     845  |
|         9000       |         107          |     736  |
|         20000       |         107         |     660  |

[Data Access Size B2500]

`mlc --loaded_latency -b2500`
| Inject Delay   | Latency (ns)    | Bandwidth (MB/sec) |
| :-------------:| :-------------: |:----:              |
 | 00000 | 133.93 |  434253.9 |
 | 00002 | 128.36 |  438573.3 |
 | 00008 | 126.04 |  437772.8 |
 | 00015 | 132.57 |  393928.5 |
 | 00050 | 131.40 |  163776.8 |
 | 00100 | 141.12 |   70788.0 |
 | 00200 | 121.83 |   44137.7 |
 | 00300 | 121.86 |   31803.5 |
 | 00400 | 123.29 |   24806.1 |
 | 00500 | 122.40 |   20329.8 |
 | 00700 | 125.09 |   14813.3 |
 | 01000 | 124.01 |   10587.1 |
 | 01300 | 122.30 |    8288.4 |
 | 01700 | 123.93 |    6465.9 |
 | 02500 | 124.60 |    4574.4 |
 | 03500 | 119.87 |    3449.9 |
 | 05000 | 122.25 |    2556.6 |
 | 09000 | 113.88 |    1704.1 |
 | 20000 | 115.08 |    1068.4 |

[Data Access Size B500]

`mlc --loaded_latency -b500`
| Inject Delay   | Latency (ns)    | Bandwidth (MB/sec) |
| :-------------:| :-------------: |:----:              |
 | 00000 | 114.89 |  502468.6 |
 | 00002 | 110.10 |  501643.8 |
 | 00008 | 111.36 |  490138.4 |
 | 00015 | 113.65 |  485357.6 |
 | 00050 | 137.43 |  167315.4 |
 | 00100 | 118.36 |   74613.4 |
 | 00200 | 129.14 |   42542.0 |
 | 00300 | 126.30 |   30709.2 |
 | 00400 | 115.19 |   24470.4 |
 | 00500 | 115.51 |   20149.4 |
 | 00700 | 119.20 |   14608.4 |
 | 01000 | 117.39 |   10599.2 |
 | 01300 | 119.40 |    8165.1 |
 | 01700 | 113.02 |    6527.5 |
 | 02500 | 120.65 |    4553.5 |
 | 03500 | 123.81 |    3396.3 |
 | 05000 | 123.18 |    2527.3 |
 | 09000 | 123.84 |    1631.9 |
 | 20000 | 122.62 |    1030.9 |

[Data Access Size B100]

`mlc --loaded_latency -b100`
| Inject Delay   | Latency (ns)    | Bandwidth (MB/sec) |
| :-------------:    | :-------------:     |:----:     |
| 00000 | 115.45 |  649200.0 |
| 00002 | 116.19 |  648573.5 |
| 00008 | 115.69 |  639978.8 |
| 00015 | 116.98 |  564097.9 |
| 00050 | 115.79 |  192551.1 |
| 00100 | 119.03 |   75857.7 |
| 00200 | 116.08 |   44574.3 |
| 00300 | 117.10 |   31329.0 |
| 00400 | 116.62 |   24305.1 |
| 00500 | 142.40 |   18820.1 |
| 00700 | 120.80 |   14516.9 |
| 01000 | 124.60 |   10338.8 |
| 01300 | 116.18 |    8312.6 |
| 01700 | 128.75 |    6255.7 |
| 02500 | 119.42 |    4583.3 |
| 03500 | 124.40 |    3354.3 |
| 05000 | 120.62 |    2554.5 |
| 09000 | 124.21 |    1637.9 |
| 20000 | 126.53 |    1009.3 |
 
[Throughput vs. Latency]

```
		mlc --loaded_latency -d0
		mlc --loaded_latency -d2000
		mlc --loaded_latency -d4000
		mlc --loaded_latency -d6000
		mlc --loaded_latency -d8000
		mlc --loaded_latency -d10000
		mlc --loaded_latency -d12000
		mlc --loaded_latency -d14000
		mlc --loaded_latency -d16000
		mlc --loaded_latency -d18000
		mlc --loaded_latency -d20000
		mlc --loaded_latency -d22000
		mlc --loaded_latency -d24000
		mlc --loaded_latency -d26000
		mlc --loaded_latency -d28000
		mlc --loaded_latency -d30000
  ```
  
| Inject Delay   | Latency (ns)    | Bandwidth (MB/sec) |
| :-------------:    | :-------------:     |:----:     |
| 00000 | 888.61  |  34029.0 |
| 02000 | 118.22  |   3085.0 |
| 04000 | 158.75  |   1602.6 |
| 06000 | 123.05  |   1362.0 |
| 08000 | 120.34  |   1163.1 |
| 10000 | 125.13  |   1015.4 |
| 12000 | 118.99  |    962.4 |
| 14000 | 122.13  |    886.4 |
| 16000 | 133.68  |    792.4 |
| 18000 | 118.45  |    823.1 |
| 20000 | 119.82  |    789.3 |
| 22000 | 126.52  |    736.3 |
| 24000 | 106.17  |    818.9 |
| 26000 | 122.28  |    718.6 |
| 28000 | 127.31  |    681.0 |
| 30000 | 128.91  |    665.1 |


##### System 2 Results (Laptop)
[Read vs. Write Intensity]
[Read Only]

`mlc --loaded_latency -R`

| Inject Delay   | Latency (ns)    | Bandwidth (MB/sec) |
| :-------------:    | :-------------:     |:----:     |
|       0          |       473          |    11089   |
|       2          |          674        |     11121   |
|       8         |          449        |     11167   |
|       15         |         311         |     13193   |
|         50        |      179            |      14987  |
|       100          |      118            |     10594  |
|       200          |         113         |     6411  |
|       300         |      160            |     4453  |
|       400          |      113            |     3748  |
|      500          |        104        |     3102  |
|        700      |         106          |     2361  |
|        1000      |         104          |     1861  |
|        1300      |         104          |     1578  |
|        1700      |         104          |     1345  |
|        2500      |         102          |     1132  |
|        3500      |         105          |     975  |
|        5000      |        101         |     890  |
|        9000      |           107          |     741  |
|        20000      |         146         |     507  |

[50% Read and Write]

`mlc --loaded_latency -W5`

| Inject Delay   | Latency (ns)    | Bandwidth (MB/sec) |
| :-------------:    | :-------------:     |:----:     |
|       0          |       740          |    13057   |
|       2          |          595        |     13354   |
|       8         |          607        |     13320   |
|       15         |         680         |     13130   |
|         50        |      738            |      12921  |
|       100          |      657            |     13095  |
|       200          |         294         |     13037  |
|       300         |      120            |     10124  |
|       400          |      104            |     7964  |
|      500          |        96        |     6685  |
|        700      |         91          |     5113  |
|        1000      |         88          |     3837  |
|        1300      |         85          |     3152  |
|        1700      |         95          |     2495  |
|        2500      |         95          |     1923  |
|        3500      |         93          |     1575  |
|        5000      |        94         |     1305  |
|        9000      |           95          |     1017  |
|        20000      |         92         |     846  |

[Write Only]

`mlc --loaded_latency -W6`

| Inject Delay   | Latency (ns)    | Bandwidth (MB/sec) |
| :-------------:    | :-------------:     |:----:     |
|       0          |       595          |    13855   |
|       2          |          560        |     14098   |
|       8         |          565        |     14005   |
|       15         |         539         |     13859   |
|         50        |      102            |      7485  |
|       100          |      93            |     3978  |
|       200          |         102         |     2427  |
|       300         |      101            |     1873  |
|       400          |      99            |     1578  |
|      500          |        97        |     1420  |
|        700      |         95          |     1216  |
|        1000      |         95          |     1064  |
|        1300      |         94          |     976  |
|        1700      |         92          |     922  |
|        2500      |         93          |     843  |
|        3500      |         91          |     808  |
|        5000      |        93         |     760  |
|        9000      |           92          |     737  |
|        20000      |         94         |     698  |

[Data Access Size B2500]

`mlc --loaded_latency -b2500`

| Inject Delay   | Latency (ns)    | Bandwidth (MB/sec) |
| :-------------:    | :-------------:     |:----:     |
|00000 | 395.01  |  13281.2 |
| 00002 | 371.13 |   14329.3 |
| 00008 | 327.58 |   15770.0 |
| 00015 | 288.73 |   17258.3 |
| 00050 | 181.77 |   21442.4  |
| 00100 | 122.50 |   18854.7 |
|  00200 |  21.31  |  17863.4  |
| 00300  | 19.88  |  14175.4  |
|  00400 |  19.09  |  11999.4 |
|  00500 |  19.08  |  10408.5 |
|  00700 |  19.92  |   8224.7  |
|   01000 |  19.22  |   6927.1  |
|  01300 |  19.25  |   6125.0 |
|   01700 |  19.18  |   5488.2  |
|    02500  | 18.62  |   4941.2  |
|   03500 |  19.03  |   4431.6  |
|  05000  | 18.58  |   4204.5  |
|  09000  | 18.71  |   3841.0 |
|  20000  | 18.60   |  3632.1  |

[Data Access Size B500]

`mlc --loaded_latency -b500`

| Inject Delay   | Latency (ns)    | Bandwidth (MB/sec) |
| :-------------:    | :-------------:     |:----:     |
| 00000  | 21.96 |  128707.8 |
| 00002  | 21.76 |  130105.4 |
| 00008  | 22.17 |  122436.7 |
| 00015  | 22.34 |  116915.5 |
| 00050  | 18.99 |   76103.9 |
| 00100  | 18.61 |   32165.5 |
| 00200  | 18.34 |   20573.9 |
| 00300  | 18.76 |   15228.2 |
| 00400  | 18.84 |   12317.9 |
| 00500  | 18.34 |   10861.7 |
| 00700  | 18.17 |    8883.9 |
| 01000  | 18.77 |    7129.8  |
| 01300  | 22.13 |    5610.7  |
| 01700  | 20.94 |    5131.9  |
| 02500  | 19.01 |    4862.2 |
| 03500  | 18.44 |    4558.1  |
| 05000  | 17.94 |    4341.4 |
| 09000  | 20.21 |    3577.0 |
| 20000  | 27.40 |    2507.0 |


[Data Access Size B100]

`mlc --loaded_latency -b100`

| Inject Delay   | Latency (ns)    | Bandwidth (MB/sec) |
| :-------------:    | :-------------:     |:----:     |
|  00000 |   5.22 |  281787.0 |
|  00002 |   5.69 |  274668.1 |
|  00008 |   5.57 |  253624.3 |
|  00015 |   5.25 |  227143.0 |
|  00050 |   5.06 |   85333.4 |
|  00100 |   5.07 |   41823.9 |
|  00200 |   5.22 |   29175.2 |
|  00300 |   4.99 |   24901.1 |
|  00400 |   5.03 |   21891.8 |
|  00500 |   7.08 |   15598.0 |
|  00700 |   7.29 |   12985.6 |
|  01000 |   7.36 |   11543.3 |
|  01300 |   6.85 |   11557.5 |
|  01700 |   7.23 |   10459.6 |
|  02500 |   7.26 |    9907.2 |
|  03500 |   7.17 |    9711.2 |
|  05000 |   7.22 |    9440.1 |
|  09000 |   7.22 |    9179.7 |
|  20000 |   7.12 |    9122.2 |
 
 
[Throughput vs. Latency]

```		mlc --loaded_latency -d0
		mlc --loaded_latency -d2000
		mlc --loaded_latency -d4000
		mlc --loaded_latency -d6000
		mlc --loaded_latency -d8000
		mlc --loaded_latency -d10000
		mlc --loaded_latency -d12000
		mlc --loaded_latency -d14000
		mlc --loaded_latency -d16000
		mlc --loaded_latency -d18000
		mlc --loaded_latency -d20000
		mlc --loaded_latency -d22000
		mlc --loaded_latency -d24000
		mlc --loaded_latency -d26000
		mlc --loaded_latency -d28000
		mlc --loaded_latency -d30000 
```

| Inject Delay   | Latency (ns)    | Bandwidth (MB/sec) |
| :-------------:    | :-------------:     |:----:     |
| 00000 | 248.30  |  16144.0 |
| 02000 |  81.90  |   2028.1 |
| 04000 |  88.03  |   1225.9 |  
| 06000 |  89.10  |   1061.8 |
| 08000 |  94.67  |    912.7 | 
| 10000 |  86.81  |    938.9 |
| 12000 |  83.09  |    944.7 |
| 14000 |  83.99  |    906.5 |
| 16000 |  86.76  |    861.4 |
| 18000 |  84.55  |    869.9 |
| 20000 |  83.19  |    874.9 |
| 22000 |  84.88  |    846.3 |
| 24000 |  84.90  |    838.3 |
| 26000 |  83.36  |    846.8 |
| 28000 |  82.93  |    846.9 |
| 30000 |  82.73  |    863.4 |
 
#### FIO Testing
##### System 1 Results
[Read vs. Write Intensity]

`fio readWrite.fio`

| Percentage Read    | Percentage Write    | IOPS Read | IOPS Write |
| :-------------:    | :-------------:     |:----:     | :---:      |
|        0%          |       100%          |    Null   | 2266       |
|       10%          |          90%        |     222   | 2044       |
|       20%          |          80%        |     458   | 1809       |
|       30%          |         70%         |     676   | 1590       |
|         40%        |      60%            |      903  | 1364       |
|       50%          |      50%            |     1139  | 1128       |
|       60%          |         40%         |     1372  | 897        |
|       70%          |      30%            |     1597  | 672        |
|       80%          |      20%            |     1815  | 455        |
|       90%          |          10%        |     2049  | 222        |
|         100%       |         0%          |     2269  | Null       |

[Data Access Size For Read]

`fio readDataSize.fio`

| Block Size         |          IOPS       | 
| :-------------:    | :-------------:     |
|       4k          |       6714          |   
|       8k          |          4259       |     
|       16k          |          3917        | 
|       32k          |         2831         | 
|         64k        |      1578            | 
|       128k          |      842            | 
|       256k          |         493         |

[Data Access Size For Write]

`fio writeDataSize.fio`

| Block Size         |          IOPS       | 
| :-------------:    | :-------------:     |
|       4k          |       7311          |   
|       8k          |          6764     |     
|       16k          |          6268        | 
|       32k          |         5018         | 
|         64k        |      2529            | 
|       128k          |      1525            | 
|       256k          |         958         |

[Throughput vs. Latency]

`fio lataencyTest.fio`

| Latency (ms)         |          IOPS       | 
| :-------------:    | :-------------:     |
|       50          |       5532          |   
|       100          |          5731      |     
|       250          |          5691        | 
|       500          |         5718         | 
|         1000        |      5816            | 

##### System 2 Results
[Read vs. Write Intensity]

`fio readWrite.fio`

| Percentage Read    | Percentage Write    | IOPS Read | IOPS Write |
| :-------------:    | :-------------:     |:----:     | :---:      |
|        0%          |       100%          |    Null   | 109k       |
|       10%          |          90%        |     10.9k   | 95.9k       |
|       20%          |          80%        |     22.3k   | 87.3k       |
|       30%          |         70%         |     31.8k   | 74.2k       |
|         40%        |      60%            |      43.2k  | 64.6k       |
|       50%          |      50%            |     53.4k  | 53.3k       |
|       60%          |         40%         |     66.3k  | 44k        |
|       70%          |      30%            |     75.5k  | 32.3k        |
|       80%          |      20%            |     89.6k  | 22.6k        |
|       90%          |          10%        |     101k  | 11.2k        |
|         100%       |         0%          |     112k  | Null       |

[Data Access Size For Read]

`fio readDataSize.fio`

| Block Size         |          IOPS       | 
| :-------------:    | :-------------:     |
|       4k          |       117k          |   
|       8k          |          92k       |     
|       16k          |          81.9k        | 
|       32k          |         61.1k         | 
|         64k        |      40.2k            | 
|       128k          |      26.3k            | 
|       256k          |         14.6k         |

[Data Access Size For Write]

`fio writeDataSize.fio`

| Block Size         |          IOPS       | 
| :-------------:    | :-------------:     |
|       4k          |       103k          |   
|       8k          |          93.1k      |     
|       16k          |          70.6k        | 
|       32k          |         55.4k         | 
|         64k        |      35.4k            | 
|       128k          |      21.3k            | 
|       256k          |         11.6k         |

[Throughput vs. Latency]

`fio latencyTest.fio`

| Latency (ms)         |          IOPS       | 
| :-------------:    | :-------------:     |
|       50          |       44.9k          |   
|       100          |          47.2k      |     
|       250          |          51.8k        | 
|       500          |         55.7k         | 
|         1000        |      55.5k            | 

### Analysis
All data from above as has been aggregated into graphs to better understand the different tests.
#### Testing from MLC
![System 1 Read vs Write Latency](https://github.com/ethanstockbridge/ECSE-4961/blob/main/Project%203/Images/Inject-Delay-vs-Latency-for-System-1.PNG)
![System 2 Read vs Write Latency](https://github.com/ethanstockbridge/ECSE-4961/blob/main/Project%203/Images/Inject%20Delay%20vs%20Latency%20for%20System%202.PNG)

From the graph for system 1 it can be seen that at lower inject delays, writing has the highest latency and having a mix of reads and writes has the lowest latency. As inject delay gets larger than 2000, having a mix of reads and writes has the most latency while writes have the lowest latency. 
The graph for system 2 does not reflect the same data that system 1 had. The inverse appears to happen for system 2 with the exception of the mixed read and write data. 

![System 1 Data Access Size](https://github.com/ethanstockbridge/ECSE-4961/blob/main/Project%203/Images/Data-Access-Size-for-System-1.PNG)
![System 2 Data Access Size](https://github.com/ethanstockbridge/ECSE-4961/blob/main/Project%203/Images/Data-Access-Size-for-System-2.PNG)

The graphs for data access size both show that the as the data access size gets lower, the higher the bandwidth which is consistent with what we learned in class.

![System 1 Throughput vs. Latency](https://github.com/ethanstockbridge/ECSE-4961/blob/main/Project%203/Images/System-1-Throughput-vs-Latency.PNG)
![System 2 Throughput vs. Latency](https://github.com/ethanstockbridge/ECSE-4961/blob/main/Project%203/Images/System-2-Throughput-vs-Latency.PNG)

As it can been seen, latency and bandwidth have a positive relationship. When bandwidth increases, so does latency.

#### Testing from FIO
![System 1 Read vs Write IOPS](https://github.com/ethanstockbridge/ECSE-4961/blob/main/Project%203/Images/Percentage-Read-vs-IOPS-for-read-and-write-for-system-1.PNG)
![System 2 TRead vs Write IOPS](https://github.com/ethanstockbridge/ECSE-4961/blob/main/Project%203/Images/Percentage-Read-vs-IOPS-for-read-and-write-system-2.PNG)

The graphs show that both read and write take approximately the same amount of IOPS. As the percentage of read increases the IOPS dedicated to read increases and the IOPS for write decreases. 

![System 1 Read vs Write IOPS](https://github.com/ethanstockbridge/ECSE-4961/blob/main/Project%203/Images/Data-Access-Size-Graph-for-System-1.PNG)
![System 2 TRead vs Write IOPS](https://github.com/ethanstockbridge/ECSE-4961/blob/main/Project%203/Images/Data-Access-Size-Graph-for-System-2.PNG)

When the block size increases, the IOPS of read and write decrease at an exponential rate for both systems.

![System 1 Read vs Write IOPS](https://github.com/ethanstockbridge/ECSE-4961/blob/main/Project%203/Images/Throughput-vs-Latency-for-System-1.PNG)
![System 2 TRead vs Write IOPS](https://github.com/ethanstockbridge/ECSE-4961/blob/main/Project%203/Images/Throughput-vs-Latency-for-System-2.PNG)

The graphs show that IOPS increases slightly as the latnecy is increased for both graphs.

### Conclusion
Based off the testing performed on the memory and storage devices, there is a clear performance difference in reading versus writing intensity, data access size, and throughput versus latency. As we discussed in calss, as the memory/storage access queue depth increases, the memory/storage devices will achieve higher resource utilization and higher throughput, but the latency will be longer.

