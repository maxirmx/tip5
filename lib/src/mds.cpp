#include "tip5xx/mds.hpp"

namespace tip5xx {

// Generated function from MDS matrix
std::array<uint64_t, 16> generated_function(const std::array<uint64_t, 16>& input) {
    uint64_t node_34 = input[0] + input[8];
    uint64_t node_38 = input[4] + input[12];
    uint64_t node_36 = input[2] + input[10];
    uint64_t node_40 = input[6] + input[14];
    uint64_t node_35 = input[1] + input[9];
    uint64_t node_39 = input[5] + input[13];
    uint64_t node_37 = input[3] + input[11];
    uint64_t node_41 = input[7] + input[15];
    uint64_t node_50 = node_34 + node_38;
    uint64_t node_52 = node_36 + node_40;
    uint64_t node_51 = node_35 + node_39;
    uint64_t node_53 = node_37 + node_41;
    uint64_t node_160 = input[0] - input[8];
    uint64_t node_161 = input[1] - input[9];
    uint64_t node_165 = input[5] - input[13];
    uint64_t node_163 = input[3] - input[11];
    uint64_t node_167 = input[7] - input[15];
    uint64_t node_162 = input[2] - input[10];
    uint64_t node_166 = input[6] - input[14];
    uint64_t node_164 = input[4] - input[12];
    uint64_t node_58 = node_50 + node_52;
    uint64_t node_59 = node_51 + node_53;
    uint64_t node_90 = node_34 - node_38;
    uint64_t node_91 = node_35 - node_39;
    uint64_t node_93 = node_37 - node_41;
    uint64_t node_92 = node_36 - node_40;
    uint64_t node_64 = (node_58 + node_59) * 524757;
    uint64_t node_67 = (node_58 - node_59) * 52427;
    uint64_t node_71 = node_50 - node_52;
    uint64_t node_72 = node_51 - node_53;
    uint64_t node_177 = node_161 + node_165;
    uint64_t node_179 = node_163 + node_167;
    uint64_t node_178 = node_162 + node_166;
    uint64_t node_176 = node_160 + node_164;
    uint64_t node_69 = node_64 + node_67;
    uint64_t node_397 = node_71 * 18446744073709525744ULL - node_72 * 53918;
    uint64_t node_1857 = node_90 * 395512;
    uint64_t node_99 = node_91 + node_93;
    uint64_t node_1865 = node_91 * 18446744073709254400ULL;
    uint64_t node_1869 = node_93 * 179380;
    uint64_t node_1873 = node_92 * 18446744073709509368ULL;
    uint64_t node_1879 = node_160 * 35608;
    uint64_t node_185 = node_161 + node_163;
    uint64_t node_1915 = node_161 * 18446744073709340312ULL;
    uint64_t node_1921 = node_163 * 18446744073709494992ULL;
    uint64_t node_1927 = node_162 * 18446744073709450808ULL;
    uint64_t node_228 = node_165 + node_167;
    uint64_t node_1939 = node_165 * 18446744073709420056ULL;
    uint64_t node_1945 = node_167 * 18446744073709505128ULL;
    uint64_t node_1951 = node_166 * 216536;
    uint64_t node_1957 = node_164 * 18446744073709515080ULL;
    uint64_t node_70 = node_64 - node_67;
    uint64_t node_702 = node_71 * 53918 + node_72 * 18446744073709525744ULL;
    uint64_t node_1961 = node_90 * 18446744073709254400ULL;
    uint64_t node_1963 = node_91 * 395512;
    uint64_t node_1965 = node_92 * 179380;
    uint64_t node_1967 = node_93 * 18446744073709509368ULL;
    uint64_t node_1970 = node_160 * 18446744073709340312ULL;
    uint64_t node_1973 = node_161 * 35608;
    uint64_t node_1982 = node_162 * 18446744073709494992ULL;
    uint64_t node_1985 = node_163 * 18446744073709450808ULL;
    uint64_t node_1988 = node_166 * 18446744073709505128ULL;
    uint64_t node_1991 = node_167 * 216536;
    uint64_t node_1994 = node_164 * 18446744073709420056ULL;
    uint64_t node_1997 = node_165 * 18446744073709515080ULL;
    uint64_t node_98 = node_90 + node_92;
    uint64_t node_184 = node_160 + node_162;
    uint64_t node_227 = node_164 + node_166;
    uint64_t node_86 = node_69 + node_397;
    uint64_t tmp1 = node_99 * 18446744073709433780ULL;
    uint64_t node_403 = node_1857 - (tmp1 - node_1865 - node_1869 + node_1873);
    uint64_t node_271 = node_177 + node_179;
    uint64_t node_1891 = node_177 * 18446744073709208752ULL;
    uint64_t node_1897 = node_179 * 18446744073709448504ULL;
    uint64_t node_1903 = node_178 * 115728;
    uint64_t node_1909 = node_185 * 18446744073709283688ULL;
    uint64_t node_1933 = node_228 * 18446744073709373568ULL;
    uint64_t node_88 = node_70 + node_702;
    uint64_t node_708 = node_1961 + node_1963 - (node_1965 + node_1967);
    uint64_t node_1976 = node_178 * 18446744073709448504ULL;
    uint64_t node_1979 = node_179 * 115728;
    uint64_t node_87 = node_69 - node_397;
    uint64_t tmp2 = node_98 * 353264;
    uint64_t node_897 = node_1865 + tmp2 - node_1857 - node_1873 - node_1869;
    uint64_t node_2007 = node_184 * 18446744073709486416ULL;
    uint64_t node_2013 = node_227 * 180000;
    uint64_t node_89 = node_70 - node_702;
    uint64_t tmp3 = node_98 * 18446744073709433780ULL;
    uint64_t tmp4 = node_99 * 353264;
    uint64_t node_1077 = tmp3 + tmp4 - (node_1961 + node_1963) - (node_1965 + node_1967);
    uint64_t node_2020 = node_184 * 18446744073709283688ULL;
    uint64_t node_2023 = node_185 * 18446744073709486416ULL;
    uint64_t node_2026 = node_227 * 18446744073709373568ULL;
    uint64_t node_2029 = node_228 * 180000;
    uint64_t node_2035 = node_176 * 18446744073709550688ULL;
    uint64_t node_2038 = node_176 * 18446744073709208752ULL;
    uint64_t node_2041 = node_177 * 18446744073709550688ULL;
    uint64_t node_270 = node_176 + node_178;
    uint64_t node_152 = node_86 + node_403;
    uint64_t tmp5 = node_271 * 18446744073709105640ULL - node_1891 - node_1897 + node_1903;
    uint64_t tmp6 = node_1909 - node_1915 - node_1921 + node_1927;
    uint64_t tmp7 = node_1933 - node_1939 - node_1945 + node_1951;
    uint64_t node_412 = node_1879 - (tmp5 - tmp6 - tmp7 + node_1957);
    uint64_t node_154 = node_88 + node_708;
    uint64_t tmp8 = node_1976 + node_1979;
    uint64_t tmp9 = node_1982 + node_1985;
    uint64_t tmp10 = node_1988 + node_1991;
    uint64_t tmp11 = node_1994 + node_1997;
    uint64_t node_717 = node_1970 + node_1973 - (tmp8 - tmp9 - tmp10 + tmp11);
    uint64_t node_156 = node_87 + node_897;
    uint64_t tmp12 = node_1897 - node_1921 - node_1945;
    uint64_t tmp13 = node_1939 + node_2013 - node_1957 - node_1951;
    uint64_t node_906 = node_1915 + node_2007 - node_1879 - node_1927 - (tmp12 + tmp13);
    uint64_t node_158 = node_89 + node_1077;
    uint64_t tmp14 = node_1970 + node_1973;
    uint64_t tmp15 = node_1982 + node_1985;
    uint64_t tmp16 = node_2026 + node_2029;
    uint64_t tmp17 = node_1994 + node_1997;
    uint64_t tmp18 = node_1988 + node_1991;
    uint64_t node_1086 = node_2020 + node_2023 - tmp14 - tmp15 - (tmp16 - tmp17 - tmp18);
    uint64_t node_153 = node_86 - node_403;
    uint64_t tmp19 = node_1909 - node_1915 - node_1921 + node_1927;
    uint64_t tmp20 = node_1933 - node_1939 - node_1945 + node_1951;
    uint64_t node_1237 = tmp19 + node_2035 - node_1879 - node_1957 - tmp20;
    uint64_t node_155 = node_88 - node_708;
    uint64_t tmp21 = node_2038 + node_2041;
    uint64_t tmp22 = node_1970 + node_1973;
    uint64_t tmp23 = node_1994 + node_1997;
    uint64_t tmp24 = node_1988 + node_1991;
    uint64_t node_1375 = node_1982 + node_1985 + tmp21 - tmp22 - tmp23 - tmp24;
    uint64_t node_157 = node_87 - node_897;
    uint64_t tmp25 = node_270 * 114800;
    uint64_t tmp26 = node_1891 + tmp25 - node_2035 - node_1903;
    uint64_t tmp27 = node_1915 + node_2007 - node_1879 - node_1927;
    uint64_t tmp28 = node_1939 + node_2013 - node_1957 - node_1951;
    uint64_t node_1492 = node_1921 + tmp26 - tmp27 - tmp28 - node_1945;
    uint64_t node_159 = node_89 - node_1077;
    uint64_t tmp29 = node_270 * 18446744073709105640ULL;
    uint64_t tmp30 = node_271 * 114800;
    uint64_t tmp31 = node_2038 + node_2041;
    uint64_t tmp32 = node_1976 + node_1979;
    uint64_t tmp33 = node_2020 + node_2023;
    uint64_t tmp34 = node_1970 + node_1973;
    uint64_t tmp35 = node_1982 + node_1985;
    uint64_t tmp36 = node_2026 + node_2029;
    uint64_t tmp37 = node_1994 + node_1997;
    uint64_t tmp38 = node_1988 + node_1991;
    uint64_t node_1657 = tmp29 + tmp30 - tmp31 - tmp32 - (tmp33 - tmp34 - tmp35) - (tmp36 - tmp37 - tmp38);

    return {
        node_152 + node_412,
        node_154 + node_717,
        node_156 + node_906,
        node_158 + node_1086,
        node_153 + node_1237,
        node_155 + node_1375,
        node_157 + node_1492,
        node_159 + node_1657,
        node_152 - node_412,
        node_154 - node_717,
        node_156 - node_906,
        node_158 - node_1086,
        node_153 - node_1237,
        node_155 - node_1375,
        node_157 - node_1492,
        node_159 - node_1657
    };
}

} // namespace tip5xx
