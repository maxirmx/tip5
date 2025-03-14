// Copyright (c) 2025 Maxim [maxirmx] Samsonov
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// This file is a part of tip5xx library

#include <gtest/gtest.h>
#include <numeric>
#include <sstream>
#include <algorithm>
#include <random>
#include <vector>
#include "tip5xx/digest.hpp"
#include "tip5xx/b_field_element.hpp"
#include "random_generator.hpp"

using namespace tip5xx;
using namespace std;

// Test fixture for Digest tests
class DigestTest : public ::testing::Test {
protected:
    RandomGenerator rng;

    // Helper function to create a digest with increasing values
    Digest createSequentialDigest(uint64_t start = 1) {
        std::array<BFieldElement, Digest::LEN> elements;
        for (size_t i = 0; i < Digest::LEN; i++) {
            elements[i] = BFieldElement::new_element(start + i);
        }
        return Digest(elements);
    }

    // Helper to create a digest with all elements set to the same value
    Digest createUniformDigest(uint64_t value) {
        std::array<BFieldElement, Digest::LEN> elements;
        std::fill(elements.begin(), elements.end(), BFieldElement::new_element(value));
        return Digest(elements);
    }
};

// Test default constructor creates an all-zero digest
TEST_F(DigestTest, DefaultConstructor) {
    Digest digest;
    for (size_t i = 0; i < Digest::LEN; i++) {
        EXPECT_EQ(digest.values()[i], BFieldElement::ZERO);
    }
}

// Test constructor with array of elements
TEST_F(DigestTest, ConstructorWithArray) {
    std::array<BFieldElement, Digest::LEN> elements = {
        BFieldElement::new_element(1),
        BFieldElement::new_element(2),
        BFieldElement::new_element(3),
        BFieldElement::new_element(4),
        BFieldElement::new_element(5)
    };

    Digest digest(elements);

    for (size_t i = 0; i < Digest::LEN; i++) {
        EXPECT_EQ(digest.values()[i], elements[i]);
    }
}

// Test reversing a digest
TEST_F(DigestTest, Reversed) {
    Digest original = createSequentialDigest();
    Digest reversed = original.reversed();

    for (size_t i = 0; i < Digest::LEN; i++) {
        EXPECT_EQ(reversed.values()[i], original.values()[Digest::LEN - 1 - i]);
    }

    // Reversing twice should yield the original digest
    Digest doubleReversed = reversed.reversed();
    EXPECT_EQ(original, doubleReversed);
}

// Test comparison operators
TEST_F(DigestTest, ComparisonOperators) {
    Digest zero = createUniformDigest(0);
    Digest one = createUniformDigest(1);
    Digest also_one = createUniformDigest(1);

    EXPECT_EQ(one, also_one);
    EXPECT_NE(zero, one);

    // Test ordering
    Digest val0 = createUniformDigest(0);
    Digest val1 = Digest({
        BFieldElement::new_element(14),
        BFieldElement::ZERO,
        BFieldElement::ZERO,
        BFieldElement::ZERO,
        BFieldElement::ZERO
    });
    EXPECT_GT(val1, val0);

    Digest val2 = createUniformDigest(14);
    EXPECT_GT(val2, val1);
    EXPECT_GT(val2, val0);

    Digest val3 = Digest({
        BFieldElement::new_element(15),
        BFieldElement::new_element(14),
        BFieldElement::new_element(14),
        BFieldElement::new_element(14),
        BFieldElement::new_element(14)
    });
    EXPECT_GT(val3, val2);
    EXPECT_GT(val3, val1);
    EXPECT_GT(val3, val0);

    Digest val4 = Digest({
        BFieldElement::new_element(14),
        BFieldElement::new_element(15),
        BFieldElement::new_element(14),
        BFieldElement::new_element(14),
        BFieldElement::new_element(14)
    });
    EXPECT_GT(val4, val3);
    EXPECT_GT(val4, val2);
    EXPECT_GT(val4, val1);
    EXPECT_GT(val4, val0);
}

// Test string representation
TEST_F(DigestTest, ToString) {
    Digest digest = Digest({
        BFieldElement::new_element(12063201067205522823ULL),
        BFieldElement::new_element(1529663126377206632ULL),
        BFieldElement::new_element(2090171368883726200ULL),
        BFieldElement::new_element(12975872837767296928ULL),
        BFieldElement::new_element(11492877804687889759ULL)
    });

    std::string expected = "12063201067205522823,1529663126377206632,2090171368883726200,12975872837767296928,11492877804687889759";
    EXPECT_EQ(digest.to_string(), expected);
}

