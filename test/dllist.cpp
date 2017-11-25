#include "gtest/gtest.h"
#include "dllist.c"


class ListTestFixture : public ::testing::Test {
protected:
	DLList* l = nullptr;
	static const int nnums = 5;
	int numbers[nnums] = {1, 2, 3, 4, 5};

	virtual void SetUp() {
		l = dllist_init(nullptr);
	}

	virtual void TearDown() {
		dllist_free(l);
	}
};

TEST_F(ListTestFixture, Insertfirst) {
	dllist_insert_first(l, &numbers[0]);

	ASSERT_NE(l->first, nullptr);
	EXPECT_EQ(DEREF_DATA(l->first->data, int), numbers[0]);
	EXPECT_EQ(l->first, l->last);
	EXPECT_EQ(l->first->prev, nullptr);
	EXPECT_EQ(l->first->next, nullptr);

	dllist_insert_first(l, &numbers[1]);

	ASSERT_NE(l->first, nullptr);
	EXPECT_EQ(DEREF_DATA(l->first->data, int), numbers[1]);
	ASSERT_NE(l->first->next, nullptr);
	EXPECT_EQ(DEREF_DATA(l->first->next->data, int), numbers[0]);
	EXPECT_NE(l->last, nullptr);
	EXPECT_EQ(l->last, l->first->next);
	EXPECT_EQ(l->first, l->last->prev);
}

TEST_F(ListTestFixture, ComplexInsert) {
	dllist_insert_first(l, &numbers[0]);

	dllist_activate_first(l);

	for (int i = 1; i < nnums; i++) {
		dllist_post_insert(l, &numbers[i]);
	}

	dllist_insert_first(l, &numbers[0]);

	EXPECT_EQ(dllist_length(l), 6);

	dllist_activate_first(l);
	ASSERT_TRUE(dllist_active(l));
	EXPECT_EQ(DEREF_DATA(dllist_get_active(l), int), numbers[0]);
	dllist_succ(l);
	ASSERT_TRUE(dllist_active(l));
	EXPECT_EQ(DEREF_DATA(dllist_get_active(l), int), numbers[0]);
	dllist_succ(l);
	ASSERT_TRUE(dllist_active(l));
	EXPECT_EQ(DEREF_DATA(dllist_get_active(l), int), numbers[4]);
	dllist_succ(l);
	ASSERT_TRUE(dllist_active(l));
	EXPECT_EQ(DEREF_DATA(dllist_get_active(l), int), numbers[3]);
	dllist_succ(l);
	ASSERT_TRUE(dllist_active(l));
	EXPECT_EQ(DEREF_DATA(dllist_get_active(l), int), numbers[2]);
	dllist_succ(l);
	ASSERT_TRUE(dllist_active(l));
	EXPECT_EQ(DEREF_DATA(dllist_get_active(l), int), numbers[1]);
	dllist_succ(l);
	ASSERT_FALSE(dllist_active(l));
	
	// Test values from last to first
	dllist_activate_last(l);

	ASSERT_TRUE(dllist_active(l));
	EXPECT_EQ(DEREF_DATA(dllist_get_active(l), int), numbers[1]);
	dllist_pred(l);
	ASSERT_TRUE(dllist_active(l));
	EXPECT_EQ(DEREF_DATA(dllist_get_active(l), int), numbers[2]);
	dllist_pred(l);
	ASSERT_TRUE(dllist_active(l));
	EXPECT_EQ(DEREF_DATA(dllist_get_active(l), int), numbers[3]);
	dllist_pred(l);
	ASSERT_TRUE(dllist_active(l));
	EXPECT_EQ(DEREF_DATA(dllist_get_active(l), int), numbers[4]);
	dllist_pred(l);
	ASSERT_TRUE(dllist_active(l));
	EXPECT_EQ(DEREF_DATA(dllist_get_active(l), int), numbers[0]);
	dllist_pred(l);
	ASSERT_TRUE(dllist_active(l));
	EXPECT_EQ(DEREF_DATA(dllist_get_active(l), int), numbers[0]);
	dllist_pred(l);
	ASSERT_FALSE(dllist_active(l));
}

