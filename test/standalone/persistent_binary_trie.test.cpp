// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <optional>
#include <random>
#include <vector>
#include "../../src/structure/trie/persistent_binary_trie.hpp"

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
    PersistentBinaryTrie<int, 10, 30000, 1024> trie;
    std::vector<std::vector<int>> versions(1);
    std::mt19937 rng(20260718);

    for(int step = 0; step < 700; step++){
        int base = static_cast<int>(rng() % versions.size());
        int x = static_cast<int>(rng() % 1024);
        int type = static_cast<int>(rng() % 10);
        int next_version;
        auto next_values = versions[static_cast<std::size_t>(base)];
        if(type < 5){
            next_version = trie.insert(base, x);
            next_values.push_back(x);
        }else if(type < 8){
            next_version = trie.erase(base, x);
            auto it = std::find(next_values.begin(), next_values.end(), x);
            if(it != next_values.end()) next_values.erase(it);
        }else{
            next_version = trie.fork(base);
        }
        versions.push_back(next_values);
        assert(next_version == static_cast<int>(versions.size()) - 1);

        for(int q = 0; q < 8; q++){
            int v = static_cast<int>(rng() % versions.size());
            int y = static_cast<int>(rng() % 1024);
            int z = static_cast<int>(rng() % 1024);
            int k = static_cast<int>(rng() % 1100) - 20;
            auto& values = versions[static_cast<std::size_t>(v)];
            assert(trie.size(v) == static_cast<int>(values.size()));
            assert(trie.count(v, y) == count_naive(values, y));
            assert(trie.contains(v, y) == (count_naive(values, y) > 0));
            assert(trie.kth(v, k, z) == kth_naive(values, k, z));
            assert(trie.min_element(v, z) == kth_naive(values, 0, z));
            assert(trie.max_element(v, z) == kth_naive(values, static_cast<int>(values.size()) - 1, z));
            assert(trie.count_less(v, y, z) == count_less_naive(values, y, z));
        }
    }
}
