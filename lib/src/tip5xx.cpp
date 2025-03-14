/**
 * Copyright (c) 2025 Maxim [maxirmx] Samsonov
 *
 * This file is a part of tip5xx library
 */

#include "tip5xx/tip5xx.hpp"
#include "tip5xx/digest.hpp"
#include <array>

namespace tip5xx {

static constexpr std::array<uint8_t, 256> LOOKUP_TABLE = {
    0, 7, 26, 63, 124, 215, 85, 254, 214, 228, 45, 185, 140, 173, 33, 240, 29, 177, 176, 32, 8,
    110, 87, 202, 204, 99, 150, 106, 230, 14, 235, 128, 213, 239, 212, 138, 23, 130, 208, 6, 44,
    71, 93, 116, 146, 189, 251, 81, 199, 97, 38, 28, 73, 179, 95, 84, 152, 48, 35, 119, 49, 88,
    242, 3, 148, 169, 72, 120, 62, 161, 166, 83, 175, 191, 137, 19, 100, 129, 112, 55, 221, 102,
    218, 61, 151, 237, 68, 164, 17, 147, 46, 234, 203, 216, 22, 141, 65, 57, 123, 12, 244, 54, 219,
    231, 96, 77, 180, 154, 5, 253, 133, 165, 98, 195, 205, 134, 245, 30, 9, 188, 59, 142, 186, 197,
    181, 144, 92, 31, 224, 163, 111, 74, 58, 69, 113, 196, 67, 246, 225, 10, 121, 50, 60, 157, 90,
    122, 2, 250, 101, 75, 178, 159, 24, 36, 201, 11, 243, 132, 198, 190, 114, 233, 39, 52, 21, 209,
    108, 238, 91, 187, 18, 104, 194, 37, 153, 34, 200, 143, 126, 155, 236, 118, 64, 80, 172, 89,
    94, 193, 135, 183, 86, 107, 252, 13, 167, 206, 136, 220, 207, 103, 171, 160, 76, 182, 227, 217,
    158, 56, 174, 4, 66, 109, 139, 162, 184, 211, 249, 47, 125, 232, 117, 43, 16, 42, 127, 20, 241,
    25, 149, 105, 156, 51, 53, 168, 145, 247, 223, 79, 78, 226, 15, 222, 82, 115, 70, 210, 27, 41,
    1, 170, 40, 131, 192, 229, 248, 255
};

static const std::array<BFieldElement, Tip5::NUM_ROUNDS * Tip5::STATE_SIZE> ROUND_CONSTANTS = {
    bfe(13630775303355457758ULL), bfe(16896927574093233874ULL), bfe(10379449653650130495ULL),
    bfe(1965408364413093495ULL), bfe(15232538947090185111ULL), bfe(15892634398091747074ULL),
    bfe(3989134140024871768ULL), bfe(2851411912127730865ULL), bfe(8709136439293758776ULL),
    bfe(3694858669662939734ULL), bfe(12692440244315327141ULL), bfe(10722316166358076749ULL),
    bfe(12745429320441639448ULL), bfe(17932424223723990421ULL), bfe(7558102534867937463ULL),
    bfe(15551047435855531404ULL), bfe(17532528648579384106ULL), bfe(5216785850422679555ULL),
    bfe(15418071332095031847ULL), bfe(11921929762955146258ULL), bfe(9738718993677019874ULL),
    bfe(3464580399432997147ULL), bfe(13408434769117164050ULL), bfe(264428218649616431ULL),
    bfe(4436247869008081381ULL), bfe(4063129435850804221ULL), bfe(2865073155741120117ULL),
    bfe(5749834437609765994ULL), bfe(6804196764189408435ULL), bfe(17060469201292988508ULL),
    bfe(9475383556737206708ULL), bfe(12876344085611465020ULL), bfe(13835756199368269249ULL),
    bfe(1648753455944344172ULL), bfe(9836124473569258483ULL), bfe(12867641597107932229ULL),
    bfe(11254152636692960595ULL), bfe(16550832737139861108ULL), bfe(11861573970480733262ULL),
    bfe(1256660473588673495ULL), bfe(13879506000676455136ULL), bfe(10564103842682358721ULL),
    bfe(16142842524796397521ULL), bfe(3287098591948630584ULL), bfe(685911471061284805ULL),
    bfe(5285298776918878023ULL), bfe(18310953571768047354ULL), bfe(3142266350630002035ULL),
    bfe(549990724933663297ULL), bfe(4901984846118077401ULL), bfe(11458643033696775769ULL),
    bfe(8706785264119212710ULL), bfe(12521758138015724072ULL), bfe(11877914062416978196ULL),
    bfe(11333318251134523752ULL), bfe(3933899631278608623ULL), bfe(16635128972021157924ULL),
    bfe(10291337173108950450ULL), bfe(4142107155024199350ULL), bfe(16973934533787743537ULL),
    bfe(11068111539125175221ULL), bfe(17546769694830203606ULL), bfe(5315217744825068993ULL),
    bfe(4609594252909613081ULL), bfe(3350107164315270407ULL), bfe(17715942834299349177ULL),
    bfe(9600609149219873996ULL), bfe(12894357635820003949ULL), bfe(4597649658040514631ULL),
    bfe(7735563950920491847ULL), bfe(1663379455870887181ULL), bfe(13889298103638829706ULL),
    bfe(7375530351220884434ULL), bfe(3502022433285269151ULL), bfe(9231805330431056952ULL),
    bfe(9252272755288523725ULL), bfe(10014268662326746219ULL), bfe(15565031632950843234ULL),
    bfe(1209725273521819323ULL), bfe(6024642864597845108ULL)
};

void Tip5::split_and_lookup(BFieldElement& element) {
    auto bytes = element.raw_bytes();
    for (size_t i = 0; i < 8; ++i) {
        bytes[i] = LOOKUP_TABLE[bytes[i]];
    }
    element = BFieldElement::from_raw_bytes(bytes);
}

void Tip5::sbox_layer(std::array<BFieldElement, STATE_SIZE>& state) {
    // Apply lookup table to first NUM_SPLIT_AND_LOOKUP elements
    for (size_t i = 0; i < NUM_SPLIT_AND_LOOKUP; ++i) {
        split_and_lookup(state[i]);
    }

    // Apply power function to remaining elements
    for (size_t i = NUM_SPLIT_AND_LOOKUP; i < STATE_SIZE; ++i) {
        auto& elem = state[i];
        auto sq = elem * elem;
        auto qu = sq * sq;
        elem *= sq * qu;  // x^7
    }
}

void Tip5::mds_generated(std::array<BFieldElement, STATE_SIZE>& state) {
    // This is a simplified MDS implementation - in a real system you would want
    // to use optimized matrix multiplication based on the Rust implementation
    std::array<uint64_t, STATE_SIZE> lo = {0};
    std::array<uint64_t, STATE_SIZE> hi = {0};

    for (size_t i = 0; i < STATE_SIZE; ++i) {
        uint64_t raw = state[i].raw_u64();
        hi[i] = raw >> 32;
        lo[i] = raw & 0xFFFFFFFFULL;
    }

    // Apply MDS matrix multiplication (simplified)
    std::array<uint64_t, STATE_SIZE> new_lo = {0};
    std::array<uint64_t, STATE_SIZE> new_hi = {0};

    for (size_t i = 0; i < STATE_SIZE; ++i) {
        for (size_t j = 0; j < STATE_SIZE; ++j) {
            size_t k = (i + j) % STATE_SIZE;
            new_lo[i] = (new_lo[i] + lo[j]) & 0xFFFFFFFFULL;
            new_hi[i] = (new_hi[i] + hi[j]) & 0xFFFFFFFFULL;
        }
    }

    for (size_t i = 0; i < STATE_SIZE; ++i) {
        uint64_t s_lo = new_lo[i] >> 4;
        uint64_t s_hi = new_hi[i] << 28;
        uint64_t combined = s_lo + (s_hi & 0xFFFFFFFFULL);
        state[i] = bfe(combined);
    }
}

void Tip5::round(std::array<BFieldElement, STATE_SIZE>& state, size_t round_idx) {
    sbox_layer(state);
    mds_generated(state);

    // Add round constants
    for (size_t i = 0; i < STATE_SIZE; ++i) {
        state[i] += ROUND_CONSTANTS[round_idx * STATE_SIZE + i];
    }
}

void Tip5::permutation(std::array<BFieldElement, STATE_SIZE>& state) {
    for (size_t i = 0; i < NUM_ROUNDS; ++i) {
        round(state, i);
    }
}

Digest Tip5::hash_pair(const Digest& left, const Digest& right) {
    std::array<BFieldElement, STATE_SIZE> state;
    state.fill(bfe(0));  // Initialize state to zero

    // Copy digest values into state
    const auto& left_values = left.values();
    const auto& right_values = right.values();
    std::copy(left_values.begin(), left_values.end(), state.begin());
    std::copy(right_values.begin(), right_values.end(), state.begin() + left_values.size());

    permutation(state);

    // Extract result
    std::array<BFieldElement, 5> digest_values;
    std::copy(state.begin(), state.begin() + digest_values.size(), digest_values.begin());
    return Digest(digest_values);
}

Digest Tip5::hash_varlen(const std::vector<BFieldElement>& input) {
    std::array<BFieldElement, STATE_SIZE> state;
    state.fill(bfe(0));  // Initialize state to zero

    // Process input in chunks of RATE size
    for (size_t i = 0; i < input.size(); i += RATE) {
        size_t chunk_size = std::min(RATE, input.size() - i);
        std::copy(input.begin() + i, input.begin() + i + chunk_size, state.begin());
        permutation(state);
    }

    // Extract result
    std::array<BFieldElement, 5> digest_values;
    std::copy(state.begin(), state.begin() + digest_values.size(), digest_values.begin());
    return Digest(digest_values);
}

} // namespace tip5xx
