// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <vector>
#include "../../src/structure/trie/trie.hpp"

template<class Function>
void expect_trie_error(Function&& function){
    bool thrown = false;
    try{
        function();
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

void test_node_api(){
    Trie<2, 16, '-'> trie;
    trie.insert("--");
    trie.insert("--");
    trie.insert("-.");

    const int dash = trie.next_node(0, '-');
    const int dash_dash = trie.next_node(dash, '-');
    const int dash_dot = trie.next_node(dash, '.');
    assert(dash >= 0 && dash_dash >= 0 && dash_dot >= 0);
    assert(trie.next_node(0, '.') == -1);
    assert(trie.terminal_count(0) == 0);
    assert(trie.terminal_count(dash) == 0);
    assert(trie.terminal_count(dash_dash) == 2);
    assert(trie.terminal_count(dash_dot) == 1);
    expect_trie_error([&]{ (void)trie.next_node(-1, '-'); });
    expect_trie_error([&]{ (void)trie.next_node(trie.node_count(), '-'); });
    expect_trie_error([&]{ (void)trie.next_node(0, 'a'); });
    expect_trie_error([&]{ (void)trie.terminal_count(-1); });
    expect_trie_error([&]{ (void)trie.terminal_count(trie.node_count()); });

    trie.clear();
    assert(trie.empty());
    assert(trie.size() == 0);
    assert(trie.node_count() == 1);
    assert(trie.next_node(0, '-') == -1);
    assert(trie.terminal_count(0) == 0);
    trie.insert(".");
    assert(trie.count(".") == 1);
}

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
    test_node_api();
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
