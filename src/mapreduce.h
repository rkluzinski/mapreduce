#ifndef MAPREDUCE_H
#define MAPREDUCE_H

// function pointer types used by library functions
typedef void (*Mapper)(char *file_name);
typedef void (*Reducer)(char *key, int partition_number);

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
            Reducer concate, int num_reducers);

/**
 * Writes a key-value pair to a partition
 * Parameters:
 *      key - The key to write to the partition
 *      value - The value to associate to that key
 */
void MR_Emit(char *key, char *value);

/**
 * Assigns a key to a partition using a hash function
 * Uses DJB2 hashing algorithm provided with assignment spec
 * Parameters:
 *      key - The key to hash
 *      num_partitions - The total number of partitions
 */
unsigned long MR_Partition(char *key, int num_partitions);

/**
 * Processes a partition using the reducer function
 * Parameters:
 *      partition_number - The partition to process
 */
void MR_ProcessPartition(int partition_number);

/**
 * Gets the next value for that key from the given partition
 * Parameters:
 *      key - The key to get the value from
 *      partition_number - The partition number to look in
 */
char *MR_GetNext(char *key, int partition_number);

#endif