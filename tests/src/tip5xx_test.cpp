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
#include "tip5xx/tip5xx.hpp"
#include "random_generator.hpp"

using namespace tip5xx;

// Test fixture for Tip5Test tests
class Tip5Test : public ::testing::Test {
protected:
    RandomGenerator rng;
public:
Tip5 randomly_seeded() {
    // Create a new sponge with default initialization
    Tip5 sponge(Domain::VariableLength);

    // Generate RATE random BFieldElements using the test fixture's random generator
    std::vector<BFieldElement> random_vec = rng.random_elements(RATE);

    // Convert vector to array
    std::array<BFieldElement, RATE> random_elements;
    std::copy_n(random_vec.begin(), RATE, random_elements.begin());

    // Absorb the random elements into the sponge
    sponge.absorb(random_elements);
    return sponge;
}

};

TEST_F(Tip5Test, GetSizeTest) {
    auto tip5 = randomly_seeded();
    EXPECT_EQ(STATE_SIZE * sizeof(BFieldElement), sizeof(tip5.state));
}

TEST_F(Tip5Test, LookupTableIsCorrect) {
    // Test lookup table correctness
    for (uint16_t i = 0; i < 256; i++) {
        uint16_t result = Tip5::offset_fermat_cube_map(i);
        EXPECT_EQ(LOOKUP_TABLE[i], static_cast<uint8_t>(result));
    }
}

TEST_F(Tip5Test, TestFermatCubeMapIsPermutation) {
    bool touched[256] = {false};
    for (uint16_t i = 0; i < 256; i++) {
        touched[Tip5::offset_fermat_cube_map(i)] = true;
    }

    // Check all values are touched (it's a permutation)
    for (bool t : touched) {
        EXPECT_TRUE(t);
    }

    // Check boundary cases
    EXPECT_EQ(Tip5::offset_fermat_cube_map(0), 0);
    EXPECT_EQ(Tip5::offset_fermat_cube_map(255), 255);
}


TEST_F(Tip5Test, Hash10TestVectors) {
    std::array<BFieldElement, RATE> preimage{};
    std::array<BFieldElement, Digest::LEN> digest;

    // Test a sequence of hashes
    for (size_t i = 0; i < 6; i++) {
        digest = Tip5::hash_10(preimage);
        // Copy digest to next preimage
        std::copy(digest.begin(), digest.end(), preimage.begin() + i);
    }

    digest = Tip5::hash_10(preimage);

    // Expected final digest values
    std::array<uint64_t, Digest::LEN> expected = {
        10869784347448351760ULL,
        1853783032222938415ULL,
        6856460589287344822ULL,
        17178399545409290325ULL,
        7650660984651717733ULL
    };

    for (size_t i = 0; i < Digest::LEN; i++) {
        EXPECT_EQ(digest[i].value(), expected[i]);
    }
}

TEST_F(Tip5Test, HashVarLenTestVectors) {
    std::array<BFieldElement, Digest::LEN> digest_sum{};

    // Test different input lengths
    for (size_t i = 0; i < 20; i++) {
        std::vector<BFieldElement> preimage;
        for (size_t j = 0; j < i; j++) {
            preimage.push_back(BFieldElement::new_element(j));
        }

        auto digest = Tip5::hash_varlen(preimage);

        // Add to running sum
        for (size_t j = 0; j < Digest::LEN; j++) {
            digest_sum[j] += digest.values()[j];
        }
    }

    // Expected sum values
    std::array<uint64_t, Digest::LEN> expected = {
        7610004073009036015ULL,
        5725198067541094245ULL,
        4721320565792709122ULL,
        1732504843634706218ULL,
        259800783350288362ULL
    };

    for (size_t i = 0; i < Digest::LEN; i++) {
        EXPECT_EQ(digest_sum[i].value(), expected[i]);
    }
}

TEST_F(Tip5Test, TestLinearityOfMds) {
    // Generate random test data
    RandomGenerator rng;
    auto a = rng.random_bfe();
    auto b = rng.random_bfe();
    auto u = rng.random_elements(STATE_SIZE);
    auto v = rng.random_elements(STATE_SIZE);

    // Create and apply MDS to au + bv
    std::array<BFieldElement, STATE_SIZE> w;
    std::copy(u.begin(), u.begin() + STATE_SIZE, w.begin());
    for (size_t i = 0; i < STATE_SIZE; i++) {
        w[i] = a * u[i] + b * v[i];
    }

    Tip5 sponge_w(Domain::VariableLength);
    sponge_w.state = w;
    sponge_w.mds_generated();
    w = sponge_w.state;

    // Apply MDS to u and v separately
    Tip5 sponge_u(Domain::VariableLength);
    std::copy(u.begin(), u.begin() + STATE_SIZE, sponge_u.state.begin());
    sponge_u.mds_generated();
    u.assign(sponge_u.state.begin(), sponge_u.state.end());

    Tip5 sponge_v(Domain::VariableLength);
    std::copy(v.begin(), v.begin() + STATE_SIZE, sponge_v.state.begin());
    sponge_v.mds_generated();
    v.assign(sponge_v.state.begin(), sponge_v.state.end());

    // Check linearity: MDS(au + bv) = a*MDS(u) + b*MDS(v)
    for (size_t i = 0; i < STATE_SIZE; i++) {
        EXPECT_EQ(w[i], a * u[i] + b * v[i]);
    }
}

