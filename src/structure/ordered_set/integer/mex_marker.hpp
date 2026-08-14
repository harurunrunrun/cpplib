#ifndef CPPLIB_SRC_STRUCTURE_ORDERED_SET_INTEGER_MEX_MARKER_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_ORDERED_SET_INTEGER_MEX_MARKER_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

class MexMarker{
    std::vector<std::size_t> stamp_;
    std::size_t token_ = 1;

public:
    explicit MexMarker(std::size_t universe): stamp_(universe, 0){}

    std::size_t universe() const noexcept{ return stamp_.size(); }

    void clear() noexcept{
        if(token_ == std::numeric_limits<std::size_t>::max()){
            std::fill(stamp_.begin(), stamp_.end(), 0);
            token_ = 1;
        }else{
            ++token_;
        }
    }

    void mark(std::size_t value){
        if(value >= stamp_.size()) throw std::out_of_range("MexMarker::mark: value is outside the universe");
        stamp_[value] = token_;
    }

    bool marked(std::size_t value) const{
        if(value >= stamp_.size()) throw std::out_of_range("MexMarker::marked: value is outside the universe");
        return stamp_[value] == token_;
    }

    std::size_t mex() const noexcept{
        std::size_t value = 0;
        while(value < stamp_.size() && stamp_[value] == token_) ++value;
        return value;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_ORDERED_SET_INTEGER_MEX_MARKER_HPP_INCLUDED
