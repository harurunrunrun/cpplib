#pragma once

#include <cstddef>
#include <utility>
#include <vector>

#include "dynamic_rectangle_add_point_get.hpp"

template<class T, class Coordinate = long long>
class OfflineRectangleAddPointGet{
    struct Rectangle{
        Coordinate x1;
        Coordinate y1;
        Coordinate x2;
        Coordinate y2;
        T value;
    };
    struct Operation{
        bool is_query;
        Rectangle rectangle;
        Coordinate x;
        Coordinate y;
    };

    std::vector<Rectangle> initial_rectangles_;
    std::vector<Operation> operations_;
    std::size_t query_count_ = 0;

public:
    void add_initial_rectangle(
        Coordinate x1,
        Coordinate y1,
        Coordinate x2,
        Coordinate y2,
        const T& value
    ){
        initial_rectangles_.push_back({x1, y1, x2, y2, value});
    }

    void add_rectangle(
        Coordinate x1,
        Coordinate y1,
        Coordinate x2,
        Coordinate y2,
        const T& value
    ){
        operations_.push_back({
            false,
            {x1, y1, x2, y2, value},
            Coordinate{},
            Coordinate{}
        });
    }

    std::size_t get_point(Coordinate x, Coordinate y){
        const std::size_t index = query_count_++;
        operations_.push_back({true, {}, x, y});
        return index;
    }

    std::size_t query_count() const noexcept{ return query_count_; }

    std::vector<T> solve() const{
        DynamicRectangleAddPointGet<T, Coordinate> structure;
        for(const Rectangle& rectangle: initial_rectangles_){
            structure.reserve_rectangle(
                rectangle.x1,
                rectangle.y1,
                rectangle.x2,
                rectangle.y2
            );
        }
        for(const Operation& operation: operations_){
            if(!operation.is_query){
                const Rectangle& rectangle = operation.rectangle;
                structure.reserve_rectangle(
                    rectangle.x1,
                    rectangle.y1,
                    rectangle.x2,
                    rectangle.y2
                );
            }
        }
        structure.build();

        for(const Rectangle& rectangle: initial_rectangles_){
            structure.add(
                rectangle.x1,
                rectangle.y1,
                rectangle.x2,
                rectangle.y2,
                rectangle.value
            );
        }
        std::vector<T> answer;
        answer.reserve(query_count_);
        for(const Operation& operation: operations_){
            if(operation.is_query){
                answer.push_back(structure.get(operation.x, operation.y));
            }else{
                const Rectangle& rectangle = operation.rectangle;
                structure.add(
                    rectangle.x1,
                    rectangle.y1,
                    rectangle.x2,
                    rectangle.y2,
                    rectangle.value
                );
            }
        }
        return answer;
    }
};
