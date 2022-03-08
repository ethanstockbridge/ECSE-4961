# Project 4: Dictionary encoder 

## Background

Dictionary encoding is a data compression technique popular for column based data. This type of encoding is very effective for columns of data consisting of many repeating elements. Each unique element will be linked to an encoded ID, having a goal to make this encoded ID smaller than the original data. After the encoding process, we now have a dictionary which points each decoded data point to the corresponding encoded ID, and then the output in encoded data.

## Goal

The goal of this project is to take a text-based input file, use the encoder and produce an output consisting of the dictionary, and encoded data. Ideally, the  output file will require less disk space than the input file due to the encoding process.

## Methodology

For the dictionary encoding process we utilize uses a hashtable for the encoding specifically. Each input item will pass though a hash function to find where it belongs in the hashtable. A decoder list will also keep track of each unique element's decoded data and encoded ID. After reading all of the columnd data, with the usage of optimization, we can then determine the optimal encoding. This process consists of setting the most commonly used elements equal to the smallest ID, so storage of the output file requires less data. 

## Build process

To compile, use the following line:  

```g++  -g *.cpp -Wall -o ./a.out```

### Additional optional build options
```-DOPTIMIZE_ENCODING``` This build flag will optimize the encoding such that the most used elements out of the column data will correspond to the smallest encoding, producing a more efficient output encoding.

## Execution

After building the project, you can then choose one of the two following functions:

### Encoding

Encoding will take a text input file consisting of column data (as an example, view the file `small.txt`), and produce an output file consisting of the dictionary and encoded data with format:

```
dictionary:
asdfghjkl
zxcvbnm
...
data:
0
1
0
1
1
...
```

The dictionary represents each decoded item, along with it's associated encoded ID as the placement. So, "asdfghjkl" is the decoded data, and it is assocated with encoded ID of 0. Similarly, "zxcvbnm" as an encoded ID of 1.

To perform this, use the following format:

```./a.out ENCODE <input> <output>```  

Example:

```./a.out ENCODE small.txt encoded.txt```  

### Query

Query will enable the user to query an existing encoded file to determine whether a dictionary item exists. If it does exist, the program will print how many times it exists in the file.  
*This means you must call this function using a file output by `ENCODE`. 

To perform this, use the following format:

```./a.out ENCODE <input> <query>```  

Example:

```./a.out QUERY encoded.txt ircplberj```

## Results

### Testing procedure 

Multiple files of varying sized columns were tested for encoding and query. One of which, `small.txt` can be found in this repository. Each trial was tested multiple times to ensure consistency and an average of all samples was taken. All other computer programs were closed to prevent inconsistent CPU usage.

All testing was performed on: Intel i7-8750h 6 core, 12 logical processor CPU

### Testing: Encode various column text files

Input files:
| File name | File size | Total Data elements | Unique data elements | 
|-|-|-|-|
|small.txt|932KB|119,649|1,000|
|medium.txt|9,382KB|1,199,755|10,000|
|large.txt|184,132KB|23,574,717|195,427|

Performance:

| File name | Encoding Optimization | Encoding time | Optimization time |  Write time | Encoded file size |
|-|-|-|-|-|-|
|small.txt|Disabled|0.03383s|x|0.01224s|462KB|
|small.txt|Enabled| 0.03386s|0.000481s|0.01204s|450KB|
|medium.txt|Disabled|0.3574s|x|0.104708s|5,787KB|
|medium.txt|Enabled|0.3554s|0.006195s|0.09522s|5,663KB|
|large.txt|Disabled|19.6222s|x|5.40373s|148,402KB|
|large.txt|Enabled|19.5151s|0.189173s|4.82749s|144,907KB|

### Testing: Query from encoded file

Using the input file, `small.txt`, we are able to first encode this, and then test the query functionality of this program on the output file. Results are shown below:

| Query | Output |
|-|-|
|ahmkhrfiby | 88 occurances |
|znzafvkn| 423 occurances |
|abcdefg | Not found |
|kgzbhydw|107 occurances|

These results are consistent with the `small.txt` column data

## Conclusion

As shown from the results above, encoding files using this implimentation of a dictionary encoder allows the resulting files to be smaller in size and require less disk space. It is also notable that optimizing the encoding pattern with build flag, `-DOPTIMIZE_ENCODING` allows the file size to be reduced further, as shown in the tests above. Note that the write times for the files with optimizaiton enabled have faster write times than with this disabled. This is because in the encoded data, the elements which appear most often will have the lowest encoded IDs, requiring less space, resulting in a faster write time and smaller file size.