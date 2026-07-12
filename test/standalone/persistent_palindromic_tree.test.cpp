// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <set>
#include <string>
#include <vector>
#include "../../src/algorithm/string/persistent_palindromic_tree.hpp"

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

long long count_pal_substrings_naive(const std::string& s){
    long long result = 0;
    for(int l = 0; l < static_cast<int>(s.size()); l++){
        for(int r = l + 1; r <= static_cast<int>(s.size()); r++){
            result += is_palindrome(s.substr(static_cast<std::size_t>(l), static_cast<std::size_t>(r - l)));
        }
    }
    return result;
}

std::vector<int> suffix_pal_lengths_naive(const std::string& s){
    std::vector<int> result;
    for(int len = static_cast<int>(s.size()); len >= 1; len--){
        if(is_palindrome(s.substr(s.size() - static_cast<std::size_t>(len)))) result.push_back(len);
    }
    return result;
}

int longest_pal_naive(const std::set<std::string>& palindromes){
    int result = 0;
    for(const auto& s: palindromes) result = std::max(result, static_cast<int>(s.size()));
    return result;
}

template<int MAX_NODES, int MAX_LENGTH>
void check_prefixes(const PersistentPalindromicTree<4, MAX_NODES, MAX_LENGTH>& tree, const std::string& s){
    assert(tree.versions() == static_cast<int>(s.size()) + 1);
    for(int version = 0; version <= static_cast<int>(s.size()); version++){
        std::string prefix = s.substr(0, static_cast<std::size_t>(version));
        auto set = palindrome_set(prefix);
        assert(tree.size(version) == version);
        assert(tree.distinct_palindromes(version) == static_cast<int>(set.size()));
        assert(tree.count_palindromic_substrings(version) == count_pal_substrings_naive(prefix));
        assert(tree.longest_palindrome_length(version) == longest_pal_naive(set));
        assert(tree.suffix_palindrome_lengths(version) == suffix_pal_lengths_naive(prefix));
        auto pals = tree.palindromes(version);
        assert(std::set<std::string>(pals.begin(), pals.end()) == set);
        for(const auto& t: set){
            assert(tree.contains(t, version));
            int node = tree.find_node(t, version);
            assert(node != -1);
            assert(tree.palindrome(node) == t);
        }
        assert(!tree.contains("abcd", version));
    }
    for(int node = 2; node < tree.node_count(); node++){
        int link = tree.link(node);
        assert(tree.diff(node) == tree.length(node) - tree.length(link));
        if(tree.diff(node) == tree.diff(link)){
            assert(tree.series_link(node) == tree.series_link(link));
        }else{
            assert(tree.series_link(node) == link);
        }
    }
}

void self_test(){
    {
        PersistentPalindromicTree<4, 64, 32> tree("ababa");
        check_prefixes(tree, "ababa");
        assert(tree.longest_suffix_palindrome_length(5) == 5);
        assert(tree.longest_suffix_palindrome_length(4) == 3);
        assert(tree.count_palindromic_substrings(5) == 9);
        assert(tree.contains("aba", 3));
        assert(!tree.contains("ababa", 4));
    }

    std::mt19937 rng(20261114);
    for(int n = 0; n <= 55; n++){
        for(int step = 0; step < 30; step++){
            PersistentPalindromicTree<4, 128, 64> tree;
            std::string s;
            for(int i = 0; i < n; i++){
                char c = static_cast<char>('a' + rng() % 4);
                s.push_back(c);
                assert(tree.append(c) == i + 1);
            }
            check_prefixes(tree, s);
        }
    }
}

int main(){
    int q;
    if(!(std::cin >> q)){
        self_test();
        return 0;
    }

    while(q--){
        std::string s;
        int m;
        std::cin >> s >> m;
        PersistentPalindromicTree<4, 512, 256> tree(s);
        for(int version = 0; version <= static_cast<int>(s.size()); version++){
            std::cout << tree.distinct_palindromes(version) << ' '
                      << tree.count_palindromic_substrings(version) << ' '
                      << tree.longest_palindrome_length(version) << ' '
                      << tree.longest_suffix_palindrome_length(version) << '\n';
        }
        while(m--){
            int version;
            std::string t;
            std::cin >> version >> t;
            std::cout << tree.contains(t, version) << '\n';
        }
    }
}
