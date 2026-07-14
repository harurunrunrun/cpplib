#pragma once

#include <concepts>

template<std::integral Coordinate>
struct IntegerPoint{
    Coordinate x{};
    Coordinate y{};

    bool operator==(const IntegerPoint&) const = default;

    bool operator<(const IntegerPoint& other) const{
        if(x != other.x) return x < other.x;
        return y < other.y;
    }
};
