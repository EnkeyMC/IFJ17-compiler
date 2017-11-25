/**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */

#ifndef IFJ17_COMPILER_SPARSE_TABLE_H
#define IFJ17_COMPILER_SPARSE_TABLE_H

#include <stdbool.h>

/**
 * Sparse table list item
 */
typedef struct sparse_table_item_t {
    unsigned int column;  /// Column index
    int value;  /// Item value
    struct sparse_table_item_t* next;  /// Next item in list
} SparseTableListItem;

/**
 * Sparse table object structure
 */
typedef struct {
    SparseTableListItem** table;  /// Array of lists
    unsigned int nrows;  /// Number of rows
    unsigned int ncols;  /// Number of columns
    int dominant_value;  /// Dominant sparse table value
} SparseTable;

/**
 * Initialize sparse table object
 * @param nrows number of rows in table
 * @param ncols number of columns in table
 * @param dominant_value dominant value of the table
 * @return new sparse table object, NULL on allocation error
 */
SparseTable* sparse_table_init(unsigned int nrows, unsigned int ncols, int dominant_value);

/**
 * Free sparse table object
 * @param stab valid pointer to SparseTable object
 */
void sparse_table_free(SparseTable* stab);

/**
 * Get value from sparse table
 * @param row row index
 * @param column column index
 * @return value on given row and column, dominant value if row or column is out of bounds
 */
int sparse_table_get(SparseTable* stab, unsigned int row, unsigned int column);

/**
 * Set value to given row and column
 * @param stab valid pointer to SparseTable
 * @param row row index
 * @param column column index
 * @param value value to set
 * @return true on success, false otherwise (allocation error or row or column out of bounds)
 */
bool sparse_table_set(SparseTable* stab, unsigned int row, unsigned int column, int value);

#endif //IFJ17_COMPILER_SPARSE_TABLE_H