TEST_F(Tip5Test, TestMdsCirculancy) {
    Tip5 sponge(Domain::VariableLength);
    sponge.state.fill(BFieldElement::zero());
    sponge.state[0] = BFieldElement::one();

    // Apply MDS
    sponge.mds_generated();

    // Check first row matches expectations
    std::array<BFieldElement, STATE_SIZE> first_row;
    first_row[0] = sponge.state[0];
    for (size_t i = 1; i < STATE_SIZE; i++) {
        first_row[i] = sponge.state[STATE_SIZE - i];
    }

    // Test with random input
    RandomGenerator rng_test;
    auto initial_state = rng_test.random_elements(STATE_SIZE);

    // Apply MDS matrix multiplication manually
    std::array<BFieldElement, STATE_SIZE> manual_result{};
    for (size_t i = 0; i < STATE_SIZE; i++) {
        manual_result[i] = BFieldElement::zero();
        for (size_t j = 0; j < STATE_SIZE; j++) {
            size_t k = (STATE_SIZE - i + j) % STATE_SIZE;
            manual_result[i] += first_row[k] * initial_state[j];
        }
    }

    // Apply MDS using sponge
    Tip5 test_sponge(Domain::VariableLength);
    std::copy(initial_state.begin(), initial_state.begin() + STATE_SIZE, test_sponge.state.begin());
    test_sponge.mds_generated();

    // Compare results
    for (size_t i = 0; i < STATE_SIZE; i++) {
        EXPECT_EQ(test_sponge.state[i], manual_result[i]);
    }
}

TEST_F(Tip5Test, SampleScalarsTest) {
    Tip5 sponge = randomly_seeded();
    BFieldElement product = BFieldElement::one();

    // Sample some random values and multiply them
    for (size_t i = 0; i < 4; i++) {
        auto idx = sponge.sample_indices(256, 1)[0];
        product *= BFieldElement::new_element(idx);
    }

    // Product should not be zero (extremely unlikely with random values)
    EXPECT_NE(product, BFieldElement::zero());
}

TEST_F(Tip5Test, HashPairTest) {
    // Create two random digests
    auto elements1 = rng.random_elements(Digest::LEN);
    auto elements2 = rng.random_elements(Digest::LEN);

    std::array<BFieldElement, Digest::LEN> arr1, arr2;
    std::copy_n(elements1.begin(), Digest::LEN, arr1.begin());
    std::copy_n(elements2.begin(), Digest::LEN, arr2.begin());

    Digest left(arr1);
    Digest right(arr2);

    auto result = Tip5::hash_pair(left, right);

    // Manual calculation for verification
    Tip5 sponge(Domain::FixedLength);
    std::copy_n(left.values().begin(), Digest::LEN, sponge.state.begin());
    std::copy_n(right.values().begin(), Digest::LEN, sponge.state.begin() + Digest::LEN);

    sponge.permutation();

    std::array<BFieldElement, Digest::LEN> expected;
    std::copy_n(sponge.state.begin(), Digest::LEN, expected.begin());
    Digest expected_digest(expected);

    EXPECT_EQ(result, expected_digest);
}

TEST_F(Tip5Test, HashVarLenEquivalenceCornerCases) {
    // Test different corner cases of input lengths
    for (size_t preimage_length = 0; preimage_length <= 11; preimage_length++) {
        std::vector<BFieldElement> preimage(preimage_length, BFieldElement::new_element(42));

        auto hash_varlen_digest = Tip5::hash_varlen(preimage);

        // Implement manual hashing for comparison
        Tip5 sponge(Domain::VariableLength);
        size_t pos = 0;

        // Process full chunks
        while (pos + RATE <= preimage.size()) {
            for (size_t i = 0; i < RATE; i++) {
                sponge.state[i] = preimage[pos + i];
            }
            sponge.permutation();
            pos += RATE;
        }

        // Handle remaining elements with padding
        size_t remaining = preimage.size() - pos;
        for (size_t i = 0; i < remaining; i++) {
            sponge.state[i] = preimage[pos + i];
        }

        // Add padding
        sponge.state[remaining] = BFieldElement::one();
        for (size_t i = remaining + 1; i < RATE; i++) {
            sponge.state[i] = BFieldElement::zero();
        }

        sponge.permutation();

        // Create digest
        std::array<BFieldElement, Digest::LEN> manual_result;
        std::copy_n(sponge.state.begin(), Digest::LEN, manual_result.begin());
        auto manual_digest = Digest(manual_result);

        EXPECT_EQ(manual_digest, hash_varlen_digest);
    }
}

