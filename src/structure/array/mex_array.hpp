#ifndef CPPLIB_SRC_STRUCTURE_ARRAY_MEX_ARRAY_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_ARRAY_MEX_ARRAY_HPP_INCLUDED

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../ordered_set/integer/mex_multiset.hpp"

class MexArray{
    std::vector<std::int64_t> values_;
    MexMultiset tracked_;

    bool is_tracked(std::int64_t value) const noexcept{
        return value >= 0 && static_cast<std::uint64_t>(value) <= values_.size();
    }

public:
    explicit MexArray(std::vector<std::int64_t> values):
        values_(std::move(values)), tracked_(values_.size() + 1){
        for(const std::int64_t value: values_){
            if(is_tracked(value)) tracked_.insert(static_cast<std::size_t>(value));
        }
    }

    std::size_t size() const noexcept{ return values_.size(); }
    const std::int64_t& operator[](std::size_t index) const{ return values_.at(index); }
    std::size_t mex() const noexcept{ return tracked_.mex(); }

    void set(std::size_t index, std::int64_t value){
        if(index >= values_.size()) throw std::out_of_range("MexArray::set: index is out of range");
        const std::int64_t old = values_[index];
        if(is_tracked(old)) tracked_.erase(static_cast<std::size_t>(old));
        values_[index] = value;
        if(is_tracked(value)) tracked_.insert(static_cast<std::size_t>(value));
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_ARRAY_MEX_ARRAY_HPP_INCLUDED
