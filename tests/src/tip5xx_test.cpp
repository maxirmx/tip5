/**
 *
 * Copyright (c) 2025 Maxim [maxirmx] Samsonov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * This fiel is a part of tip5xx library
 *
 */

#include <gtest/gtest.h>
#include "tip5xx/tip5xx.hpp"

// Helper function to create test vectors
std::vector<uint8_t> make_test_vector(const std::initializer_list<uint8_t>& data) {
    return std::vector<uint8_t>(data);
}

TEST(Tip5HashTest, HashPairHandlesBasicInput) {
    auto left = make_test_vector({1, 2, 3, 4});
    auto right = make_test_vector({5, 6, 7, 8});

    auto result = tip5xx::Tip5::hash_pair(left, right);

    EXPECT_EQ(result.size(), 32); // Check output size is correct
    EXPECT_NE(result, std::vector<uint8_t>(32, 0)); // Check it's not all zeros
}

TEST(Tip5HashTest, HashPairHandlesEmptyInputs) {
    auto empty = std::vector<uint8_t>();
    auto some_data = make_test_vector({1, 2, 3, 4});

    auto result1 = tip5xx::Tip5::hash_pair(empty, empty);
    auto result2 = tip5xx::Tip5::hash_pair(some_data, empty);
    auto result3 = tip5xx::Tip5::hash_pair(empty, some_data);

    EXPECT_EQ(result1.size(), 32);
    EXPECT_EQ(result2.size(), 32);
    EXPECT_EQ(result3.size(), 32);
}

TEST(Tip5HashTest, HashVarlenHandlesEmptyInput) {
    std::vector<std::vector<uint8_t>> empty_input;
    auto result = tip5xx::Tip5::hash_varlen(empty_input);

    EXPECT_EQ(result.size(), 32);
    EXPECT_EQ(result, std::vector<uint8_t>(32, 0)); // Should be zero hash for empty input
}

TEST(Tip5HashTest, HashVarlenHandlesSingleInput) {
    auto input = make_test_vector({1, 2, 3, 4});
    auto result = tip5xx::Tip5::hash_varlen({input});

    EXPECT_EQ(result.size(), 32);
}

TEST(Tip5HashTest, HashVarlenHandlesMultipleInputs) {
    auto input1 = make_test_vector({1, 2, 3, 4});
    auto input2 = make_test_vector({5, 6, 7, 8});
    auto input3 = make_test_vector({9, 10, 11, 12});

    auto result = tip5xx::Tip5::hash_varlen({input1, input2, input3});

    EXPECT_EQ(result.size(), 32);
    EXPECT_NE(result, std::vector<uint8_t>(32, 0));
}
