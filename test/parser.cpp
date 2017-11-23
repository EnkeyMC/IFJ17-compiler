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
#include "3ac.c"
#include "parser.c"
#include "sem_analyzer.c"

class ParserTestFixture : public ::testing::Test {
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

TEST(UIDGeneratorTest, UIDS200) {
	constexpr int n = 500;
	char* uids[n];
	for (int i = 0; i < n; i++) {
		uids[i] = generate_uid();

		for (int j = 0; j < i; j++) {
			EXPECT_STRNE(uids[j], uids[i]);
		}
	}

	for (int i = 0; i < n; i++) {
		free(uids[i]);
	}
}

TEST_F(ParserTestFixture, SuccEmpty) {
	SetInputFile("test_files/empty.fbc");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccFactorialIt) {
	SetInputFile("test_files/factorial_it.fbc");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccFactorialRecur) {
	SetInputFile("test_files/factorial_recur.fbc");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccStrings) {
	SetInputFile("test_files/strings.fbc");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccBasic02) {
	SetInputFile("test_files/syntax/basic/02.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccBasic03) {
	SetInputFile("test_files/syntax/basic/03.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccBasic05) {
	SetInputFile("test_files/syntax/basic/05.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccBasic06) {
	SetInputFile("test_files/syntax/basic/06.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccBasic08) {
	SetInputFile("test_files/syntax/basic/08.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccBasic09) {
	SetInputFile("test_files/syntax/basic/09.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccBasic10) {
	SetInputFile("test_files/syntax/basic/10.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccBasic18) {
	SetInputFile("test_files/syntax/basic/18.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccBasic19) {
	SetInputFile("test_files/syntax/basic/19.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccBasic20) {
	SetInputFile("test_files/syntax/basic/20_modified.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

// --------
// FUNCTIONS
// --------

TEST_F(ParserTestFixture, SuccFunctions01) {
	SetInputFile("test_files/syntax/functions/01.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccFunctions02) {
	SetInputFile("test_files/syntax/functions/02.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccFunctions03) {
	SetInputFile("test_files/syntax/functions/03.code");

	EXPECT_EQ(parse(parser), EXIT_SEMANTIC_PROG_ERROR);
}

TEST_F(ParserTestFixture, SuccFunctions04) {
	SetInputFile("test_files/syntax/functions/04.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccFunctions05) {
	SetInputFile("test_files/syntax/functions/05.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccFunctions06) {
	SetInputFile("test_files/syntax/functions/06.code");

	EXPECT_EQ(parse(parser), EXIT_SEMANTIC_PROG_ERROR);
}

TEST_F(ParserTestFixture, SuccFunctions07) {
	SetInputFile("test_files/syntax/functions/07.code");

	EXPECT_EQ(parse(parser), EXIT_SEMANTIC_PROG_ERROR);
}

TEST_F(ParserTestFixture, SuccFunctions08) {
	SetInputFile("test_files/syntax/functions/08.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccFunctions09) {
	SetInputFile("test_files/syntax/functions/09.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccFunctions10) {
	SetInputFile("test_files/syntax/functions/10.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccFunctions11) {
	SetInputFile("test_files/syntax/functions/11.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccFunctions12) {
	SetInputFile("test_files/syntax/functions/12.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccFunctions13) {
	SetInputFile("test_files/syntax/functions/13.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccFunctions14) {
	SetInputFile("test_files/syntax/functions/14.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccFunctions15) {
	SetInputFile("test_files/syntax/functions/15.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccFunctions16) {
	SetInputFile("test_files/syntax/functions/16.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccFunctions17) {
	SetInputFile("test_files/syntax/functions/17.code");

	EXPECT_EQ(parse(parser), EXIT_SYNTAX_ERROR);
}

TEST_F(ParserTestFixture, SuccFunctions18) {
	SetInputFile("test_files/syntax/functions/18.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccFunctions19) {
	SetInputFile("test_files/syntax/functions/19.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccFunctions20) {
	SetInputFile("test_files/syntax/functions/20.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccFunctions21) {
	SetInputFile("test_files/syntax/functions/21.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccFunctions22) {
	SetInputFile("test_files/syntax/functions/22.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccFunctions23) {
	SetInputFile("test_files/syntax/functions/23.code");

	EXPECT_EQ(parse(parser), EXIT_SEMANTIC_COMP_ERROR);
}

TEST_F(ParserTestFixture, SuccFunctions24) {
	SetInputFile("test_files/syntax/functions/24.code");

	EXPECT_EQ(parse(parser), EXIT_SEMANTIC_OTHER_ERROR);
}

TEST_F(ParserTestFixture, SuccFunctions26) {
	SetInputFile("test_files/syntax/functions/26.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccFunctions27) {
	SetInputFile("test_files/syntax/functions/27.code");

	EXPECT_EQ(parse(parser), EXIT_SEMANTIC_COMP_ERROR);
}

TEST_F(ParserTestFixture, SuccFunctions28) {
	SetInputFile("test_files/syntax/functions/28.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

// ----------
// STATEMENTS
// ----------

TEST_F(ParserTestFixture, SuccStatements06) {
	SetInputFile("test_files/syntax/statements/06_modified.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccStatements07) {
	SetInputFile("test_files/syntax/statements/07.code");

	EXPECT_EQ(parse(parser), EXIT_SYNTAX_ERROR);
}

// ----
// BASE
// ----

TEST_F(ParserTestFixture, SuccBase01) {
	SetInputFile("test_files/syntax/base/01.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccBase02) {
	SetInputFile("test_files/syntax/base/02.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccBase03) {
	SetInputFile("test_files/syntax/base/03.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccBase04) {
	SetInputFile("test_files/syntax/base/04.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccBase05) {
	SetInputFile("test_files/syntax/base/05.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccBase06) {
	SetInputFile("test_files/syntax/base/06.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccBase07) {
	SetInputFile("test_files/syntax/base/07.code");

	EXPECT_EQ(parse(parser), EXIT_LEX_ERROR);
}

TEST_F(ParserTestFixture, SuccBase08) {
	SetInputFile("test_files/syntax/base/08.code");

	EXPECT_EQ(parse(parser), EXIT_LEX_ERROR);
}

TEST_F(ParserTestFixture, SuccBase09) {
	SetInputFile("test_files/syntax/base/09.code");

	EXPECT_EQ(parse(parser), EXIT_LEX_ERROR);
}

TEST_F(ParserTestFixture, SuccBase10) {
	SetInputFile("test_files/syntax/base/10.code");

	EXPECT_EQ(parse(parser), EXIT_LEX_ERROR);
}

TEST_F(ParserTestFixture, SuccBase11) {
	SetInputFile("test_files/syntax/base/11.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}
// -----
// LOOPS
// -----

TEST_F(ParserTestFixture, SuccLoops01) {
	SetInputFile("test_files/syntax/loops/01.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccLoops02) {
	SetInputFile("test_files/syntax/loops/02.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccLoops03) {
	SetInputFile("test_files/syntax/loops/03.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccLoops04) {
	SetInputFile("test_files/syntax/loops/04.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccLoops05) {
	SetInputFile("test_files/syntax/loops/05.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccLoops06) {
	SetInputFile("test_files/syntax/loops/06.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccLoops18) {
	SetInputFile("test_files/syntax/loops/18.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

// ----------
// CONDITIONS
// ----------

TEST_F(ParserTestFixture, SuccConditions01) {
	SetInputFile("test_files/syntax/conditions/01.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccConditions02) {
	SetInputFile("test_files/syntax/conditions/02.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccConditions03) {
	SetInputFile("test_files/syntax/conditions/03.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccConditions04) {
	SetInputFile("test_files/syntax/conditions/04.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccConditions05) {
	SetInputFile("test_files/syntax/conditions/05.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccConditions06) {
	SetInputFile("test_files/syntax/conditions/06.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccConditions07) {
	SetInputFile("test_files/syntax/conditions/07.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccConditions08) {
	SetInputFile("test_files/syntax/conditions/08.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccConditions09) {
	SetInputFile("test_files/syntax/conditions/09.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccConditions10) {
	SetInputFile("test_files/syntax/conditions/10.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccConditions11) {
	SetInputFile("test_files/syntax/conditions/11.code");

	EXPECT_EQ(parse(parser), EXIT_SEMANTIC_PROG_ERROR);
}

TEST_F(ParserTestFixture, SuccConditions12) {
	SetInputFile("test_files/syntax/conditions/12.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccConditions13) {
	SetInputFile("test_files/syntax/conditions/13.code");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}

TEST_F(ParserTestFixture, SuccConditions14) {
	SetInputFile("test_files/syntax/conditions/14.code");

	EXPECT_EQ(parse(parser), EXIT_LEX_ERROR);
}

// ----
// EXPR
// ----

TEST_F(ParserTestFixture, SuccSimpleExpr01) {
	SetInputFile("test_files/simple_expr_01.fbc");

	EXPECT_EQ(parse(parser), EXIT_SUCCESS);
}
