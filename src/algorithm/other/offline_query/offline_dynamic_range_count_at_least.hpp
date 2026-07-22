#ifndef CPPLIB_SRC_ALGORITHM_OTHER_OFFLINE_QUERY_OFFLINE_DYNAMIC_RANGE_COUNT_AT_LEAST_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_OFFLINE_QUERY_OFFLINE_DYNAMIC_RANGE_COUNT_AT_LEAST_HPP_INCLUDED

#include <stdexcept>
#include <utility>
#include <vector>

#include "dynamic_point_add_rectangle_sum.hpp"

class OfflineDynamicRangeCountAtLeast{
    struct Operation{
        bool update;
        int first;
        int second;
        int value;
    };

    std::vector<int> initial_;
    std::vector<Operation> operations_;

public:
    explicit OfflineDynamicRangeCountAtLeast(std::vector<int> initial)
        : initial_(std::move(initial)){}

    int size() const{ return static_cast<int>(initial_.size()); }

    void add_set(int position, int value){
        if(position < 0 || size() <= position)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: index out of range "
                "(OfflineDynamicRangeCountAtLeast)."
            );
        }
        operations_.push_back({true, position, 0, value});
    }

    void add_count_at_least(int left, int right, int threshold){
        if(left < 0 || right < left || size() < right)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range out of bounds "
                "(OfflineDynamicRangeCountAtLeast)."
            );
        }
        operations_.push_back({false, left, right, threshold});
    }

    std::vector<int> solve() const{
        DynamicPointAddRectangleSum<int> points;
        for(int index = 0; index < size(); ++index){
            points.reserve_point(index, initial_[static_cast<std::size_t>(index)]);
        }
        for(const Operation& operation: operations_){
            if(operation.update){
                points.reserve_point(operation.first, operation.value);
            }
        }
        points.build();
        std::vector<int> current = initial_;
        for(int index = 0; index < size(); ++index){
            points.add(index, current[static_cast<std::size_t>(index)], 1);
        }

        std::vector<int> answer;
        for(const Operation& operation: operations_){
            if(operation.update){
                int& value = current[static_cast<std::size_t>(operation.first)];
                points.add(operation.first, value, -1);
                value = operation.value;
                points.add(operation.first, value, 1);
            }else{
                const int below =
                    points.prefix_sum_less(operation.second, operation.value)
                    - points.prefix_sum_less(operation.first, operation.value);
                answer.push_back(operation.second - operation.first - below);
            }
        }
        return answer;
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_OFFLINE_QUERY_OFFLINE_DYNAMIC_RANGE_COUNT_AT_LEAST_HPP_INCLUDED
