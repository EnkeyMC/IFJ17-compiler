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
#include "token.c"

class ScannerTestFixture : public ::testing::Test {
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

TEST_F(ScannerTestFixture, EmptyFile) {
	SetInputFile("test_files/empty.fbc");

	Token *token = scanner_get_token(scanner);

    ASSERT_NE(token, nullptr);
	EXPECT_EQ(
		token->id,
		TOKEN_EOF
	);

	token_free(token);
}

TEST_F(ScannerTestFixture, ErrBase01) {
	SetInputFile("test_files/scanner/lex_err_base_01.fbc");

	Token *token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	EXPECT_EQ(
		token->id,
		LEX_ERROR
	);

	token_free(token);
}

TEST_F(ScannerTestFixture, ErrBase02) {
	SetInputFile("test_files/scanner/lex_err_base_02.fbc");

	Token *token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	EXPECT_EQ(
		token->id,
		LEX_ERROR
	);

	token_free(token);
}

TEST_F(ScannerTestFixture, ErrBase03) {
	SetInputFile("test_files/scanner/lex_err_base_03.fbc");

	Token *token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	EXPECT_EQ(
		token->id,
		LEX_ERROR
	);

	token_free(token);
}

TEST_F(ScannerTestFixture, ErrBase04) {
	SetInputFile("test_files/scanner/lex_err_base_04.fbc");

	Token *token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	EXPECT_EQ(
		token->id,
		LEX_ERROR
	);

	token_free(token);
}

TEST_F(ScannerTestFixture, ErrBlockComm01) {
	SetInputFile("test_files/scanner/lex_err_block_comment_01.fbc");

	Token *token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	EXPECT_EQ(
		token->id,
		LEX_ERROR
	);

	token_free(token);
}

TEST_F(ScannerTestFixture, ErrBlockComm02) {
	SetInputFile("test_files/scanner/lex_err_block_comment_02.fbc");

	Token *token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	EXPECT_EQ(
		token->id,
		LEX_ERROR
	);

	token_free(token);
}

TEST_F(ScannerTestFixture, ErrReal01) {
	SetInputFile("test_files/scanner/lex_err_real_01.fbc");

	Token *token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	EXPECT_EQ(
		token->id,
		LEX_ERROR
	);

	token_free(token);
}

TEST_F(ScannerTestFixture, ErrReal02) {
	SetInputFile("test_files/scanner/lex_err_real_02.fbc");

	Token *token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	EXPECT_EQ(
		token->id,
		LEX_ERROR
	);

	token_free(token);
}

TEST_F(ScannerTestFixture, ErrReal03) {
	SetInputFile("test_files/scanner/lex_err_real_03.fbc");

	Token *token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	EXPECT_EQ(
		token->id,
		LEX_ERROR
	);

	token_free(token);
}

TEST_F(ScannerTestFixture, ErrReal04) {
	SetInputFile("test_files/scanner/lex_err_real_04.fbc");

	Token *token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	EXPECT_EQ(
		token->id,
		LEX_ERROR
	);

	token_free(token);
}

TEST_F(ScannerTestFixture, ErrReal05) {
	SetInputFile("test_files/scanner/lex_err_real_05.fbc");

	Token *token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	EXPECT_EQ(
		token->id,
		LEX_ERROR
	);

	token_free(token);
}

TEST_F(ScannerTestFixture, ErrString01) {
	SetInputFile("test_files/scanner/lex_err_string_01.fbc");

	Token *token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	EXPECT_EQ(
		token->id,
		LEX_ERROR
	);

	token_free(token);
}

TEST_F(ScannerTestFixture, ErrString02) {
	SetInputFile("test_files/scanner/lex_err_string_02.fbc");

	Token *token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	EXPECT_EQ(
		token->id,
		LEX_ERROR
	);

	token_free(token);
}

TEST_F(ScannerTestFixture, ErrString03) {
	SetInputFile("test_files/scanner/lex_err_string_03.fbc");

	Token *token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	EXPECT_EQ(
		token->id,
		LEX_ERROR
	);

	token_free(token);
}

TEST_F(ScannerTestFixture, ErrString04) {
	SetInputFile("test_files/scanner/lex_err_string_04.fbc");

	Token *token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	EXPECT_EQ(
		token->id,
		LEX_ERROR
	);

	token_free(token);
}

TEST_F(ScannerTestFixture, ErrString05) {
	SetInputFile("test_files/scanner/lex_err_string_05.fbc");

	Token *token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	EXPECT_EQ(
		token->id,
		LEX_ERROR
	);

	token_free(token);
}

TEST_F(ScannerTestFixture, ErrString06) {
	SetInputFile("test_files/scanner/lex_err_string_06.fbc");

	Token *token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	EXPECT_EQ(
		token->id,
		LEX_ERROR
	);

	token_free(token);
}

TEST_F(ScannerTestFixture, ErrUnknownChar) {
	SetInputFile("test_files/scanner/lex_err_unknown_char.fbc");

	Token *token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	EXPECT_EQ(
		token->id,
		LEX_ERROR
	);

	token_free(token);
}

TEST_F(ScannerTestFixture, ParseInteger) {
	SetInputFile("test_files/scanner/parse_int.fbc");

	// 0
	Token *token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	ASSERT_EQ(token->id, TOKEN_INT);
	EXPECT_EQ(token->i, (unsigned) 0);
	token_free(token);

	// EOL
	token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	ASSERT_EQ(token->id, TOKEN_EOL);

	// 5
	token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	ASSERT_EQ(token->id, TOKEN_INT);
	EXPECT_EQ(token->i, (unsigned) 5);
	token_free(token);

	// EOL
	token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	ASSERT_EQ(token->id, TOKEN_EOL);

	// 19549
	token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	ASSERT_EQ(token->id, TOKEN_INT);
	EXPECT_EQ(token->i, (unsigned) 19549);
	token_free(token);

	// EOL
	token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	ASSERT_EQ(token->id, TOKEN_EOL);

	// EOF
	token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	ASSERT_EQ(token->id, TOKEN_EOF);
}

TEST_F(ScannerTestFixture, ParseReal) {
	SetInputFile("test_files/scanner/parse_real.fbc");

	// 5e-5
	Token *token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	ASSERT_EQ(token->id, TOKEN_REAL);
	EXPECT_DOUBLE_EQ(token->d, 5e-5);
	token_free(token);

	// EOL
	token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	ASSERT_EQ(token->id, TOKEN_EOL);

	// 3E+6
	token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	ASSERT_EQ(token->id, TOKEN_REAL);
	EXPECT_EQ(token->d, 3E+6);
	token_free(token);

	// EOL
	token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	ASSERT_EQ(token->id, TOKEN_EOL);

	// 0.8125
	token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	ASSERT_EQ(token->id, TOKEN_REAL);
	EXPECT_EQ(token->d, 0.8125);
	token_free(token);

	// EOL
	token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	ASSERT_EQ(token->id, TOKEN_EOL);

	// 0.5e-2
	token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	ASSERT_EQ(token->id, TOKEN_REAL);
	EXPECT_EQ(token->d, 0.5e-2);
	token_free(token);

	// EOL
	token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	ASSERT_EQ(token->id, TOKEN_EOL);

	// 0.3e+20
	token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	ASSERT_EQ(token->id, TOKEN_REAL);
	EXPECT_EQ(token->d, 0.3e20);
	token_free(token);

	// EOL
	token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	ASSERT_EQ(token->id, TOKEN_EOL);

	// EOF
	token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	ASSERT_EQ(token->id, TOKEN_EOF);
}

TEST_F(ScannerTestFixture, ParseString) {
	SetInputFile("test_files/scanner/parse_string.fbc");

	// "Test string \001\n"
	Token *token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	ASSERT_EQ(token->id, TOKEN_STRING);
	EXPECT_STREQ(token->str, "Test string \\001\\n");
	token_free(token);

	// EOL
	token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	ASSERT_EQ(token->id, TOKEN_EOL);

	// "\\\n Test\ttwo"
	token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	ASSERT_EQ(token->id, TOKEN_STRING);
	EXPECT_STREQ(token->str, "\\\\\\n Test\\ttwo");
	token_free(token);

	// EOL
	token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	ASSERT_EQ(token->id, TOKEN_EOL);

	// EOF
	token = scanner_get_token(scanner);

	ASSERT_NE(token, nullptr);
	ASSERT_EQ(token->id, TOKEN_EOF);
}

TEST_F(ScannerTestFixture, Basic02) {
	SetInputFile("test_files/syntax/basic/02.code");

	token_e tokens[] = {
		TOKEN_EOL,
		TOKEN_KW_SCOPE, TOKEN_EOL,
		TOKEN_KW_DIM, TOKEN_IDENTIFIER, TOKEN_KW_AS, TOKEN_KW_STRING, TOKEN_EOL,
		TOKEN_KW_DIM, TOKEN_IDENTIFIER, TOKEN_KW_AS, TOKEN_KW_INTEGER, TOKEN_EOL,
		TOKEN_KW_DIM, TOKEN_IDENTIFIER, TOKEN_KW_AS, TOKEN_KW_DOUBLE, TOKEN_EOL,
		TOKEN_KW_END, TOKEN_KW_SCOPE, TOKEN_EOF
	};

	Token* token;
	for (unsigned int i = 0; i < sizeof(tokens) / sizeof(token_e); i++) {
		token = scanner_get_token(scanner);
		ASSERT_NE(token, nullptr) << "Iter: " << i;
		ASSERT_EQ(token->id, tokens[i]) << "Iter: " << i;
		token_free(token);
	}
}

TEST_F(ScannerTestFixture, Simple01) {
	SetInputFile("test_files/scanner/simple01.fbc");

	token_e tokens[] = {
		TOKEN_EOL,
		TOKEN_IDENTIFIER, TOKEN_EQUAL, TOKEN_REAL, TOKEN_EOL,
		TOKEN_IDENTIFIER, TOKEN_EQUAL, TOKEN_IDENTIFIER, TOKEN_DIVR, TOKEN_INT, TOKEN_EOL,
		TOKEN_IDENTIFIER, TOKEN_EQUAL, TOKEN_INT, TOKEN_EOL,
		TOKEN_IDENTIFIER, TOKEN_LE, TOKEN_REAL, TOKEN_EOL,
		TOKEN_EOL,
		TOKEN_EOF
	};

	Token* token;
	for (unsigned int i = 0; i < sizeof(tokens) / sizeof(token_e); i++) {
		token = scanner_get_token(scanner);
		ASSERT_NE(token, nullptr) << "Iter: " << i;
		ASSERT_EQ(token->id, tokens[i]) << "Iter: " << i;
		token_free(token);
	}
}

TEST_F(ScannerTestFixture, FactorialRecursive) {
	SetInputFile("test_files/factorial_recur.fbc");

	token_e tokens[] = {
            TOKEN_EOL,
            TOKEN_EOL,
            TOKEN_KW_DECLARE, TOKEN_KW_FUNCTION, TOKEN_IDENTIFIER, TOKEN_LPAR, TOKEN_IDENTIFIER, TOKEN_KW_AS, TOKEN_KW_INTEGER, TOKEN_RPAR, TOKEN_KW_AS, TOKEN_KW_INTEGER, TOKEN_EOL,
            TOKEN_KW_FUNCTION, TOKEN_IDENTIFIER, TOKEN_LPAR, TOKEN_IDENTIFIER, TOKEN_KW_AS, TOKEN_KW_INTEGER, TOKEN_RPAR, TOKEN_KW_AS, TOKEN_KW_INTEGER, TOKEN_EOL,
            TOKEN_KW_DIM, TOKEN_IDENTIFIER, TOKEN_KW_AS, TOKEN_KW_INTEGER, TOKEN_EOL,
            TOKEN_KW_DIM, TOKEN_IDENTIFIER, TOKEN_KW_AS, TOKEN_KW_INTEGER, TOKEN_EOL,
			TOKEN_KW_DIM, TOKEN_IDENTIFIER, TOKEN_KW_AS, TOKEN_KW_INTEGER, TOKEN_EOL,
            TOKEN_KW_IF, TOKEN_IDENTIFIER, TOKEN_LT, TOKEN_INT, TOKEN_KW_THEN, TOKEN_EOL,
            TOKEN_IDENTIFIER, TOKEN_EQUAL, TOKEN_INT, TOKEN_EOL,
            TOKEN_KW_ELSE, TOKEN_EOL,
            TOKEN_IDENTIFIER, TOKEN_EQUAL, TOKEN_IDENTIFIER, TOKEN_SUB, TOKEN_INT, TOKEN_EOL,
            TOKEN_IDENTIFIER, TOKEN_EQUAL, TOKEN_IDENTIFIER, TOKEN_LPAR, TOKEN_IDENTIFIER, TOKEN_RPAR, TOKEN_EOL,
            TOKEN_IDENTIFIER, TOKEN_EQUAL, TOKEN_IDENTIFIER, TOKEN_MUL, TOKEN_IDENTIFIER, TOKEN_EOL,
            TOKEN_KW_END, TOKEN_KW_IF, TOKEN_EOL,
            TOKEN_KW_RETURN, TOKEN_IDENTIFIER, TOKEN_EOL,
            TOKEN_KW_END, TOKEN_KW_FUNCTION, TOKEN_EOL,
            TOKEN_EOL,
            TOKEN_KW_SCOPE, TOKEN_EOL,
            TOKEN_KW_DIM, TOKEN_IDENTIFIER, TOKEN_KW_AS, TOKEN_KW_INTEGER, TOKEN_EOL,
            TOKEN_KW_DIM, TOKEN_IDENTIFIER, TOKEN_KW_AS, TOKEN_KW_INTEGER, TOKEN_EOL,
            TOKEN_EOL,
            TOKEN_KW_PRINT, TOKEN_STRING, TOKEN_SEMICOLON, TOKEN_EOL,
            TOKEN_KW_INPUT, TOKEN_IDENTIFIER, TOKEN_EOL,
            TOKEN_KW_IF, TOKEN_IDENTIFIER, TOKEN_LT, TOKEN_INT, TOKEN_KW_THEN, TOKEN_EOL,
            TOKEN_KW_PRINT, TOKEN_STRING, TOKEN_SEMICOLON, TOKEN_EOL,
            TOKEN_KW_ELSE, TOKEN_EOL,
			TOKEN_IDENTIFIER, TOKEN_EQUAL, TOKEN_IDENTIFIER, TOKEN_LPAR, TOKEN_IDENTIFIER, TOKEN_RPAR, TOKEN_EOL,
            TOKEN_KW_PRINT, TOKEN_STRING, TOKEN_SEMICOLON, TOKEN_IDENTIFIER, TOKEN_SEMICOLON, TOKEN_STRING, TOKEN_SEMICOLON, TOKEN_EOL,
            TOKEN_KW_END, TOKEN_KW_IF, TOKEN_EOL,
            TOKEN_KW_END, TOKEN_KW_SCOPE, TOKEN_EOL,
            TOKEN_EOF
	};

	Token* token;
	for (unsigned int i = 0; i < sizeof(tokens) / sizeof(token_e); i++) {
		token = scanner_get_token(scanner);
		ASSERT_NE(token, nullptr) << "Iter: " << i;
		ASSERT_EQ(token->id, tokens[i]) << "Iter: " << i;
		token_free(token);
	}
}

TEST_F(ScannerTestFixture, Strings) {
    SetInputFile("test_files/strings.fbc");

    token_e tokens[] = {
            TOKEN_EOL,
            TOKEN_EOL,
            TOKEN_KW_SCOPE, TOKEN_EOL,
            TOKEN_KW_DIM, TOKEN_IDENTIFIER, TOKEN_KW_AS, TOKEN_KW_STRING, TOKEN_EOL,
            TOKEN_KW_DIM, TOKEN_IDENTIFIER, TOKEN_KW_AS, TOKEN_KW_STRING, TOKEN_EOL,
            TOKEN_KW_DIM, TOKEN_IDENTIFIER, TOKEN_KW_AS, TOKEN_KW_INTEGER, TOKEN_EOL,
            TOKEN_EOL,
            TOKEN_IDENTIFIER, TOKEN_EQUAL, TOKEN_STRING, TOKEN_EOL,
            TOKEN_IDENTIFIER, TOKEN_EQUAL, TOKEN_IDENTIFIER, TOKEN_ADD, TOKEN_STRING, TOKEN_EOL,
            TOKEN_KW_PRINT, TOKEN_IDENTIFIER, TOKEN_SEMICOLON, TOKEN_STRING, TOKEN_SEMICOLON, TOKEN_IDENTIFIER, TOKEN_SEMICOLON, TOKEN_STRING, TOKEN_SEMICOLON, TOKEN_EOL,
            TOKEN_IDENTIFIER, TOKEN_EQUAL, TOKEN_IDENTIFIER, TOKEN_LPAR, TOKEN_IDENTIFIER, TOKEN_RPAR, TOKEN_EOL,
            TOKEN_IDENTIFIER, TOKEN_EQUAL, TOKEN_IDENTIFIER, TOKEN_SUB, TOKEN_INT, TOKEN_ADD, TOKEN_INT, TOKEN_EOL,
            TOKEN_IDENTIFIER, TOKEN_EQUAL, TOKEN_IDENTIFIER, TOKEN_LPAR, TOKEN_IDENTIFIER, TOKEN_COMMA, TOKEN_IDENTIFIER, TOKEN_COMMA, TOKEN_INT, TOKEN_RPAR, TOKEN_EOL,
            TOKEN_KW_PRINT, TOKEN_STRING, TOKEN_SEMICOLON, TOKEN_IDENTIFIER, TOKEN_SEMICOLON, TOKEN_STRING, TOKEN_SEMICOLON, TOKEN_IDENTIFIER, TOKEN_SEMICOLON, TOKEN_STRING, TOKEN_SEMICOLON, TOKEN_IDENTIFIER, TOKEN_SEMICOLON, TOKEN_STRING, TOKEN_SEMICOLON, TOKEN_EOL,
            TOKEN_KW_PRINT, TOKEN_STRING, TOKEN_SEMICOLON, TOKEN_EOL,
            TOKEN_KW_PRINT, TOKEN_STRING, TOKEN_SEMICOLON, TOKEN_EOL,
            TOKEN_KW_INPUT, TOKEN_IDENTIFIER, TOKEN_EOL,
            TOKEN_KW_DO, TOKEN_KW_WHILE, TOKEN_LPAR, TOKEN_IDENTIFIER, TOKEN_NE, TOKEN_STRING, TOKEN_RPAR, TOKEN_EOL,
            TOKEN_KW_PRINT, TOKEN_STRING, TOKEN_SEMICOLON, TOKEN_EOL,
            TOKEN_KW_INPUT, TOKEN_IDENTIFIER, TOKEN_EOL,
            TOKEN_KW_LOOP, TOKEN_EOL,
            TOKEN_KW_END, TOKEN_KW_SCOPE, TOKEN_EOL,
            TOKEN_EOF
    };

    Token* token;
    for (unsigned int i = 0; i < sizeof(tokens) / sizeof(token_e); i++) {
        token = scanner_get_token(scanner);
        ASSERT_NE(token, nullptr) << "Iter: " << i;
        ASSERT_EQ(token->id, tokens[i]) << "Iter: " << i;
		token_free(token);
    }
}

TEST_F(ScannerTestFixture, FactorialIterative) {
	SetInputFile("test_files/factorial_it.fbc");

	Token* token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";
	token_free(token);

	// scope
	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KW_SCOPE
	) << "scope";
	token_free(token);

