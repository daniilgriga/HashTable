![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)
![C](https://img.shields.io/badge/c-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white)
![Python](https://img.shields.io/badge/python-3670A0?style=for-the-badge&logo=python&logoColor=ffdd54)

# Hash Table

![image](img/ht.png)

# General information

Lab work on programming in the [ded32](https://github.com/ded32) course on optimizing search in a hash table.

A **Hash Table** is a data structure that maps keys to values for efficient lookups and insertions.

It uses a hash function to compute an index into an array where the value is stored. **Collisions** (when different keys map to the same index) are resolved using one of techniques like separate chaining - **linked lists** (there is also an **open addressing** technique).

Hash tables offer average-case time **complexity of O(1)** for operations, making them ideal for applications requiring fast access, such as caches, dictionaries, and databases.

**Load Factor** is the ratio of the number of elements in a hash table to its total number of slots (array size).

Typically, a **load factor of `0.7` (`70%`) is ideal—balancing memory usage and performance**.

# Context

- [Research of hash functions](#research-of-hash-functions)
    - [LENGTH](#length)
    - [ASCII](#ascii)
    - [SUM_POS](#sum_pos)
    - [MULT](#mult)
    - [DJB2](#djb2)
    - [SDBM](#sdbm)
    - [CRC32](#crc32)
    - [FNV_1a](#fnv_1a)
    - [JENKINS](#jenkins)
    - [XXHASH](#xxhash)
    - [Conclusion](#conclusion)
- [Hardware](#hardware)
- [Choosing profiler and setting targets](#choosing-profiler-and-setting-targets)
- [Optimization of hash table](#optimization-of-hash-table)
    - [Optimization of input data](#optimization-of-input-data)
    - [strcmp optimization](#strcmp-optimization)
    - [Hash function optimization](#hash-function-optimization)
    - [Hash function optimization. Part 2](#hash-function-optimization-part-2)
    - [Summary](#summary)
- [Conclusions](#conclusions)

# Research of hash functions

1. To investigate hash functions, I load `Leo Tolstoy's text “War and Peace”` into a hash table.

2. I load the number of buckets (`2000`) and the number of elements in it in `data.txt`.

3. **Load factor** is `10.9`. (For training purposes, so that assembly optimizations are more **visible**)

3. I use `histo.py` to build histograms using the data from `data.txt`.

## LENGTH

Hash = length of key.

<details>
<summary>Click to expand/collapse</summary>

```C
static inline uint32_t hash_LENGTH (const char* key)
{
    uint32_t hash = 0;
    while (*key++)
        hash++;

    return hash;
}
```
</details>

![length](img/histoLENGTH.png)

> [!NOTE]
> Histogram is scaled down on the **x-axis** for better visibility.

Terrible hash function, **no comments**.

## ASCII

Hash = sum of ASCII codes of each symbol.

<details>
<summary>Click to expand/collapse</summary>

```C
static inline uint32_t hash_ASCII (const char* key)
{
    uint32_t hash = 0;
    while (*key)
        hash += (uint32_t) ((unsigned char) *key++);

    return hash;
}
```
</details>


![ascii](img/histoASCII.png)

This function is already better, but the **distribution over the buckets is uneven**.

## SUM_POS

Hash = sum of each character multiplied by its position in the string.

<details>
<summary>Click to expand/collapse</summary>

```C
static inline uint32_t hash_SUM_POS (const char* key)
{
    uint32_t hash = 0;
    int pos = 1;
    while (*key)
        hash += (uint32_t) ((unsigned char)*key++ * pos++);

    return hash;
}
```
</details>

![sumpos](img/histoSUM_POS.png)

Improved distribution, but **it's still bad**.

## MULT

Hash = multiply the current hash by 31 and add the next character.

<details>
<summary>Click to expand/collapse</summary>

```C
static inline uint32_t hash_MULT (const char* key)
{
    uint32_t hash = 1;
    while (*key)
    {
        hash *= 31;
        hash += (unsigned char)*key++;
    }

    return hash;
}
```
</details>

![mult](img/histoMULT.png)

Introduces multiplication between iterations, **potentially improving distribution** but increasing computational cost.

## DJB2

Hash = combines a left shift and addition to multiply the hash by 33, then adds the next character in each iteration.

<details>
<summary>Click to expand/collapse</summary>

```C
static inline uint32_t hash_DJB2 (const char* key)
{
    uint32_t hash = 5381;
    unsigned char c = 0;

    while ((c = (unsigned char) *key++) != 0)
        hash = (((hash << 5) + hash) + (uint32_t) c);

    return hash;
}
```
</details>

![djb](img/histoDJB2.png)

Uses a more **effective multiplication factor (33)** and a **non-zero initial value**.

## SDBM

Hash = adding the character to a combination of shifted hash values (left by 6 and 16 bits) and subtracting the original hash.

<details>
<summary>Click to expand/collapse</summary>

```C
static inline uint32_t hash_SDBM (const char* key)
{
    uint32_t hash = 0;
    int c = 0;
    while ((c = *key++))
        hash = (uint32_t) c + (hash << 6) + (hash << 16) - hash;

    return hash;
}
```
</details>

![sdbm](img/histoSDBM.png)

Employs more complex bit shifts and subtraction, which can enhance **the avalanche effect** but may be slower due to additional operations.

> [!NOTE]
> **Avalanche Effect**: A property of a hash function where a small change in the input *(e.g., flipping a single bit)* causes a significant and unpredictable change in the output hash.

## CRC32

Hash = using a lookup table and computing a cyclic redundancy check by XOR'ing the hash with each character and shifting the result (poly = `0x1EDC6F41`).

<details>
<summary>Click to expand/collapse</summary>

```C
static inline uint32_t hash_CRC32 (const char* key)
{
    uint32_t hash = 0xFFFFFFFF;
    while (*key)
    {
        uint8_t byte = (unsigned char)*key++;
        hash = (hash >> 8) ^ crc32_table[(hash ^ byte) & 0xFF];
    }

    return hash ^ 0xFFFFFFFF;
}
```
</details>

![crc32](img/histoCRC32.png)

**Leverages a precomputed table** for **better distribution** and error detection, though **it is slower** due to table lookups and bitwise operations.

## FNV_1a

Hash = XORs the hash with each character and multiplies the result by a prime number (`16777619`) in each iteration.

<details>
<summary>Click to expand/collapse</summary>

```C
static inline uint32_t hash_FNV_1a (const char* key)
{
    uint32_t hash = 2166136261u;
    while (*key)
    {
        hash ^= (unsigned char)*key++;
        hash *= 16777619u;
    }

    return hash;
}
```
</details>

![fnv](img/histoFNV_1a.png)

Avoids table lookups, making it **faster** while maintaining good distribution through the use of a prime multiplier.

## JENKINS

Hash = adds each character to the hash, shifts it left by `10`, XORs with a right shift by `6`, and finalizes with additional shifts and XORs.

<details>
<summary>Click to expand/collapse</summary>

```C
static inline uint32_t hash_JENKINS (const char* key)
{
    uint32_t hash = 0;
    while (*key)
    {
        hash += (unsigned char)*key++;
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}
```
</details>

![jenkins](img/histoJENKINS.png)

Incorporates **more aggressive** bit mixing with shifts and XORs, improving the **avalanche effect** and **reducing collisions** at the cost of extra operations.

## XXHASH

Hash = multiplies each character by a seed, applies a rotation (left shift by `17`), multiplies by a constant, and finalizes with a series of `XORs` and multiplications.

<details>
<summary>Click to expand/collapse</summary>

```C
static inline uint32_t hash_XXHASH (const char* key)
{
    uint32_t hash = 0;
    uint32_t seed = 0x1e3b2d4f;
    while (*key)
    {
        uint32_t c = (unsigned char)*key++;
        hash += c * seed;
        hash = (hash << 17) | (hash >> 15);
        hash *= 0x9e3779b1;
    }

    hash ^= hash >> 15;
    hash *= 0x85ebca77;
    hash ^= hash >> 13;
    hash *= 0xc2b2ae3d;
    hash ^= hash >> 16;

    return hash;
}
```
</details>

![xxhash](img/histoXXHASH.png)

Uses a seed and **more complex finalization**.

## Conclusion

Based on the **calculated variance**, the **JENKINS** hash function demonstrates the **lowest variance** among the provided list, indicating a **more uniform distribution** of hash values, which makes it one of the best choices for **minimizing collisions** and ensuring **efficient performance**.

**BUT**, **CRC32** matches the hardware better (it exists as a separate assembler instruction).  **CRC32** is statistically slightly worse than **JENKINS**, but they are very similar. For JENKINS or any other function, we would have to implement **complex logic in assembly language**, which we don't have enough instructional time for.

# Hardware

- **Compiler:** `g++ 13.3.0`
- **Processor:** `AMD Ryzen 5 4500U 2.3 GHz (4 GHz in Turbo)`
- **OS:** `Ubuntu 24.04.2 LTS`

# Choosing profiler and setting targets

For the first profiling of the base version of the program, we will use `valgrind 3.22.0` (to visualize the profiling data: `kcachegrind 23.08.5`) and `perf 6.11.11` (for convenience, we will use the `perf` `hotspot 1.3.0` graphical handler).

Our goal is to determine which profiler is better for our particular program.

Using `valgrind` we get the program's hot spots:

![without_opt](img/without_opt_val.png)

And using `perf`:

![perf_without](img/without_opt_perf.png)

`Perf` did a better job of determining which functions were hot in our program, while `valgrind` determined all function calls from **main**.

> [!NOTE]
> It is also worth considering in the comparison that `perf` uses **hardware counters** to measure CPU cycles, including kernel activity. `valgrind (callgrind)`, on the other hand, **emulates execution to count instructions executed (IR)**, offering detailed code-level analysis, but with significant slowdown and limited kernel visibility.

For this reason, **we will use `perf` for further profiling**.

Our optimization **targets**:

### 1. `strcmp`

In our case all words in the hash table are of a certain length, so we can write `strcmp` version for our case.

### 2. `hash_CRC32`

Find and implement a way to speed up hash calculation.

After this steps we should check the program hot spots again.

# Optimization of hash table

## Optimization of input data

To make it easier to read words from the literature, both for filling the hash table and for testing the search function, we need to **align words by 32 bytes** (this will also be useful for **AVX2 instructions**):

<table>
    <thead>
        <tr>
            <th rowspan=1 style="text-align: center"><img src="img/b_WaP.png"  alt="War and Peace.bin" width="500"></th>
            <th colspan=1 style="text-align: center"><img src="img/b_F451.png" alt="Fahrenhait 451.bi" width="500"></th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td style="text-align: center"><b>War and Peace.bin</b> (for filling Hash Table)</td>
            <td style="text-align: center"><b>Fahrenhait 451.bin</b> (for search func test) </td>
        </tr>
    </tbody>
</table>

I did it with [get_binary.py](https://github.com/daniilgriga/HashTable/blob/main/test/get_binary.py).

## `strcmp` optimization

From the analysis of hash functions (especially the LENGTH function), we know that Leo Tolstoy's text contains words of this length:

| Length | Number of words |
|--------|-----------------|
|   19   |        0        |
|   18   |        1        |
|   17   |        8        |
|   16   |       21        |
|   15   |       53        |
|  ...   |       ...       |
|    2   |       316       |
|    1   |       58        |

Based on these statistics, we can conclude that the comparison function can be rewritten for our particular case:

1. There are no words longer than 32 characters in the text, so we can use **YMM-registers** to store words.

2. We don't need the full functionality of the `strcmp`, we just need to know whether **the words matched or not**.

```C
#define MASK = 0xFFFFFFFF

int boost_strcmp (const char* str_1, const char* str_2)
{
    __m256i string_1 = _mm256_loadu_si256 ((const __m256i*)(str_1));
    __m256i string_2 = _mm256_loadu_si256 ((const __m256i*)(str_2));

    __m256i mask_avx = _mm256_cmpeq_epi8 (string_1, string_2);

    if (_mm256_movemask_epi8 (mask_avx) == MASK)
        return 0;

    return 1;
}

#undef MASK
```

Re-profiling result:

![optimization_1](img/optimization_1.png)

The program has become *`1.41`* times faster than the previous version of the program, i.e. gain = *`41%`*.

Linear search is still the program's hottest spot, but **you can't do better than Intrinsics does**.

## Hash function optimization

I have implemented a CRC32 hash function in `NASM assembly` language using `SSE4.2` processor instructions. The implementation targets the `x86-64` architecture in `Linux`, adhering to `ABI System V`. The function computes the CRC32 byte by byte.

``` nasm
section .text
global hash_CRC32_nasm

hash_CRC32_nasm:
    mov eax, 0xFFFFFFFF
    xor rcx, rcx

.lesgo:
    movzx edx, byte [rdi + rcx]
    test dl, dl
    jz .end

    crc32 eax, dl
    inc rcx
    jmp .lesgo

.end:
    xor eax, 0xFFFFFFFF
    ret
```

Re-profiling result:

![optimization_2](img/optimization_2.png)

The highlighted function is our `CRC32` in `NASM`.
The program has become *`1.25`* times faster, i.e. gain = *`25%`*.

## Hash function optimization. Part 2

For educational purposes, I needed to familiarize myself with **inline assembler**, so I used it to write another version of CRC32:

```C
static inline uint32_t hash_CRC32_inline (const char* key)
{
    uint32_t hash = 0xFFFFFFFF;

    while (*key)
    {
        uint8_t byte = (uint8_t)(*key++);
        __asm__ volatile ("crc32b %1, %0" : "+r"(hash) : "r"(byte));
    }
    return hash ^ 0xFFFFFFFF;
}
```

Re-profiling result:

![optimization_3](img/optimization_3.png)

The program has become *`1.15`* times faster than the previous version of the program, i.e. gain = *`15%`*.

The hot spot of the program remains the **search function**, but that's okay because **it's the main functionality of the program**.

Also from the program profile we can see that **most of the load is taken by the linear search**.

## Summary

The optimizations we did made sense.

They made our program much **faster** and **kept our code readable**.

<table>
    <thead>
        <tr>
            <th rowspan=2>Optimization</th>
            <th colspan=2 style="text-align: center">The whole program</th>
            <th colspan=2 style="text-align: center">Search function</th>
        </tr>
        <tr>
            <th style="text-align: center">Ir * 10^10</th>
            <th style="text-align: center">gain in % than last</th>
            <th style="text-align: center">Ir * 10^10</th>
            <th style="text-align: center">gain in % than last</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td><b>base</b></td>
            <td style="text-align: center">4.458</td>
            <td style="text-align: center">-</td>
            <td style="text-align: center">4.445</td>
            <td style="text-align: center">-</td>
        <tr>
        </tr>
            <td><b>hash func (1)</b></td>
            <td style="text-align: center">3.155</td>
            <td style="text-align: center">41</td>
            <td style="text-align: center">3.144</td>
            <td style="text-align: center">41</td>
        <tr>
        </tr>
            <td><b>strcmp</b></td>
            <td style="text-align: center">2.528</td>
            <td style="text-align: center">25</td>
            <td style="text-align: center">2.517</td>
            <td style="text-align: center">25</td>
        <tr>
        </tr>
            <td><b>hash func (2)</b></td>
            <td style="text-align: center">2.192</td>
            <td style="text-align: center">15</td>
            <td style="text-align: center">2.182</td>
            <td style="text-align: center">15</td>
        </tr>
    </tbody>
</table>

# Conclusions

I conducted a performance analysis of hash functions and implemented the following optimizations for a hash table, using Callgrind and Perf for profiling:
 - **Analysis of Hash Functions:** Evaluated different hash functions to identify bottlenecks in the hash table.

 - **Profiling Tools:** Used `сallgrind` and `perf` to get hardware metrics (instructions, cycles) and compared them in the case of working with my program

 - **Optimization 1:** Replaced the `CRC32 hash function` with a custom NASM implementation for faster computation.

 - **Optimization 2:** Wrote a custom `strcmp` using intrinsics to improve string comparison performance.

 - **Optimization 3:** Utilized inline assembly for the `CRC32 hash function` to further enhance speed.

 - **Final program acceleration:** program performance increased by over **`103%`**
