# Project 3: Memory and Storage Performance Profiling
### Introduction


### Environment


### Results
#### MLC Testing

##### System 1 Results

##### System 2 Results

#### FIO Testing
##### System 1 Results
[Read vs. Write Intensity]
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
| Block Size         |          IOPS       | 
| :-------------:    | :-------------:     |
|       4k          |       7311          |   
|       8k          |          67641      |     
|       16k          |          6268        | 
|       32k          |         5018         | 
|         64k        |      2529            | 
|       128k          |      1525            | 
|       256k          |         958         |

[Throughput vs. Latency]
| Latency (ms)         |          IOPS       | 
| :-------------:    | :-------------:     |
|       50          |       5532          |   
|       100          |          5731      |     
|       250          |          5691        | 
|       500          |         5018         | 
|         1000        |      2529            | 

##### System 2 Results
[Read vs. Write Intensity]
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

### Analysis


### Conclusion
