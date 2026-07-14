#pragma once

#include <algorithm>
#include <string>
#include <vector>

#include "suffix_array.hpp"

namespace maximum_consecutive_repetitions_internal{

struct LcpQuery{
    int length;
    std::vector<int> rank;
    std::vector<int> logarithm;
    std::vector<std::vector<int>> table;

    explicit LcpQuery(const std::string& text): length(static_cast<int>(text.size())){
        const std::vector<int> suffixes = suffix_array(text);
        rank.resize(static_cast<std::size_t>(length));
        for(int index = 0; index < length; index++){
            rank[static_cast<std::size_t>(suffixes[static_cast<std::size_t>(index)])] = index;
        }

        const std::vector<int> lcp = lcp_array(text, suffixes);
        logarithm.resize(lcp.size() + 1);
        for(std::size_t size = 2; size < logarithm.size(); size++){
            logarithm[size] = logarithm[size / 2] + 1;
        }
        if(lcp.empty()) return;

        table.push_back(lcp);
        for(int level = 1; (1 << level) <= static_cast<int>(lcp.size()); level++){
            const int width = 1 << level;
            const int half = width >> 1;
            std::vector<int> row(lcp.size() - static_cast<std::size_t>(width) + 1);
            for(std::size_t left = 0; left < row.size(); left++){
                row[left] = std::min(
                    table[static_cast<std::size_t>(level - 1)][left],
                    table[static_cast<std::size_t>(level - 1)][left + static_cast<std::size_t>(half)]
                );
            }
            table.push_back(std::move(row));
        }
    }

    int query(int first, int second) const{
        if(first == second) return length - first;
        int left = rank[static_cast<std::size_t>(first)];
        int right = rank[static_cast<std::size_t>(second)];
        if(right < left) std::swap(left, right);
        const int width = right - left;
        const int level = logarithm[static_cast<std::size_t>(width)];
        return std::min(
            table[static_cast<std::size_t>(level)][static_cast<std::size_t>(left)],
            table[static_cast<std::size_t>(level)][static_cast<std::size_t>(right - (1 << level))]
        );
    }
};

} // namespace maximum_consecutive_repetitions_internal

inline int maximum_consecutive_repetitions(const std::string& text){
    const int length = static_cast<int>(text.size());
    if(length == 0) return 0;

    const maximum_consecutive_repetitions_internal::LcpQuery forward(text);
    const std::string reversed(text.rbegin(), text.rend());
    const maximum_consecutive_repetitions_internal::LcpQuery backward(reversed);

    int answer = 1;
    for(int period = 1; period < length; period++){
        for(int left = 0; left + period < length; left += period){
            const int common_right = forward.query(left, left + period);
            const int common_left = left == 0
                ? 0
                : backward.query(length - left, length - left - period);
            answer = std::max(answer, (common_left + common_right) / period + 1);
        }
    }
    return answer;
}
