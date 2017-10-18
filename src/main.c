/**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */


#include <stdio.h>
#include <malloc.h>

#include "scanner.h"

int main() {
	Scanner* scanner = (Scanner*) malloc(sizeof(Scanner));
	scanner_init(scanner);

	free(scanner);
	return 0;
}
