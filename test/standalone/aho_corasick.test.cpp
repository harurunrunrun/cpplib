// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>
#include "../../src/algorithm/string/automata/aho_corasick.hpp"

template<class Function>
void expect_aho_error(Function&& function){
    bool thrown = false;
    try{
        function();
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

void test_state_api(){
    AhoCorasick<3, 32> ac;
    assert(ac.node_count() == 1);
    assert(!ac.is_built());

    const int root = ac.add("");
    const int a = ac.add("a");
    assert(ac.add("a") == a);
    const int ba = ac.add("ba");
    const int aba = ac.add("aba");
    assert(ac.node_count() == 6);
    expect_aho_error([&]{ (void)ac.add("d"); });
    assert(ac.node_count() == 6);
    const int b_trie = ac.parent(ba);
    const int ab_trie = ac.parent(aba);
    assert(ac.parent(root) == root);
    assert(ac.parent(a) == root);
    assert(ac.parent(b_trie) == root);
    assert(ac.parent(ba) == b_trie);
    assert(ac.parent(ab_trie) == a);
    assert(ac.parent(aba) == ab_trie);
    assert(root == 0);
    assert(ac.terminal_count(root) == 1);
    assert(ac.terminal_count(a) == 2);
    assert(ac.terminal_count(ba) == 1);
    assert(ac.terminal_count(aba) == 1);
    expect_aho_error([&]{ (void)ac.next_state(0, 'a'); });
    expect_aho_error([&]{ (void)ac.output_count(0); });
    expect_aho_error([&]{ (void)ac.failure_link(0); });
    expect_aho_error([&]{ (void)ac.output_link(0); });
    expect_aho_error([&]{ (void)ac.count_matches("a"); });

    ac.build();
    assert(ac.is_built());
    ac.build();
    const int b = ac.next_state(root, 'b');
    const int ab = ac.next_state(a, 'b');
    assert(b == b_trie);
    assert(ab == ab_trie);
    assert(ac.next_state(b, 'a') == ba);
    assert(ac.next_state(ab, 'a') == aba);
    assert(ac.next_state(a, 'a') == a);
    assert(ac.terminal_count(b) == 0);
    assert(ac.failure_link(root) == root);
    assert(ac.failure_link(a) == root);
    assert(ac.failure_link(b) == root);
    assert(ac.failure_link(ba) == a);
    assert(ac.failure_link(ab) == b);
    assert(ac.failure_link(aba) == ba);
    assert(ac.output_link(root) == -1);
    assert(ac.output_link(a) == root);
    assert(ac.output_link(b) == root);
    assert(ac.output_link(ba) == a);
    assert(ac.output_link(ab) == root);
    assert(ac.output_link(aba) == ba);
    assert(ac.terminal_count(ab) == 0);
    assert(ac.output_count(root) == 1);
    assert(ac.output_count(a) == 3);
    assert(ac.output_count(b) == 1);
    assert(ac.output_count(ba) == 4);
    assert(ac.output_count(ab) == 1);
    assert(ac.output_count(aba) == 5);
    assert(ac.count_matches("aba") == 10);
    expect_aho_error([&]{ (void)ac.count_matches("d"); });

    expect_aho_error([&]{ (void)ac.next_state(-1, 'a'); });
    expect_aho_error([&]{ (void)ac.next_state(ac.node_count(), 'a'); });
    expect_aho_error([&]{ (void)ac.next_state(0, 'd'); });
    expect_aho_error([&]{ (void)ac.terminal_count(-1); });
    expect_aho_error([&]{ (void)ac.terminal_count(ac.node_count()); });
    expect_aho_error([&]{ (void)ac.output_count(-1); });
    expect_aho_error([&]{ (void)ac.output_count(ac.node_count()); });
    expect_aho_error([&]{ (void)ac.add("c"); });

    expect_aho_error([&]{ (void)ac.parent(-1); });
    expect_aho_error([&]{ (void)ac.parent(ac.node_count()); });
    expect_aho_error([&]{ (void)ac.failure_link(-1); });
    expect_aho_error([&]{ (void)ac.failure_link(ac.node_count()); });
    expect_aho_error([&]{ (void)ac.output_link(-1); });
    expect_aho_error([&]{ (void)ac.output_link(ac.node_count()); });

    ac.clear();
    assert(ac.node_count() == 1);
    assert(!ac.is_built());
    assert(ac.add("c") == 1);
    ac.build();
    assert(ac.count_matches("cc") == 2);

    AhoCorasick<2, 2> capacity;
    capacity.add("a");
    expect_aho_error([&]{ (void)capacity.add("b"); });
}

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
    test_state_api();
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
