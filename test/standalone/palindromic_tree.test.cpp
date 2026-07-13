// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <random>
#include <set>
#include <string>
#include <vector>
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

std::vector<int> suffix_pal_lengths_naive(const std::string& s){
    std::vector<int> result;
    for(int len = static_cast<int>(s.size()); len >= 1; len--){
        std::string t = s.substr(s.size() - static_cast<std::size_t>(len));
        if(is_palindrome(t)) result.push_back(len);
    }
    return result;
}

int longest_pal_naive(const std::set<std::string>& palindromes){
    int result = 0;
    for(const auto& s: palindromes){
        result = std::max(result, static_cast<int>(s.size()));
    }
    return result;
}

std::string random_string(std::mt19937& rng, int n){
    std::string s;
    for(int i = 0; i < n; i++) s.push_back(static_cast<char>('a' + rng() % 4));
    return s;
}

void check_series_links(const PalindromicTree<4, 128>& tree){
    for(int v = 2; v < tree.node_count(); v++){
        const int parent = tree.parent(v);
        if(tree.length(v) == 1){
            assert(parent == 0);
        }else if(tree.length(v) == 2){
            assert(parent == 1);
        }else{
            const std::string palindrome = tree.palindrome(v);
            assert(tree.palindrome(parent) == palindrome.substr(1, palindrome.size() - 2));
        }
        int link = tree.link(v);
        assert(tree.diff(v) == tree.length(v) - tree.length(link));
        if(tree.diff(v) == tree.diff(link)){
            assert(tree.series_link(v) == tree.series_link(link));
        }else{
            assert(tree.series_link(v) == link);
        }
    }
}

void self_test(){
    {
        PalindromicTree<26, 32> tree("ababa");
        assert(tree.distinct_palindromes() == 5);
        assert(tree.contains("aba"));
        assert(!tree.contains("abb"));
        assert(tree.contains(""));
        assert(tree.find_node("aba") != -1);
        assert(tree.occurrence_count("a") == 3);
        assert(tree.occurrence_count("aba") == 2);
        assert(tree.occurrence_count("") == 6);
        assert(tree.count_palindromic_substrings() == 9);
        assert(tree.longest_palindrome_length() == 5);
        assert(tree.palindrome(tree.longest_palindrome_node()) == "ababa");
        assert((tree.suffix_palindrome_lengths() == std::vector<int>{5, 3, 1}));
        auto nodes = tree.palindrome_nodes();
        for(int v: nodes){
            auto [l, r] = tree.first_occurrence_range(v);
            assert(tree.palindrome(v) == tree.str().substr(static_cast<std::size_t>(l), static_cast<std::size_t>(r - l)));
        }
        tree.add('a');
        assert(tree.occurrence_count("a") == 4);
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
                assert(tree.suffix_palindrome_lengths() == suffix_pal_lengths_naive(prefix));
                assert(tree.count_suffix_palindromes() == static_cast<int>(suffix_pal_lengths_naive(prefix).size()));
            }
            auto set = palindrome_set(s);
            assert(tree.distinct_palindromes() == static_cast<int>(set.size()));
            assert(tree.count_palindromic_substrings() == count_pal_substrings_naive(s));
            assert(tree.longest_palindrome_length() == longest_pal_naive(set));
            auto pals = tree.palindromes();
            assert(std::set<std::string>(pals.begin(), pals.end()) == set);
            check_series_links(tree);
            for(const auto& t: set){
                assert(tree.contains(t));
                assert(tree.find_node(t) != -1);
                assert(tree.occurrence_count(t) == occurrence_naive(s, t));
            }
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
        if(m == -1){
            auto large_tree = std::make_unique<PalindromicTree<1, 100005>>(s);
            std::cout << large_tree->count_palindromic_substrings() << '\n';
            continue;
        }
        PalindromicTree<4, 256> tree(s);
        auto pals = tree.palindromes();
        std::sort(pals.begin(), pals.end());
        auto suffix = tree.suffix_palindrome_lengths();

        std::cout << tree.distinct_palindromes() << ' '
                  << tree.count_palindromic_substrings() << ' '
                  << tree.longest_palindrome_length() << ' '
                  << tree.longest_suffix_palindrome_length() << '\n';
        for(int i = 0; i < static_cast<int>(pals.size()); i++){
            if(i) std::cout << ' ';
            std::cout << pals[static_cast<std::size_t>(i)];
        }
        std::cout << '\n';
        for(int i = 0; i < static_cast<int>(suffix.size()); i++){
            if(i) std::cout << ' ';
            std::cout << suffix[static_cast<std::size_t>(i)];
        }
        std::cout << '\n';
        while(m--){
            std::string t;
            std::cin >> t;
            std::cout << tree.contains(t) << ' ' << tree.occurrence_count(t) << '\n';
        }
    }
}
