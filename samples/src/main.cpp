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
 * This file is a part of tip5xx library
 *
 */

#include <iostream>
#include <iomanip>
#include <vector>
#include "tip5xx/tip5xx.hpp"

// Helper function to print hash result
void print_hash(const std::vector<uint8_t>& hash) {
    for (const auto& byte : hash) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    std::cout << std::endl;
}

int main() {
    try {
        // Example of hash_pair
        std::vector<uint8_t> input1 = {1, 2, 3, 4};
        std::vector<uint8_t> input2 = {5, 6, 7, 8};

        std::cout << "Hash pair example:" << std::endl;
        auto pair_result = tip5xx::Tip5::hash_pair(input1, input2);
        std::cout << "hash_pair result: ";
        print_hash(pair_result);

        // Example of hash_varlen
        std::vector<std::vector<uint8_t>> inputs = {
            {1, 2, 3, 4},
            {5, 6, 7, 8},
            {9, 10, 11, 12}
        };

        std::cout << "\nHash varlen example:" << std::endl;
        auto varlen_result = tip5xx::Tip5::hash_varlen(inputs);
        std::cout << "hash_varlen result: ";
        print_hash(varlen_result);

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
