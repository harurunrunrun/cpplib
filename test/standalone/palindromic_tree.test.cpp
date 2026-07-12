// competitive-verifier: STANDALONE

#include <cassert>
#include <random>
#include <set>
#include <string>
#include "../../src/algorithm/string/palindromic_tree.hpp"

bool is_palindrome(const std::string& s){
    for(int i = 0; i < static_cast<int>(s.size()); i++){
        if(s[static_cast<std::size_t>(i)] != s[static_cast<std::size_t>(s.size() - 1 - i)]) return false;
    }
    return true;
}

std::set<std::string> palindrome_set(const std::string& s){
    std::set<std::string> result;
    for(int l = 0; l < static_cast<int>(s.size()); l++){
        for(int r = l + 1; r <= static_cast<int>(s.size()); r++){
            std::string t = s.substr(static_cast<std::size_t>(l), static_cast<std::size_t>(r - l));
            if(is_palindrome(t)) result.insert(t);
        }
    }
    return result;
}

int count_pal_substrings_naive(const std::string& s){
    int result = 0;
    for(int l = 0; l < static_cast<int>(s.size()); l++){
        for(int r = l + 1; r <= static_cast<int>(s.size()); r++){
            if(is_palindrome(s.substr(static_cast<std::size_t>(l), static_cast<std::size_t>(r - l)))) result++;
        }
    }
    return result;
}

int occurrence_naive(const std::string& s, const std::string& t){
    int result = 0;
    for(int i = 0; i + static_cast<int>(t.size()) <= static_cast<int>(s.size()); i++){
        if(s.compare(static_cast<std::size_t>(i), t.size(), t) == 0) result++;
    }
    return result;
}

int longest_suffix_pal_naive(const std::string& s){
    for(int len = static_cast<int>(s.size()); len >= 1; len--){
        std::string t = s.substr(s.size() - static_cast<std::size_t>(len));
        if(is_palindrome(t)) return len;
    }
    return 0;
}

std::string random_string(std::mt19937& rng, int n){
    std::string s;
    for(int i = 0; i < n; i++) s.push_back(static_cast<char>('a' + rng() % 4));
    return s;
}

int main(){
    {
        PalindromicTree<26, 32> tree("ababa");
        assert(tree.distinct_palindromes() == 5);
        assert(tree.contains("aba"));
        assert(!tree.contains("abb"));
        assert(tree.occurrence_count("a") == 3);
        assert(tree.occurrence_count("aba") == 2);
        assert(tree.count_palindromic_substrings() == 9);
    }

    std::mt19937 rng(20260725);
    for(int n = 0; n <= 60; n++){
        for(int step = 0; step < 40; step++){
            std::string s = random_string(rng, n);
            PalindromicTree<4, 128> tree;
            std::string prefix;
            for(char c: s){
                prefix.push_back(c);
                tree.add(c);
                assert(tree.longest_suffix_palindrome_length() == longest_suffix_pal_naive(prefix));
            }
            auto set = palindrome_set(s);
            assert(tree.distinct_palindromes() == static_cast<int>(set.size()));
            assert(tree.count_palindromic_substrings() == count_pal_substrings_naive(s));
            for(const auto& t: set){
                assert(tree.contains(t));
                assert(tree.occurrence_count(t) == occurrence_naive(s, t));
            }
        }
    }
}
