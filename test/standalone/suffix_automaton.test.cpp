// competitive-verifier: STANDALONE

#include "../../src/algorithm/string/suffix_automaton.hpp"

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>

void self_test(){
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

    const int extended = automaton.extend('c');
    assert(extended == automaton.last());
    assert(automaton[extended].length == 6);
    assert(automaton.contains("bac"));

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