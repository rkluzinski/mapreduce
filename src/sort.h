#ifndef SORT_H
#define SORT_H

#include "vector.h"

/**
 * Sorts filename in-place by size
 * Sorts the filenames array inplace
 * Files that do not exists are removed
 * Parameters:
 *      filenames - The list of filenames to be sorted
 *      count - The number of filesnames in the array
 * Returns:
 *      The number of filenames in the sorted array
 */
int sort_by_size(char *filenames[], int count);

#endif