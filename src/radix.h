#ifndef RADIX_H
#define RADIX_H

#include <stdlib.h>

// In all these functions we assume that x has length n *including* the
// terminal '\0' sentinel. That way, we can manipulate it both as an
// array and a standard C string.
// For count_sort, we add the '\0' terminator to y so it is a valid
// string. For the suffix array constructors, the suffix array and x have
// the same length, because the sentinel is already included in the
// length n.

void count_sort(size_t n, const char x[n], char y[n]);
void bucket_sort(size_t n, const char x[n], const int idx[n], int out[n]);
void lsd_radix_sort(size_t n, const char x[n], int sa[n]);
void msd_radix_sort(size_t n, const char x[n], int sa[n]);

#endif // RADIX_H
