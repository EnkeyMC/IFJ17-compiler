#include "gtest/gtest.h"
#include "expr_grammar.c"
#include "ext_stack.c"
#include "grammar.c"


class ExtendedStackTestFixture : public ::testing::Test {
protected:
	ExtStack* s = nullptr;

	virtual void SetUp() {
		mem_manager_init();
		expr_grammar_init();
		s = ext_stack_init();
	}

	virtual void TearDown() {
		ext_stack_free(s);
		expr_grammar_free();
		mem_manager_free();
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
		ext_stack_push(s, push_items[i], NULL);
		EXPECT_EQ(ext_stack_top(s), push_items[i]);
	}

	i--; // Index of last termnal inserted
	ext_stack_push(s, NT_EXPRESSION, NULL); // Push non_terminal
	EXPECT_EQ(ext_stack_top(s), push_items[i]); // TOp should return last TERMINAL

	ext_stack_push(s, NT_LIST, NULL);
	EXPECT_EQ(ext_stack_top(s), push_items[i]);
}
