/**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */

#include "gtest/gtest.h"
#include "scanner.c"

class ScannerTestFixture : public ::testing::Test {
protected:

	FILE* test_file;

	virtual void SetUp() {

	}

	virtual void TearDown() {
		fclose(test_file);
	}

	void SetInputFile(const char *file) {
		test_file = fopen(file, "r");

		set_input_stream(test_file);

		if (test_file == NULL)
			perror("Error");
	}
};

TEST_F(ScannerTestFixture, EmptyFile) {
	SetInputFile("test_files/empty.fbc");

	token_t* token = get_token();

	EXPECT_EQ(
		token,
		nullptr
	);
}
