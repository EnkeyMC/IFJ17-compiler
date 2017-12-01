/**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */

#include "gtest/gtest.h"
#include "memory_manager.c"

TEST(MemoryManagerTest, InitFreeTest) {
	mem_manager_init();
	mem_manager_free();
}

TEST(MemoryManagerTest, SimpleAllocAndFree) {
	mem_manager_init();
	EXPECT_EQ(mm.size, (unsigned) MEMORY_CHUNK);

	auto ptr = (int*) mm_malloc(sizeof(int));
	EXPECT_EQ(mm.first_free, (unsigned) 1);
	ASSERT_NE(ptr, nullptr);
	*ptr = 8;
	mm_free(ptr);
	EXPECT_EQ(mm.first_free, (unsigned) 0);

	mem_manager_free();
	EXPECT_EQ(mm.first_free, (unsigned) 0);
}

TEST(MemoryManagerTest, MultipleAllocsNoFree) {
	mem_manager_init();

	int* arr[5];

	for (auto &i : arr) {
		i = (int*) mm_malloc(sizeof(int));
	}

	EXPECT_EQ(mm.first_free, (unsigned) 5);

	mem_manager_free();
	EXPECT_EQ(mm.first_free, (unsigned) 0);
}

TEST(MemoryManagerTest, MultipleAllocsAndFrees) {
	mem_manager_init();

	int* arr[5];

	for (auto &i : arr) {
		i = (int*) mm_malloc(sizeof(int));
	}

	EXPECT_EQ(mm.first_free, (unsigned) 5);

	for (auto &i : arr) {
		mm_free(i);
	}

	EXPECT_EQ(mm.first_free, (unsigned) 0);

	mem_manager_free();
}

TEST(MemoryManagerTest, MemChunkAlloc) {
	mem_manager_init();

	int* arr[MEMORY_CHUNK+1];

	for (auto &i : arr) {
		i  = (int*) mm_malloc(sizeof(int));
	}

	EXPECT_EQ(mm.first_free, (unsigned) MEMORY_CHUNK + 1);
	EXPECT_EQ(mm.size, (unsigned) MEMORY_CHUNK * 2);

	for (auto &i : arr) {
		mm_free(i);
	}

	EXPECT_EQ(mm.first_free, (unsigned) 0);
	mem_manager_free();
}

TEST(MemoryManagerTest, AllocsAndFreesMixed) {
	mem_manager_init();

	int* arr[3];

	arr[0] = (int*) mm_malloc(sizeof(int));
	EXPECT_EQ(mm.first_free, (unsigned) 1);
	arr[1] = (int*) mm_malloc(sizeof(int));
	EXPECT_EQ(mm.first_free, (unsigned) 2);
	mm_free(arr[0]);
	EXPECT_EQ(mm.first_free, (unsigned) 1);
	arr[0] = (int*) mm_malloc(sizeof(int));
	EXPECT_EQ(mm.first_free, (unsigned) 2);
	arr[2] = (int*) mm_malloc(sizeof(int));
	EXPECT_EQ(mm.first_free, (unsigned) 3);
	mm_free(arr[1]);
	EXPECT_EQ(mm.first_free, (unsigned) 2);
	mm_free(arr[2]);
	EXPECT_EQ(mm.first_free, (unsigned) 1);
	arr[1] = (int*) mm_malloc(sizeof(int));
	EXPECT_EQ(mm.first_free, (unsigned) 2);
	mm_free(arr[0]);
	EXPECT_EQ(mm.first_free, (unsigned) 1);

	mem_manager_free();
	EXPECT_EQ(mm.first_free, (unsigned) 0);
}

TEST(MemoryManagerTest, Realloc) {
	mem_manager_init();

	auto ptr = (int*) mm_malloc(sizeof(int));
	EXPECT_EQ(mm.first_free, (unsigned) 1);
	ptr = (int*) mm_realloc(ptr, sizeof(int) * 2);
	EXPECT_EQ(mm.first_free, (unsigned) 1);
	mm_free(ptr);
	EXPECT_EQ(mm.first_free, (unsigned) 0);

	mem_manager_free();
}
