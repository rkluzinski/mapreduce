#include <stdlib.h>         // for malloc, free and qsort
#include <sys/stat.h>       // for struct stat datatype
#include <unistd.h>         // for stat syscall

#include "mapreduce.h"
#include "threadpool.h"

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
 * Parameters:
 *      ptr1 - Void pointer to the first file object
 *      ptr2 - Void pointer to the second file object
 */
int File_compare(const void *ptr1, const void *ptr2) {
    File_t *file1 = (File_t*) ptr1;
    File_t *file2 = (File_t*) ptr2;
    return file2->size - file1->size;
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
int sort_files(char *filenames[], int count) {
    File_t *file_array = (File_t *) malloc(sizeof(File_t) * count);
    int new_count = 0;
    struct stat statbuf;
    
    for (int i = 0; i < count; i++) {
        // disregard files that cannot be opened
        if (stat(filenames[i], &statbuf) == 0) {
            file_array[new_count].filename = filenames[i];
            file_array[new_count].size = statbuf.st_size;
            new_count++;   
        }
    }

    // sort the array by size using libc qsort
    qsort((void *) file_array, new_count, sizeof(File_t), File_compare);
    
    // copy filenames back to original array
    for (int i = 0; i < new_count; i++) {
        filenames[i] = file_array[i].filename;
    }

    free(file_array);
    
    return new_count;
}

/**
 * Executes MapReduce
 * Parameters:
 *      num_files - The length of the filenames array
 *      filenames - The array of files to processed
 *      map - The map function to apply to each file
 *      num_mappers - The number of mapper threads
 *      concate - The reduce function to apply to each file
 *      num_reducers - The number of reducer threads
 */
void MR_Run(int num_files, char *filenames[], 
            Mapper map, int num_mappers,
            Reducer concate, int num_reducers) {
    
    // sorts filenames and removes files that stat cannot open
    num_files = sort_files(filenames, num_files);
    ThreadPool_t *mapperPool = ThreadPool_create(num_mappers);

    // assign data splits to the mapper threads
    for (int i = 0; i < num_files; i++) {
        ThreadPool_add_work(mapperPool, (thread_func_t) map, filenames[i]);
    }

    ThreadPool_destroy(mapperPool);
}

/**
 * Writes a key-value pair to a partition
 * Parameters:
 *      key - The key to write to the partition
 *      value - The value to associate to that key
 */
void MR_Emit(char *key, char *value) {
    
}

/**
 * Assigns a key to a partition using a hash function
 * Parameters:
 *      key - The key to hash
 *      num_partitions - The total number of partitions
 */
unsigned long MR_Partition(char *key, int num_partitions) {
    unsigned long hash = 5381;
    for (char c = *key; *key != 0; key++) {
        hash = hash * 33 + c;
    }
    return hash % num_partitions;
}

/**
 * Processes a partition using the reducer function
 * Parameters:
 *      partition_number - The partition to process
 */
void MR_ProcessPartition(int partition_number) {

}

/**
 * Gets the next value for that key from the given partition
 * Parameters:
 *      key - The key to get the value from
 *      partition_number - The partition number to look in
 */
char *MR_GetNext(char *key, int partition_number) {

}
