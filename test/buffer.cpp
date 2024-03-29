#include <malloc.h>
#include "gtest/gtest.h"
#include "buffer.c"

class BufferDataTestFixture : public ::testing::Test {
protected:
	Buffer* buffer = nullptr;

	virtual void SetUp() {
		mem_manager_init();
		buffer = buffer_init(0);
	}

	virtual void TearDown() {
		EXPECT_NO_FATAL_FAILURE(buffer_free(buffer));
		mem_manager_free();
	}
};

TEST_F(BufferDataTestFixture, Initialization) {
	EXPECT_NE(buffer, nullptr) << "Initialized buffer is not null\n";
}

TEST_F(BufferDataTestFixture, AppendChar) {

	static const size_t num_of_chars = 20;
	const char *s = "very_long_identifier";

	unsigned int i = 0;
	for ( ; i < num_of_chars; i++) {
		buffer_append_c(buffer, s[i]);
	}

	EXPECT_STREQ(
			buffer->str,
			"very_long_identifier"
	) << "Buffer should contain inserted text\n";

}

TEST_F(BufferDataTestFixture, AppendStr) {

	static const size_t num_of_str = 4;
	const char* str[num_of_str] = { "str_to_append_1, ", "str_to_append_2, ", "str_to_append_3, ", "str_to_append_4\n" };

	for (auto &s : str) {
		buffer_append_str(buffer, s);
	}

	EXPECT_STREQ(
			buffer->str,
			"str_to_append_1, str_to_append_2, str_to_append_3, str_to_append_4\n"
	) << "Buffer should contain inserted text\n";
}

TEST_F(BufferDataTestFixture, SetStr) {

	static const size_t num_of_str = 4;
	const char* str[num_of_str] = { "str11", "str22", "str33", "str44" };

	for (auto &s : str) {
		buffer_set_str(buffer, s);

		EXPECT_STREQ(
				buffer->str,
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

	buffer_clear(buffer);

	EXPECT_EQ(
			buffer->len,
			(unsigned) 0
	) << "Length should be zero\n";
}
