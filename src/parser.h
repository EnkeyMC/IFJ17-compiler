/**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */


#ifndef IFJ17_COMPILER_PARSER_H
#define IFJ17_COMPILER_PARSER_H

#include "scanner.h"
#include "stack.h"
#include "symtable.h"

/**
 * Parser object structure
 */
typedef struct parser_t {
    Scanner* scanner;  /// Input scanner
    Stack* dtree_stack;  /// Stack for simulating syntax derivation tree
    DLList* sem_an_stack;  /// Stack of semantic analyzers
    DLList* sym_tab_stack;  /// Stack of local symbol tables
    HashTable* sym_tab_global;  /// Global symbol table
    HashTable* sym_tab_functions;  /// Functions symbol table
    DLList* il_override;  /// If this variable is not NULL get_current_il_list will return it
    bool static_var_decl;  /// Indicates if static variable is currently being defined
	bool step_found; /// Indicates whether for loop has explicitly specified step value
} Parser;

/**
 * Init parser
 * @param scanner initialized Scanner object
 * @return new parser object
 */
Parser* parser_init(Scanner* scanner);

/**
 * Free parser
 * @param parser
 */
void parser_free(Parser* parser);

/**
 * Syntax and semantic analysis of input stream given by scanner
 * @param parser
 * @return error (success) code
 */
int parse(Parser* parser);

#endif //IFJ17_COMPILER_PARSER_H
