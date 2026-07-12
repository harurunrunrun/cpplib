// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <random>
#include <string>
#include <vector>
#include "../../src/algorithm/string/diff.hpp"
#include "../../src/algorithm/string/edit_distance.hpp"

int edit_distance_naive(const std::string& a, const std::string& b){
    const int n = static_cast<int>(a.size());
    const int m = static_cast<int>(b.size());
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1, 1000000));
    dp[0][0] = 0;
    for(int i = 0; i <= n; i++){
        for(int j = 0; j <= m; j++){
            if(i < n) dp[i + 1][j] = std::min(dp[i + 1][j], dp[i][j] + 1);
            if(j < m) dp[i][j + 1] = std::min(dp[i][j + 1], dp[i][j] + 1);
            if(i < n && j < m) dp[i + 1][j + 1] = std::min(dp[i + 1][j + 1], dp[i][j] + (a[i] == b[j] ? 0 : 1));
        }
    }
    return dp[n][m];
}

std::string apply_diff_source(const std::vector<DiffOp<char>>& ops){
    std::string result;
    for(auto op: ops){
        if(op.tag != DiffTag::Insert) result.push_back(op.value);
    }
    return result;
}

std::string apply_diff_target(const std::vector<DiffOp<char>>& ops){
    std::string result;
    for(auto op: ops){
        if(op.tag != DiffTag::Erase) result.push_back(op.value);
    }
    return result;
}

std::string random_string(std::mt19937& rng, int n){
    std::string s;
    for(int i = 0; i < n; i++) s.push_back(static_cast<char>('a' + rng() % 4));
    return s;
}

int main(){
    assert(edit_distance(std::string("kitten"), std::string("sitting")) == 3);
    assert(edit_distance(std::string("abc"), std::string("abc")) == 0);
    assert(edit_distance(std::string("abc"), std::string("")) == 3);

    {
        auto ops = diff_sequence(std::string("abcab"), std::string("bcba"));
        assert(apply_diff_source(ops) == "abcab");
        assert(apply_diff_target(ops) == "bcba");
    }

    std::mt19937 rng(20260723);
    for(int n = 0; n <= 10; n++){
        for(int m = 0; m <= 10; m++){
            for(int step = 0; step < 50; step++){
                std::string a = random_string(rng, n);
                std::string b = random_string(rng, m);
                assert(edit_distance(a, b) == edit_distance_naive(a, b));
                auto ops = diff_sequence(a, b);
                assert(apply_diff_source(ops) == a);
                assert(apply_diff_target(ops) == b);
            }
        }
    }

    std::vector<int> x = {1, 2, 3};
    std::vector<int> y = {2, 4, 3};
    auto ops = diff_sequence(x, y);
    std::vector<int> sx, sy;
    for(auto op: ops){
        if(op.tag != DiffTag::Insert) sx.push_back(op.value);
        if(op.tag != DiffTag::Erase) sy.push_back(op.value);
    }
    assert(sx == x);
    assert(sy == y);
}
