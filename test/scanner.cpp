/**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */

#include <scanner.h>
#include "gtest/gtest.h"
#include "scanner.c"

class ScannerTestFixture : public ::testing::Test {
protected:

	FILE* test_file;

	virtual void SetUp() {

	}

	virtual void TearDown() {
		fclose(test_file);
	}

	void SetInputFile(const char *file) {
		test_file = fopen(file, "r");

		set_input_stream(test_file);

		if (test_file == NULL)
			perror("Error");
	}
};

TEST_F(ScannerTestFixture, EmptyFile) {
	SetInputFile("test_files/empty.fbc");

	token_t *token = get_token();

	EXPECT_EQ(
		token,
		nullptr
	);
}

TEST_F(ScannerTestFixture, FactorialRecursive) {
	SetInputFile("test_files/factorial_recur.fbc");

	int tokens[] = {TOKEN_EOL,
					TOKEN_EOL,
					TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_IDENTIFIER, TOKEN_LPAR, TOKEN_IDENTIFIER, TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_RPAR, TOKEN_KEYWORD, TOKEN_KEYWORD,
					TOKEN_KEYWORD, TOKEN_IDENTIFIER, TOKEN_LPAR, TOKEN_IDENTIFIER, TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_RPAR, TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_EOL,
					TOKEN_KEYWORD, TOKEN_IDENTIFIER, TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_EOL,
					TOKEN_KEYWORD, TOKEN_IDENTIFIER, TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_EOL,
					TOKEN_KEYWORD, TOKEN_IDENTIFIER, TOKEN_LT, TOKEN_INT, TOKEN_KEYWORD, TOKEN_EOL,
					TOKEN_IDENTIFIER, TOKEN_EQUAL, TOKEN_INT, TOKEN_EOL,
					TOKEN_KEYWORD, TOKEN_EOL
	};

	token_t* token;
	for (int i = 0; i < sizeof(tokens) / sizeof(int); i++) {
		token = get_token();
		ASSERT_NE(token, nullptr);
		ASSERT_EQ(token, token[i]);
	}
}

TEST_F(ScannerTestFixture, FactorialIterative) {
	SetInputFile("test_files/factorial_it.fbc");

	token_t* token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";

	// scope
	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KEYWORD
	) << "scope";

	// EOL
	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";

	// Dim
	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KEYWORD
	) << "Dim";

	// a
	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_IDENTIFIER
	) << "a";

	// As
	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KEYWORD
	) << "As";

	// Integer
	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KEYWORD
	) << "Integer";

	// EOL
	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";

	// vysl
	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_IDENTIFIER
	) << "vysl";

	// AS
	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KEYWORD
	) << "AS";

	// INTEGER
	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KEYWORD
	) << "INTEGER";

	// EOL
	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_STRING
	) << "!\"Zadejte cislo pro vypocet faktorialu\"";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_SEMICOLON
	) << ";";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KEYWORD
	) << "InpuT";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_IDENTIFIER
	) << "A";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KEYWORD
	) << "If";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_IDENTIFIER
	) << "a";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_LT
	) << "<";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_INT
	) << "0";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KEYWORD
	) << "THEN";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KEYWORD
	) << "print";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_STRING
	) << "!\"\\nFaktorial nelze spocitat\\n\"";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_SEMICOLON
	) << ";";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KEYWORD
	) << "ELSE";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_IDENTIFIER
	) << "Vysl";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EQUAL
	) << "=";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_INT
	) << "1";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KEYWORD
	) << "Do";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KEYWORD
	) << "WHile";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_IDENTIFIER
	) << "A";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_GT
	) << ">";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_INT
	) << "0";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_IDENTIFIER
	) << "VYSL";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EQUAL
	) << "=";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_IDENTIFIER
	) << "vysl";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_MUL
	) << "*";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_IDENTIFIER
	) << "a";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_IDENTIFIER
	) << "a";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EQUAL
	) << "=";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_IDENTIFIER
	) << "A";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_SUB
	) << "-";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_INT
	) << "1";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KEYWORD
	) << "LooP";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KEYWORD
	) << "Print";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_STRING
	) << "!\"\\nVysledek je:\"";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_SEMICOLON
	) << ";";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_IDENTIFIER
	) << "vYsl";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_SEMICOLON
	) << ";";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_STRING
	) << "!\"\\n\"";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_SEMICOLON
	) << ";";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KEYWORD
	) << "end";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KEYWORD
	) << "IF";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KEYWORD
	) << "END";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KEYWORD
	) << "SCOPE";

	token = get_token();
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";
}
