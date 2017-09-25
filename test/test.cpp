/**
 * File is part of project IFJ2017.
 *
 * Brno University of Technology, Faculty of Information Technology
 *
 * @package IFJ2017
 * @authors xomach00 - Martin Omacht, xchova19 - Zdeněk Chovanec, xhendr03 - Petr Hendrych
 */

#include "superrandom.c"
#include "gtest/gtest.h"

TEST(RandomTest, SuperRandom) {
    ASSERT_EQ(superrandom(), 5);
}
