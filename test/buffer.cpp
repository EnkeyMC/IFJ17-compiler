#include <malloc.h>
#include "gtest/gtest.h"
#include "buffer.c"

class BufferDataTestFixture : public ::testing::Test {
protected:
	Buffer* buffer = nullptr;

	virtual void SetUp() {
		buffer = buffer_init(0);
	}

	virtual void TearDown() {
		EXPECT_NO_FATAL_FAILURE(buffer_free(buffer));
	}
};

TEST_F(BufferDataTestFixture, Initialization) {
	EXPECT_NE(buffer, nullptr) << "Initialized buffer is not null\n";
}

TEST_F(BufferDataTestFixture, AppendChar) {

	static const size_t num_of_chars = 20;
	const char *s = "very_long_identifier";

	int i = 0;
	for ( ; i < num_of_chars; i++) {
		EXPECT_EQ(
			buffer_append_c(buffer, s[i]),
			true
		) << "Function should insert all chars\n";
	}

	EXPECT_STREQ(
			buffer->arr,
			"very_long_identifier"
	) << "Buffer should contain inserted text\n";

}

TEST_F(BufferDataTestFixture, AppendStr) {

	static const size_t num_of_str = 4;
	const char* str[num_of_str] = { "str_to_append_1, ", "str_to_append_2, ", "str_to_append_3, ", "str_to_append_4\n" };

	for (auto &s : str) {
		EXPECT_EQ(
			buffer_append_str(buffer, s),
			true
		) << "Function should insert all strings\n";
	}

	EXPECT_STREQ(
			buffer->arr,
			"str_to_append_1, str_to_append_2, str_to_append_3, str_to_append_4\n"
	) << "Buffer should contain inserted text\n";
}

TEST_F(BufferDataTestFixture, SetStr) {

	static const size_t num_of_str = 4;
	const char* str[num_of_str] = { "str11", "str22", "str33", "str44" };

	for (auto &s : str) {
		EXPECT_EQ(
			buffer_set_str(buffer, s),
			true
		) << "Function should insert all strings\n";

		EXPECT_STREQ(
				buffer->arr,
				s
		) << "Buffer should contain string\n";
	}
}

TEST_F(BufferDataTestFixture, BufferClear) {
	static const size_t num_of_str = 8;
	const char* str[num_of_str] = { "str11", "str22", "str33", "str44", "str11", "str22", "str33", "str44" };

	for (auto &s : str) {
		buffer_append_str(buffer, s);
	}

	EXPECT_EQ(
			buffer_clear(buffer),
			true
	) << "Clearing the buffer should success\n";

	EXPECT_EQ(
			buffer->len,
			0
	) << "Length should be zero\n";
}
