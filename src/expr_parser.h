/**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */

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
