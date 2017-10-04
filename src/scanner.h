/**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */

#ifndef IFJ17_COMPILER_SCANNER_H
#define IFJ17_COMPILER_SCANNER_H

#include <stdio.h>

typedef struct {
    int id;
    void* attr;
} token_t;

void set_input_stream(FILE* stream);

/**
 * Get next token
 * @return token
 */
token_t* get_token();

#endif //IFJ17_COMPILER_SCANNER_H