// Test parsing from string
TEST_F(DigestTest, FromString) {
    std::string validDigestString = "12063201067205522823,1529663126377206632,2090171368883726200,12975872837767296928,11492877804687889759";
    auto validDigest = Digest::from_string(validDigestString);
    ASSERT_TRUE(validDigest.has_value());

    std::string invalidDigestString = "00059361073062755064,05168490802189810700";
    auto invalidDigest = Digest::from_string(invalidDigestString);
    ASSERT_FALSE(invalidDigest.has_value());

    std::string invalidBFieldString = "this_is_not_a_bfield_element,05168490802189810700";
    auto invalidBFieldDigest = Digest::from_string(invalidBFieldString);
    ASSERT_FALSE(invalidBFieldDigest.has_value());
}

// Test byte conversions
TEST_F(DigestTest, ByteConversions) {
    // Create a digest with known values
    Digest original = createSequentialDigest();

    // Convert to bytes
    std::array<uint8_t, Digest::BYTES> bytes = original.to_bytes();

    // Convert back to digest
    auto convertedResult = Digest::from_bytes(bytes);
    ASSERT_TRUE(convertedResult.has_value());

    Digest converted = convertedResult.value();
    EXPECT_EQ(original, converted);
}

// Test non-canonical bytes
TEST_F(DigestTest, NonCanonicalBytes) {
    std::array<uint8_t, Digest::BYTES> nonCanonicalBytes;
    std::fill(nonCanonicalBytes.begin(), nonCanonicalBytes.end(), 0xFF);

    auto result = Digest::from_bytes(nonCanonicalBytes);
    ASSERT_FALSE(result.has_value());
}

// Test hex string conversions
TEST_F(DigestTest, HexConversions) {
    // Create digests with known values
    Digest zeroDigest = createUniformDigest(0);
    std::string zeroHexExpected = std::string(80, '0'); // 40 bytes = 80 hex chars

    Digest mixedDigest = Digest({
        BFieldElement::ZERO,
        BFieldElement::new_element(1),
        BFieldElement::new_element(10),
        BFieldElement::new_element(15),
        BFieldElement::new_element(255)
    });
    std::string mixedHexExpected = "00000000000000000000000000000001000000000000000a000000000000000f00000000000000ff";

    // Test to_hex
    EXPECT_EQ(zeroDigest.to_hex(), zeroHexExpected);
    EXPECT_EQ(mixedDigest.to_hex(), mixedHexExpected);

    // Test from_hex
    auto zeroFromHex = Digest::from_hex(zeroHexExpected);
    ASSERT_TRUE(zeroFromHex.has_value());
    EXPECT_EQ(zeroFromHex.value(), zeroDigest);

    auto mixedFromHex = Digest::from_hex(mixedHexExpected);
    ASSERT_TRUE(mixedFromHex.has_value());
    EXPECT_EQ(mixedFromHex.value(), mixedDigest);
}

// Test invalid hex string conversions
TEST_F(DigestTest, InvalidHexConversions) {
    // Test with invalid hex character
    auto result1 = Digest::from_hex("taco");
    ASSERT_FALSE(result1.has_value());

    // Test with odd length
    auto result2 = Digest::from_hex("0");
    ASSERT_FALSE(result2.has_value());

    // Test with too short length
    auto result3 = Digest::from_hex("00");
    ASSERT_FALSE(result3.has_value());

    // Test with non-canonical values (all FF bytes)
    std::string nonCanonicalHex(80, 'f');
    auto result4 = Digest::from_hex(nonCanonicalHex);
    ASSERT_FALSE(result4.has_value());
}

