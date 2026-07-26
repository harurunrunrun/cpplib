// competitive-verifier: STANDALONE

#include "../../src/algorithm/string/suffix/suffix_automaton.hpp"

#include <cassert>
#include <cstddef>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

void test_byte_suffix_automaton_capacity(){
    using suffix_automaton_internal::
        byte_suffix_automaton_max_text_size;
    using suffix_automaton_internal::
        checked_byte_suffix_automaton_state_capacity;

    const std::size_t int_max =
        static_cast<std::size_t>(std::numeric_limits<int>::max());
    assert(byte_suffix_automaton_max_text_size == int_max / 2 + 1);
    assert(checked_byte_suffix_automaton_state_capacity(0) == 1);
    assert(checked_byte_suffix_automaton_state_capacity(1) == 2);
    assert(checked_byte_suffix_automaton_state_capacity(2) == 3);
    assert(checked_byte_suffix_automaton_state_capacity(
        byte_suffix_automaton_max_text_size
    ) == int_max);

    bool thrown = false;
    try{
        (void)checked_byte_suffix_automaton_state_capacity(
            byte_suffix_automaton_max_text_size + 1
        );
    }catch(const std::length_error&){
        thrown = true;
    }
    assert(thrown);
}

void self_test(){
    test_byte_suffix_automaton_capacity();

    SuffixAutomaton<3, 16> empty;
    assert(empty.size() == 1);
    assert(empty.last() == 0);
    assert(empty[0].link == -1);
    assert(empty[0].length == 0);
    for(int next: empty[0].next) assert(next == -1);

    SuffixAutomaton<3, 16> automaton("ababa");
    assert(automaton.size() >= 6);
    assert(0 <= automaton.last() && automaton.last() < automaton.size());
    assert(automaton[automaton.last()].length == 5);
    assert(automaton[0].next[0] != -1);
    assert(automaton.contains("aba"));
    assert(!automaton.contains("ac"));
    assert(automaton.count_distinct_substrings() == 9);
    automaton.build_occurrences();
    automaton.build_occurrences();
    assert(automaton.occurrence_count("aba") == 2);
    assert((automaton.maximum_occurrences_by_length() ==
            std::vector<long long>{3, 2, 2, 1, 1}));

    const int extended = automaton.extend('c');
    assert(extended == automaton.last());
    assert(automaton[extended].length == 6);
    assert(automaton.contains("bac"));
    assert((automaton.maximum_occurrences_by_length() ==
            std::vector<long long>{3, 2, 2, 1, 1, 1}));

    bool thrown = false;
    try{
        (void)automaton[-1];
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        (void)automaton.contains("d");
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    SuffixAutomaton<1, 2> full;
    assert(full.maximum_occurrences_by_length().empty());
    full.extend('a');
    thrown = false;
    try{
        (void)full.extend('a');
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    self_test();
    int case_count;
    if(!(std::cin >> case_count)) return 0;

    for(int case_index = 0; case_index < case_count; case_index++){
        std::string source, target;
        int query_count;
        std::cin >> source >> target >> query_count;

        SuffixAutomaton<4, 256> automaton;
        for(char c: source){
            automaton.extend(c);
            automaton.build_occurrences();
        }

        auto match = automaton.longest_common_substring(target);
        if(match.position < 0 ||
           match.position + match.length > static_cast<int>(target.size()) ||
           !automaton.contains(target.substr(
               static_cast<std::size_t>(match.position),
               static_cast<std::size_t>(match.length)
           ))){
            return 1;
        }

        std::cout << automaton.count_distinct_substrings()
                  << ' ' << match.length << '\n';
        for(int query_index = 0; query_index < query_count; query_index++){
            std::string pattern;
            std::cin >> pattern;
            std::cout << automaton.contains(pattern)
                      << ' ' << automaton.occurrence_count(pattern) << '\n';
        }
    }
}
