/**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */

#include "parser.h"
#include "error_code.h"

int main() {
	Scanner* scanner = scanner_init();
	if (scanner == NULL)
		return EXIT_INTERN_ERROR;

	int ret_code = parse(scanner);

	scanner_free(scanner);
	return ret_code;
}
