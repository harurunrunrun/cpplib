#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string_view>
#include <vector>

inline std::size_t longest_common_subsequence_length_bytes(
    std::string_view a,
    std::string_view b
){
    if(a.size() < b.size()) std::swap(a, b);
    if(b.empty()) return 0;

    constexpr std::size_t alphabet_size = 256;
    const std::size_t word_count = b.size() / 64 + (b.size() % 64 != 0);
    std::vector<std::uint64_t> state(word_count);
    if(word_count > state.max_size() / alphabet_size){
        throw std::length_error(
            "longest_common_subsequence_length_bytes input is too long"
        );
    }
    std::vector<std::uint64_t> matches(alphabet_size * word_count);
    for(std::size_t index = 0; index < b.size(); index++){
        const auto symbol = static_cast<unsigned char>(b[index]);
        matches[static_cast<std::size_t>(symbol) * word_count + index / 64]
            |= std::uint64_t{1} << (index % 64);
    }

    for(char value: a){
        const auto symbol = static_cast<unsigned char>(value);
        const std::size_t offset = static_cast<std::size_t>(symbol) * word_count;
        std::uint64_t shift_carry = 1;
        bool subtract_borrow = false;
        for(std::size_t word = 0; word < word_count; word++){
            const std::uint64_t old_state = state[word];
            const std::uint64_t x = old_state | matches[offset + word];
            const std::uint64_t shifted = (old_state << 1) | shift_carry;
            shift_carry = old_state >> 63;

            const std::uint64_t subtrahend
                = shifted + static_cast<std::uint64_t>(subtract_borrow);
            const bool addition_overflow = subtrahend < shifted;
            const std::uint64_t difference = x - subtrahend;
            subtract_borrow = addition_overflow || x < subtrahend;
            state[word] = x & ~difference;
        }
    }

    std::size_t result = 0;
    for(std::uint64_t word: state){
        result += static_cast<std::size_t>(__builtin_popcountll(word));
    }
    return result;
}

template<class Sequence>
int longest_common_subsequence_length(const Sequence& a, const Sequence& b){
    const int n = static_cast<int>(a.size());
    const int m = static_cast<int>(b.size());
    std::vector<int> dp(m + 1), next(m + 1);
    for(int i = 0; i < n; i++){
        next[0] = 0;
        for(int j = 0; j < m; j++){
            if(a[i] == b[j]){
                next[j + 1] = dp[j] + 1;
            }else{
                next[j + 1] = std::max(next[j], dp[j + 1]);
            }
        }
        dp.swap(next);
    }
    return dp[m];
}

template<class Sequence>
Sequence longest_common_subsequence(const Sequence& a, const Sequence& b){
    const int n = static_cast<int>(a.size());
    const int m = static_cast<int>(b.size());
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1));
    for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            if(a[i] == b[j]){
                dp[i + 1][j + 1] = dp[i][j] + 1;
            }else{
                dp[i + 1][j + 1] = std::max(dp[i][j + 1], dp[i + 1][j]);
            }
        }
    }
    Sequence result;
    int i = n, j = m;
    while(i > 0 && j > 0){
        if(a[i - 1] == b[j - 1]){
            result.push_back(a[i - 1]);
            i--;
            j--;
        }else if(dp[i - 1][j] >= dp[i][j - 1]){
            i--;
        }else{
            j--;
        }
    }
    std::reverse(result.begin(), result.end());
    return result;
}
