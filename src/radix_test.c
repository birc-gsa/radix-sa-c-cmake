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

int main(void)
{
    check_count_sort();
    return 0;
}
