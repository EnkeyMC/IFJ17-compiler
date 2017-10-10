#include <malloc.h>
#include "gtest/gtest.h"
#include "symtable.c"

int foreach_cnt = 0;

void foreach_count(htab_item_t *item_ptr) {
	foreach_cnt++;
}

class HashTableTestFixture : public ::testing::Test {
protected:
	htab_t* hash_table = nullptr;

	virtual void SetUp() {
		hash_table = htab_init(8);
		foreach_cnt = 0;
	}

	virtual void TearDown() {
		EXPECT_NO_FATAL_FAILURE(htab_free(hash_table));
	}
};

class HashTableWithDataTestFixture : public ::testing::Test {
protected:
	htab_t* hash_table = nullptr;
	static const size_t n_samples = 5;
	const char* keys[n_samples] = {"test1", "test2", "test3", "test4", "test5"};

	virtual void SetUp() {
		foreach_cnt = 0;
		hash_table = htab_init(n_samples);

		// Insert items
		for (auto &key : keys) {
			htab_lookup(hash_table, key);
		}
	}

	virtual void TearDown() {
		EXPECT_NO_FATAL_FAILURE(htab_free(hash_table));
	}
};

TEST_F(HashTableTestFixture, Initialization) {
	EXPECT_NE(hash_table, nullptr) << "Initialized hash table is not null";
}

TEST_F(HashTableTestFixture, SizeEmpty) {
	EXPECT_EQ(
		htab_size(hash_table),
		0
	) << "Size is not 0";
}

TEST_F(HashTableTestFixture, InsertItems) {
	constexpr size_t n_samples = 3;
	const char *keys[n_samples] = {"test1", "test2", "test3"};

	// Test function
	ASSERT_EQ(
		htab_lookup(nullptr, "null"),
		nullptr
	) << "No item should be created with nullptr passed as table";

	// Insert items
	for (auto &key : keys) {
		EXPECT_NE(
			htab_lookup(hash_table, key),
		nullptr
		) << "Function should return item ptr";
	}

	// Check size
	EXPECT_EQ(
		htab_size(hash_table),
		n_samples
	) << "Hash table should have " << n_samples << " items";
}

TEST_F(HashTableTestFixture, MemoryDeallocation) {
// TODO: Test memory deallocation
}

TEST_F(HashTableTestFixture, FindOnEmptyTable) {
	ASSERT_EQ(
		htab_find(nullptr, "nokey"),
		nullptr
	) << "nullptr as hash table should return nullptr";

	EXPECT_EQ(
		htab_find(hash_table, "nokey"),
		nullptr
	) << "Empty hash table should return nullptr";
}

TEST_F(HashTableWithDataTestFixture, FindInvalidItem) {
	htab_item_t* item = htab_find(hash_table, "invalid");

	ASSERT_EQ(
		item,
		nullptr
	) << "Found item should be nullptr";
}

TEST_F(HashTableWithDataTestFixture, FindValidItem) {

	htab_item_t* item = htab_find(hash_table, keys[1]);

	ASSERT_NE(
		item,
		nullptr
	) << "Found item should not be nullptr";

	EXPECT_STREQ(
		item->key,
		keys[1]
	) << "The items key should be equal to searched key";
}

TEST_F(HashTableWithDataTestFixture, RemoveInvalidItem) {
	EXPECT_FALSE(htab_remove(hash_table, "invalid")) << "Invalid key should return false";
}

TEST_F(HashTableWithDataTestFixture, DeleteValidItem) {
	EXPECT_TRUE(htab_remove(hash_table, keys[2])) << "Deleting valid key should return true";
}

TEST_F(HashTableTestFixture, RemoveOnEmptyTable) {
	ASSERT_FALSE(htab_remove(hash_table, "nokey")) << "Empty table should return false";
}


TEST_F(HashTableTestFixture, InvalidRemove) {
	ASSERT_FALSE(htab_remove(nullptr, "nokey")) << "Null table should return false";
	ASSERT_FALSE(htab_remove(hash_table, nullptr)) << "Null key should return false";
}

TEST_F(HashTableTestFixture, ForeachInvalid) {
	htab_foreach(nullptr, foreach_count);

	EXPECT_EQ(
		foreach_cnt,
		0
	) << "Callback function should not be called";
}

TEST_F(HashTableTestFixture, ForeachOnEmptyTable) {
	htab_foreach(hash_table, foreach_count);

	EXPECT_EQ(
		foreach_cnt,
		0
	) << "Callback function should not be called";
}

TEST_F(HashTableWithDataTestFixture, Foreach) {
	htab_foreach(hash_table, foreach_count);

	EXPECT_EQ(
		foreach_cnt,
		5
	) << "Callback function should be called " << 5 << " times";
}
