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

/**
 * Parser object structure
 */
typedef struct parser_t {
    Scanner* scanner;  /// Input scanner
    Stack* dtree_stack;  /// Stack for simulating syntax derivation tree
} Parser;

/**
 * Init parser
 * @param scanner initialized Scanner object
 * @return new parser object, NULL on allocation error
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
