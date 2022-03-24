#include "radix.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static void check_count_sort_output(const char *input, const char *expected)
{
    size_t n = strlen(input) + 1;
    char out[n];
    count_sort(n, input, out);
    assert(strcmp(expected, out) == 0);
}

static void check_count_sort(void)
{
    check_count_sort_output("abaab", "aaabb");
    check_count_sort_output("mississippi", "iiiimppssss");
    check_count_sort_output("", "");
}

static void check_bucket_sort_output(const char *x, int idx[], int expected[])
{
    size_t n = strlen(x) + 1;
    int out[n];
    bucket_sort(n, x, idx, out);
    for (size_t i = 0; i < n; i++)
    {
        assert(out[i] == expected[i]);
    }
}

static void check_bucket_sort(void)
{
    check_bucket_sort_output(
        "abaab", (int[]){0, 1, 2, 3, 4, 5}, (int[]){5, 0, 2, 3, 1, 4});
    check_bucket_sort_output(
        "abaab", (int[]){5, 4, 3, 2, 1, 0}, (int[]){5, 3, 2, 0, 4, 1});
    check_bucket_sort_output(
        "mississippi",
        (int[]){0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11},
        (int[]){11, 1, 4, 7, 10, 0, 8, 9, 2, 3, 5, 6});
    check_bucket_sort_output(
        "mississippi",
        (int[]){11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0},
        (int[]){11, 10, 7, 4, 1, 0, 9, 8, 6, 5, 3, 2});
}

// Construct random nul-terminated string of length n (including sentinel)
static void random_string(size_t n, char x[n], const char *alpha, int alpha_size)
{
    for (size_t i = 0; i < n - 1; i++)
    {
        x[i] = alpha[rand() % alpha_size];
    }
    x[n - 1] = '\0';
}

static void check_suffixes(size_t n, const char x[n], int sa[n])
{
    for (size_t i = 1; i < n; i++)
    {
        assert(strcmp(x + sa[i - 1], x + sa[i]) < 0);
    }
}

static void check_lsd_radix_sort(void)
{
    size_t n = 10;
    char x[n];
    int sa[n];
    for (int k = 0; k < 5; k++)
    {
        random_string(n, x, "acgt", 4);
        lsd_radix_sort(n, x, sa);
        check_suffixes(n, x, sa);
    }
}

static void check_msd_radix_sort(void)
{
    size_t n = 10;
    char x[n];
    int sa[n];
    for (int k = 0; k < 5; k++)
    {
        random_string(n, x, "acgt", 4);
        lsd_radix_sort(n, x, sa);
        check_suffixes(n, x, sa);
    }
}

int main(void)
{
    check_count_sort();
    check_bucket_sort();
    check_lsd_radix_sort();
    // FIXME check_msd_radix_sort();
    return 0;
}
