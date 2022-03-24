#include "radix.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int letter_count[256];
static inline void count_letters(letter_count *c, size_t n, const char x[n])
{
    memset(c, 0, sizeof *c);
    for (size_t i = 0; i < n; i++)
    {
        (*c)[(unsigned)x[i]]++;
    }
}

void count_sort(size_t n, const char x[n], char y[n])
{
    letter_count count;
    count_letters(&count, n - 1, x); // don't include sentinel here
    for (int i = 0; i < 256; i++)
    {
        for (int k = 0; k < count[i]; k++)
        {
            (*y++) = (char)i;
        }
    }
    *y = '\0';
}

typedef int buckets[256];
static inline void cumsum(buckets *b, letter_count *c)
{
    int sum = 0;
    for (int i = 0; i < 256; i++)
    {
        (*b)[i] = sum;
        sum += (*c)[i];
    }
}

void bucket_sort(size_t n, const char x[n], const int idx[n], int out[n])
{
    letter_count c;
    count_letters(&c, n, x);
    buckets b;
    cumsum(&b, &c);
    for (size_t i = 0; i < n; i++)
    {
        out[b[(unsigned)x[idx[i]]]++] = idx[i];
    }
}

void lsd_radix_sort(size_t n, const char x[n], int sa[n])
{
}

void msd_radix_sort(size_t n, const char x[n], int sa[n])
{
}
