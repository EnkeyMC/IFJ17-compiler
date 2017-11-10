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

TEST_F(ParserTestFixture, SuccBasic02) {
	SetInputFile("test_files/syntax/basic/02.code");

	EXPECT_EQ(parse(scanner), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccBasic03) {
	SetInputFile("test_files/syntax/basic/03.code");

	EXPECT_EQ(parse(scanner), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccBasic05) {
	SetInputFile("test_files/syntax/basic/05.code");

	EXPECT_EQ(parse(scanner), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccBasic06) {
	SetInputFile("test_files/syntax/basic/06.code");

	EXPECT_EQ(parse(scanner), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccBasic08) {
	SetInputFile("test_files/syntax/basic/08.code");

	EXPECT_EQ(parse(scanner), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccBasic09) {
	SetInputFile("test_files/syntax/basic/09.code");

	EXPECT_EQ(parse(scanner), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccBasic10) {
	SetInputFile("test_files/syntax/basic/10.code");

	EXPECT_EQ(parse(scanner), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccBasic18) {
	SetInputFile("test_files/syntax/basic/18.code");

	EXPECT_EQ(parse(scanner), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccBasic19) {
	SetInputFile("test_files/syntax/basic/19.code");

	EXPECT_EQ(parse(scanner), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccBasic20) {
	SetInputFile("test_files/syntax/basic/20_modified.code");

	EXPECT_EQ(parse(scanner), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccFunctions01) {
	SetInputFile("test_files/syntax/functions/01.code");

	EXPECT_EQ(parse(scanner), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccFunctions02) {
	SetInputFile("test_files/syntax/functions/02.code");

	EXPECT_EQ(parse(scanner), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccFunctions04) {
	SetInputFile("test_files/syntax/functions/04.code");

	EXPECT_EQ(parse(scanner), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccFunctions05) {
	SetInputFile("test_files/syntax/functions/05.code");

	EXPECT_EQ(parse(scanner), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccFunctions17) {
	SetInputFile("test_files/syntax/functions/17.code");

	EXPECT_EQ(parse(scanner), EXIT_SYNTAX_ERROR);
}

TEST_F(ParserTestFixture, SuccStatements06) {
	SetInputFile("test_files/syntax/statements/06_modified.code");

	EXPECT_EQ(parse(scanner), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccStatements07) {
	SetInputFile("test_files/syntax/statements/07.code");

	EXPECT_EQ(parse(scanner), EXIT_SYNTAX_ERROR);
}

TEST_F(ParserTestFixture, SuccBase01) {
	SetInputFile("test_files/syntax/base/01.code");

	EXPECT_EQ(parse(scanner), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccBase02) {
	SetInputFile("test_files/syntax/base/02.code");

	EXPECT_EQ(parse(scanner), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccBase03) {
	SetInputFile("test_files/syntax/base/03.code");

	EXPECT_EQ(parse(scanner), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccBase04) {
	SetInputFile("test_files/syntax/base/04.code");

	EXPECT_EQ(parse(scanner), EXIT_SUCCESS);
}
