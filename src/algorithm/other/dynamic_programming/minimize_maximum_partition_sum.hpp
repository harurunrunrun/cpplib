#ifndef CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_MINIMIZE_MAXIMUM_PARTITION_SUM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_MINIMIZE_MAXIMUM_PARTITION_SUM_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

namespace minimize_maximum_partition_sum_internal{

using WideInteger = __int128_t;

struct SegmentCountRange{
    std::size_t minimum = std::numeric_limits<std::size_t>::max();
    std::size_t maximum = 0;

    bool reachable() const{
        return minimum != std::numeric_limits<std::size_t>::max();
    }
};

class SuffixRangeTree{
    std::size_t coordinate_count_;
    std::vector<std::size_t> minimum_data_;
    std::vector<std::size_t> maximum_data_;

public:
    explicit SuffixRangeTree(std::size_t coordinate_count)
        : coordinate_count_(coordinate_count),
          minimum_data_(coordinate_count + 1,
              std::numeric_limits<std::size_t>::max()),
          maximum_data_(coordinate_count + 1, 0){}

    void clear(){
        std::fill(
            minimum_data_.begin(),
            minimum_data_.end(),
            std::numeric_limits<std::size_t>::max()
        );
        std::fill(maximum_data_.begin(), maximum_data_.end(), 0);
    }

    void add(std::size_t coordinate, SegmentCountRange value){
        std::size_t index = coordinate_count_ - coordinate;
        while(index <= coordinate_count_){
            minimum_data_[index] = std::min(minimum_data_[index], value.minimum);
            maximum_data_[index] = std::max(maximum_data_[index], value.maximum);
            const std::size_t next = index + (index & (~index + 1));
            if(next <= index) break;
            index = next;
        }
    }

    SegmentCountRange suffix(std::size_t first_coordinate) const{
        SegmentCountRange result;
        std::size_t index = coordinate_count_ - first_coordinate;
        while(index > 0){
            result.minimum = std::min(result.minimum, minimum_data_[index]);
            result.maximum = std::max(result.maximum, maximum_data_[index]);
            index -= index & (~index + 1);
        }
        return result;
    }
};

inline long long narrow_answer(WideInteger answer){
    if(answer < static_cast<WideInteger>(std::numeric_limits<long long>::lowest())
        || answer > static_cast<WideInteger>(std::numeric_limits<long long>::max())){
        throw std::overflow_error(
            "library assertion fault: result is not representable by long long "
            "(minimize_maximum_partition_sum)."
        );
    }
    return static_cast<long long>(answer);
}

inline bool feasible(
    const std::vector<WideInteger>& prefix,
    const std::vector<WideInteger>& coordinates,
    std::size_t partition_count,
    WideInteger limit,
    SuffixRangeTree& range_tree
){
    range_tree.clear();
    const auto coordinate_of = [&](WideInteger value){
        return static_cast<std::size_t>(
            std::lower_bound(coordinates.begin(), coordinates.end(), value)
                - coordinates.begin()
        );
    };

    range_tree.add(coordinate_of(prefix.front()), {0, 0});
    SegmentCountRange last;
    for(std::size_t position = 1; position < prefix.size(); ++position){
        const WideInteger minimum_predecessor = prefix[position] - limit;
        const std::size_t first_coordinate = static_cast<std::size_t>(
            std::lower_bound(
                coordinates.begin(),
                coordinates.end(),
                minimum_predecessor
            ) - coordinates.begin()
        );
        const SegmentCountRange predecessor = range_tree.suffix(first_coordinate);
        if(!predecessor.reachable()){
            if(position + 1 == prefix.size()) last = {};
            continue;
        }
        const SegmentCountRange current{
            predecessor.minimum + 1,
            predecessor.maximum + 1
        };
        range_tree.add(coordinate_of(prefix[position]), current);
        if(position + 1 == prefix.size()) last = current;
    }
    return last.reachable()
        && last.minimum <= partition_count
        && partition_count <= last.maximum;
}

} // namespace minimize_maximum_partition_sum_internal

inline long long minimize_maximum_partition_sum(
    const std::vector<long long>& values,
    std::size_t partition_count
){
    using minimize_maximum_partition_sum_internal::WideInteger;
    if(values.empty()){
        throw std::invalid_argument(
            "library assertion fault: values must not be empty "
            "(minimize_maximum_partition_sum)."
        );
    }
    if(partition_count == 0 || partition_count > values.size()){
        throw std::invalid_argument(
            "library assertion fault: range violation "
            "(minimize_maximum_partition_sum)."
        );
    }

    if(partition_count == values.size()){
        return *std::max_element(values.begin(), values.end());
    }

    std::vector<WideInteger> prefix(values.size() + 1, 0);
    WideInteger lower = 0;
    WideInteger upper = 0;
    for(std::size_t index = 0; index < values.size(); ++index){
        const WideInteger value = static_cast<WideInteger>(values[index]);
        prefix[index + 1] = prefix[index] + value;
        if(value < 0){
            lower += value;
        }else{
            upper += value;
        }
    }
    if(partition_count == 1){
        return minimize_maximum_partition_sum_internal::narrow_answer(prefix.back());
    }

    std::vector<WideInteger> coordinates = prefix;
    std::sort(coordinates.begin(), coordinates.end());
    coordinates.erase(
        std::unique(coordinates.begin(), coordinates.end()),
        coordinates.end()
    );
    minimize_maximum_partition_sum_internal::SuffixRangeTree range_tree(
        coordinates.size()
    );

    while(lower < upper){
        const WideInteger middle = lower + (upper - lower) / 2;
        if(minimize_maximum_partition_sum_internal::feasible(
            prefix,
            coordinates,
            partition_count,
            middle,
            range_tree
        )){
            upper = middle;
        }else{
            lower = middle + 1;
        }
    }
    return minimize_maximum_partition_sum_internal::narrow_answer(lower);
}

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_MINIMIZE_MAXIMUM_PARTITION_SUM_HPP_INCLUDED
