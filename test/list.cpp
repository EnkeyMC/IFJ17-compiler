#include "gtest/gtest.h"
#include "list.c"


class ListTestFixture : public ::testing::Test {
protected:
	List* l = nullptr;

	virtual void SetUp() {
		l = list_init(nullptr);
	}

	virtual void TearDown() {
		list_free(l);
	}

	static void FreeData(void* data) {
		if (data != nullptr)
			free(data);
	}
};

TEST_F(ListTestFixture, InsertFirst) {
	list_insert_first(l, nullptr);

	ASSERT_NE(l->first, nullptr);
	EXPECT_EQ(l->first->data, nullptr);

	list_insert_first(l, nullptr);

	ASSERT_NE(l->first, nullptr);
	EXPECT_EQ(l->first->data, nullptr);
	ASSERT_NE(l->first->next, nullptr);
	EXPECT_EQ(l->first->data, nullptr);
}

TEST_F(ListTestFixture, ComplexInsert) {
	int numbers[] = {1, 2, 3, 4, 5};
	list_insert_first(l, &numbers[0]);

	list_activate(l);

	for (int i = 1; i < 5; i++) {
		list_post_insert(l, &numbers[i]);
	}

	list_insert_first(l, &numbers[0]);

	list_activate(l);
	ASSERT_TRUE(list_active(l));
	EXPECT_EQ(*((int*)list_get_active(l)), numbers[0]);
	list_succ(l);
	ASSERT_TRUE(list_active(l));
	EXPECT_EQ(*((int*)list_get_active(l)), numbers[0]);
	list_succ(l);
	ASSERT_TRUE(list_active(l));
	EXPECT_EQ(*((int*)list_get_active(l)), numbers[4]);
	list_succ(l);
	ASSERT_TRUE(list_active(l));
	EXPECT_EQ(*((int*)list_get_active(l)), numbers[3]);
	list_succ(l);
	ASSERT_TRUE(list_active(l));
	EXPECT_EQ(*((int*)list_get_active(l)), numbers[2]);
	list_succ(l);
	ASSERT_TRUE(list_active(l));
	EXPECT_EQ(*((int*)list_get_active(l)), numbers[1]);
}

TEST_F(ListTestFixture, Empty) {
	EXPECT_TRUE(list_empty(l));
}
