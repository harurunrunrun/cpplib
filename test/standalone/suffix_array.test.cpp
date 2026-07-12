// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include "../../src/algorithm/string/suffix_array.hpp"

std::vector<int> naive_suffix_array(const std::string& s){
    std::vector<int> sa(s.size());
    for(int i = 0; i < static_cast<int>(s.size()); i++) sa[static_cast<std::size_t>(i)] = i;
    std::sort(sa.begin(), sa.end(), [&](int i, int j){
        return s.substr(static_cast<std::size_t>(i)) < s.substr(static_cast<std::size_t>(j));
    });
    return sa;
}

std::vector<int> naive_lcp_array(const std::string& s, const std::vector<int>& sa){
    std::vector<int> lcp;
    for(int i = 0; i + 1 < static_cast<int>(sa.size()); i++){
        int a = sa[static_cast<std::size_t>(i)];
        int b = sa[static_cast<std::size_t>(i + 1)];
        int len = 0;
        while(a + len < static_cast<int>(s.size()) &&
              b + len < static_cast<int>(s.size()) &&
              s[static_cast<std::size_t>(a + len)] == s[static_cast<std::size_t>(b + len)]){
            len++;
        }
        lcp.push_back(len);
    }
    return lcp;
}

std::string random_string(std::mt19937& rng, int n){
    std::string s;
    for(int i = 0; i < n; i++) s.push_back(static_cast<char>('a' + rng() % 5));
    return s;
}

int main(){
    int query_count;
    if(std::cin >> query_count){
        while(query_count--){
            std::string s;
            std::cin >> s;
            if(s == "-") s.clear();
            const auto sa = suffix_array(s);
            const auto lcp = lcp_array(s, sa);
            std::cout << sa.size();
            for(int value: sa) std::cout << ' ' << value;
            std::cout << '\n' << lcp.size();
            for(int value: lcp) std::cout << ' ' << value;
            std::cout << '\n';
        }
        return 0;
    }
    {
        std::string s = "banana";
        std::vector<int> expected_sa = {5, 3, 1, 0, 4, 2};
        std::vector<int> expected_lcp = {1, 3, 0, 0, 2};
        assert(suffix_array(s) == expected_sa);
        assert(lcp_array(s, expected_sa) == expected_lcp);
        assert(lcp_array(s) == expected_lcp);
    }

    std::mt19937 rng(20260720);
    for(int n = 0; n <= 80; n++){
        for(int step = 0; step < 40; step++){
            std::string s = random_string(rng, n);
            auto expected_sa = naive_suffix_array(s);
            auto expected_lcp = naive_lcp_array(s, expected_sa);
            assert(suffix_array(s) == expected_sa);
            assert(lcp_array(s) == expected_lcp);

            std::vector<int> v;
            for(char c: s) v.push_back(c - 'a');
            assert(suffix_array(v) == expected_sa);
            assert(lcp_array(v) == expected_lcp);
        }
    }
}
