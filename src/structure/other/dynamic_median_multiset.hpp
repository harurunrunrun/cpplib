#pragma once

#include <cstddef>
#include <functional>
#include <iterator>
#include <set>
#include <stdexcept>
#include <utility>

template<class T, class Compare = std::less<T>>
class DynamicMedianMultiset{
public:
    using value_type = T;
    using size_type = std::size_t;
    using compare_type = Compare;

private:
    std::multiset<value_type, compare_type> lower_;
    std::multiset<value_type, compare_type> upper_;

    void rebalance(){
        const size_type required_lower = (size() + 1) / 2;
        while(lower_.size() > required_lower){
            auto node = lower_.extract(std::prev(lower_.end()));
            upper_.insert(std::move(node));
        }
        while(lower_.size() < required_lower){
            auto node = upper_.extract(upper_.begin());
            lower_.insert(std::move(node));
        }
    }

    [[noreturn]] static void throw_empty(const char* operation){
        throw std::out_of_range(operation);
    }

public:
    explicit DynamicMedianMultiset(
        const compare_type& compare = compare_type()
    )
        : lower_(compare), upper_(compare){}

    size_type size() const noexcept{
        return lower_.size() + upper_.size();
    }

    bool empty() const noexcept{
        return lower_.empty();
    }

    void clear() noexcept{
        lower_.clear();
        upper_.clear();
    }

    void insert(value_type value){
        if(
            lower_.empty()
            || !lower_.key_comp()(*std::prev(lower_.end()), value)
        ){
            lower_.insert(std::move(value));
        }else{
            upper_.insert(std::move(value));
        }
        rebalance();
    }

    bool contains(const value_type& value) const{
        return lower_.find(value) != lower_.end()
            || upper_.find(value) != upper_.end();
    }

    size_type count(const value_type& value) const{
        return lower_.count(value) + upper_.count(value);
    }

    bool erase_one(const value_type& value){
        auto iterator = lower_.find(value);
        if(iterator != lower_.end()){
            lower_.erase(iterator);
            rebalance();
            return true;
        }
        iterator = upper_.find(value);
        if(iterator == upper_.end()) return false;
        upper_.erase(iterator);
        rebalance();
        return true;
    }

    size_type erase_all(const value_type& value){
        const size_type erased =
            lower_.erase(value) + upper_.erase(value);
        rebalance();
        return erased;
    }

    const value_type& lower_median() const{
        if(empty())[[unlikely]]{
            throw_empty("DynamicMedianMultiset::lower_median: empty multiset");
        }
        return *std::prev(lower_.end());
    }

    const value_type& upper_median() const{
        if(empty())[[unlikely]]{
            throw_empty("DynamicMedianMultiset::upper_median: empty multiset");
        }
        if((size() & 1U) != 0U) return *std::prev(lower_.end());
        return *upper_.begin();
    }

    const value_type& odd_median() const{
        if(empty())[[unlikely]]{
            throw_empty("DynamicMedianMultiset::odd_median: empty multiset");
        }
        if((size() & 1U) == 0U)[[unlikely]]{
            throw std::logic_error(
                "DynamicMedianMultiset::odd_median: size must be odd"
            );
        }
        return *std::prev(lower_.end());
    }

    std::pair<value_type, value_type> even_medians() const{
        if(empty())[[unlikely]]{
            throw_empty("DynamicMedianMultiset::even_medians: empty multiset");
        }
        if((size() & 1U) != 0U)[[unlikely]]{
            throw std::logic_error(
                "DynamicMedianMultiset::even_medians: size must be even"
            );
        }
        return {*std::prev(lower_.end()), *upper_.begin()};
    }
};
