// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <vector>
#include "../../src/structure/trie/persistent_trie.hpp"

std::string random_string(std::mt19937& rng){
    int n = static_cast<int>(rng() % 8);
    std::string s;
    for(int i = 0; i < n; i++) s.push_back(static_cast<char>('a' + rng() % 4));
    return s;
}

int prefix_count_naive(const std::map<std::string, int>& mp, const std::string& prefix){
    int result = 0;
    for(auto& [s, count]: mp){
        if(s.size() >= prefix.size() && s.compare(0, prefix.size(), prefix) == 0){
            result += count;
        }
    }
    return result;
}

int main(){
    int input_q;
    if(std::cin >> input_q){
        PersistentTrie<4, 30000, 1024> input_trie;
        while(input_q--){
            std::string type;
            std::cin >> type;
            if(type == "INSERT"){
                int version;
                std::string value;
                std::cin >> version >> value;
                std::cout << input_trie.insert(version, value) << '\n';
            }else if(type == "ERASE"){
                int version;
                std::string value;
                std::cin >> version >> value;
                std::cout << input_trie.erase(version, value) << '\n';
            }else if(type == "FORK"){
                int version;
                std::cin >> version;
                std::cout << input_trie.fork(version) << '\n';
            }else if(type == "COUNT"){
                int version;
                std::string value;
                std::cin >> version >> value;
                std::cout << input_trie.count(version, value) << '\n';
            }else if(type == "CONTAINS"){
                int version;
                std::string value;
                std::cin >> version >> value;
                std::cout << input_trie.contains(version, value) << '\n';
            }else if(type == "PREFIX"){
                int version;
                std::string value;
                std::cin >> version >> value;
                std::cout << input_trie.prefix_count(version, value) << '\n';
            }
        }
        return 0;
    }
    PersistentTrie<4, 30000, 1024> trie;
    std::vector<std::map<std::string, int>> versions(1);
    std::mt19937 rng(20260717);
    assert(trie.versions() == 1);
    assert(trie.node_count() == 2);
    assert(trie.node(0, "") == trie.root(0));
    assert(trie.size(0) == 0);

    for(int step = 0; step < 700; step++){
        int base = static_cast<int>(rng() % versions.size());
        std::string s = random_string(rng);
        int type = static_cast<int>(rng() % 10);
        int next_version;
        auto next_map = versions[static_cast<std::size_t>(base)];
        int nodes_before = trie.node_count();
        int base_root = trie.root(base);
        bool existed = trie.contains(base, s);
        if(type < 5){
            next_version = trie.insert(base, s);
            next_map[s]++;
            assert(trie.node_count() == nodes_before + static_cast<int>(s.size()) + 1);
        }else if(type < 8){
            next_version = trie.erase(base, s);
            if(next_map[s] > 0) next_map[s]--;
            int expected_growth = existed ? static_cast<int>(s.size()) + 1 : 0;
            assert(trie.node_count() == nodes_before + expected_growth);
        }else{
            next_version = trie.fork(base);
            assert(trie.node_count() == nodes_before);
        }
        versions.push_back(next_map);
        assert(next_version == static_cast<int>(versions.size()) - 1);
        assert(trie.versions() == static_cast<int>(versions.size()));
        assert(trie.node(next_version, "") == trie.root(next_version));
        if(type >= 8 || (type >= 5 && !existed)){
            assert(trie.root(next_version) == base_root);
        }else{
            assert(trie.root(next_version) != base_root);
        }

        for(int q = 0; q < 8; q++){
            int v = static_cast<int>(rng() % versions.size());
            std::string t = random_string(rng);
            auto& mp = versions[static_cast<std::size_t>(v)];
            assert(trie.count(v, t) == mp[t]);
            assert(trie.contains(v, t) == (mp[t] > 0));
            assert(trie.prefix_count(v, t) == prefix_count_naive(mp, t));
            int expected_size = 0;
            for(const auto& entry: mp) expected_size += entry.second;
            assert(trie.size(v) == expected_size);
            assert(trie.node(v, "") == trie.root(v));
        }
    }
}