// Test uppercase/lowercase hex conversions
TEST_F(DigestTest, UpperLowerHexConversions) {
    Digest digest = createSequentialDigest();

    std::string lowerHex = digest.to_hex();
    std::string upperHex = digest.to_hex_upper();

    // Convert lowercase to uppercase and compare
    std::string lowerToUpper = lowerHex;
    std::transform(lowerToUpper.begin(), lowerToUpper.end(), lowerToUpper.begin(), ::toupper);
    EXPECT_EQ(upperHex, lowerToUpper);

    // Both should convert to the same digest
    auto fromLower = Digest::from_hex(lowerHex);
    auto fromUpper = Digest::from_hex(upperHex);

    ASSERT_TRUE(fromLower.has_value());
    ASSERT_TRUE(fromUpper.has_value());
    EXPECT_EQ(fromLower.value(), fromUpper.value());
    EXPECT_EQ(fromLower.value(), digest);
}

// Test random digests conversion to/from hex
TEST_F(DigestTest, RandomDigestHexConversions) {
    for (int i = 0; i < 10; i++) {
        // Create random digest
        std::array<BFieldElement, Digest::LEN> elements;
        for (size_t j = 0; j < Digest::LEN; j++) {
            elements[j] = rng.random_bfe();
        }
        Digest original(elements);

        // Convert to hex and back
        std::string hex = original.to_hex();
        auto converted = Digest::from_hex(hex);

        ASSERT_TRUE(converted.has_value());
        EXPECT_EQ(original, converted.value());

        // Do the same with uppercase
        std::string upperHex = original.to_hex_upper();
        auto convertedUpper = Digest::from_hex(upperHex);

        ASSERT_TRUE(convertedUpper.has_value());
        EXPECT_EQ(original, convertedUpper.value());
    }
}

// Test BigUint conversion (if you implement it)
TEST_F(DigestTest, BigUintConversions) {
    // This test would require porting the BigUint functionality
    // Or using a C++ equivalent like boost::multiprecision
    // Omitted as it depends on your implementation details
}

// Test hashing functionality if implemented
TEST_F(DigestTest, HashFunction) {
    // This test would check the Tip5 hash implementation
    // Omitted as it depends on your implementation details
}

// Custom corruptor helper for tests (equivalent to DigestCorruptor in Rust)
class DigestCorruptor {
public:
    DigestCorruptor(const std::vector<size_t>& indices, const std::vector<BFieldElement>& elements)
        : corrupt_indices(indices), corrupt_elements(elements) {
        assert(indices.size() == elements.size());
    }

    std::optional<Digest> corrupt_digest(const Digest& digest) {
        Digest corrupt_digest = digest;

        for (size_t i = 0; i < corrupt_indices.size(); i++) {
            size_t index = corrupt_indices[i];
            corrupt_digest.mutable_values()[index] = corrupt_elements[i];
        }

        // Check if the corruption actually changed the digest
        if (corrupt_digest == digest) {
            return std::nullopt; // Corruption must change digest
        }

        return corrupt_digest;
    }

private:
    std::vector<size_t> corrupt_indices;
    std::vector<BFieldElement> corrupt_elements;
};

// Test the DigestCorruptor helper
TEST_F(DigestTest, DigestCorruptorTest) {
    Digest digest = Digest({
        BFieldElement::new_element(1),
        BFieldElement::new_element(2),
        BFieldElement::new_element(3),
        BFieldElement::new_element(4),
        BFieldElement::new_element(5)
    });

    // Corruptor that doesn't change anything
    DigestCorruptor noChangeCorruptor({0}, {BFieldElement::new_element(1)});
    auto noChangeResult = noChangeCorruptor.corrupt_digest(digest);
    ASSERT_FALSE(noChangeResult.has_value());

    // Corruptor that changes a value
    DigestCorruptor validCorruptor({0}, {BFieldElement::new_element(42)});
    auto validResult = validCorruptor.corrupt_digest(digest);
    ASSERT_TRUE(validResult.has_value());
    EXPECT_NE(validResult.value(), digest);
    EXPECT_EQ(validResult.value().values()[0], BFieldElement::new_element(42));
}

// Test handling of non-canonical string values
TEST_F(DigestTest, NonCanonicalStringParsing) {
    // Create a string with a value that exceeds BFieldElement::MAX_VALUE
    std::string str = "0,0,0,0," + std::to_string(BFieldElement::MAX_VALUE + 1);
    auto result = Digest::from_string(str);
    ASSERT_FALSE(result.has_value());

    // Test with max valid value
    std::string validStr = "0,0,0,0," + std::to_string(BFieldElement::MAX_VALUE);
    auto validResult = Digest::from_string(validStr);
    ASSERT_TRUE(validResult.has_value());
}

