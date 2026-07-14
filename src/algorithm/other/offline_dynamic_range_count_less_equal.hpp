#pragma once

#include <stdexcept>
#include <utility>
#include <vector>

#include "dynamic_point_add_rectangle_sum.hpp"

template<class T>
class OfflineDynamicRangeCountLessEqual{
    struct Operation{ bool update; int first; int second; T value; };
    std::vector<T> initial_;
    std::vector<Operation> operations_;

public:
    explicit OfflineDynamicRangeCountLessEqual(std::vector<T> initial)
        : initial_(std::move(initial)){}

    int size() const{ return static_cast<int>(initial_.size()); }

    void add_set(int position, T value){
        if(position < 0 || size() <= position)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: invalid index "
                "(OfflineDynamicRangeCountLessEqual)."
            );
        }
        operations_.push_back({true, position, 0, std::move(value)});
    }

    void add_count_less_equal(int left, int right, T upper){
        if(left < 0 || right < left || size() < right)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: invalid range "
                "(OfflineDynamicRangeCountLessEqual)."
            );
        }
        operations_.push_back({false, left, right, std::move(upper)});
    }

    std::vector<int> solve() const{
        DynamicPointAddRectangleSum<int, T> points;
        for(int index = 0; index < size(); ++index){
            points.reserve_point(index, initial_[static_cast<std::size_t>(index)]);
        }
        for(const auto& operation: operations_){
            if(operation.update) points.reserve_point(operation.first, operation.value);
        }
        points.build();
        std::vector<T> current = initial_;
        for(int index = 0; index < size(); ++index){
            points.add(index, current[static_cast<std::size_t>(index)], 1);
        }
        std::vector<int> answer;
        for(const auto& operation: operations_){
            if(operation.update){
                T& value = current[static_cast<std::size_t>(operation.first)];
                points.add(operation.first, value, -1);
                value = operation.value;
                points.add(operation.first, value, 1);
            }else{
                answer.push_back(
                    points.prefix_sum_leq(operation.second - 1, operation.value)
                    - points.prefix_sum_leq(operation.first - 1, operation.value)
                );
            }
        }
        return answer;
    }
};