TEST_F(ListTestFixture, Empty) {
	EXPECT_TRUE(dllist_empty(l));
}

TEST_F(ListTestFixture, InsertLastOneItem) {

	dllist_insert_last(l, &numbers[0]);

	EXPECT_EQ(
		numbers[0],
		DEREF_DATA(l->first->data, int)
	);

	EXPECT_EQ(
		l->first,
		l->last
	);

	EXPECT_EQ(
		NULL,
		l->first->prev
	);

	EXPECT_EQ(
		NULL,
		l->first->next
	);

	EXPECT_EQ(
		NULL,
		l->last->prev
	);

	EXPECT_EQ(
		NULL,
		l->last->next
	);

	EXPECT_EQ(
		NULL,
		l->active
	);

}

TEST_F(ListTestFixture, InsertLastMoreItems) {

	dllist_insert_last(l, &numbers[0]);
	dllist_insert_last(l, &numbers[1]);

	EXPECT_EQ(
		DEREF_DATA(l->first->data, int),
		numbers[0]
	);

	EXPECT_EQ(
		DEREF_DATA(l->last->data, int),
		numbers[1]
	);

	EXPECT_EQ(
		l->first,
		l->last->prev
	);

	EXPECT_EQ(
		l->last,
		l->first->next
	);

	EXPECT_EQ(
		NULL,
		l->first->prev
	);


	EXPECT_EQ(
		NULL,
		l->last->next
	);
}

TEST_F(ListTestFixture, SetActivityFirst) {
	dllist_insert_last(l, &numbers[0]);
	dllist_insert_last(l, &numbers[1]);

	dllist_activate_first(l);

	EXPECT_EQ(
		l->first,
		l->active
	);

	EXPECT_EQ(
		DEREF_DATA(l->active->data, int),
		numbers[0]
	);
}

TEST_F(ListTestFixture, SetActivityLast) {
	dllist_insert_last(l, &numbers[0]);
	dllist_insert_last(l, &numbers[1]);

	dllist_activate_last(l);

	EXPECT_EQ(
		l->last,
		l->active
	);

	EXPECT_EQ(
		DEREF_DATA(l->active->data, int),
		numbers[1]
	);
}

TEST_F(ListTestFixture, GetFirst) {
	EXPECT_EQ(dllist_get_first(l), nullptr);

	dllist_insert_last(l, &numbers[0]);
	dllist_insert_last(l, &numbers[1]);
	dllist_insert_last(l, &numbers[2]);

	EXPECT_EQ(
		DEREF_DATA(dllist_get_first(l), int),
		numbers[0]
	);
}

TEST_F(ListTestFixture, CopyListNoItem) {
	DLList *copy = dllist_copy(l);
	ASSERT_NE(copy, nullptr);

	EXPECT_EQ(dllist_get_first(copy), nullptr);
	EXPECT_EQ(dllist_get_last(copy), nullptr);
	dllist_free(copy);
}

TEST_F(ListTestFixture, CopyListOneItem) {
	dllist_insert_first(l, &numbers[0]);

	DLList *copy = dllist_copy(l);
	ASSERT_NE(copy, nullptr);

	EXPECT_EQ(DEREF_DATA(dllist_get_first(copy), int), numbers[0]);
	dllist_free(copy);

	EXPECT_EQ(DEREF_DATA(dllist_get_first(l), int), numbers[0]);
}

TEST_F(ListTestFixture, CopyListMultipleItems) {
	dllist_insert_first(l, &numbers[4]);
	dllist_insert_first(l, &numbers[3]);
	dllist_insert_first(l, &numbers[2]);
	dllist_insert_first(l, &numbers[1]);
	dllist_insert_first(l, &numbers[0]);

	DLList *copy = dllist_copy(l);
	ASSERT_NE(copy, nullptr);

	int i;
	dllist_activate_first(copy);
	for (i = 0; i < nnums; i++) {
		EXPECT_EQ(DEREF_DATA(dllist_get_active(copy), int), numbers[i]);
		dllist_succ(copy);
	}
	dllist_free(copy);

	dllist_activate_first(l);
	for (i = 0; i < nnums; i++) {
		EXPECT_EQ(DEREF_DATA(dllist_get_active(l), int), numbers[i]);
		dllist_succ(l);
	}
}

