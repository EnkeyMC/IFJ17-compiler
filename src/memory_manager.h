/**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */

#ifndef IFJ17_COMPILER_MEMORY_MANAGER_H
#define IFJ17_COMPILER_MEMORY_MANAGER_H

/**
 * Initialize Memory Manager
 */
void mem_manager_init();

/**
 * Free Memory Manager and all allocated memory by mm_malloc
 */
void mem_manager_free();

/**
 * Allocate memory of given size
 * @param size Number of bytes to allocate
 * @return pointer to allocated memory (never NULL)
 */
void* mm_malloc(size_t size);

/**
 * Reallocate memory pointed to by ptr to given size
 * @param ptr Pointer to memory block to reallocate
 * @param size New size of memory block
 * @return pointer to allocated memory (can be different than given ptr, but never NULL)
 */
void* mm_realloc(void* ptr, size_t size);

/**
 * Free given memory block
 * @param ptr Pointer to memory block
 */
void mm_free(void* ptr);

#endif //IFJ17_COMPILER_MEMORY_MANAGER_H
