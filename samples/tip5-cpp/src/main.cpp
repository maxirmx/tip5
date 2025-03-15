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
#include <string>
#include "tip5xx/tip5xx.hpp"
#include <CLI/CLI.hpp>

// Helper function to print digest result
void print_digest(const tip5xx::Digest& digest) {
    std::cout << "Digest(";
    for (size_t i = 0; i < tip5xx::Digest::LEN; ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << digest[i];
    }
    std::cout << ")" << std::endl;
}

// Helper function to parse a number in various formats
tip5xx::BFieldElement parse_number(const std::string& input) {
    std::string trimmed = input;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r\f\v"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r\f\v") + 1);

    try {
        if (trimmed.substr(0, 2) == "0x" || trimmed.substr(0, 2) == "0X") {
            // Hexadecimal
            uint64_t value = std::stoull(trimmed.substr(2), nullptr, 16);
            return tip5xx::BFieldElement::new_element(value);
        } else {
            // Decimal
            uint64_t value = std::stoull(trimmed, nullptr, 10);
            return tip5xx::BFieldElement::new_element(value);
        }
    } catch (const std::exception& e) {
        throw std::runtime_error("Invalid number format: " + input);
    }
}

// Helper function to parse a digest in (n1,n2,n3,n4,n5) format
tip5xx::Digest parse_digest(const std::string& input) {
    // Remove whitespace
    std::string str = input;
    str.erase(0, str.find_first_not_of(" \t\n\r\f\v"));
    str.erase(str.find_last_not_of(" \t\n\r\f\v") + 1);

    // Check and remove parentheses
    if (str.empty() || str[0] != '(' || str[str.length()-1] != ')') {
        throw std::runtime_error("Digest must be enclosed in parentheses");
    }
    str = str.substr(1, str.length()-2);

    // Split into numbers
    std::vector<std::string> numbers;
    size_t start = 0, end = 0;
    while ((end = str.find(',', start)) != std::string::npos) {
        numbers.push_back(str.substr(start, end - start));
        start = end + 1;
    }
    numbers.push_back(str.substr(start));

    // Validate number count
    if (numbers.size() != 5) {
        throw std::runtime_error("Each digest must contain exactly 5 numbers");
    }

    // Parse numbers and create digest
    std::array<tip5xx::BFieldElement, 5> elements;
    for (size_t i = 0; i < 5; ++i) {
        elements[i] = parse_number(numbers[i]);
    }

    return tip5xx::Digest(elements);
}

int main(int argc, char** argv) {
    CLI::App app{"TIP5 Hash Calculator"};

    // Command line options
    std::string mode = "pair";
    app.add_option("-m,--mode", mode, "Hash mode: 'pair' or 'varlen'")->check(CLI::IsMember({"pair", "varlen"}));

    std::vector<std::string> inputs;
    app.add_option("inputs", inputs, "Input digests")->required()
       ->description("For pair mode: provide exactly 2 digests\n"
                    "For varlen mode: provide 2 or more digests\n"
                    "Each digest must be in format (n1,n2,n3,n4,n5) where each number can be:\n"
                    "- Hexadecimal: 0x1F (must use 0x prefix)\n"
                    "- Decimal: 42 (numbers starting with 0 like 077 are treated as decimal)");

    CLI11_PARSE(app, argc, argv);

    try {
        if (mode == "pair") {
            if (inputs.size() != 2) {
                std::cerr << "Error: pair mode requires exactly 2 digests" << std::endl;
                return 1;
            }

            auto digest1 = parse_digest(inputs[0]);
            auto digest2 = parse_digest(inputs[1]);

            std::cout << "Hash pair mode Digest(" << inputs[0] << "), Digest(" << inputs[1] << ")" << std::endl;
            auto pair_result = tip5xx::Tip5::hash_pair(digest1, digest2);
            std::cout << "Result: ";
            print_digest(pair_result);
        }
        else { // varlen mode
            if (inputs.size() < 2) {
                std::cerr << "Error: varlen mode requires at least 2 digests" << std::endl;
                return 1;
            }

            std::vector<tip5xx::BFieldElement> elements;
            for (const auto& input : inputs) {
                auto digest = parse_digest(input);
                for (size_t i = 0; i < tip5xx::Digest::LEN; ++i) {
                    elements.push_back(digest[i]);
                }
            }

            std::cout << "Hash varlen mode [";
            for (size_t i = 0; i < inputs.size(); ++i) {
                std::cout << inputs[i];
                if (i < inputs.size() - 1) std::cout << ", ";
            }
            std::cout << "]:" << std::endl;
            auto varlen_result = tip5xx::Tip5::hash_varlen(elements);
            std::cout << "Result: ";
            print_digest(varlen_result);
        }

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