// Test vector/array conversion methods
TEST_F(DigestTest, VectorArrayConversions) {
    // Test from_bfield_elements with valid vector
    std::vector<BFieldElement> validVec = {
        BFieldElement::new_element(1),
        BFieldElement::new_element(2),
        BFieldElement::new_element(3),
        BFieldElement::new_element(4),
        BFieldElement::new_element(5)
    };

    auto fromVec = Digest::from_bfield_elements(validVec);
    ASSERT_TRUE(fromVec.has_value());

    // Test round trip conversion
    std::vector<BFieldElement> roundTrip = fromVec.value().to_bfield_elements();
    ASSERT_EQ(validVec.size(), roundTrip.size());
    for (size_t i = 0; i < validVec.size(); i++) {
        EXPECT_EQ(validVec[i], roundTrip[i]);
    }

    // Test invalid size vector
    std::vector<BFieldElement> invalidVec = {
        BFieldElement::new_element(1),
        BFieldElement::new_element(2)
    };

    auto fromInvalidVec = Digest::from_bfield_elements(invalidVec);
    ASSERT_FALSE(fromInvalidVec.has_value());

    // Test from_slice with valid array
    BFieldElement validArray[Digest::LEN] = {
        BFieldElement::new_element(6),
        BFieldElement::new_element(7),
        BFieldElement::new_element(8),
        BFieldElement::new_element(9),
        BFieldElement::new_element(10)
    };

    auto fromSlice = Digest::from_slice(validArray, Digest::LEN);
    ASSERT_TRUE(fromSlice.has_value());

    // Test from_slice with invalid size
    auto fromInvalidSlice = Digest::from_slice(validArray, Digest::LEN - 1);
    ASSERT_FALSE(fromInvalidSlice.has_value());
}

// Test the hash function implementation
/*TEST_F(DigestTest, HashFunctionComprehensive) {
    // Test that hash of default digest is not the same as input
    Digest empty;
    Digest hashed = empty.hash();
    EXPECT_NE(empty, hashed);

    // Test that hash is deterministic
    Digest digest1 = createSequentialDigest();
    Digest hash1 = digest1.hash();
    Digest hash2 = digest1.hash();
    EXPECT_EQ(hash1, hash2);

    // Test that different digests produce different hashes
    Digest digest2 = createUniformDigest(42);
    Digest hash3 = digest2.hash();
    EXPECT_NE(hash1, hash3);

    // Test hash of complex digest
    Digest complex = Digest({
        BFieldElement::new_element(12345),
        BFieldElement::new_element(67890),
        BFieldElement::new_element(13579),
        BFieldElement::new_element(24680),
        BFieldElement::new_element(97531)
    });

    Digest hashComplex = complex.hash();
    EXPECT_NE(complex, hashComplex);
}
*/
// Test overflow handling in various operations
TEST_F(DigestTest, OverflowHandling) {
    // Create digest with maximum values
    std::array<BFieldElement, Digest::LEN> maxElements;
    for (size_t i = 0; i < Digest::LEN; i++) {
        maxElements[i] = BFieldElement::new_element(BFieldElement::MAX_VALUE - 1);
    }
    Digest maxDigest(maxElements);

    // Test serialization and deserialization of max values
    std::string maxStr = maxDigest.to_string();
    auto parsedMax = Digest::from_string(maxStr);
    ASSERT_TRUE(parsedMax.has_value());
    EXPECT_EQ(maxDigest, parsedMax.value());

    // Test hex conversion of max values
    std::string maxHex = maxDigest.to_hex();
    auto parsedMaxHex = Digest::from_hex(maxHex);
    ASSERT_TRUE(parsedMaxHex.has_value());
    EXPECT_EQ(maxDigest, parsedMaxHex.value());

    // Test byte conversion of max values
    auto maxBytes = maxDigest.to_bytes();
    auto parsedMaxBytes = Digest::from_bytes(maxBytes);
    ASSERT_TRUE(parsedMaxBytes.has_value());
    EXPECT_EQ(maxDigest, parsedMaxBytes.value());
}

