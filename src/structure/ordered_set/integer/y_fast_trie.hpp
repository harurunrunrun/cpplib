#ifndef CPPLIB_SRC_STRUCTURE_ORDERED_SET_INTEGER_Y_FAST_TRIE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_ORDERED_SET_INTEGER_Y_FAST_TRIE_HPP_INCLUDED

#include <cstdint>

#include "detail/y_fast_trie_implementation.hpp"

template<int BITS>
class YFastTrie : public YFastTrieImplementation<std::uint64_t, BITS> {};

#endif  // CPPLIB_SRC_STRUCTURE_ORDERED_SET_INTEGER_Y_FAST_TRIE_HPP_INCLUDED
