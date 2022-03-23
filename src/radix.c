#include "radix.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int letter_count[256];
static inline void count_letters(letter_count *c, size_t n, const char x[n])
{
    for (size_t i = 0; i < n - 1; i++)
    {
        (*c)[(unsigned)x[i]]++;
    }
}

void count_sort(size_t n, const char x[n], char y[n])
{
    letter_count count = {0};
    count_letters(&count, n, x);
    for (int i = 0; i < 256; i++)
    {
        for (int k = 0; k < count[i]; k++)
        {
            (*y++) = (char)i;
        }
    }
    *y = '\0';
}

void bucket_sort(size_t n, const char x[n], const int idx[n], int out[n])
{
}

void lsd_radix_sort(size_t n, const char x[n], int sa[n])
{
}

void msd_radix_sort(size_t n, const char x[n], int sa[n])
{
}
