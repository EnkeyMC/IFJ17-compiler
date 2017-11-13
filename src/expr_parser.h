#ifndef IFJ17_COMPILER_EXPR_PARSER_H
#define IFJ17_COMPILER_EXPR_PARSER_H

#include "parser.h"

/**
 * Syntax and semantic analysis of expressions
 * @param parser
 * @return error (success) code
 */
int parse_expression(Parser* parser);

#endif //IFJ17_COMPILER_EXPR_PARSER_H
