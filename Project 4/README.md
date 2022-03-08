# Project 4: Dictionary encoder 

## Background

Dictionary encoding is one of the most popular styles of encoding for lossless data compression. This type of encoding is very effective for columns of data which consists of many repeating elements. Each unique element will be set equal to an encoded ID, having a goal to make this encoded ID smaller than the original data. After the encoding process, we are left with a dictionary which points each decoded data point to the corresponding encoded ID, and then the output encoded data.

## Goal

The goal of this project is to take a text input file, use the encoder and produce an output consisting of the dictionary, and encoded data, and be able to have the output file require less disc space than the input file. 

## Methodology

The dictionary encoding process we utilize uses a hashtable for the encoding process. Each input item will pass though a hash function to find where it belongs in the hashtable, which makes it more efficient to find the encoding of the item. A decoder list will also keep track of each unique element's data and encoded ID. After reading all of the columnd data, with the usage of optimization, we can then determine the optimal encoding, setting the most commonly used elements equal to the smallest ID, so it requires even less data to store in certain circumstances. 

## Build process

To compile, use the following line:  

```g++  -g *.cpp -Wall -o ./a.out```

### Additional optional build options
```-DOPTIMIZE_ENCODING``` This build flag will optimize the encoding such that the most used elements out of the column data will correspond to the smallest encoding, producing a more efficient output encoding.

## Execution

After building the project, you can then choose one of the two following functions:

### Encoding

Encoding will take a text input file consisting of column data, and produce an output file consisting of the dictionary and encoded data with format:

```
dictionary:
firstItem
secondItem
...
data:
0
1
0
1
1
...
```

To perform this, use the following:

```./a.out ENCODE small.txt output.txt```  

### Query

Query will enable the user to query an existing encoded file to determine whether a dictionary item exists. If it does, print how many times it exists in the file. This means you must call this on a file output by above. 

To perform this, use the following:

```./a.out QUERY output.txt ircplberj```

## Results

### Testing procedure 

Multiple file sizes were tested for encoding and query. One of the following, `small.txt` can be found in this repository. Each trial was tested multiple times to ensure consistency and an average of all samples was taken. All other computer programs were closed to prevent inconsistent CPU usage.

All testing was performed on: Intel i7-8750h 6 core, 12 logical processor CPU

### Input 1: Encode small.txt

---

## Conclusion

---