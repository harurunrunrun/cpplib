#ifndef CPPLIB_SRC_STRUCTURE_OTHER_RANGE_ADD_POINT_GET_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_OTHER_RANGE_ADD_POINT_GET_HPP_INCLUDED

#include <memory>
#include <stdexcept>

#include "fenwick_tree.hpp"

template<class T, int MAX_SIZE>
class RangeAddPointGet{
    std::unique_ptr<FenwickTree<T, MAX_SIZE>> difference_;

public:
    explicit RangeAddPointGet(int size)
        : difference_(std::make_unique<FenwickTree<T, MAX_SIZE>>(size)){}

    int size() const{ return difference_->size(); }

    void add(int left, int right, const T& value){
        if(left < 0 || right < left || size() < right)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: invalid range (RangeAddPointGet)."
            );
        }
        if(left == right) return;
        difference_->add(left, value);
        if(right < size()) difference_->add(right, -value);
    }

    T get(int position) const{
        if(position < 0 || size() <= position)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: invalid index (RangeAddPointGet)."
            );
        }
        return difference_->prefix_sum(position + 1);
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_OTHER_RANGE_ADD_POINT_GET_HPP_INCLUDED