TEST_F(ListTestFixture, GetLast) {
	EXPECT_EQ(dllist_get_last(l), nullptr);

	dllist_insert_last(l, &numbers[0]);
	dllist_insert_last(l, &numbers[1]);
	dllist_insert_last(l, &numbers[2]);

	EXPECT_EQ(
		DEREF_DATA(dllist_get_last(l), int),
		numbers[2]
	);
}

TEST_F(ListTestFixture, DeleteFirst) {

	dllist_insert_first(l, &numbers[0]);
	dllist_insert_last(l, &numbers[1]);

	dllist_activate_first(l);

	EXPECT_TRUE(dllist_active(l));

	EXPECT_EQ(
		DEREF_DATA(dllist_delete_first(l), int),
		numbers[0]
	);

	EXPECT_FALSE(dllist_active(l));

	EXPECT_EQ(
		numbers[1],
		DEREF_DATA(l->first->data, int)
	);

	EXPECT_EQ(
		l->first,
		l->last
	);

	EXPECT_EQ(
		NULL,
		l->first->prev
	);

	EXPECT_EQ(
		NULL,
		l->first->next
	);

	EXPECT_EQ(
		NULL,
		l->last->prev
	);

	EXPECT_EQ(
		NULL,
		l->last->next
	);

	EXPECT_EQ(
		NULL,
		l->active
	);
}

TEST_F(ListTestFixture, DeleteLast) {

	dllist_insert_first(l, &numbers[0]);
	dllist_insert_last(l, &numbers[1]);

	dllist_activate_last(l);

	EXPECT_TRUE(dllist_active(l));

	EXPECT_EQ(
		DEREF_DATA(dllist_delete_last(l), int),
		numbers[1]
	);

	EXPECT_FALSE(dllist_active(l));

	EXPECT_EQ(
		numbers[0],
		DEREF_DATA(l->last->data, int)
	);

	EXPECT_EQ(
		l->first,
		l->last
	);

	EXPECT_EQ(
		NULL,
		l->first->prev
	);

	EXPECT_EQ(
		NULL,
		l->first->next
	);

	EXPECT_EQ(
		NULL,
		l->last->prev
	);

	EXPECT_EQ(
		NULL,
		l->last->next
	);

	EXPECT_EQ(
		NULL,
		l->active
	);
}

TEST_F(ListTestFixture, ListActive) {

	dllist_insert_first(l, &numbers[0]);
	dllist_insert_last(l, &numbers[1]);
	dllist_insert_last(l, &numbers[2]);

	dllist_activate_first(l);

	EXPECT_TRUE(dllist_active(l));

	dllist_pred(l);

	EXPECT_FALSE(dllist_active(l));

	dllist_activate_first(l);

	EXPECT_TRUE(dllist_active(l));

	dllist_succ(l);

	EXPECT_TRUE(dllist_active(l));

	dllist_succ(l);

	EXPECT_TRUE(dllist_active(l));

	dllist_succ(l);

	EXPECT_FALSE(dllist_active(l));
}

TEST_F(ListTestFixture, TestUpdate) {

	dllist_insert_last(l, &numbers[0]);
	dllist_insert_last(l, &numbers[1]);
	dllist_insert_last(l, &numbers[2]);

	dllist_activate_first(l);

	dllist_succ(l);

	dllist_update(l, &numbers[3]);

	EXPECT_EQ(
		DEREF_DATA(l->active->data, int),
		numbers[3]
	);

	EXPECT_EQ(
		DEREF_DATA(l->first->next->data, int),
		numbers[3]
	);

	dllist_activate_first(l);
	dllist_pred(l);

	EXPECT_EQ(dllist_update(l, &numbers[4]), nullptr);
}

TEST_F(ListTestFixture, GetActive) {
	dllist_insert_first(l, &numbers[0]);
	dllist_insert_last(l, &numbers[1]);
	dllist_insert_last(l, &numbers[2]);

	dllist_activate_first(l);

	EXPECT_EQ(
		DEREF_DATA(dllist_get_active(l), int),
		numbers[0]
	);

	dllist_succ(l);

	EXPECT_EQ(
		DEREF_DATA(dllist_get_active(l), int),
		numbers[1]
	);

	dllist_succ(l);

	EXPECT_EQ(
		DEREF_DATA(dllist_get_active(l), int),
		numbers[2]
	);

	dllist_succ(l);

	EXPECT_EQ(
		dllist_get_active(l),
		nullptr
	);
}

