#pragma once

#include <algorithm>

#include "dynamic_point_add_rectangle_sum.hpp"

template<class T, class Coordinate = long long>
class DynamicRectangleAddPointGet{
    DynamicPointAddRectangleSum<T, Coordinate> bit;
    bool built = false;

public:
    DynamicRectangleAddPointGet() = default;

    void reserve_rectangle(Coordinate x1, Coordinate y1, Coordinate x2, Coordinate y2){
        if(x1 > x2) std::swap(x1, x2);
        if(y1 > y2) std::swap(y1, y2);
        if(x1 == x2 || y1 == y2) return;
        bit.reserve_point(x1, y1);
        bit.reserve_point(x2, y1);
        bit.reserve_point(x1, y2);
        bit.reserve_point(x2, y2);
    }

    void build(){
        bit.build();
        built = true;
    }

    void add(Coordinate x1, Coordinate y1, Coordinate x2, Coordinate y2, const T& value){
        if(x1 > x2) std::swap(x1, x2);
        if(y1 > y2) std::swap(y1, y2);
        if(x1 == x2 || y1 == y2) return;
        if(!built)[[unlikely]]{
            throw std::runtime_error("library assertion fault: DynamicRectangleAddPointGet is not built.");
        }
        bit.add(x1, y1, value);
        bit.add(x2, y1, -value);
        bit.add(x1, y2, -value);
        bit.add(x2, y2, value);
    }

    T get(Coordinate x, Coordinate y) const{
        if(!built)[[unlikely]]{
            throw std::runtime_error("library assertion fault: DynamicRectangleAddPointGet is not built.");
        }
        return bit.prefix_sum_leq(x, y);
    }
};
