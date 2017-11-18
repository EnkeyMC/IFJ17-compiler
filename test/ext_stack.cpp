#include "gtest/gtest.h"
#include "expr_grammar.c"
#include "ext_stack.c"
#include "grammar.c"


class ExtendedStackTestFixture : public ::testing::Test {
protected:
	ExtStack* s = nullptr;

	virtual void SetUp() {
		expr_grammar_init();
		s = ext_stack_init();
	}

	virtual void TearDown() {
		ext_stack_free(s);
		expr_grammar_free();
	}
};

TEST_F(ExtendedStackTestFixture, PushAndTop) {
	EXPECT_EQ(ext_stack_top(s), EXPR_END_MARKER);

	static const int npush = 10;
	unsigned push_items[npush] = {
		// random TERMINALS
		EXPR_HANDLE_MARKER,
		TOKEN_LPAR,
		EXPR_HANDLE_MARKER,
		TOKEN_IDENTIFIER,
		TOKEN_COMMA,
		TOKEN_LT,
		TOKEN_SUB,
		TOKEN_DIVR,
		TOKEN_STRING,
		TOKEN_DIVI
	};

	int i;
	for (i = 0; i < npush; i++) {
		EXPECT_EQ(ext_stack_push(s, push_items[i], NULL), true);
		EXPECT_EQ(ext_stack_top(s), push_items[i]);
	}

	i--; // Index of last termnal inserted
	EXPECT_EQ(ext_stack_push(s, NT_EXPRESSION, NULL), true); // Push non_terminal
	EXPECT_EQ(ext_stack_top(s), push_items[i]); // TOp should return last TERMINAL

	EXPECT_EQ(ext_stack_push(s, NT_LIST, NULL), true);
	EXPECT_EQ(ext_stack_top(s), push_items[i]);
}

TEST_F(ExtendedStackTestFixture, ShiftReduceValid01) {
	/* VALID EXPRESSION */
	// trying to parse expression: "2 * 2 + 3"
/*
	EXPECT_EQ(ext_stack_shift(s, TOKEN_INT), true);
	EXPECT_EQ(ext_stack_top(s), TOKEN_INT);
	// '$' '<' 'INT'

	EXPECT_EQ(ext_stack_reduce(s), true);
	EXPECT_EQ(ext_stack_top(s), EXPR_END_MARKER);
	// '$' "expr"

	EXPECT_EQ(ext_stack_shift(s, TOKEN_MUL), true);
	EXPECT_EQ(ext_stack_top(s), TOKEN_MUL);
	// '$' '<' "expr" '*'

	EXPECT_EQ(ext_stack_shift(s, TOKEN_INT), true);
	EXPECT_EQ(ext_stack_top(s), TOKEN_INT);
	// '$' '<' "expr" '*' '<' 'INT'

	EXPECT_EQ(ext_stack_reduce(s), true);
	EXPECT_EQ(ext_stack_top(s), TOKEN_MUL);
	// '$' '<' "expr" '*' "expr"

	EXPECT_EQ(ext_stack_reduce(s), true);
	EXPECT_EQ(ext_stack_top(s), EXPR_END_MARKER);
	// '$' "expr"

	EXPECT_EQ(ext_stack_shift(s, TOKEN_ADD), true);
	EXPECT_EQ(ext_stack_top(s), TOKEN_ADD);
	// '$' '<' "expr" '+'

	EXPECT_EQ(ext_stack_shift(s, TOKEN_INT), true);
	EXPECT_EQ(ext_stack_top(s), TOKEN_INT);
	// '$' '<' "expr" '+' '<' 'INT'

	EXPECT_EQ(ext_stack_reduce(s), true);
	EXPECT_EQ(ext_stack_top(s), TOKEN_ADD);
	// '$' '<' "expr" '+' "expr"

	EXPECT_EQ(ext_stack_reduce(s), true);
	EXPECT_EQ(ext_stack_top(s), EXPR_END_MARKER);*/
	// '$' "expr" ---> END
}

