![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)
![C](https://img.shields.io/badge/c-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white)
![Python](https://img.shields.io/badge/python-3670A0?style=for-the-badge&logo=python&logoColor=ffdd54)

# Hash Table

![image](img/ht.png)

# General information

Lab work on programming in the [ded32](https://github.com/ded32) course on optimizing search in a hash table.

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
- [Optimization of hash table](#optimization-of-hash-table)
    - [Hardware](#hardware)
    - [Profiling and set targets](#profiling-and-set-targets)
    - [Hash function optimization](#hash-function-optimization)
    - [strcmp optimization](#strcmp-optimization)
    - [Hash function optimization. Part 2](#hash-function-optimization-part-2)
# Research of hash functions

1. To investigate hash functions, I load `Leo Tolstoy's text “War and Peace”` into a hash table.

2. I load the number of bucket (`2000`) and the number of elements in it in `data.txt`.

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
> **Avalanche Effect**: A property of a hash function where a small change in the input *(e.g., flipping a single bit)* causes a significant and unpredictable change in the output hash, ideally altering approximately 50% of the bits in a random manner, ensuring better distribution and collision resistance.

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

Hash = XORs the hash with each character and multiplies the result by a prime number (16777619) in each iteration.

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

Hash = adds each character to the hash, shifts it left by 10, XORs with a right shift by 6, and finalizes with additional shifts and XORs.

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

Hash = multiplies each character by a seed, applies a rotation (left shift by 17), multiplies by a constant, and finalizes with a series of XORs and multiplications.

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

**BUT**, in the training framework, for simplicity, we will use **CRC32**. It's not that bad, but it's statistically slightly worse than **JENKINS**. For JENKINS or any other function, we'll have to implement **complex logic in assembly**, which we don't have enough class time for.

# Optimization of hash table

## Hardware

- **Compiler:** `g++ 13.3.0`
- **Processor:** `AMD Ryzen 5 4500U 2.3 GHz (4 GHz in Turbo)`
- **OS:** `Ubuntu 24.04.2 LTS`
- **Profiler:** `valgrind 3.22.0`
- **To visualize profiling data:** `kcachegrind 23.08.5`

## Profiling and set targets

Using `valgrind` to get the program's hot spots:

![without_opt](img/without_opt.png)

Our optimization targets:

### 1. hash_CRC32

Find and implement a way to speed up hash calculation.

### 2. `strcmp`

In our case all words in the hash table are of a certain length, so we can write `strcmp` version for our case.

After this steps we should check the program hot spots again.

## Hash function optimization

I have implemented a CRC32 hash function in `NASM assembly` language using `SSE4.2` processor instructions. The implementation targets the `x86-64` architecture in `Linux`, adhering to `ABI System V`. The function computes the CRC32 byte by byte.

``` nasm
section .text
global hash_crc32

hash_crc32:
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

![optimization1](img/optimization1.png)

The highlighted function is our `CRC32` in `NASM`.
The program has become *`1.7`* times faster, i.e. gain = *`70%`*.

## `strcmp` optimization

From the analysis of hash functions (the LENGTH function), we know that Leo Tolstoy's text contains words of this length:

| Length | Number of words |
|--------|-----------------|
|   18   |        1        |
|   17   |        8        |
|   16   |       21        |
|   15   |       53        |
|  ...   |       ...       |
|    2   |       316       |
|    1   |       58        |

Based on these statistics, we can conclude that the comparison function can be rewritten for our particular case:

1. For strings shorter than 16 bytes, the function uses `SSE` to compare 16 bytes simultaneously, speeding up the process considerably.

2. For strings longer than 16 bytes, the remaining characters are processed byte-by-byte to ensure correct comparison.

<details>
<summary>Click to expand/collapse <strong>new strcmp</strong></summary>

```C
int boost_strcmp (const char *str_1, const char *str_2)
{
    __m128i s1 = _mm_loadu_si128 ((const __m128i*)str_1);   // load 16 bytes into SIMD registers
    __m128i s2 = _mm_loadu_si128 ((const __m128i*)str_2);

    __m128i eq = _mm_cmpeq_epi8 (s1, s2);                   // compare bytes

    __m128i zero  = _mm_setzero_si128();                    // zero vector
    __m128i term1 = _mm_cmpeq_epi8 (s1, zero);              // compare bytes

    int mask_eq    = _mm_movemask_epi8 (eq);                // creates bit masks (1 - equal; 0 - differ)
    int mask_term1 = _mm_movemask_epi8 (term1);             //                   (1 - where '\0'

    if (mask_eq != 0xFFFF)                                  // if not all bytes are equal
    {
        int pos = __builtin_ctz (~mask_eq);                 // find first differing byte | `~` = complement of the original number
        if (mask_term1 & (1 << pos))                        // if diff position in '\0' -> strings are equal
            return 0;

        return (unsigned char)str_1[pos] - (unsigned char)str_2[pos];
    }

    if (mask_term1)                                         // if there is in first 16 bytes '\0' -> equal
        return 0;

    str_1 += 16;
    str_2 += 16;
    while (*str_1 && *str_1 == *str_2)                      // compare remaining bytes one by one
    {
        str_1++;
        str_2++;
    }
    return (unsigned char)*str_1 - (unsigned char)*str_2;
}
```
</details>

Re-profiling result:

![optimization2](img/optimization2.png)

The compiler inlined `boost_strcmp`, so let's see how many instructions the main has changed by.

The program has become *`4`* times faster than the previous version of the program, i.e. gain = *`400%`*.

Also, we notice that the hash counting function remains the hottest one.

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

![optimization3](img/optimization3.png)

The program has become *`1.2`* times faster than the previous version of the program, i.e. gain = *`20%`*.


