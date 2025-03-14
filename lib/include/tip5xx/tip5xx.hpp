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

#pragma once

#include <array>
#include <cstdint>
#include <vector>
#include "tip5xx/b_field_element.hpp"
#include "tip5xx/digest.hpp"

namespace tip5xx {

constexpr size_t STATE_SIZE = 16;
constexpr size_t NUM_SPLIT_AND_LOOKUP = 4;
constexpr size_t LOG2_STATE_SIZE = 4;
constexpr size_t CAPACITY = 6;
constexpr size_t RATE = 10;
constexpr size_t NUM_ROUNDS = 5;

// Lookup table with high algebraic degree used in TIP-5 permutation
constexpr std::array<uint8_t, 256> LOOKUP_TABLE = {{
    0, 7, 26, 63, 124, 215, 85, 254, 214, 228, 45, 185, 140, 173, 33, 240,
    29, 177, 176, 32, 8, 110, 87, 202, 204, 99, 150, 106, 230, 14, 235, 128,
    213, 239, 212, 138, 23, 130, 208, 6, 44, 71, 93, 116, 146, 189, 251, 81,
    199, 97, 38, 28, 73, 179, 95, 84, 152, 48, 35, 119, 49, 88, 242, 3,
    148, 169, 72, 120, 62, 161, 166, 83, 175, 191, 137, 19, 100, 129, 112, 55,
    221, 102, 218, 61, 151, 237, 68, 164, 17, 147, 46, 234, 203, 216, 22, 141,
    65, 57, 123, 12, 244, 54, 219, 231, 96, 77, 180, 154, 5, 253, 133, 165,
    98, 195, 205, 134, 245, 30, 9, 188, 59, 142, 186, 197, 181, 144, 92, 31,
    224, 163, 111, 74, 58, 69, 113, 196, 67, 246, 225, 10, 121, 50, 60, 157,
    90, 122, 2, 250, 101, 75, 178, 159, 24, 36, 201, 11, 243, 132, 198, 190,
    114, 233, 39, 52, 21, 209, 108, 238, 91, 187, 18, 104, 194, 37, 153, 34,
    200, 143, 126, 155, 236, 118, 64, 80, 172, 89, 94, 193, 135, 183, 86, 107,
    252, 13, 167, 206, 136, 220, 207, 103, 171, 160, 76, 182, 227, 217, 158, 56,
    174, 4, 66, 109, 139, 162, 184, 211, 249, 47, 125, 232, 117, 43, 16, 42,
    127, 20, 241, 25, 149, 105, 156, 51, 53, 168, 145, 247, 223, 79, 78, 226,
    15, 222, 82, 115, 70, 210, 27, 41, 1, 170, 40, 131, 192, 229, 248, 255
}};

// Round constants used in Tip5 permutation
constexpr std::array<uint64_t, NUM_ROUNDS * STATE_SIZE> ROUND_CONSTANTS_RAW = {{
    13630775303355457758ULL, 16896927574093233874ULL, 10379449653650130495ULL, 1965408364413093495ULL,
    15232538947090185111ULL, 15892634398091747074ULL, 3989134140024871768ULL, 2851411912127730865ULL,
    8709136439293758776ULL, 3694858669662939734ULL, 12692440244315327141ULL, 10722316166358076749ULL,
    12745429320441639448ULL, 17932424223723990421ULL, 7558102534867937463ULL, 15551047435855531404ULL,
    17532528648579384106ULL, 5216785850422679555ULL, 15418071332095031847ULL, 11921929762955146258ULL,
    9738718993677019874ULL, 3464580399432997147ULL, 13408434769117164050ULL, 264428218649616431ULL,
    4436247869008081381ULL, 4063129435850804221ULL, 2865073155741120117ULL, 5749834437609765994ULL,
    6804196764189408435ULL, 17060469201292988508ULL, 9475383556737206708ULL, 12876344085611465020ULL,
    13835756199368269249ULL, 1648753455944344172ULL, 9836124473569258483ULL, 12867641597107932229ULL,
    11254152636692960595ULL, 16550832737139861108ULL, 11861573970480733262ULL, 1256660473588673495ULL,
    13879506000676455136ULL, 10564103842682358721ULL, 16142842524796397521ULL, 3287098591948630584ULL,
    685911471061284805ULL, 5285298776918878023ULL, 18310953571768047354ULL, 3142266350630002035ULL,
    549990724933663297ULL, 4901984846118077401ULL, 11458643033696775769ULL, 8706785264119212710ULL,
    12521758138015724072ULL, 11877914062416978196ULL, 11333318251134523752ULL, 3933899631278608623ULL,
    16635128972021157924ULL, 10291337173108950450ULL, 4142107155024199350ULL, 16973934533787743537ULL,
    11068111539125175221ULL, 17546769694830203606ULL, 5315217744825068993ULL, 4609594252909613081ULL,
    3350107164315270407ULL, 17715942834299349177ULL, 9600609149219873996ULL, 12894357635820003949ULL,
    4597649658040514631ULL, 7735563950920491847ULL, 1663379455870887181ULL, 13889298103638829706ULL,
    7375530351220884434ULL, 3502022433285269151ULL, 9231805330431056952ULL, 9252272755288523725ULL,
    10014268662326746219ULL, 15565031632950843234ULL, 1209725273521819323ULL, 6024642864597845108ULL
}};

// MDS matrix first column
constexpr std::array<int64_t, STATE_SIZE> MDS_MATRIX_FIRST_COLUMN = {{
    61402, 1108, 28750, 33823, 7454, 43244, 53865, 12034,
    56951, 27521, 41351, 40901, 12021, 59689, 26798, 17845
}};

enum class Domain {
    VariableLength,
    FixedLength
};

class Tip5 {
public:
    std::array<BFieldElement, STATE_SIZE> state;

