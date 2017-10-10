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
#include "scanner.c"

class ScannerTestFixture : public ::testing::Test {
protected:

    Scanner* scanner;
	FILE* test_file;

	virtual void SetUp() {
        scanner_init(scanner);
	}

	virtual void TearDown() {
		fclose(test_file);
	}

	void SetInputFile(const char *file) {
		test_file = fopen(file, "r");

		scanner->stream = test_file;

		if (test_file == NULL)
			perror("Error");
	}
};

TEST_F(ScannerTestFixture, EmptyFile) {
	SetInputFile("test_files/empty.fbc");

	token_t *token = scanner_get_token(scanner);

    ASSERT_NE(token, nullptr);
	EXPECT_EQ(
		token->id,
		EOF
	);
}

TEST_F(ScannerTestFixture, FactorialRecursive) {
	SetInputFile("test_files/factorial_recur.fbc");

	int tokens[] = {
            TOKEN_EOL,
            TOKEN_EOL,
            TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_IDENTIFIER, TOKEN_LPAR, TOKEN_IDENTIFIER, TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_RPAR, TOKEN_KEYWORD, TOKEN_KEYWORD,
            TOKEN_KEYWORD, TOKEN_IDENTIFIER, TOKEN_LPAR, TOKEN_IDENTIFIER, TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_RPAR, TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_EOL,
            TOKEN_KEYWORD, TOKEN_IDENTIFIER, TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_EOL,
            TOKEN_KEYWORD, TOKEN_IDENTIFIER, TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_EOL,
            TOKEN_KEYWORD, TOKEN_IDENTIFIER, TOKEN_LT, TOKEN_INT, TOKEN_KEYWORD, TOKEN_EOL,
            TOKEN_IDENTIFIER, TOKEN_EQUAL, TOKEN_INT, TOKEN_EOL,
            TOKEN_KEYWORD, TOKEN_EOL,
            TOKEN_IDENTIFIER, TOKEN_EQUAL, TOKEN_IDENTIFIER, TOKEN_SUB, TOKEN_INT, TOKEN_EOL,
            TOKEN_IDENTIFIER, TOKEN_EQUAL, TOKEN_IDENTIFIER, TOKEN_LPAR, TOKEN_IDENTIFIER, TOKEN_RPAR, TOKEN_EOL,
            TOKEN_IDENTIFIER, TOKEN_EQUAL, TOKEN_IDENTIFIER, TOKEN_MUL, TOKEN_IDENTIFIER, TOKEN_EOL,
            TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_EOL,
            TOKEN_KEYWORD, TOKEN_IDENTIFIER, TOKEN_EOL,
            TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_EOL,
            TOKEN_EOL,
            TOKEN_KEYWORD, TOKEN_EOL,
            TOKEN_KEYWORD, TOKEN_IDENTIFIER, TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_EOL,
            TOKEN_KEYWORD, TOKEN_IDENTIFIER, TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_EOL,
            TOKEN_EOL,
            TOKEN_KEYWORD, TOKEN_STRING, TOKEN_SEMICOLON, TOKEN_EOL,
            TOKEN_KEYWORD, TOKEN_IDENTIFIER, TOKEN_EOL,
            TOKEN_KEYWORD, TOKEN_IDENTIFIER, TOKEN_LT, TOKEN_INT, TOKEN_KEYWORD, TOKEN_EOL,
            TOKEN_KEYWORD, TOKEN_STRING, TOKEN_SEMICOLON, TOKEN_EOL,
            TOKEN_KEYWORD, TOKEN_EOL,
            TOKEN_KEYWORD, TOKEN_STRING, TOKEN_SEMICOLON, TOKEN_IDENTIFIER, TOKEN_SEMICOLON, TOKEN_STRING, TOKEN_SEMICOLON, TOKEN_EOL,
            TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_EOL,
            TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_EOL,
            TOKEN_EOF
	};

	token_t* token;
	for (int i = 0; i < sizeof(tokens) / sizeof(int); i++) {
		token = scanner_get_token(scanner);
		ASSERT_NE(token, nullptr);
		ASSERT_EQ(token->id, tokens[i]);
	}
}

