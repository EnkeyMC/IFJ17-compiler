/**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */

#include <cstdio>
#include "gtest/gtest.h"
#include "grammar.c"
#include "parser.c"

class ParserTestFixture : public ::testing::Test {
protected:

	Scanner* scanner;
	FILE* test_file;

	virtual void SetUp() {
		scanner = scanner_init();
	}

	virtual void TearDown() {
		fclose(test_file);
		scanner_free(scanner);
	}

	void SetInputFile(const char *file) {
		test_file = fopen(file, "r");

		scanner->stream = test_file;

		if (test_file == NULL)
			perror("Error");
	}
};


TEST_F(ParserTestFixture, SuccEmpty) {
	SetInputFile("test_files/empty.fbc");

	EXPECT_EQ(parse(scanner), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccFactorialIt) {
	SetInputFile("test_files/factorial_it.fbc");

	EXPECT_EQ(parse(scanner), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccFactorialRecur) {
	SetInputFile("test_files/factorial_recur.fbc");

	EXPECT_EQ(parse(scanner), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccStrings) {
	SetInputFile("test_files/strings.fbc");

	EXPECT_EQ(parse(scanner), EXIT_SUCCESS);
}
