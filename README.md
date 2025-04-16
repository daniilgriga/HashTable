# Hash Table

![image](img/ht.png)

## General information

Lab work on programming in the [ded32](https://github.com/ded32) course on optimizing search in a hash table.

## Context

 - [Research of hash functions](#research)
    - [LENGTH](#length)
    - [ASCII](#ascii)
    - [SUM_POS](#sumpos)
    - [MULT](#mult)
    - [DJB2](#djb)
    - [SDBM](#sdbm)
    - [CRC32](#crc)
    - [FNV_1a](#fnv)
    - [JENKINS](#jenkins)
    - [XXHASH](#xxhash)
 - [Optimization of hash table](#optimization)

 ## Research of hash functions

1. To investigate hash functions, I load `Leo Tolstoy's text “War and Peace”` into a hash table.

2. I load the number of bucket and the number of elements in it in `data.txt`.

3. I use histo.py to build histograms using the data from `data.txt`.
