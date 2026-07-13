#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"

inline Point3 centroid(const Triangle3& triangle){
    return (triangle.a + triangle.b + triangle.c) / 3;
}

inline Point3 centroid(const Tetrahedron3& tetrahedron){
    return (
        tetrahedron.a + tetrahedron.b + tetrahedron.c + tetrahedron.d
    ) / 4;
}
