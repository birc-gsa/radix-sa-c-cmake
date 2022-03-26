#include "radix.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
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
    assert(n > 0);
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

// clang-format off
typedef struct stack *stack;
struct stack
{
    size_t len, used;
    struct frame { int i, j, col; } data[];
};
// clang-format on

const size_t init_stack_size = 4;
static stack new_stack(void)
{
    stack stack = malloc(offsetof(struct stack, data) + init_stack_size * sizeof stack->data[0]);
    assert(stack);
    stack->len = init_stack_size;
    stack->used = 0;
    return stack;
}

static inline bool is_empty(stack *stack)
{
    return (*stack)->used == 0;
}

static void push(stack *stack, int i, int j, int col)
{
    if ((*stack)->used == (*stack)->len)
    {
        (*stack)->len *= 2;
        *stack = realloc(stack, offsetof(struct stack, data) + (*stack)->len * sizeof(*stack)->data[0]);
        assert(*stack);
    }
    (*stack)->data[(*stack)->used++] = (struct frame){.i = i, .j = j, .col = col};
}

static void pop(stack *stack, int *i, int *j, int *col)
{
    (*stack)->used--;
    *i = (*stack)->data[(*stack)->used].i;
    *j = (*stack)->data[(*stack)->used].j;
    *col = (*stack)->data[(*stack)->used].col;
}

static inline void
count_letters_col_range(counts counts,
                        size_t n, const char x[n], int sa[n],
                        int i, int j, int col)
{
    memset(counts, 0, 256 * sizeof *counts);
    for (int k = i; k < j; k++)
    {
        counts[rot_idx(n, x, sa[k] + col)]++;
    }
}

static void push_next(counts buckets, int i, int j, int col, stack *stack)
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
            push(stack, i + buckets[prev], i + buckets[k], col + 1);
        }
        prev = k;
    }
    if (buckets[prev] + 1 < buckets[255])
    {
        push(stack, i + buckets[prev], j, col + 1);
    }
}

static inline void swap(int *i, int *j)
{
    int tmp = *i;
    *i = *j;
    *j = tmp;
}

#define BUCKET_(K) (buckets[rot_idx(n, x, sa[K] + col)])
#define BUCKET(K) (i + BUCKET_(K))
#define SWAP_INTO_BUCKET(K) swap(&sa[k], &sa[i + BUCKET_(K)++])

static void sort_col(size_t n, const char x[n], int sa[n], int i, int j, int col, stack *stack)
{
    counts counts, buckets, orig;
    count_letters_col_range(counts, n, x, sa, i, j, col);
    cumsum(buckets, counts);

    // add sub-intervals to the stack so we will recurse later
    push_next(buckets, i, j, col, stack);

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
    for (int i = 0; i < n; i++)
    {
        sa[i] = i;
    }

    stack stack = new_stack();
    push(&stack, 0, n, 0);
    while (!is_empty(&stack))
    {
        int i, j, col;
        pop(&stack, &i, &j, &col);
        sort_col(n, x, sa, i, j, col, &stack);
    }
    free(stack);
}
