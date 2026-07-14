#pragma once

#include <array>
#include <string>

#include "suffix_array.hpp"

inline std::array<long long, 256>
distinct_substrings_by_first_byte(const std::string& text){
    std::array<long long, 256> result{};
    const std::vector<int> sa = suffix_array(text);
    const std::vector<int> lcp = lcp_array(text, sa);
    const int size = static_cast<int>(text.size());
    for(int rank = 0; rank < size; rank++){
        const int suffix = sa[static_cast<std::size_t>(rank)];
        const int duplicated_prefix = rank == 0
            ? 0
            : lcp[static_cast<std::size_t>(rank - 1)];
        result[static_cast<unsigned char>(text[static_cast<std::size_t>(suffix)])]
            += size - suffix - duplicated_prefix;
    }
    return result;
}
