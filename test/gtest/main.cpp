// SPDX-License-Identifier: AGPL-3.0-or-later
#include <iostream>

#include <gtest/gtest.h>

#include "testutils.h"

using namespace std;

/// \brief Testing program.
/// \brief Main function running all the (G)tests.
int main(int argc, char** argv)
{

    int res;

    /***********************************************************
     *                       Gtests                            *
     ***********************************************************/

    // Initialize Google Test framework
    ::testing::InitGoogleTest(&argc, argv);

    // Run all the tests
    auto result = RUN_ALL_TESTS();

    // Avoid memory leaks
    TestUtils::cleanEnv();

    return result;
}
