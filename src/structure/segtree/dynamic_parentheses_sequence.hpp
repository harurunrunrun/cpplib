#pragma once

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "segtree.hpp"

struct ParenthesesAggregate{
    int balance;
    int minimum_prefix;
};

struct ParenthesesMonoid{
    using S = ParenthesesAggregate;
    static constexpr S op(S left, S right){
        return {
            left.balance + right.balance,
            std::min(left.minimum_prefix, left.balance + right.minimum_prefix)
        };
    }
    static constexpr S e(){ return {0, 0}; }
};

template<int MAX_SIZE>
class DynamicParenthesesSequence{
    static constexpr ParenthesesMonoid monoid_{};
    std::string sequence_;
    std::unique_ptr<Segtree<monoid_, MAX_SIZE>> tree_;

    static constexpr ParenthesesAggregate singleton(char value){
        return value == '(' ? ParenthesesAggregate{1, 0}
                            : ParenthesesAggregate{-1, -1};
    }

public:
    explicit DynamicParenthesesSequence(std::string sequence)
        : sequence_(std::move(sequence)){
        std::vector<ParenthesesAggregate> initial;
        initial.reserve(sequence_.size());
        for(char value: sequence_){
            if(value != '(' && value != ')')[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: invalid character "
                    "(DynamicParenthesesSequence)."
                );
            }
            initial.push_back(singleton(value));
        }
        tree_ = std::make_unique<Segtree<monoid_, MAX_SIZE>>(initial);
    }

    int size() const{ return static_cast<int>(sequence_.size()); }

    void toggle(int position){
        if(position < 0 || size() <= position)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: index out of range "
                "(DynamicParenthesesSequence)."
            );
        }
        char& value = sequence_[static_cast<std::size_t>(position)];
        value = value == '(' ? ')' : '(';
        tree_->set(position, singleton(value));
    }

    bool balanced() const{
        const auto whole = tree_->all_prod();
        return whole.balance == 0 && whole.minimum_prefix == 0;
    }
};
