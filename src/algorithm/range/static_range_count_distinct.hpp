#ifndef CPPLIB_SRC_ALGORITHM_RANGE_STATIC_RANGE_COUNT_DISTINCT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_RANGE_STATIC_RANGE_COUNT_DISTINCT_HPP_INCLUDED

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "../../structure/wavelet_matrix/wavelet_matrix.hpp"

template<
    class T,
    int MAX_SIZE,
    class Hash = std::hash<T>,
    class KeyEqual = std::equal_to<T>
>
class StaticRangeCountDistinct{
    static_assert(MAX_SIZE >= 0);

    int n_;
    WaveletMatrix<int, MAX_SIZE> previous_occurrence_;

    static int checked_size(const std::vector<T>& values){
        if(values.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(StaticRangeCountDistinct constructor)."
            );
        }
        return static_cast<int>(values.size());
    }

    static std::vector<int> build_previous(const std::vector<T>& values){
        std::unordered_map<T, int, Hash, KeyEqual> last;
        last.reserve(values.size() * 2 + 1);
        std::vector<int> previous(values.size(), -1);
        for(int index = 0; index < static_cast<int>(values.size()); ++index){
            auto [iterator, inserted] = last.emplace(
                values[static_cast<std::size_t>(index)], index
            );
            if(!inserted){
                previous[static_cast<std::size_t>(index)] = iterator->second;
                iterator->second = index;
            }
        }
        return previous;
    }

    void check_range(int left, int right) const{
        if(left < 0 || right < left || n_ < right)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(StaticRangeCountDistinct::count)."
            );
        }
    }

public:
    explicit StaticRangeCountDistinct(const std::vector<T>& values):
        n_(checked_size(values)),
        previous_occurrence_(build_previous(values)){}

    int size() const noexcept{ return n_; }

    int count(int left, int right) const{
        check_range(left, right);
        return previous_occurrence_.range_freq(left, right, left);
    }

    int count_distinct(int left, int right) const{ return count(left, right); }
};

#endif  // CPPLIB_SRC_ALGORITHM_RANGE_STATIC_RANGE_COUNT_DISTINCT_HPP_INCLUDED
