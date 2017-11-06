#include <malloc.h>
#include "gtest/gtest.h"
#include "stack.c"

class StackTestFixture : public ::testing::Test {
protected:
	Stack* stack = nullptr;

	virtual void SetUp() {
		stack = stack_init(0);
	}

	virtual void TearDown() {
		if (stack != nullptr) {
			EXPECT_NO_FATAL_FAILURE(stack_free(stack, FreeData));
		}
	}

	static void FreeData(void* data) {
		if (data != nullptr)
			free(data);
	}
};

TEST_F(StackTestFixture, Initialization) {
	ASSERT_NE(stack, nullptr) << "Initialized stack is not null\n";
	EXPECT_TRUE(stack_empty(stack));
}

TEST_F(StackTestFixture, PushSingle) {
	int val = 42;
	bool ret = stack_push(stack, &val);

	ASSERT_TRUE(ret) << "Stack push shouldn't fail";
	EXPECT_FALSE(stack_empty(stack)) << "Stack should not be empty";
	EXPECT_EQ(*((int*) stack_top(stack)), val);

	stack_free(stack, nullptr);
	stack = nullptr;
}

TEST_F(StackTestFixture, PushMultiple) {
	int values[] = {1,2,3,4,5,6,7,8,9,10};

	int i;
	for (i = 0; i < 10; i++) {
		ASSERT_TRUE(stack_push(stack, values + i)) << "Stack push shouldn't fail";
	}

	while (!stack_empty(stack) && i > 0) {
		i--;
		EXPECT_EQ(*((int*) stack_pop(stack)), values[i]);
	}
}

TEST_F(StackTestFixture, PushMultipleMalloc) {
	int* values[5];
	for(int i = 0; i < 5; i++) {
		values[i] = (int*) malloc(sizeof(int));
		ASSERT_NE(values[i], nullptr) << "Malloc failed";
		*values[i] = i;
	}

	for (int i = 0; i < 5; i++) {
		ASSERT_TRUE(stack_push(stack, values[i])) << "Stack push shouldn't fail";
		EXPECT_EQ(*((int*) stack_top(stack)), *values[i]);
	}

	stack_free(stack, FreeData);
	stack = NULL;
}
