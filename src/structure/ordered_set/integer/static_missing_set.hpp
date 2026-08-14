#ifndef CPPLIB_SRC_STRUCTURE_ORDERED_SET_INTEGER_STATIC_MISSING_SET_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_ORDERED_SET_INTEGER_STATIC_MISSING_SET_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

class StaticMissingSet{
    std::vector<std::uint64_t> values_;
    std::uint64_t first_ = 0;

public:
    explicit StaticMissingSet(
        std::vector<std::uint64_t> values,
        std::uint64_t first = 0
    ): first_(first){
        std::sort(values.begin(), values.end());
        values.erase(std::unique(values.begin(), values.end()), values.end());
        const auto begin = std::lower_bound(values.begin(), values.end(), first_);
        values_.assign(begin, values.end());
    }

    std::size_t stored_size() const noexcept{ return values_.size(); }
    std::uint64_t first() const noexcept{ return first_; }

    std::uint64_t kth_missing(std::uint64_t index) const{
        std::size_t low = 0;
        std::size_t high = values_.size();
        while(low < high){
            const std::size_t middle = low + (high - low) / 2;
            const unsigned __int128 missing_through =
                static_cast<unsigned __int128>(values_[middle]) - first_ - middle;
            if(missing_through > index) high = middle;
            else low = middle + 1;
        }
        const unsigned __int128 answer =
            static_cast<unsigned __int128>(first_) + index + low;
        if(answer > std::numeric_limits<std::uint64_t>::max()){
            throw std::overflow_error("StaticMissingSet::kth_missing: result overflows uint64_t");
        }
        return static_cast<std::uint64_t>(answer);
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_ORDERED_SET_INTEGER_STATIC_MISSING_SET_HPP_INCLUDED
