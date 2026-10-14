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

#include "../../src/algorithm/string/automata/aho_corasick.hpp"
#include "../../src/algorithm/string/palindrome/palindromic_tree.hpp"
#include "../../src/algorithm/string/palindrome/persistent_palindromic_tree.hpp"
#include "../../src/algorithm/string/palindrome/fully_persistent_palindromic_tree.hpp"
#include "../../src/algorithm/string/palindrome/deque_palindromic_tree.hpp"
#include "../../src/algorithm/tree/decomposition/auxiliary_tree.hpp"
#include "../../src/algorithm/tree/decomposition/tree_mo.hpp"
#include "../../src/algorithm/tree/query/tree_path_order_statistics.hpp"

#include "../../src/algorithm/string/automata/aho_corasick_representation.hpp"
#include "../../src/algorithm/string/palindrome/palindromic_tree_representation.hpp"
#include "../../src/algorithm/string/palindrome/deque_palindrome_queries.hpp"
#include "../../src/algorithm/string/suffix/maximum_substring_occurrences_by_length.hpp"
#include "../../src/algorithm/string/suffix/longest_common_substring_multiple_length.hpp"

void dynamic_capacity_string_tree_test(){
 assert(aho_corasick_representation<26>({"ab", "bc"}).terminal_state.size()==2);
 assert(palindromic_tree_representation("ababa").nodes.size()==5);
 DequePalindromeQueries<26> queries; queries.push_front('a'); queries.push_back('b'); assert(queries.size()==2); queries.pop_front(); assert(queries.size()==1);
 assert((maximum_substring_occurrences_by_length("aaa")==std::vector<long long>{3,2,1}));
 assert(longest_common_substring_multiple_length(std::vector<std::string>{"abcab","zzcab"})==3);

AhoCorasick<26> ac;ac.add("aaa");ac.build();
 PalindromicTree<26> p("ababa");assert(p.distinct_palindromes()==5);
 PersistentPalindromicTree<26> pp("ababa");assert(pp.distinct_palindromes(5)==5);
 FullyPersistentPalindromicTree<26> fp;int v=fp.append(0,'a');v=fp.append(v,'b');v=fp.append(v,'a');assert(fp.distinct_palindromes(v)==3);int u=fp.append(0,'b');u=fp.append(u,'b');assert(fp.distinct_palindromes(u)==2);assert(fp.str(v)=="aba");
 DequePalindromicTree<26> dp;dp.push_back('a');dp.push_back('b');dp.push_front('b');dp.push_front('a');assert(dp.size()==4);dp.pop_back();assert(dp.size()==3);
 SuffixAutomaton<26> sam("ababa");assert(sam.count_distinct_substrings()==9);
 AuxiliaryTree<> aux(4);aux.add_edge(0,1);aux.add_edge(1,2);aux.add_edge(1,3);aux.build();
 TreeMo<> mo(4);mo.add_edge(0,1);mo.add_edge(1,2);mo.add_edge(1,3);mo.build();
 TreePathOrderStatistics<int> stats(4);stats.add_edge(0,1);stats.add_edge(1,2);stats.add_edge(1,3);stats.build({3,1,4,2});assert(stats.kth_smallest(2,3,1)==2);
}

int main(){
    dynamic_capacity_string_tree_test();
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
