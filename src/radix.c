#include "radix.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int counts[256];

static inline void count_letters(counts c, size_t n, const char x[n])
{
    memset(c, 0, 256 * sizeof *c);
    for (size_t i = 0; i < n; i++)
    {
        c[(unsigned)x[i]]++;
    }
}

void count_sort(size_t n, const char x[n], char y[n])
{
    counts count;
    count_letters(count, n - 1, x); // don't include sentinel here
    for (int i = 0; i < 256; i++)
    {
        for (int k = 0; k < count[i]; k++)
        {
            (*y++) = (char)i;
        }
    }
    *y = '\0';
}

static inline void cumsum(counts buckets, counts counts)
{
    int sum = 0;
    for (int i = 0; i < 256; i++)
    {
        buckets[i] = sum;
        sum += counts[i];
    }
}

void bucket_sort(size_t n, const char x[n], const int idx[n], int out[n])
{
    counts counts, buckets;
    count_letters(counts, n, x);
    cumsum(buckets, counts);

    for (size_t i = 0; i < n; i++)
    {
        out[buckets[(unsigned)x[idx[i]]]++] = idx[i];
    }
}

static inline unsigned rot_idx(size_t n, const char x[n], int i)
{
    return (unsigned)x[i % n];
}

static void buck_sort(size_t n, const char x[n], int in[n], int out[n], counts buckets, int col)
{
    for (size_t i = 0; i < n; i++)
    {
        out[buckets[rot_idx(n, x, in[i] + col)]++] = in[i];
    }
}

void lsd_radix_sort(size_t n, const char x[n], int sa[n])
{
    // All columns have the same letters, so we can reuse these
    counts counts, orig_buckets, buckets;
    count_letters(counts, n, x);
    cumsum(orig_buckets, counts);

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
        memcpy(buckets, orig_buckets, sizeof buckets);
        buck_sort(n, x, bufs[in_buf], bufs[!in_buf], buckets, i);
        in_buf = !in_buf;
    }

    // After we flip the flag, bufs[in_buf] holds the result of the sort
    memcpy(sa, bufs[in_buf], n * sizeof *sa);

    free(buf0);
    free(buf1);
}

typedef struct stack *stack;
struct stack
{
    int FIXME;
};

static inline void count_letters_col_range(counts counts, size_t n, const char x[n],
                                           int i, int j, int col)
{
    memset(counts, 0, 256 * sizeof *counts);
    for (int k = i; k < j; k++)
    {
        counts[rot_idx(n, x, k + col)]++;
    }
}

static void push_next(counts buckets, int i, int j, int col, stack stack)
{
    int prev = 0;
    for (int k = 1; k < 256; k++)
    {
        if (buckets[k] == buckets[prev])
        {
            continue;
        }
        if (buckets[prev] + 1 < buckets[k])
        {
            // FIXME: push to stack
            printf("<%d>[%d,%d)\n", col + 1, i + buckets[prev], i + buckets[k]);
        }
        prev = k;
    }
    if (buckets[prev] + 1 < buckets[255])
    {
        // FIXME: push to stack
        printf("<%d>[%d,%d)\n", col + 1, i + buckets[prev], j);
    }
}

static inline void swap(int *i, int *j)
{
    int tmp = *i;
    *i = *j;
    *j = tmp;
}

#define BUCKET_(K) (buckets[rot_idx(n, x, K + col)])
#define BUCKET(K) (i + BUCKET_(K))
#define SWAP_INTO_BUCKET(K) swap(&sa[k], &sa[i + BUCKET_(K)++])

static void sort_col(size_t n, const char x[n], int sa[n], int i, int j, int col, stack stack)
{
    counts counts, buckets;
    count_letters_col_range(counts, n, x, i, j, col);
    cumsum(buckets, counts);
    
    // add sub-intervals to the stack so we will recurse later
    push_next(buckets, 0, n, 0, stack);
    
    // then sort the existing interval. Since the MSD sort doesn't have to be a
    // stable sort, we can use an inplace algorithm that simply swaps around the
    // elments in sa[i:j].
    for (int k = i; k < j; k++)
    {
        while (k >= BUCKET(k))
        {
            SWAP_INTO_BUCKET(k);
        }
    }
}

void msd_radix_sort(size_t n, const char x[n], int sa[n])
{
    struct stack stack;
    sort_col(n, x, sa, 0, n, 0, &stack);
}
