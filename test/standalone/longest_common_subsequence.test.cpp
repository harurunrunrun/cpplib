// competitive-verifier: STANDALONE

#include <cassert>
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

std::string random_string(std::mt19937& rng, int n){
    std::string s;
    for(int i = 0; i < n; i++) s.push_back(static_cast<char>('a' + rng() % 4));
    return s;
}

int main(){
    assert(longest_common_subsequence_length(std::string("abcde"), std::string("ace")) == 3);
    assert(longest_common_subsequence(std::string("abcde"), std::string("ace")) == "ace");

    std::mt19937 rng(20260721);
    for(int n = 0; n <= 10; n++){
        for(int m = 0; m <= 10; m++){
            for(int step = 0; step < 40; step++){
                std::string a = random_string(rng, n);
                std::string b = random_string(rng, m);
                int expected = lcs_naive(a, b);
                auto lcs = longest_common_subsequence(a, b);
                assert(longest_common_subsequence_length(a, b) == expected);
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
}
