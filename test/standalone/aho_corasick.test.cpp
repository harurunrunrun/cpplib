// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include "../../src/algorithm/string/aho_corasick.hpp"

std::string random_string(std::mt19937& rng, int max_len){
    int n = static_cast<int>(rng() % (max_len + 1));
    std::string s;
    for(int i = 0; i < n; i++) s.push_back(static_cast<char>('a' + rng() % 4));
    return s;
}

long long naive_count(const std::vector<std::string>& patterns, const std::string& text){
    long long result = 0;
    for(const std::string& p: patterns){
        for(int i = 0; i + static_cast<int>(p.size()) <= static_cast<int>(text.size()); i++){
            bool ok = true;
            for(int j = 0; j < static_cast<int>(p.size()); j++){
                if(text[static_cast<std::size_t>(i + j)] != p[static_cast<std::size_t>(j)]){
                    ok = false;
                    break;
                }
            }
            if(ok) result++;
        }
    }
    return result;
}

int main(){
    int pattern_count, query_count;
    if(std::cin >> pattern_count >> query_count){
        AhoCorasick<3, 4096> ac;
        for(int i = 0; i < pattern_count; i++){
            std::string pattern;
            std::cin >> pattern;
            if(pattern == "-") pattern.clear();
            ac.add(pattern);
        }
        ac.build();
        while(query_count--){
            std::string text;
            std::cin >> text;
            if(text == "-") text.clear();
            std::cout << ac.count_matches(text) << '\n';
        }
        return 0;
    }

    {
        AhoCorasick<26, 32> ac;
        const int a = ac.add("a");
        const int aa = ac.add("aa");
        const int aaa = ac.add("aaa");
        ac.build();
        assert(ac.count_matches("aaaa") == 9);
        assert(ac.parent(a) == 0);
        assert(ac.parent(aa) == a);
        assert(ac.parent(aaa) == aa);
        assert(ac.failure_link(a) == 0);
        assert(ac.failure_link(aa) == a);
        assert(ac.failure_link(aaa) == aa);
    }

    std::mt19937 rng(20260719);
    AhoCorasick<4, 2048> ac;
    std::vector<std::string> patterns;
    for(int i = 0; i < 200; i++){
        patterns.push_back(random_string(rng, 8));
        ac.add(patterns.back());
    }
    ac.build();

    for(int q = 0; q < 500; q++){
        std::string text = random_string(rng, 60);
        assert(ac.count_matches(text) == naive_count(patterns, text));
    }
}
