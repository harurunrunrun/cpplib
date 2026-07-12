// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <optional>
#include <random>
#include <vector>
#include "../../src/structure/trie/binary_trie.hpp"

std::optional<int> kth_naive(std::vector<int> values, int k, int xor_value){
    if(k < 0 || static_cast<int>(values.size()) <= k) return std::nullopt;
    std::stable_sort(values.begin(), values.end(), [&](int a, int b){
        return (a ^ xor_value) < (b ^ xor_value);
    });
    return values[static_cast<std::size_t>(k)];
}

int count_less_naive(const std::vector<int>& values, int x, int xor_value){
    int result = 0;
    for(int v: values) if((v ^ xor_value) < x) result++;
    return result;
}

int count_naive(const std::vector<int>& values, int x){
    return static_cast<int>(std::count(values.begin(), values.end(), x));
}

int main(){
    BinaryTrie<int, 10, 20000> trie;
    std::vector<int> values;
    std::mt19937 rng(20260716);

    for(int step = 0; step < 5000; step++){
        int x = static_cast<int>(rng() % 1024);
        int type = static_cast<int>(rng() % 10);
        if(type < 4){
            trie.insert(x);
            values.push_back(x);
        }else if(type < 6){
            bool expected = count_naive(values, x) > 0;
            assert(trie.erase(x) == expected);
            if(expected){
                values.erase(std::find(values.begin(), values.end(), x));
            }
        }else{
            int y = static_cast<int>(rng() % 1024);
            int k = static_cast<int>(rng() % 1100) - 20;
            assert(trie.count(x) == count_naive(values, x));
            assert(trie.contains(x) == (count_naive(values, x) > 0));
            assert(trie.kth(k, y) == kth_naive(values, k, y));
            assert(trie.min_element(y) == kth_naive(values, 0, y));
            assert(trie.max_element(y) == kth_naive(values, static_cast<int>(values.size()) - 1, y));
            assert(trie.count_less(x, y) == count_less_naive(values, x, y));
        }
        assert(trie.size() == static_cast<int>(values.size()));
        assert(trie.empty() == values.empty());
    }
}