TEST_F(ListTestFixture, CopyFirstAndLast) {
	dllist_insert_first(l, &numbers[0]);
	dllist_insert_first(l, &numbers[1]);
	dllist_insert_first(l, &numbers[2]);

	EXPECT_EQ(
		DEREF_DATA(dllist_get_first(l), int),
		numbers[2]
	);


	EXPECT_EQ(
		DEREF_DATA(dllist_get_last(l), int),
		numbers[0]
	);

	for (int i = 0; i < 3; i++)
		dllist_delete_first(l);

	EXPECT_EQ(
		dllist_get_first(l),
		nullptr
	);

	EXPECT_EQ(
		dllist_get_last(l),
		nullptr
	);
}

TEST_F(ListTestFixture, AddAfterAct) {

	dllist_insert_first(l, &numbers[0]);

	dllist_activate_first(l);

	dllist_post_insert(l, &numbers[1]);

	EXPECT_EQ(
		DEREF_DATA(l->first->data, int),
		numbers[0]
	);

	EXPECT_EQ(
		DEREF_DATA(l->first->next->data, int),
		numbers[1]
	);

	EXPECT_EQ(
		NULL,
		l->first->next->next
	);

	dllist_post_insert(l, &numbers[2]);

	EXPECT_EQ(
		DEREF_DATA(l->first->data, int),
		numbers[0]
	);

	EXPECT_EQ(
		DEREF_DATA(l->first->next->data, int),
		numbers[2]
	);

	EXPECT_EQ(
		DEREF_DATA(l->first->next->next->data, int),
		numbers[1]
	);
}

TEST_F(ListTestFixture, AddBeforeACt) {

	dllist_insert_first(l, &numbers[0]);

	dllist_activate_first(l);

	dllist_pre_insert(l, &numbers[1]);

	EXPECT_EQ(
		DEREF_DATA(l->first->data, int),
		numbers[1]
	);

	EXPECT_EQ(
		DEREF_DATA(l->first->next->data, int),
		numbers[0]
	);

	EXPECT_EQ(
		NULL,
		l->first->next->next
	);

	dllist_pre_insert(l, &numbers[2]);

	EXPECT_EQ(
		DEREF_DATA(l->first->data, int),
		numbers[1]
	);

	EXPECT_EQ(
		DEREF_DATA(l->first->next->data, int),
		numbers[2]
	);

	EXPECT_EQ(
		DEREF_DATA(l->first->next->next->data, int),
		numbers[0]
	);
}

TEST_F(ListTestFixture, DeleteAndSucc) {
	for (int i = 0; i < nnums; i++) {
		dllist_insert_last(l, &numbers[i]);
	}

	dllist_activate_first(l);

	int i;
	for(i = 0; i < nnums && dllist_active(l); i++) {
		EXPECT_EQ(DEREF_DATA(dllist_delete_and_succ(l), int), numbers[i]);
	}

	int final_i = nnums;

	EXPECT_EQ(i, final_i);

	EXPECT_TRUE(dllist_empty(l));

	EXPECT_EQ(l->first, nullptr);
	EXPECT_EQ(l->last, nullptr);
	EXPECT_EQ(l->active, nullptr);
}

TEST_F(ListTestFixture, DeleteAndPrev) {
	for (int i = 0; i < nnums; i++) {
		dllist_insert_last(l, &numbers[i]);
	}

	dllist_activate_last(l);

	int i;
	for(i = nnums - 1; i >= 0 && dllist_active(l); i--) {
		EXPECT_EQ(DEREF_DATA(dllist_delete_and_pred(l), int), numbers[i]);
	}

	EXPECT_EQ(i, -1);

	EXPECT_TRUE(dllist_empty(l));

	EXPECT_EQ(l->first, nullptr);
	EXPECT_EQ(l->last, nullptr);
	EXPECT_EQ(l->active, nullptr);
}
