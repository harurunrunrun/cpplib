#ifndef CPPLIB_SRC_ALGORITHM_STRING_SEQUENCE_PREFIX_SUBSTRING_LCS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_SEQUENCE_PREFIX_SUBSTRING_LCS_HPP_INCLUDED

#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <string_view>
#include <vector>

struct PrefixSubstringLcsQuery{
    int prefix_length;
    int left;
    int right;
};

inline std::vector<int> prefix_substring_lcs(
    std::string_view first,
    std::string_view second,
    const std::vector<PrefixSubstringLcsQuery>& queries
){
    const int m = static_cast<int>(first.size());
    const int n = static_cast<int>(second.size());
    std::vector<int> order(queries.size());
    std::iota(order.begin(), order.end(), 0);
    for(const auto& q: queries){
        if(q.prefix_length < 0 || q.prefix_length > m ||
           q.left < 0 || q.right < q.left || q.right > n){
            throw std::out_of_range("prefix_substring_lcs: invalid query");
        }
    }
    std::sort(order.begin(), order.end(), [&](int a, int b){
        const auto& x = queries[a]; const auto& y = queries[b];
        return x.prefix_length != y.prefix_length
            ? x.prefix_length < y.prefix_length : x.left < y.left;
    });
    std::vector<int> seaweed(n), positions(n), fenwick(n+1), answer(queries.size());
    std::iota(seaweed.begin(), seaweed.end(), 0);
    std::iota(positions.begin(), positions.end(), 0);
    int row = 0;
    for(std::size_t begin = 0; begin < order.size();){
        const int prefix = queries[order[begin]].prefix_length;
        while(row < prefix){
            int incoming = -row-1;
            for(int column = 0; column < n; ++column){
                if(first[row] == second[column] || incoming > seaweed[column])
                    std::swap(incoming, seaweed[column]);
            }
            ++row;
        }
        std::sort(positions.begin(), positions.end(), [&](int a, int b){
            return seaweed[a] < seaweed[b];
        });
        std::fill(fenwick.begin(), fenwick.end(), 0);
        int activated = 0;
        std::size_t end = begin;
        const auto prefix_sum = [&](int index){
            int sum = 0;
            for(; index > 0; index -= index & -index) sum += fenwick[index];
            return sum;
        };
        while(end < order.size() && queries[order[end]].prefix_length == prefix){
            const auto& q = queries[order[end]];
            while(activated < n && seaweed[positions[activated]] < q.left){
                for(int index = positions[activated]+1; index <= n; index += index & -index)
                    ++fenwick[index];
                ++activated;
            }
            answer[order[end]] = prefix_sum(q.right)-prefix_sum(q.left);
            ++end;
        }
        begin = end;
    }
    return answer;
}

#endif  // CPPLIB_SRC_ALGORITHM_STRING_SEQUENCE_PREFIX_SUBSTRING_LCS_HPP_INCLUDED
