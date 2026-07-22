#ifndef CPPLIB_SRC_ALGORITHM_STRING_PALINDROME_DEQUE_PALINDROME_QUERIES_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_PALINDROME_DEQUE_PALINDROME_QUERIES_HPP_INCLUDED

#include "deque_palindromic_tree.hpp"

template<int ALPHABET, int MAX_NODES, int MAX_LENGTH, char OFFSET = 'a'>
class DequePalindromeQueries{
    DequePalindromicTree<ALPHABET, MAX_NODES, MAX_LENGTH, OFFSET> tree_;

public:
    int size() const{ return tree_.size(); }
    bool empty() const{ return tree_.empty(); }

    DequePalindromeStatistics push_front(char character){
        tree_.push_front(character);
        return tree_.statistics();
    }

    DequePalindromeStatistics push_back(char character){
        tree_.push_back(character);
        return tree_.statistics();
    }

    DequePalindromeStatistics pop_front(){
        tree_.pop_front();
        return tree_.statistics();
    }

    DequePalindromeStatistics pop_back(){
        tree_.pop_back();
        return tree_.statistics();
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_STRING_PALINDROME_DEQUE_PALINDROME_QUERIES_HPP_INCLUDED
