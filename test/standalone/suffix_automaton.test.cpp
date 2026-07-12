// competitive-verifier: STANDALONE

#include <cassert>
#include <set>
#include <random>
#include <string>
#include "../../src/algorithm/string/suffix_automaton.hpp"

std::string random_string(std::mt19937& rng, int n){
    std::string s;
    for(int i = 0; i < n; i++) s.push_back(static_cast<char>('a' + rng() % 4));
    return s;
}

long long distinct_naive(const std::string& s){
    std::set<std::string> set;
    for(int l = 0; l < static_cast<int>(s.size()); l++){
        for(int r = l + 1; r <= static_cast<int>(s.size()); r++){
            set.insert(s.substr(static_cast<std::size_t>(l), static_cast<std::size_t>(r - l)));
        }
    }
    return static_cast<long long>(set.size());
}

int occurrence_naive(const std::string& s, const std::string& t){
    int result = 0;
    for(int i = 0; i + static_cast<int>(t.size()) <= static_cast<int>(s.size()); i++){
        if(s.compare(static_cast<std::size_t>(i), t.size(), t) == 0) result++;
    }
    return result;
}

int lcs_naive(const std::string& a, const std::string& b){
    int best = 0;
    for(int i = 0; i < static_cast<int>(a.size()); i++){
        for(int j = 0; j < static_cast<int>(b.size()); j++){
            int len = 0;
            while(i + len < static_cast<int>(a.size()) &&
                  j + len < static_cast<int>(b.size()) &&
                  a[static_cast<std::size_t>(i + len)] == b[static_cast<std::size_t>(j + len)]){
                len++;
            }
            best = std::max(best, len);
        }
    }
    return best;
}

int main(){
    {
        SuffixAutomaton<26, 32> sam("ababa");
        assert(sam.contains("aba"));
        assert(!sam.contains("abb"));
        assert(sam.count_distinct_substrings() == distinct_naive("ababa"));
        assert(sam.occurrence_count("a") == 3);
        assert(sam.occurrence_count("aba") == 2);
        assert(sam.occurrence_count("ba") == 2);
    }

    std::mt19937 rng(20260724);
    for(int n = 0; n <= 40; n++){
        for(int step = 0; step < 60; step++){
            std::string s = random_string(rng, n);
            std::string t = random_string(rng, n);
            SuffixAutomaton<4, 128> sam(s);
            assert(sam.count_distinct_substrings() == distinct_naive(s));
            for(int l = 0; l <= n; l++){
                for(int r = l + 1; r <= n; r++){
                    std::string sub = s.substr(static_cast<std::size_t>(l), static_cast<std::size_t>(r - l));
                    assert(sam.contains(sub));
                    assert(sam.occurrence_count(sub) == occurrence_naive(s, sub));
                }
            }
            auto match = sam.longest_common_substring(t);
            assert(match.length == lcs_naive(s, t));
            assert(match.position >= 0);
            assert(match.position + match.length <= static_cast<int>(t.size()));
        }
    }
}
