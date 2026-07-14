#pragma once

#include "implicit_treap.hpp"

template<auto Monoid, int MAX_SIZE>
class ImplicitTreapDeque{
    ImplicitTreap<Monoid, MAX_SIZE> sequence_;

public:
    using value_type = typename ImplicitTreap<Monoid, MAX_SIZE>::S;

    int size() const{ return sequence_.size(); }
    bool empty() const{ return sequence_.empty(); }

    void push_front(const value_type& value){ sequence_.push_front(value); }
    void push_back(const value_type& value){ sequence_.push_back(value); }
    void pop_front(){ sequence_.erase(0); }
    void pop_back(){ sequence_.erase(sequence_.size() - 1); }
    value_type get(int index){ return sequence_.get(index); }
};
