#include <cstdio>
#include "gtest/gtest.h"
#include "expr_parser.c"

class ExprParserTestFixture : public ::testing::Test {
protected:

	Scanner* scanner;
	Parser* parser;
	FILE* test_file;

	virtual void SetUp() {
		scanner = scanner_init();
		parser = parser_init(scanner);
	}

	virtual void TearDown() {
		fclose(test_file);
		scanner_free(scanner);
		parser_free(parser);
	}

	void SetInputFile(const char *file) {
		test_file = fopen(file, "r");

		scanner->stream = test_file;

		if (test_file == NULL)
			perror("Error");
	}
};

TEST_F(ExprParserTestFixture, ExprTest01) {
	SetInputFile("test_files/expressions/01.test");

	EXPECT_EQ(parse_expression(parser), EXIT_SUCCESS);
}

TEST_F(ExprParserTestFixture, ExprTest02) {
	SetInputFile("test_files/expressions/02.test");

	EXPECT_EQ(parse_expression(parser), EXIT_SUCCESS);
}

TEST_F(ExprParserTestFixture, ExprTest03) {
	SetInputFile("test_files/expressions/03.test");

	EXPECT_EQ(parse_expression(parser), EXIT_SUCCESS);
}

TEST_F(ExprParserTestFixture, ExprTest04) {
	SetInputFile("test_files/expressions/04.test");

	EXPECT_EQ(parse_expression(parser), EXIT_SUCCESS);
}

TEST_F(ExprParserTestFixture, ExprTest05) {
	SetInputFile("test_files/expressions/05.test");

	EXPECT_EQ(parse_expression(parser), EXIT_SUCCESS);
}

TEST_F(ExprParserTestFixture, ExprTest06) {
	SetInputFile("test_files/expressions/06.test");

	EXPECT_EQ(parse_expression(parser), EXIT_SUCCESS);
}

TEST_F(ExprParserTestFixture, ExprTest07) {
	SetInputFile("test_files/expressions/07.test");

	EXPECT_EQ(parse_expression(parser), EXIT_SYNTAX_ERROR);
}