// Test more edge cases for hex string parsing
TEST_F(DigestTest, HexEdgeCases) {
    // Test with empty string
    auto emptyResult = Digest::from_hex("");
    ASSERT_FALSE(emptyResult.has_value());

    // Test with almost correct length but not quite
    std::string almostHex = std::string(79, '0');
    auto almostResult = Digest::from_hex(almostHex);
    ASSERT_FALSE(almostResult.has_value());

    // Test with mixed case (should still work)
    Digest original = createSequentialDigest();
    std::string lowerHex = original.to_hex();

    // Convert some characters to uppercase
    for (size_t i = 0; i < lowerHex.length(); i += 5) {
        lowerHex[i] = std::toupper(lowerHex[i]);
    }

    auto mixedCaseResult = Digest::from_hex(lowerHex);
    ASSERT_TRUE(mixedCaseResult.has_value());
    EXPECT_EQ(original, mixedCaseResult.value());
}

// Property-based testing equivalents
TEST_F(DigestTest, PropertyBasedTests) {
    // Test that any 40 valid bytes can be converted to a digest and back
    for (int test = 0; test < 10; test++) {
        std::array<uint8_t, Digest::BYTES> randomBytes;

        // Generate random bytes, ensuring they represent valid field elements
        for (size_t i = 0; i < Digest::LEN; i++) {
            uint64_t validValue = rng.random_bfe().value();

            // Convert to bytes in little-endian order
            for (size_t j = 0; j < 8; j++) {
                randomBytes[i * 8 + j] = static_cast<uint8_t>(validValue & 0xFF);
                validValue >>= 8;
            }
        }

        auto digestResult = Digest::from_bytes(randomBytes);
        ASSERT_TRUE(digestResult.has_value());

        auto roundTripBytes = digestResult.value().to_bytes();
        EXPECT_EQ(randomBytes, roundTripBytes);
    }

    // Test that any digest can be converted to string and back
    for (int test = 0; test < 10; test++) {
        // Create random digest
        std::array<BFieldElement, Digest::LEN> elements;
        for (size_t i = 0; i < Digest::LEN; i++) {
            elements[i] = rng.random_bfe();
        }
        Digest original(elements);

        // Round trip through string representation
        std::string str = original.to_string();
        auto parsed = Digest::from_string(str);

        ASSERT_TRUE(parsed.has_value());
        EXPECT_EQ(original, parsed.value());
    }
}

// Tests for try_from_hex functionality
TEST_F(DigestTest, TryFromHexTests) {
    // Test success case
    std::string validHex = std::string(80, '0');
    Digest result = Digest::try_from_hex(validHex);
    Digest expected = createUniformDigest(0);
    EXPECT_EQ(result, expected);

    // Test exception handling
    bool exceptionCaught = false;
    try {
        Digest badResult = Digest::try_from_hex("invalid");
    } catch (const std::exception& e) {
        exceptionCaught = true;
    }
    EXPECT_TRUE(exceptionCaught);
}

// Test the legacy digest_from_string function
TEST_F(DigestTest, LegacyDigestFromString) {
    std::string validStr = "1,2,3,4,5";
    Digest result = digest_from_string(validStr);

    Digest expected = createSequentialDigest();
    EXPECT_EQ(result, expected);

    // Test exception handling
    bool exceptionCaught = false;
    try {
        Digest invalid = digest_from_string("1,2");
    } catch (const std::exception& e) {
        exceptionCaught = true;
    }
    EXPECT_TRUE(exceptionCaught);
}

/*
// Test specific Tip5 hash function behavior
TEST_F(DigestTest, Tip5HashFunction) {
    // Test specific properties of the Tip5 hash function
    // This depends on your actual implementation

    // Example: Test that hash of empty digest matches your reference value
    Digest empty;
    Digest emptyHash = empty.hash();

    // Replace with your known hash value for empty digest
    BFieldElement knownHash[Digest::LEN] = {
        // Put your known values here based on your implementation
        BFieldElement::new_element(0xSOME_KNOWN_VALUE),
        BFieldElement::new_element(0xSOME_KNOWN_VALUE),
        BFieldElement::new_element(0xSOME_KNOWN_VALUE),
        BFieldElement::new_element(0xSOME_KNOWN_VALUE),
        BFieldElement::new_element(0xSOME_KNOWN_VALUE)
    };

    // Only enable this test if you have reference values
    // EXPECT_EQ(emptyHash, Digest({knownHash[0], knownHash[1], knownHash[2], knownHash[3], knownHash[4]}));
}
*/