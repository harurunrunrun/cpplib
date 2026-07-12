// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <vector>
#include "../../src/structure/trie/trie.hpp"

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
    int query_count;
    if(std::cin >> query_count){
        Trie<4, 65536> trie;
        while(query_count--){
            int type;
            std::string s;
            std::cin >> type >> s;
            if(s == "-") s.clear();
            if(type == 0){
                trie.insert(s);
                std::cout << trie.size() << '\n';
            }else if(type == 1){
                std::cout << trie.erase(s) << ' ' << trie.size() << '\n';
            }else if(type == 2){
                std::cout << trie.count(s) << ' ' << trie.contains(s) << ' '
                          << trie.size() << '\n';
            }else{
                std::cout << trie.prefix_count(s) << ' ' << trie.size() << '\n';
            }
        }
        return 0;
    }
    Trie<4, 2048> trie;
    std::map<std::string, int> naive;
    std::mt19937 rng(20260715);
    int total = 0;

    for(int step = 0; step < 5000; step++){
        std::string s = random_string(rng);
        int type = static_cast<int>(rng() % 8);
        if(type < 4){
            trie.insert(s);
            naive[s]++;
            total++;
        }else if(type < 6){
            bool expected = naive[s] > 0;
            assert(trie.erase(s) == expected);
            if(expected){
                naive[s]--;
                total--;
            }
        }else{
            assert(trie.count(s) == naive[s]);
            assert(trie.contains(s) == (naive[s] > 0));
            assert(trie.prefix_count(s) == prefix_count_naive(naive, s));
        }
        assert(trie.size() == total);
        assert(trie.empty() == (total == 0));
    }
}