    // Constructor
    explicit Tip5(Domain domain = Domain::VariableLength);

    // Core permutation functions
    void permutation();

    // State inspection functions
    [[nodiscard]] std::array<std::array<BFieldElement, STATE_SIZE>, NUM_ROUNDS + 1> trace();

    // Test helper functions
    static constexpr uint16_t offset_fermat_cube_map(uint16_t x) {
        uint64_t xx = (x + 1);
        uint64_t xxx = xx * xx * xx;
        return static_cast<uint16_t>((xxx + 256) % 257);
    }
    void mds_generated();

    // Hash functions
    static std::array<BFieldElement, Digest::LEN> hash_10(const std::array<BFieldElement, RATE>& input);
    static Digest hash_pair(const Digest& left, const Digest& right);
    template<typename T>
    static Digest hash(const T& value);
    static Digest hash_varlen(const std::vector<BFieldElement>& input);

    // Sampling functions
    std::vector<uint32_t> sample_indices(uint32_t upper_bound, size_t num_indices);

    void absorb(const std::array<BFieldElement, RATE>& input);
    std::array<BFieldElement, RATE> squeeze();

private:
    // Internal permutation steps
    void sbox_layer();
    void round(size_t round_index);
    static void split_and_lookup(BFieldElement& element);

    // Matrix multiply implementations
    static std::array<int64_t, 16> fast_cyclomul16(const std::array<int64_t, 16>& f,
                                                  const std::array<int64_t, 16>& g);
    static std::array<int64_t, 8> fast_cyclomul8(const std::array<int64_t, 8>& f,
                                                const std::array<int64_t, 8>& g);
    static std::array<int64_t, 4> fast_cyclomul4(const std::array<int64_t, 4>& f,
                                                const std::array<int64_t, 4>& g);
    static std::array<int64_t, 2> fast_cyclomul2(const std::array<int64_t, 2>& f,
                                                const std::array<int64_t, 2>& g);

    static std::array<int64_t, 8> complex_negacyclomul8(const std::array<int64_t, 8>& f,
                                                       const std::array<int64_t, 8>& g);
    static std::array<int64_t, 4> complex_negacyclomul4(const std::array<int64_t, 4>& f,
                                                       const std::array<int64_t, 4>& g);
    static std::array<int64_t, 2> complex_negacyclomul2(const std::array<int64_t, 2>& f,
                                                       const std::array<int64_t, 2>& g);
};

} // namespace tip5xx
