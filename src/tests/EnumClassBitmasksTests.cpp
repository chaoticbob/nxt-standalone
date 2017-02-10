// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gtest/gtest.h"

#include "nxt/EnumClassBitmasks.h"

namespace nxt {

    enum class Color : uint32_t {
        R = 1,
        G = 2,
        B = 4,
        A = 8,
    };

    template<>
    struct IsNXTBitmask<Color> {
        static constexpr bool enable = true;
    };

    TEST(BitmaskTests, BasicOperations) {
        Color test1 = Color::R | Color::G;
        ASSERT_EQ(1 | 2, static_cast<uint32_t>(test1));

        Color test2 = test1 ^ (Color::R | Color::A);
        ASSERT_EQ(2 | 8, static_cast<uint32_t>(test2));

        Color test3 = test2 & Color::A;
        ASSERT_EQ(8, static_cast<uint32_t>(test3));

        Color test4 = ~test3;
        ASSERT_EQ(~uint32_t(8), static_cast<uint32_t>(test4));
    }

    TEST(BitmaskTests, AssignOperations) {
        Color test1 = Color::R;
        test1 |= Color::G;
        ASSERT_EQ(1 | 2, static_cast<uint32_t>(test1));

        Color test2 = test1;
        test2 ^= (Color::R | Color::A);
        ASSERT_EQ(2 | 8, static_cast<uint32_t>(test2));

        Color test3 = test2;
        test3 &= Color::A;
        ASSERT_EQ(8, static_cast<uint32_t>(test3));
    }

    TEST(BitmaskTests, BoolConversion) {
        bool test1 = Color::R | Color::G;
        ASSERT_TRUE(test1);

        bool test2 = Color::R & Color::G;
        ASSERT_FALSE(test2);

        bool test3 = Color::R ^ Color::G;
        ASSERT_TRUE(test3);

        if (Color::R & ~Color::R) {
            ASSERT_TRUE(false);
        }
    }

    TEST(BitmaskTests, ThreeOrs) {
        Color c = Color::R | Color::G | Color::B;
        ASSERT_EQ(7, static_cast<uint32_t>(c));
    }

}
