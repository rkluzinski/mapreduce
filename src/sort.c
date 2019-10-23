#include <sys/stat.h>   // for struct stat datatype
#include <unistd.h>     // for stat syscall
#include "stdlib.h"     // for qsort
#include "sort.h"

/**
 * Stores the name and size of file
 * Used for sorting files
 */
typedef struct {
    char *filename;
    int size;
} File_t;


/**
 * Compares two file objects for sorting in descending order
 * Used with libc qsort
 * Parameters:
 *      l - Void pointer to the first file object
 *      r - Void pointer to the second file object
 */
int File_compare(const void *l, const void *r) {
    File_t *l_file = (File_t*) l;
    File_t *r_file = (File_t*) r;
    return r_file->size - l_file->size;
}

/**
 * Sorts a list of files by the
 * Sorts the filenames array inplace
 * Filenames that cannot be opened by stat are disregarded
 * Parameters:
 *      filenames - The list of filenames to be sorted
 *      count - The number of filesnames in the array
 * Returns:
 *      The number of filenames in the sorted array
 */
int sort_by_size(char *filenames[], int count) {
    int new_count = 0;
    File_t *file_array = (File_t *) malloc(sizeof(File_t) * count);
    
    struct stat statbuf;
    for (int i = 0; i < count; i++) {
        // disregard files that cannot be opened
        if (stat(filenames[i], &statbuf) == 0) {
            // store file name and file size
            file_array[new_count].filename = filenames[i];
            file_array[new_count].size = statbuf.st_size;
            new_count++;   
        }
    }

    // sort the array by size using libc qsort
    qsort((void *) file_array, new_count, sizeof(File_t), File_compare);
    
    // push filenames to array
    for (int i = 0; i < new_count; i++) {
        filenames[i] = file_array[i].filename;
    }

    free(file_array);
    
    return new_count;
}