TEST_F(Tip5Test, TraceMethodProducesDifferentStatesAcrossRounds) {
    // Create a sponge with fixed-length domain.
    Tip5 sponge(Domain::FixedLength);

    // Initialize the state to a known value:
    std::array<BFieldElement, STATE_SIZE> init_state;
    for (size_t i = 0; i < STATE_SIZE; i++) {
        init_state[i] = BFieldElement::from_raw_u64(i + 10); // Use distinct starting values
    }
    sponge.state = std::move(init_state);

    // Capture the trace across rounds.
    auto trace = sponge.trace();
    ASSERT_EQ(trace.size(), NUM_ROUNDS + 1);

    // Verify that the state changes from one round to next.
    for (size_t r = 0; r < trace.size() - 1; r++) {
        bool changed = false;
        for (size_t i = 0; i < STATE_SIZE; i++) {
            if (trace[r][i].value() != trace[r + 1][i].value()) {
                changed = true;
                break;
            }
        }
        EXPECT_TRUE(changed) << "State did not change after round " << r;
    }
}

// Test that squeeze returns the exact first RATE elements of state before updating.
TEST_F(Tip5Test, SqueezeReturnsCorrectInitialOutput) {
    Tip5 sponge(Domain::FixedLength);

    // Set a known state. For simplicity, fill the first RATE elements with distinct values.
    for (size_t i = 0; i < RATE; i++) {
        sponge.state[i] = BFieldElement::from_raw_u64(i + 1);
    }
    // For remaining state positions, assign a constant value (or different values if desired).
    for (size_t i = RATE; i < STATE_SIZE; i++) {
        sponge.state[i] = BFieldElement::one();
    }

    // Copy expected output from the initial state (first RATE elements)
    std::array<BFieldElement, RATE> expected;
    std::copy(sponge.state.begin(), sponge.state.begin() + RATE, expected.begin());

    // Call squeeze, which should return the first RATE elements, then update the state.
    std::array<BFieldElement, RATE> output = sponge.squeeze();

    // Verify that the output matches the expected values.
    for (size_t i = 0; i < RATE; i++) {
        EXPECT_EQ(output[i], expected[i]) << "Mismatch at index " << i;
    }
}

// Test that consecutive calls to squeeze produce different outputs,
// demonstrating that the internal state is being permuted.
TEST_F(Tip5Test, SqueezeMultipleCallsProduceDifferentOutput) {
    Tip5 sponge(Domain::FixedLength);

    // Initialize state with distinct values.
    for (size_t i = 0; i < STATE_SIZE; i++) {
        sponge.state[i] = BFieldElement::from_raw_u64(i + 100);
    }

    // First squeeze
    std::array<BFieldElement, RATE> squeeze1 = sponge.squeeze();

    // Second squeeze: state has been permuted, so result should differ.
    std::array<BFieldElement, RATE> squeeze2 = sponge.squeeze();

    bool outputsDiffer = false;
    for (size_t i = 0; i < RATE; i++) {
        if (squeeze1[i] != squeeze2[i]) {
            outputsDiffer = true;
            break;
        }
    }

    EXPECT_TRUE(outputsDiffer) << "Consecutive squeezes yielded identical output, state did not change.";
}

TEST_F(Tip5Test, ReturnsRequestedNumberAndCallsPermutation) {
    // Create a sponge in fixed-length domain.
    Tip5 sponge(Domain::FixedLength);

    // For this test, we want to ensure that not every element of state produces a candidate.
    // The sample_indices function only pushes an index if the element is not equal to BFieldElement::MAX.
    // Here we simulate that by manually setting half the state elements to BFieldElement::MAX.
    // (Assuming BFieldElement::MAX is defined; if not, use a value that represents an "unused" element.)
    for (size_t i = 0; i < sponge.state.size(); i++) {
        if ((i % 2) == 0)
            sponge.state[i] = BFieldElement::MAX;  // Unusable value
        else
            sponge.state[i] = BFieldElement::from_raw_u64(i + 100);  // Usable value
    }

    // Request a sample of indices.
    // Since only half of state elements yield a candidate, one iteration over the state
    // will not be enough to produce, say, 10 indices. Thus, permutation() should be called.
    uint32_t upper_bound = 1000;
    size_t requested = 10;
    auto indices = sponge.sample_indices(upper_bound, requested);

    // Check that we got exactly the requested number of indices.
    EXPECT_EQ(indices.size(), requested);

    // And that all produced indices are within the correct upper bound.
    for (auto idx : indices) {
        EXPECT_LT(idx, upper_bound);
    }

}
