#pragma once

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "base.hpp"
#include "is_finite.hpp"

struct AABB3{
    Point3 minimum{};
    Point3 maximum{};

    bool valid() const noexcept{
        return geometry3d_is_finite(minimum) && geometry3d_is_finite(maximum)
            && minimum.x <= maximum.x && minimum.y <= maximum.y
            && minimum.z <= maximum.z;
    }

    bool contains(const Point3& point) const{
        require_valid();
        if(!geometry3d_is_finite(point)){
            throw std::invalid_argument("AABB3 query point must be finite");
        }
        return minimum.x <= point.x && point.x <= maximum.x
            && minimum.y <= point.y && point.y <= maximum.y
            && minimum.z <= point.z && point.z <= maximum.z;
    }

    bool intersects(const AABB3& other) const{
        require_valid();
        other.require_valid();
        return minimum.x <= other.maximum.x && other.minimum.x <= maximum.x
            && minimum.y <= other.maximum.y && other.minimum.y <= maximum.y
            && minimum.z <= other.maximum.z && other.minimum.z <= maximum.z;
    }

    long double distance(const Point3& point) const{
        require_valid();
        if(!geometry3d_is_finite(point)){
            throw std::invalid_argument("AABB3 query point must be finite");
        }
        const long double dx = point.x < minimum.x ? minimum.x - point.x
            : (maximum.x < point.x ? point.x - maximum.x : 0);
        const long double dy = point.y < minimum.y ? minimum.y - point.y
            : (maximum.y < point.y ? point.y - maximum.y : 0);
        const long double dz = point.z < minimum.z ? minimum.z - point.z
            : (maximum.z < point.z ? point.z - maximum.z : 0);
        return std::hypot(dx, dy, dz);
    }

    void require_valid() const{
        if(!valid()) throw std::invalid_argument("invalid AABB3");
    }
};

