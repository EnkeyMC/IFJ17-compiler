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

int main(int argc, char* argv[]) {
	Scanner* scanner = scanner_init();
	if (scanner == NULL)
		return EXIT_INTERN_ERROR;

	Parser* parser = parser_init(scanner);
	if (parser == NULL) {
		scanner_free(scanner);
		return EXIT_INTERN_ERROR;
	}

	FILE* in_file = NULL;

	if (argc == 2) {
		in_file = fopen(argv[1], "r");

		if (in_file == NULL) {
			perror("Error");
			scanner_free(scanner);
			parser_free(parser);
			return EXIT_INTERN_ERROR;
		}

		scanner->stream = in_file;
	}

	int ret_code = parse(parser);

	scanner_free(scanner);
	parser_free(parser);

	if (in_file !=  NULL) {
		fclose(in_file);
	}

	return ret_code;
}
