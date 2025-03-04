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

// Helper function to print hash result
void print_hash(const std::vector<uint8_t>& hash) {
    for (const auto& byte : hash) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    std::cout << std::endl;
}

// Helper function to detect and convert number format
std::vector<uint8_t> parse_number(const std::string& input) {
    std::vector<uint8_t> bytes;
    size_t pos = 0;
    int base = 10; // default decimal

    try {
        // Detect format
        if (input.substr(0, 2) == "0x" || input.substr(0, 2) == "0X") {
            pos = 2;
            base = 16; // hex
            // Handle hex format by byte pairs
            for (size_t i = pos; i < input.length(); i += 2) {
                if (i + 2 > input.length()) {
                    throw std::runtime_error("Hex string length must be even (full bytes)");
                }
                std::string byteString = input.substr(i, 2);
                uint8_t byte = static_cast<uint8_t>(std::stoi(byteString, nullptr, 16));
                bytes.push_back(byte);
            }
        } else if (input.substr(0, 1) == "0") {
            pos = 1;
            base = 8;  // octal
            // Handle decimal and octal as full numbers
            unsigned long value = std::stoul(input.substr(pos), nullptr, base);
            // Convert to bytes (little-endian)
            while (value > 0) {
                bytes.push_back(static_cast<uint8_t>(value & 0xFF));
                value >>= 8;
            }
            if (bytes.empty()) {
                bytes.push_back(0); // handle zero value
            }
            // Reverse to get big-endian
            std::reverse(bytes.begin(), bytes.end());
        } else {
            // Decimal
            unsigned long value = std::stoul(input, nullptr, base);
            // Convert to bytes (little-endian)
            while (value > 0) {
                bytes.push_back(static_cast<uint8_t>(value & 0xFF));
                value >>= 8;
            }
            if (bytes.empty()) {
                bytes.push_back(0); // handle zero value
            }
            // Reverse to get big-endian
            std::reverse(bytes.begin(), bytes.end());
        }
    } catch (const std::exception& e) {
        throw std::runtime_error("Invalid number format: " + input);
    }

    return bytes;
}

int main(int argc, char** argv) {
    CLI::App app{"TIP5 Hash Calculator"};

    // Command line options
    std::string mode = "pair";
    app.add_option("-m,--mode", mode, "Hash mode: 'pair' or 'varlen'")->check(CLI::IsMember({"pair", "varlen"}));

    std::vector<std::string> inputs;
    app.add_option("inputs", inputs, "Input numbers")->required()
       ->description("For pair mode: provide exactly 2 numbers\n"
                    "For varlen mode: provide 2 or more numbers\n"
                    "Supported formats:\n"
                    "- Hexadecimal: 0x01020304 (must use 0x prefix)\n"
                    "- Decimal: 16909060\n"
                    "- Octal: 0100402404 (must use 0 prefix)");

    CLI11_PARSE(app, argc, argv);

    try {
        if (mode == "pair") {
            if (inputs.size() != 2) {
                std::cerr << "Error: pair mode requires exactly 2 inputs" << std::endl;
                return 1;
            }

            auto input1 = parse_number(inputs[0]);
            auto input2 = parse_number(inputs[1]);

            std::cout << "Hash pair mode [" << inputs[0] << ", " << inputs[1] << "]:" << std::endl;
            auto pair_result = tip5xx::Tip5::hash_pair(input1, input2);
            std::cout << "Result: ";
            print_hash(pair_result);
        }
        else { // varlen mode
            if (inputs.size() < 2) {
                std::cerr << "Error: varlen mode requires at least 2 inputs" << std::endl;
                return 1;
            }

            std::vector<std::vector<uint8_t>> byte_inputs;
            for (const auto& input : inputs) {
                byte_inputs.push_back(parse_number(input));
            }

            std::cout << "Hash varlen mode [";
            for (size_t i = 0; i < inputs.size(); ++i) {
                std::cout << inputs[i];
                if (i < inputs.size() - 1) std::cout << ", ";
            }
            std::cout << "]:" << std::endl;
            auto varlen_result = tip5xx::Tip5::hash_varlen(byte_inputs);
            std::cout << "Result: ";
            print_hash(varlen_result);
        }

        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