TEST_F(ScannerTestFixture, Strings) {
    SetInputFile("test_files/strings.fbc");

    int tokens[] = {
            TOKEN_EOL,
            TOKEN_EOL,
            TOKEN_KEYWORD, TOKEN_EOL,
            TOKEN_KEYWORD, TOKEN_IDENTIFIER, TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_EOL,
            TOKEN_KEYWORD, TOKEN_IDENTIFIER, TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_EOL,
            TOKEN_KEYWORD, TOKEN_IDENTIFIER, TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_EOL,
            TOKEN_EOL,
            TOKEN_IDENTIFIER, TOKEN_EQUAL, TOKEN_STRING, TOKEN_EOL,
            TOKEN_IDENTIFIER, TOKEN_EQUAL, TOKEN_IDENTIFIER, TOKEN_ADD, TOKEN_STRING, TOKEN_EOL,
            TOKEN_KEYWORD, TOKEN_IDENTIFIER, TOKEN_SEMICOLON, TOKEN_STRING, TOKEN_IDENTIFIER, TOKEN_SEMICOLON, TOKEN_STRING, TOKEN_SEMICOLON, TOKEN_EOL,
            TOKEN_IDENTIFIER, TOKEN_EQUAL, TOKEN_IDENTIFIER, TOKEN_LPAR, TOKEN_IDENTIFIER, TOKEN_RPAR, TOKEN_EOL,
            TOKEN_IDENTIFIER, TOKEN_EQUAL, TOKEN_IDENTIFIER, TOKEN_SUB, TOKEN_INT, TOKEN_ADD, TOKEN_ADD, TOKEN_INT, TOKEN_EOL,
            TOKEN_INT, TOKEN_EQUAL, TOKEN_IDENTIFIER, TOKEN_LPAR, TOKEN_IDENTIFIER, TOKEN_COMMA, TOKEN_IDENTIFIER, TOKEN_COMMA, TOKEN_INT, TOKEN_RPAR, TOKEN_EOL,
            TOKEN_KEYWORD, TOKEN_STRING, TOKEN_SEMICOLON, TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_SEMICOLON, TOKEN_IDENTIFIER, TOKEN_SEMICOLON, TOKEN_STRING, TOKEN_SEMICOLON, TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_SEMICOLON, TOKEN_EOL,
            TOKEN_KEYWORD, TOKEN_STRING, TOKEN_SEMICOLON, TOKEN_EOL,
            TOKEN_KEYWORD, TOKEN_STRING, TOKEN_SEMICOLON, TOKEN_EOL,
            TOKEN_KEYWORD, TOKEN_IDENTIFIER, TOKEN_EOL,
            TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_LPAR, TOKEN_IDENTIFIER, TOKEN_NE, TOKEN_STRING, TOKEN_RPAR, TOKEN_EOL,
            TOKEN_KEYWORD, TOKEN_STRING, TOKEN_SEMICOLON, TOKEN_EOL,
            TOKEN_KEYWORD, TOKEN_IDENTIFIER, TOKEN_EOL,
            TOKEN_KEYWORD, TOKEN_EOL,
            TOKEN_KEYWORD, TOKEN_KEYWORD, TOKEN_EOL,
            TOKEN_EOF
    };

    token_t* token;
    for (int i = 0; i < sizeof(tokens) / sizeof(int); i++) {
        token = scanner_get_token(scanner);
        ASSERT_NE(token, nullptr);
        ASSERT_EQ(token->id, tokens[i]);
    }
}

TEST_F(ScannerTestFixture, FactorialIterative) {
	SetInputFile("test_files/factorial_it.fbc");

	token_t* token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";

	// scope
	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KEYWORD
	) << "scope";

	// EOL
	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";

	// Dim
	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KEYWORD
	) << "Dim";

	// a
	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_IDENTIFIER
	) << "a";

	// As
	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KEYWORD
	) << "As";

	// Integer
	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KEYWORD
	) << "Integer";

	// EOL
	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";

	// vysl
	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_IDENTIFIER
	) << "vysl";

	// AS
	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KEYWORD
	) << "AS";

	// INTEGER
	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KEYWORD
	) << "INTEGER";

	// EOL
	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_STRING
	) << "!\"Zadejte cislo pro vypocet faktorialu\"";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_SEMICOLON
	) << ";";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KEYWORD
	) << "InpuT";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_IDENTIFIER
	) << "A";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KEYWORD
	) << "If";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_IDENTIFIER
	) << "a";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_LT
	) << "<";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_INT
	) << "0";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KEYWORD
	) << "THEN";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KEYWORD
	) << "print";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_STRING
	) << "!\"\\nFaktorial nelze spocitat\\n\"";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_SEMICOLON
	) << ";";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KEYWORD
	) << "ELSE";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_IDENTIFIER
	) << "Vysl";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EQUAL
	) << "=";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_INT
	) << "1";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KEYWORD
	) << "Do";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KEYWORD
	) << "WHile";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_IDENTIFIER
	) << "A";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_GT
	) << ">";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_INT
	) << "0";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_IDENTIFIER
	) << "VYSL";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EQUAL
	) << "=";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_IDENTIFIER
	) << "vysl";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_MUL
	) << "*";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_IDENTIFIER
	) << "a";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_IDENTIFIER
	) << "a";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EQUAL
	) << "=";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_IDENTIFIER
	) << "A";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_SUB
	) << "-";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_INT
	) << "1";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KEYWORD
	) << "LooP";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KEYWORD
	) << "Print";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_STRING
	) << "!\"\\nVysledek je:\"";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_SEMICOLON
	) << ";";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_IDENTIFIER
	) << "vYsl";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_SEMICOLON
	) << ";";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_STRING
	) << "!\"\\n\"";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_SEMICOLON
	) << ";";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KEYWORD
	) << "end";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KEYWORD
	) << "IF";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KEYWORD
	) << "END";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KEYWORD
	) << "SCOPE";

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";

    token = scanner_get_token(scanner);
    ASSERT_NE(token, nullptr);
    ASSERT_EQ(
            token->id,
            TOKEN_EOF
    ) << "EOF";
}
