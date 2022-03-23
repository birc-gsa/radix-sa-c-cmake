#ifndef RADIX_H
#define RADIX_H

#include <stdlib.h>

// In all these functions we assume that x has length n *including* the
// terminal '\0' sentinel. That way, we can manipulate it both as an
// array and a standard C string.

// For count_sort, we add the '\0' terminator to y so it is a valid
// string. For this function to work as one would expect, we should not
// include the terminal sentinel from x in the sorting (and get \0 at
// the beginning of y). This means that for this function (alone) we
// should only look at the first n-1 letters in x.

// For bucket_sort we do include the sentinel in x, it is the one that sits
// at index n-1. Here, we cannot get into conflict with C's interpretation
// of what a string is.

// For the suffix array constructors, the suffix array and x have
// the same length, because the sentinel is already included in the
// length n.

void count_sort(size_t n, const char x[n], char y[n]);
void bucket_sort(size_t n, const char x[n], const int idx[n], int out[n]);
void lsd_radix_sort(size_t n, const char x[n], int sa[n]);
void msd_radix_sort(size_t n, const char x[n], int sa[n]);

#endif // RADIX_H
