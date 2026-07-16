// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "../../src/algorithm/string/longest_common_subsequence.hpp"

int lcs_naive(const std::string& a, const std::string& b){
    int best = 0;
    const int n = static_cast<int>(a.size());
    for(int mask = 0; mask < (1 << n); mask++){
        std::string s;
        for(int i = 0; i < n; i++) if(mask >> i & 1) s.push_back(a[static_cast<std::size_t>(i)]);
        int p = 0;
        for(char c: b){
            if(p < static_cast<int>(s.size()) && s[static_cast<std::size_t>(p)] == c) p++;
        }
        if(p == static_cast<int>(s.size())) best = std::max(best, static_cast<int>(s.size()));
    }
    return best;
}

bool is_subsequence(const std::string& s, const std::string& t){
    int p = 0;
    for(char c: t){
        if(p < static_cast<int>(s.size()) && s[static_cast<std::size_t>(p)] == c) p++;
    }
    return p == static_cast<int>(s.size());
}

int lcs_dp(const std::string& a, const std::string& b){
    std::vector<int> dp(b.size() + 1);
    for(char left: a){
        int diagonal = 0;
        for(std::size_t j = 0; j < b.size(); j++){
            const int old = dp[j + 1];
            if(left == b[j]) dp[j + 1] = diagonal + 1;
            else dp[j + 1] = std::max(dp[j + 1], dp[j]);
            diagonal = old;
        }
    }
    return dp.back();
}

std::string random_bytes(std::mt19937& rng, int n){
    std::string s;
    s.reserve(static_cast<std::size_t>(n));
    for(int i = 0; i < n; i++){
        s.push_back(static_cast<char>(rng() & 255U));
    }
    return s;
}

int hex_digit(char value){
    if('0' <= value && value <= '9') return value - '0';
    if('a' <= value && value <= 'f') return value - 'a' + 10;
    assert(false);
    return 0;
}

std::string decode_hex(const std::string& encoded){
    if(encoded == "-") return {};
    assert(encoded.size() % 2 == 0);
    std::string result;
    result.reserve(encoded.size() / 2);
    for(std::size_t i = 0; i < encoded.size(); i += 2){
        result.push_back(static_cast<char>(
            hex_digit(encoded[i]) * 16 + hex_digit(encoded[i + 1])
        ));
    }
    return result;
}

int main(){
    int q;
    if(std::cin >> q){
        while(q--){
            std::string encoded_a, encoded_b;
            std::cin >> encoded_a >> encoded_b;
            const std::string a = decode_hex(encoded_a);
            const std::string b = decode_hex(encoded_b);
            std::cout << longest_common_subsequence_length_bytes(a, b) << '\n';
        }
        return 0;
    }

    assert(longest_common_subsequence_length(std::string("abcde"), std::string("ace")) == 3);
    assert(longest_common_subsequence(std::string("abcde"), std::string("ace")) == "ace");

    std::mt19937 rng(20260721);
    for(int n = 0; n <= 10; n++){
        for(int m = 0; m <= 10; m++){
            for(int step = 0; step < 40; step++){
                std::string a = random_bytes(rng, n);
                std::string b = random_bytes(rng, m);
                int expected = lcs_naive(a, b);
                auto lcs = longest_common_subsequence(a, b);
                assert(longest_common_subsequence_length(a, b) == expected);
                assert(longest_common_subsequence_length_bytes(a, b) == static_cast<std::size_t>(expected));
                assert(static_cast<int>(lcs.size()) == expected);
                assert(is_subsequence(lcs, a));
                assert(is_subsequence(lcs, b));
            }
        }
    }

    std::vector<int> x = {1, 2, 3, 4, 1};
    std::vector<int> y = {3, 4, 1, 2, 1, 3};
    assert(longest_common_subsequence_length(x, y) == 3);
    assert(longest_common_subsequence(x, y).size() == 3);

    std::string all_bytes;
    for(int value = 0; value < 256; value++){
        all_bytes.push_back(static_cast<char>(value));
    }
    assert(longest_common_subsequence_length_bytes(all_bytes, all_bytes) == 256);
    std::string reverse_bytes = all_bytes;
    std::reverse(reverse_bytes.begin(), reverse_bytes.end());
    assert(longest_common_subsequence_length_bytes(all_bytes, reverse_bytes) == 1);
    assert(longest_common_subsequence_length_bytes(std::string(), all_bytes) == 0);

    const std::vector<int> boundary_sizes = {
        0, 1, 2, 62, 63, 64, 65, 66, 126, 127, 128, 129
    };
    for(int n: boundary_sizes){
        for(int m: boundary_sizes){
            const std::string a = random_bytes(rng, n);
            const std::string b = random_bytes(rng, m);
            const auto expected = static_cast<std::size_t>(lcs_dp(a, b));
            assert(longest_common_subsequence_length_bytes(a, b) == expected);
            assert(longest_common_subsequence_length_bytes(b, a) == expected);
        }
    }
    for(int step = 0; step < 1000; step++){
        const std::string a = random_bytes(rng, static_cast<int>(rng() % 141U));
        const std::string b = random_bytes(rng, static_cast<int>(rng() % 141U));
        assert(
            longest_common_subsequence_length_bytes(a, b)
            == static_cast<std::size_t>(lcs_dp(a, b))
        );
    }
}