TEST_F(ExtendedStackTestFixture, ShiftReduceValid02) {
	/* VALID EXPRESSION */
	// trying to parse expression: "(a - 1) / b * 3.0"
/*
	EXPECT_EQ(ext_stack_shift(s, TOKEN_LPAR), true);
	EXPECT_EQ(ext_stack_top(s), TOKEN_LPAR);
	// '$' '<' '('

	EXPECT_EQ(ext_stack_shift(s, TOKEN_IDENTIFIER), true);
	EXPECT_EQ(ext_stack_top(s), TOKEN_IDENTIFIER);
	// '$' '<' '(' '<' 'ID'

	EXPECT_EQ(ext_stack_reduce(s), true);
	EXPECT_EQ(ext_stack_top(s), TOKEN_LPAR);
	// '$' '<' '(' "expr"

	EXPECT_EQ(ext_stack_shift(s, TOKEN_SUB), true);
	EXPECT_EQ(ext_stack_top(s), TOKEN_SUB);
	// '$' '<' '(' '<' "expr" '-'

	EXPECT_EQ(ext_stack_shift(s, TOKEN_INT), true);
	EXPECT_EQ(ext_stack_top(s), TOKEN_INT);
	// '$' '<' '(' '<' "expr" '-' '<' 'INT'

	EXPECT_EQ(ext_stack_reduce(s), true);
	EXPECT_EQ(ext_stack_top(s), TOKEN_SUB);
	// '$' '<' '(' '<' "expr" '-' "expr"

	EXPECT_EQ(ext_stack_reduce(s), true);
	EXPECT_EQ(ext_stack_top(s), TOKEN_LPAR);
	// '$' '<' '(' "expr"

	EXPECT_EQ(ext_stack_push(s, TOKEN_RPAR, NULL), true);
	EXPECT_EQ(ext_stack_top(s), TOKEN_RPAR);
	// '$' '<' '(' "expr" ')'

	EXPECT_EQ(ext_stack_reduce(s), true);
	EXPECT_EQ(ext_stack_top(s), EXPR_END_MARKER);
	// '$' "expr"

	EXPECT_EQ(ext_stack_shift(s, TOKEN_DIVR), true);
	EXPECT_EQ(ext_stack_top(s), TOKEN_DIVR);
	// '$' '<' "expr" 'DIVR'

	EXPECT_EQ(ext_stack_shift(s, TOKEN_IDENTIFIER), true);
	EXPECT_EQ(ext_stack_top(s), TOKEN_IDENTIFIER);
	// '$' '<' "expr" 'DIVR' '<' 'ID'

	EXPECT_EQ(ext_stack_reduce(s), true);
	EXPECT_EQ(ext_stack_top(s), TOKEN_DIVR);
	// '$' '<' "expr" 'DIVR' "expr"

	EXPECT_EQ(ext_stack_reduce(s), true);
	EXPECT_EQ(ext_stack_top(s), EXPR_END_MARKER);
	// '$' "expr"

	EXPECT_EQ(ext_stack_shift(s, TOKEN_MUL), true);
	EXPECT_EQ(ext_stack_top(s), TOKEN_MUL);
	// '$' '<' "expr" '*'

	EXPECT_EQ(ext_stack_shift(s, TOKEN_REAL), true);
	EXPECT_EQ(ext_stack_top(s), TOKEN_REAL);
	// '$' '<' "expr" '*' '<' 'REAL'

	EXPECT_EQ(ext_stack_reduce(s), true);
	EXPECT_EQ(ext_stack_top(s), TOKEN_MUL);
	// '$' '<' "expr" '*' "expr"

	EXPECT_EQ(ext_stack_reduce(s), true);
	EXPECT_EQ(ext_stack_top(s), EXPR_END_MARKER);*/
	// '$' "expr" ---> END
}

