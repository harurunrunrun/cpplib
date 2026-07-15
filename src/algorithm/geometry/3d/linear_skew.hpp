#pragma once

#include <variant>

#include "base.hpp"
#include "line_line_intersection.hpp"
#include "linear_parallel.hpp"

inline bool skew(const Line3& first, const Line3& second){
    return !parallel(first, second) &&
        std::holds_alternative<std::monostate>(
            line_line_intersection(first, second)
        );
}

inline bool skew(const Line3& first, const Ray3& second){
    return skew(first, Line3{second.origin, second.through});
}

inline bool skew(const Ray3& first, const Line3& second){
    return skew(second, first);
}

inline bool skew(const Line3& first, const Segment3& second){
    (void)segment3_direction(second);
    return skew(first, Line3{second.a, second.b});
}

inline bool skew(const Segment3& first, const Line3& second){
    return skew(second, first);
}

inline bool skew(const Ray3& first, const Ray3& second){
    return skew(
        Line3{first.origin, first.through},
        Line3{second.origin, second.through}
    );
}

inline bool skew(const Ray3& first, const Segment3& second){
    (void)segment3_direction(second);
    return skew(
        Line3{first.origin, first.through},
        Line3{second.a, second.b}
    );
}

inline bool skew(const Segment3& first, const Ray3& second){
    return skew(second, first);
}

inline bool skew(const Segment3& first, const Segment3& second){
    (void)segment3_direction(first);
    (void)segment3_direction(second);
    return skew(Line3{first.a, first.b}, Line3{second.a, second.b});
}
