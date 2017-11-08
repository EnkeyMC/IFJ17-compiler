#include "gtest/gtest.h"
#include "dllist.c"


class ListTestFixture : public ::testing::Test {
protected:
	DLList* l = nullptr;

	virtual void SetUp() {
		l = dllist_init(nullptr);
	}

	virtual void TearDown() {
		dllist_free(l);
	}

	static void FreeData(void* data) {
		if (data != nullptr)
			free(data);
	}
};

TEST_F(ListTestFixture, InsertFirst) {
	dllist_insert_first(l, nullptr);

	ASSERT_NE(l->first, nullptr);
	EXPECT_EQ(l->first->data, nullptr);

	dllist_insert_first(l, nullptr);

	ASSERT_NE(l->first, nullptr);
	EXPECT_EQ(l->first->data, nullptr);
	ASSERT_NE(l->first->next, nullptr);
	EXPECT_EQ(l->first->data, nullptr);
}

TEST_F(ListTestFixture, ComplexInsert) {
	int numbers[] = {1, 2, 3, 4, 5};
	dllist_insert_first(l, &numbers[0]);

	dllist_activate_first(l);

	for (int i = 1; i < 5; i++) {
		dllist_post_insert(l, &numbers[i]);
	}

	dllist_insert_first(l, &numbers[0]);

	dllist_activate_first(l);
	ASSERT_TRUE(dllist_active(l));
	EXPECT_EQ(*((int*) dllist_get_active(l)), numbers[0]);
	dllist_succ(l);
	ASSERT_TRUE(dllist_active(l));
	EXPECT_EQ(*((int*) dllist_get_active(l)), numbers[0]);
	dllist_succ(l);
	ASSERT_TRUE(dllist_active(l));
	EXPECT_EQ(*((int*) dllist_get_active(l)), numbers[4]);
	dllist_succ(l);
	ASSERT_TRUE(dllist_active(l));
	EXPECT_EQ(*((int*) dllist_get_active(l)), numbers[3]);
	dllist_succ(l);
	ASSERT_TRUE(dllist_active(l));
	EXPECT_EQ(*((int*) dllist_get_active(l)), numbers[2]);
	dllist_succ(l);
	ASSERT_TRUE(dllist_active(l));
	EXPECT_EQ(*((int*) dllist_get_active(l)), numbers[1]);
}

TEST_F(ListTestFixture, Empty) {
	EXPECT_TRUE(dllist_empty(l));
}
