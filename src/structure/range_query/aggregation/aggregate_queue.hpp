#ifndef CPPLIB_SRC_STRUCTURE_RANGE_QUERY_AGGREGATION_AGGREGATE_QUEUE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_RANGE_QUERY_AGGREGATION_AGGREGATE_QUEUE_HPP_INCLUDED

#include <stdexcept>
#include <utility>
#include <vector>

template<auto Monoid>
class AggregateQueue{
public:
    using value_type = typename decltype(Monoid)::S;

private:
    struct Node{
        value_type value;
        value_type aggregate;
    };

    std::vector<Node> front_stack_;
    std::vector<Node> back_stack_;

    void transfer_to_front(){
        while(!back_stack_.empty()){
            value_type value = std::move(back_stack_.back().value);
            back_stack_.pop_back();
            const value_type aggregate = front_stack_.empty()
                ? value
                : Monoid.op(value, front_stack_.back().aggregate);
            front_stack_.push_back({std::move(value), aggregate});
        }
    }

public:
    AggregateQueue() = default;

    explicit AggregateQueue(std::size_t capacity){
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

    void push(const value_type& value){
        const value_type aggregate = back_stack_.empty()
            ? value
            : Monoid.op(back_stack_.back().aggregate, value);
        back_stack_.push_back({value, aggregate});
    }

    void push(value_type&& value){
        const value_type aggregate = back_stack_.empty()
            ? value
            : Monoid.op(back_stack_.back().aggregate, value);
        back_stack_.push_back({std::move(value), aggregate});
    }

    const value_type& front(){
        if(front_stack_.empty()) transfer_to_front();
        if(front_stack_.empty())[[unlikely]]{
            throw std::runtime_error("AggregateQueue::front on an empty queue");
        }
        return front_stack_.back().value;
    }

    void pop(){
        if(front_stack_.empty()) transfer_to_front();
        if(front_stack_.empty())[[unlikely]]{
            throw std::runtime_error("AggregateQueue::pop on an empty queue");
        }
        front_stack_.pop_back();
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

#endif  // CPPLIB_SRC_STRUCTURE_RANGE_QUERY_AGGREGATION_AGGREGATE_QUEUE_HPP_INCLUDED
