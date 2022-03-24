#include "radix.h"
#include <assert.h>
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

// by putting buckets in a struct we get copy semantics
typedef struct { int ets[256]; } buck; // buck.ets

static inline void cumsum(buck *buck, letter_count *c)
{
    int sum = 0;
    for (int i = 0; i < 256; i++)
    {
        buck->ets[i] = sum;
        sum += (*c)[i];
    }
}

void bucket_sort(size_t n, const char x[n], const int idx[n], int out[n])
{
    letter_count c;
    count_letters(&c, n, x);
    buck buck;
    cumsum(&buck, &c);
    for (size_t i = 0; i < n; i++)
    {
        out[buck.ets[(unsigned)x[idx[i]]]++] = idx[i];
    }
}

static inline unsigned rot_idx(size_t n, const char x[n], int i)
{
    return (unsigned)x[i % n];
}

// buckets is passed as value so we don't change the original. This is why we wrap
// the buckets array in a struct. If we used an array it would pass by pointer and
// we would be changing the original.
static void buck_sort(size_t n, const char x[n], int in[n], int out[n], buck buck, int col)
{
    for (size_t i = 0; i < n; i++)
    {
        out[buck.ets[rot_idx(n, x, in[i] + col)]++] = in[i];
    }
}

void lsd_radix_sort(size_t n, const char x[n], int sa[n])
{
    // All columns have the same letters, so we can reuse these
    letter_count c;
    count_letters(&c, n, x);
    buck buck;
    cumsum(&buck, &c);

    // Buffers for handling input/output in bucket sorts.
    // We need them because we cannot sort in-place with a stable
    // bucket sort.
    int *buf0 = malloc(n * sizeof *buf0);
    int *buf1 = malloc(n * sizeof *buf1);
    assert(buf0 && buf1);
    int in_buf = 0;
    int *bufs[] = {buf0, buf1};

    for (int i = 0; i < n; i++)
    {
        buf0[i] = i;
    }

    for (int i = n - 1; i >= 0; i--)
    {
        buck_sort(n, x, bufs[in_buf], bufs[!in_buf], buck, i);
        in_buf = !in_buf;
    }

    // After we flip the flag, bufs[in_buf] holds the result of the sort
    memcpy(sa, bufs[in_buf], n * sizeof *sa);

    free(buf0);
    free(buf1);
}

void msd_radix_sort(size_t n, const char x[n], int sa[n])
{
}