TEST_F(ExtendedStackTestFixture, ShiftReduceValid03) {
	/* VALID EXPRESSION */
	// trying to parse function call: "foo(a,b,c)"
/*
	EXPECT_EQ(ext_stack_shift(s, TOKEN_IDENTIFIER), true);
	EXPECT_EQ(ext_stack_top(s), TOKEN_IDENTIFIER);
	// '$' '<' 'ID'

	EXPECT_EQ(ext_stack_push(s, TOKEN_LPAR, NULL), true);
	EXPECT_EQ(ext_stack_top(s), TOKEN_LPAR);
	// '$' '<' 'ID' '('

	EXPECT_EQ(ext_stack_shift(s, TOKEN_IDENTIFIER), true);
	EXPECT_EQ(ext_stack_top(s), TOKEN_IDENTIFIER);
	// '$' '<' 'ID' '(' '<' 'ID'

	EXPECT_EQ(ext_stack_reduce(s), true);
	EXPECT_EQ(ext_stack_top(s), TOKEN_LPAR);
	// '$' '<' 'ID' '(' "expr"

	EXPECT_EQ(ext_stack_shift(s, TOKEN_COMMA), true);
	EXPECT_EQ(ext_stack_top(s), TOKEN_COMMA);
	// '$' '<' 'ID' '(' '<' "expr" ','

	EXPECT_EQ(ext_stack_shift(s, TOKEN_IDENTIFIER), true);
	EXPECT_EQ(ext_stack_top(s), TOKEN_IDENTIFIER);
	// '$' '<' 'ID' '(' '<' "expr" ',' '<' 'ID'

	EXPECT_EQ(ext_stack_reduce(s), true);
	EXPECT_EQ(ext_stack_top(s), TOKEN_COMMA);
	// '$' '<' 'ID' '(' '<' "expr" , "expr"

	EXPECT_EQ(ext_stack_reduce(s), true);
	EXPECT_EQ(ext_stack_top(s), TOKEN_LPAR);
	// '$' '<' 'ID' '(' "list"

	EXPECT_EQ(ext_stack_shift(s, TOKEN_COMMA), true);
	EXPECT_EQ(ext_stack_top(s), TOKEN_COMMA);
	// '$' '<' 'ID' '(' '<' "list" ','

	EXPECT_EQ(ext_stack_shift(s, TOKEN_IDENTIFIER), true);
	EXPECT_EQ(ext_stack_top(s), TOKEN_IDENTIFIER);
	// '$' '<' 'ID' '(' '<' "list" ',' '<' 'ID'

	EXPECT_EQ(ext_stack_reduce(s), true);
	EXPECT_EQ(ext_stack_top(s), TOKEN_COMMA);
	// '$' '<' 'ID' '(' '<' "list", "expr"

	EXPECT_EQ(ext_stack_reduce(s), true);
	EXPECT_EQ(ext_stack_top(s), TOKEN_LPAR);
	// '$' '<' 'ID' '(' "list"

	EXPECT_EQ(ext_stack_push(s, TOKEN_RPAR, NULL), true);
	EXPECT_EQ(ext_stack_top(s), TOKEN_RPAR);
	// '$' '<' 'ID' '(' "list" )

	EXPECT_EQ(ext_stack_reduce(s), true);
	EXPECT_EQ(ext_stack_top(s), EXPR_END_MARKER);*/
	// '$' "expr" ---> END
}

TEST_F(ExtendedStackTestFixture, ShiftReduceInvalid01) {
	/* REDUCE EMPTY STACK */
//	EXPECT_EQ(ext_stack_reduce(s), false);
}

TEST_F(ExtendedStackTestFixture, ShiftReduceInvalid02) {
	/* REDUCE STACK that doesn't contain EXPR_HANDLE_MARKER */
	EXPECT_EQ(ext_stack_push(s, TOKEN_IDENTIFIER, NULL), true);
	EXPECT_EQ(ext_stack_push(s, TOKEN_LPAR, NULL), true);
	EXPECT_EQ(ext_stack_push(s, TOKEN_RPAR, NULL), true);
//	EXPECT_EQ(ext_stack_reduce(s), false);
}

TEST_F(ExtendedStackTestFixture, ShiftReduceInvalid03) {
	/* INVALID EXPRESSION */
	// trying to parse expression: " 2 )"
/*
	EXPECT_EQ(ext_stack_shift(s, TOKEN_INT), true);
	EXPECT_EQ(ext_stack_top(s), TOKEN_INT);
	// '$' '<' 'INT'

	EXPECT_EQ(ext_stack_reduce(s), true);
	EXPECT_EQ(ext_stack_top(s), EXPR_END_MARKER);
	// '$' "expr"

	EXPECT_EQ(ext_stack_shift(s, TOKEN_RPAR), true);
	EXPECT_EQ(ext_stack_top(s), TOKEN_RPAR);
	// '$' '<' "expr" ')'

	EXPECT_EQ(ext_stack_reduce(s), false);*/
}
