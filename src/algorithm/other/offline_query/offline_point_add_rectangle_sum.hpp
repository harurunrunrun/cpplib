#ifndef CPPLIB_SRC_ALGORITHM_OTHER_OFFLINE_QUERY_OFFLINE_POINT_ADD_RECTANGLE_SUM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_OFFLINE_QUERY_OFFLINE_POINT_ADD_RECTANGLE_SUM_HPP_INCLUDED

#include <cstddef>
#include <stdexcept>
#include <vector>

#include "dynamic_point_add_rectangle_sum.hpp"

template<class T, class Coordinate = long long>
class OfflinePointAddRectangleSum{
    struct Point{
        Coordinate x;
        Coordinate y;
        T value;
    };
    struct Operation{
        bool is_query;
        Point point;
        Coordinate x1;
        Coordinate y1;
        Coordinate x2;
        Coordinate y2;
    };

    std::vector<Point> initial_points_;
    std::vector<Operation> operations_;
    std::size_t query_count_ = 0;

public:
    void add_initial_point(Coordinate x, Coordinate y, const T& value){
        initial_points_.push_back({x, y, value});
    }

    void add_point(Coordinate x, Coordinate y, const T& value){
        operations_.push_back({
            false,
            {x, y, value},
            Coordinate{},
            Coordinate{},
            Coordinate{},
            Coordinate{}
        });
    }

    std::size_t rectangle_sum(
        Coordinate x1,
        Coordinate y1,
        Coordinate x2,
        Coordinate y2
    ){
        if(x2 < x1 || y2 < y1)[[unlikely]]{
            throw std::invalid_argument(
                "OfflinePointAddRectangleSum query range is reversed"
            );
        }
        const std::size_t index = query_count_++;
        operations_.push_back({true, {}, x1, y1, x2, y2});
        return index;
    }

    std::size_t query_count() const noexcept{ return query_count_; }

    std::vector<T> solve() const{
        DynamicPointAddRectangleSum<T, Coordinate> structure;
        for(const Point& point: initial_points_){
            structure.reserve_point(point.x, point.y);
        }
        for(const Operation& operation: operations_){
            if(!operation.is_query){
                structure.reserve_point(
                    operation.point.x,
                    operation.point.y
                );
            }
        }
        structure.build();

        for(const Point& point: initial_points_){
            structure.add(point.x, point.y, point.value);
        }
        std::vector<T> answer;
        answer.reserve(query_count_);
        for(const Operation& operation: operations_){
            if(operation.is_query){
                answer.push_back(structure.sum(
                    operation.x1,
                    operation.y1,
                    operation.x2,
                    operation.y2
                ));
            }else{
                structure.add(
                    operation.point.x,
                    operation.point.y,
                    operation.point.value
                );
            }
        }
        return answer;
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_OFFLINE_QUERY_OFFLINE_POINT_ADD_RECTANGLE_SUM_HPP_INCLUDED
