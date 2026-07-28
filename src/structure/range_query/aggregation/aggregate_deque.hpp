#ifndef CPPLIB_SRC_STRUCTURE_RANGE_QUERY_AGGREGATION_AGGREGATE_DEQUE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_RANGE_QUERY_AGGREGATION_AGGREGATE_DEQUE_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

template<auto Monoid>
class AggregateDeque{
public:
    using value_type = typename decltype(Monoid)::S;

private:
    struct Node{
        value_type value;
        value_type aggregate;
    };

    std::vector<Node> front_stack_;
    std::vector<Node> back_stack_;

    void push_front_unchecked(const value_type& value){
        const value_type aggregate = front_stack_.empty()
            ? value
            : Monoid.op(value, front_stack_.back().aggregate);
        front_stack_.push_back({value, aggregate});
    }

    void push_back_unchecked(const value_type& value){
        const value_type aggregate = back_stack_.empty()
            ? value
            : Monoid.op(back_stack_.back().aggregate, value);
        back_stack_.push_back({value, aggregate});
    }

    void rebuild(std::size_t front_count){
        std::vector<value_type> sequence;
        sequence.reserve(size());
        for(auto iterator = front_stack_.rbegin();
            iterator != front_stack_.rend(); ++iterator){
            sequence.push_back(std::move(iterator->value));
        }
        for(Node& node: back_stack_) sequence.push_back(std::move(node.value));
        front_stack_.clear();
        back_stack_.clear();
        for(std::size_t index = front_count; index > 0; --index){
            push_front_unchecked(sequence[index - 1]);
        }
        for(std::size_t index = front_count; index < sequence.size(); ++index){
            push_back_unchecked(sequence[index]);
        }
    }

    void ensure_front(){
        if(front_stack_.empty() && !back_stack_.empty()){
            rebuild((size() + 1) / 2);
        }
    }

    void ensure_back(){
        if(back_stack_.empty() && !front_stack_.empty()){
            rebuild(size() / 2);
        }
    }

public:
    AggregateDeque() = default;

    explicit AggregateDeque(std::size_t capacity){
        front_stack_.reserve(capacity);
        back_stack_.reserve(capacity);
    }

    [[nodiscard]] std::size_t size() const{
        return front_stack_.size() + back_stack_.size();
    }

    [[nodiscard]] bool empty() const{ return size() == 0; }

    void clear(){
        front_stack_.clear();
        back_stack_.clear();
    }

    void push_front(const value_type& value){ push_front_unchecked(value); }
    void push_back(const value_type& value){ push_back_unchecked(value); }

    const value_type& front(){
        ensure_front();
        if(front_stack_.empty())[[unlikely]]{
            throw std::runtime_error("AggregateDeque::front on an empty deque");
        }
        return front_stack_.back().value;
    }

    const value_type& back(){
        ensure_back();
        if(back_stack_.empty())[[unlikely]]{
            throw std::runtime_error("AggregateDeque::back on an empty deque");
        }
        return back_stack_.back().value;
    }

    void pop_front(){
        ensure_front();
        if(front_stack_.empty())[[unlikely]]{
            throw std::runtime_error(
                "AggregateDeque::pop_front on an empty deque"
            );
        }
        front_stack_.pop_back();
    }

    void pop_back(){
        ensure_back();
        if(back_stack_.empty())[[unlikely]]{
            throw std::runtime_error(
                "AggregateDeque::pop_back on an empty deque"
            );
        }
        back_stack_.pop_back();
    }

    [[nodiscard]] value_type fold() const{
        if(front_stack_.empty()){
            return back_stack_.empty()
                ? Monoid.e() : back_stack_.back().aggregate;
        }
        if(back_stack_.empty()) return front_stack_.back().aggregate;
        return Monoid.op(
            front_stack_.back().aggregate,
            back_stack_.back().aggregate
        );
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_RANGE_QUERY_AGGREGATION_AGGREGATE_DEQUE_HPP_INCLUDED
