// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

#define GTEST_MAIN                                                                                                     \
    int main(int argc, char** argv)                                                                                    \
    {                                                                                                                  \
        ::testing::InitGoogleTest(&argc, argv);                                                                        \
        return RUN_ALL_TESTS();                                                                                        \
    }
