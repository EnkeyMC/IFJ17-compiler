//
// Created by MOmac on 21.09.2017.
//

#include "superrandom.c"
#include "gtest/gtest.h"

TEST(RandomTest, SuperRandom) {
    ASSERT_EQ(superrandom(), 5);
}