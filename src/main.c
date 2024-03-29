/**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */

#include <stdlib.h>
#include "parser.h"
#include "error_code.h"
#include "3ac.h"
#include "memory_manager.h"

int main(int argc, char* argv[]) {
	mem_manager_init();
	il_init();
	Scanner* scanner = scanner_init();
	Parser* parser = parser_init(scanner);

	FILE* in_file = NULL;

	if (argc == 2) {
		in_file = fopen(argv[1], "r");

		if (in_file == NULL) {
			perror("Error");
			mem_manager_free();
			return EXIT_INTERN_ERROR;
		}

		scanner->stream = in_file;
	}

	int ret_code = parse(parser);

	scanner_free(scanner);
	parser_free(parser);

	if (ret_code == EXIT_SUCCESS)
		generate_code();

	il_free();

	if (in_file !=  NULL) {
		fclose(in_file);
	}

	mem_manager_free();

	return ret_code;
}
