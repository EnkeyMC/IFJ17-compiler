#include "gtest/gtest.h"
#include "sparse_table.c"


class SparseTableTestFixture : public ::testing::Test {
protected:
	SparseTable* sparse_table = nullptr;
	unsigned int nrows = 5;
	unsigned int ncols = 4;
	int dominant = 0;

	virtual void SetUp() {
		sparse_table = sparse_table_init(nrows, ncols, dominant);
	}

	virtual void TearDown() {
		EXPECT_NO_FATAL_FAILURE(sparse_table_free(sparse_table));
	}
};

class SparseTableWithDataTestFixture : public ::testing::Test {
protected:
	SparseTable* sparse_table = nullptr;
	unsigned int nrows = 5;
	unsigned int ncols = 4;
	int dominant = 0;

	virtual void SetUp() {
		sparse_table = sparse_table_init(nrows, ncols, dominant);

		sparse_table_set(sparse_table, 0, 0, 1);
		sparse_table_set(sparse_table, 3, 3, 2);
		sparse_table_set(sparse_table, 3, 2, 8);
		sparse_table_set(sparse_table, 4, 2, 6);
	}

	virtual void TearDown() {
		EXPECT_NO_FATAL_FAILURE(sparse_table_free(sparse_table));
	}
};

TEST_F(SparseTableTestFixture, InitTest) {
	for (unsigned int row = 0; row < nrows; row++) {
		for (unsigned int col = 0; col < ncols; col++) {
			EXPECT_EQ(sparse_table_get(sparse_table, row, col), dominant) << "Empty sparse table should return dominant value on every index";
		}
	}
}

TEST_F(SparseTableTestFixture, SetTest) {
	for (unsigned int row = 0; row < nrows; row++) {
		for (unsigned int col = 0; col < ncols; col++) {
			EXPECT_TRUE(sparse_table_set(sparse_table, row, col, row + col)) << "Setting value to valid index should not fail";
		}
	}
}

TEST_F(SparseTableTestFixture, SetInvalid) {
	EXPECT_FALSE(sparse_table_set(sparse_table, nrows, ncols, -1)) << "Setting value to invalid index should return false";
}

TEST_F(SparseTableWithDataTestFixture, GetTest) {
	EXPECT_EQ(sparse_table_get(sparse_table, 0, 0), 1);
	EXPECT_EQ(sparse_table_get(sparse_table, 3, 3), 2);
	EXPECT_EQ(sparse_table_get(sparse_table, 4, 2), 6);
	EXPECT_EQ(sparse_table_get(sparse_table, 3, 2), 8);
}

TEST_F(SparseTableWithDataTestFixture, ChangeValue) {
	EXPECT_TRUE(sparse_table_set(sparse_table, 3, 3, 42)) << "Setting value to valid index should not fail";
	EXPECT_EQ(sparse_table_get(sparse_table, 3, 3), 42) << "The value should change to 42";
	EXPECT_TRUE(sparse_table_set(sparse_table, 0, 0, 42)) << "Setting value to valid index should not fail";
	EXPECT_EQ(sparse_table_get(sparse_table, 0, 0), 42) << "The value should change to 42";;
	EXPECT_TRUE(sparse_table_set(sparse_table, 3, 2, 42)) << "Setting value to valid index should not fail";
	EXPECT_EQ(sparse_table_get(sparse_table, 3, 2), 42) << "The value should change to 42";
}