	// EOL
	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";
	token_free(token);

	// Dim
	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KW_DIM
	) << "Dim";
	token_free(token);

	// a
	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_IDENTIFIER
	) << "a";
	token_free(token);

	// As
	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KW_AS
	) << "As";
	token_free(token);

	// Integer
	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KW_INTEGER
	) << "Integer";

	// EOL
	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";
	token_free(token);

	// Dim
	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KW_DIM
	) << "DIM";
	token_free(token);

	// vysl
	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_IDENTIFIER
	) << "vysl";
	token_free(token);

	// AS
	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KW_AS
	) << "AS";
	token_free(token);

	// INTEGER
	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KW_INTEGER
	) << "INTEGER";
	token_free(token);

	// EOL
	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KW_PRINT
	) << "PrinT";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_STRING
	) << "!\"Zadejte cislo pro vypocet faktorialu\"";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_SEMICOLON
	) << ";";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KW_INPUT
	) << "InpuT";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_IDENTIFIER
	) << "A";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KW_IF
	) << "If";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_IDENTIFIER
	) << "a";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_LT
	) << "<";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_INT
	) << "0";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KW_THEN
	) << "THEN";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KW_PRINT
	) << "print";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_STRING
	) << "!\"\\nFaktorial nelze spocitat\\n\"";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_SEMICOLON
	) << ";";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KW_ELSE
	) << "ELSE";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_IDENTIFIER
	) << "Vysl";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EQUAL
	) << "=";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_INT
	) << "1";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KW_DO
	) << "Do";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KW_WHILE
	) << "WHile";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_IDENTIFIER
	) << "A";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_GT
	) << ">";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_INT
	) << "0";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_IDENTIFIER
	) << "VYSL";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EQUAL
	) << "=";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_IDENTIFIER
	) << "vysl";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_MUL
	) << "*";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_IDENTIFIER
	) << "a";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_IDENTIFIER
	) << "a";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EQUAL
	) << "=";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_IDENTIFIER
	) << "A";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_SUB
	) << "-";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_INT
	) << "1";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KW_LOOP
	) << "LooP";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KW_PRINT
	) << "Print";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_STRING
	) << "!\"\\nVysledek je:\"";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_SEMICOLON
	) << ";";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_IDENTIFIER
	) << "vYsl";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_SEMICOLON
	) << ";";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_STRING
	) << "!\"\\n\"";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_SEMICOLON
	) << ";";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KW_END
	) << "end";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KW_IF
	) << "IF";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KW_END
	) << "END";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_KW_SCOPE
	) << "SCOPE";
	token_free(token);

	token = scanner_get_token(scanner);
	ASSERT_NE(token, nullptr);
	ASSERT_EQ(
		token->id,
		TOKEN_EOL
	) << "EOL";
	token_free(token);

    token = scanner_get_token(scanner);
    ASSERT_NE(token, nullptr);
    ASSERT_EQ(
            token->id,
            TOKEN_EOF
    ) << "EOF";
	token_free(token);
}
