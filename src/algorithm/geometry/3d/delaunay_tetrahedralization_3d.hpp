#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <map>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

#include "adaptive_insphere.hpp"
#include "adaptive_orient3d.hpp"
#include "cross.hpp"
#include "delaunay_tetrahedralization3.hpp"
#include "is_finite.hpp"

namespace delaunay_tetrahedralization_3d_detail{

struct TetrahedronIndex{
    std::array<std::size_t, 4> vertices{};
    bool alive = true;
};

inline std::vector<Point3> exact_unique_points(std::vector<Point3> points){
    for(const Point3& point: points){
        if(!geometry3d_is_finite(point))[[unlikely]]{
            throw std::invalid_argument(
                "delaunay_tetrahedralization_3d requires finite points"
            );
        }
    }
    std::sort(points.begin(), points.end());
    points.erase(std::unique(
        points.begin(), points.end(),
        [](const Point3& left, const Point3& right){
            return left.x == right.x && left.y == right.y && left.z == right.z;
        }
    ), points.end());
    return points;
}

inline std::vector<Point3> normalized_points(const std::vector<Point3>& points){
    long double scale = 0.0L;
    for(const Point3& point: points){
        scale = std::max({
            scale, std::abs(point.x), std::abs(point.y), std::abs(point.z)
        });
    }
    if(scale == 0.0L) scale = 1.0L;
    Point3 minimum{1.0L, 1.0L, 1.0L};
    Point3 maximum{-1.0L, -1.0L, -1.0L};
    std::vector<Point3> scaled;
    scaled.reserve(points.size());
    for(const Point3& point: points){
        const Point3 value = point / scale;
        scaled.push_back(value);
        minimum.x = std::min(minimum.x, value.x);
        minimum.y = std::min(minimum.y, value.y);
        minimum.z = std::min(minimum.z, value.z);
        maximum.x = std::max(maximum.x, value.x);
        maximum.y = std::max(maximum.y, value.y);
        maximum.z = std::max(maximum.z, value.z);
    }
    const Point3 center{
        minimum.x / 2.0L + maximum.x / 2.0L,
        minimum.y / 2.0L + maximum.y / 2.0L,
        minimum.z / 2.0L + maximum.z / 2.0L,
    };
    long double range = std::max({
        maximum.x - minimum.x, maximum.y - minimum.y,
        maximum.z - minimum.z,
    });
    if(range == 0.0L) range = 1.0L;
    for(Point3& point: scaled) point = (point - center) / range;
    return scaled;
}

inline int affine_dimension(const std::vector<Point3>& points){
    if(points.empty()) return -1;
    if(points.size() == 1) return 0;
    const std::vector<Point3> normalized = normalized_points(points);
    std::size_t third = normalized.size();
    for(std::size_t index = 2; index < normalized.size(); ++index){
        const Point3 product = cross(
            normalized[1] - normalized[0], normalized[index] - normalized[0]
        );
        if(std::hypot(product.x, product.y, product.z) > GEOMETRY3D_EPS){
            third = index;
            break;
        }
    }
    if(third == normalized.size()) return 1;
    for(std::size_t index = 0; index < points.size(); ++index){
        if(adaptive_orient3d(
            points[0], points[1], points[third], points[index]
        ) != 0) return 3;
    }
    return 2;
}

inline std::array<std::size_t, 4> positive_tetrahedron(
    std::array<std::size_t, 4> tetrahedron,
    const std::vector<Point3>& points
){
    const int orientation = adaptive_orient3d(
        points[tetrahedron[0]], points[tetrahedron[1]],
        points[tetrahedron[2]], points[tetrahedron[3]]
    );
    if(orientation < 0) std::swap(tetrahedron[0], tetrahedron[1]);
    return tetrahedron;
}

inline std::array<std::size_t, 3> sorted_face(
    std::size_t first,
    std::size_t second,
    std::size_t third
){
    std::array<std::size_t, 3> face{first, second, third};
    std::sort(face.begin(), face.end());
    return face;
}

inline std::vector<std::array<std::size_t, 4>> bowyer_watson(
    const std::vector<Point3>& normalized,
    long double super_extent
){
    std::vector<Point3> predicates = normalized;
    const std::size_t original_size = predicates.size();
    predicates.push_back({-super_extent, -super_extent, -super_extent});
    predicates.push_back({ super_extent, -super_extent,  super_extent});
    predicates.push_back({-super_extent,  super_extent,  super_extent});
    predicates.push_back({ super_extent,  super_extent, -super_extent});
    std::vector<TetrahedronIndex> tetrahedra{{positive_tetrahedron({
        original_size, original_size + 1, original_size + 2, original_size + 3
    }, predicates), true}};

    for(std::size_t point_index = 0; point_index < original_size; ++point_index){
        std::vector<std::size_t> bad;
        for(std::size_t index = 0; index < tetrahedra.size(); ++index){
            const TetrahedronIndex& tetrahedron = tetrahedra[index];
            if(!tetrahedron.alive) continue;
            const auto& v = tetrahedron.vertices;
            if(adaptive_insphere(
                predicates[v[0]], predicates[v[1]], predicates[v[2]],
                predicates[v[3]], predicates[point_index]
            ) > 0) bad.push_back(index);
        }
        if(bad.empty())[[unlikely]]{
            throw std::logic_error("Bowyer-Watson cavity is empty");
        }

        std::map<std::array<std::size_t, 3>, int> face_count;
        for(std::size_t index: bad){
            TetrahedronIndex& tetrahedron = tetrahedra[index];
            tetrahedron.alive = false;
            const auto& v = tetrahedron.vertices;
            ++face_count[sorted_face(v[1], v[2], v[3])];
            ++face_count[sorted_face(v[0], v[3], v[2])];
            ++face_count[sorted_face(v[0], v[1], v[3])];
            ++face_count[sorted_face(v[0], v[2], v[1])];
        }
        for(const auto& [face, count]: face_count){
            if(count != 1) continue;
            std::array<std::size_t, 4> candidate{
                face[0], face[1], face[2], point_index
            };
            if(adaptive_orient3d(
                predicates[candidate[0]], predicates[candidate[1]],
                predicates[candidate[2]], predicates[candidate[3]]
            ) == 0) continue;
            tetrahedra.push_back({positive_tetrahedron(candidate, predicates), true});
        }
    }

    std::set<std::array<std::size_t, 4>> seen;
    std::vector<std::array<std::size_t, 4>> result;
    for(const TetrahedronIndex& tetrahedron: tetrahedra){
        if(!tetrahedron.alive) continue;
        if(std::any_of(
            tetrahedron.vertices.begin(), tetrahedron.vertices.end(),
            [&](std::size_t vertex){ return vertex >= original_size; }
        )) continue;
        std::array<std::size_t, 4> key = tetrahedron.vertices;
        std::sort(key.begin(), key.end());
        if(seen.insert(key).second) result.push_back(tetrahedron.vertices);
    }
    return result;
}

inline bool covers_convex_hull(
    const std::vector<std::array<std::size_t, 4>>& tetrahedra,
    const std::vector<Point3>& points
){
    if(tetrahedra.empty()) return false;
    std::vector<bool> used(points.size(), false);
    std::map<std::array<std::size_t, 3>, int> face_count;
    for(const auto& tetrahedron: tetrahedra){
        for(std::size_t vertex: tetrahedron){
            if(vertex >= points.size()) return false;
            used[vertex] = true;
        }
        if(adaptive_orient3d(
            points[tetrahedron[0]], points[tetrahedron[1]],
            points[tetrahedron[2]], points[tetrahedron[3]]
        ) <= 0) return false;
        ++face_count[sorted_face(tetrahedron[1], tetrahedron[2], tetrahedron[3])];
        ++face_count[sorted_face(tetrahedron[0], tetrahedron[3], tetrahedron[2])];
        ++face_count[sorted_face(tetrahedron[0], tetrahedron[1], tetrahedron[3])];
        ++face_count[sorted_face(tetrahedron[0], tetrahedron[2], tetrahedron[1])];
    }
    if(std::find(used.begin(), used.end(), false) != used.end()) return false;
    for(const auto& [face, count]: face_count){
        if(count > 2) return false;
        if(count == 2) continue;
        bool positive = false;
        bool negative = false;
        for(const Point3& point: points){
            const int side = adaptive_orient3d(
                points[face[0]], points[face[1]], points[face[2]], point
            );
            positive = positive || side > 0;
            negative = negative || side < 0;
            if(positive && negative) return false;
        }
    }
    return true;
}

}  // namespace delaunay_tetrahedralization_3d_detail

inline DelaunayTetrahedralization3 delaunay_tetrahedralization_3d(
    std::vector<Point3> input
){
    using namespace delaunay_tetrahedralization_3d_detail;
    std::vector<Point3> vertices = exact_unique_points(std::move(input));
    const int dimension = affine_dimension(vertices);
    if(dimension < 3) return {dimension, std::move(vertices), {}};

    const std::vector<Point3> normalized = normalized_points(vertices);
    long double super_extent = 16.0L;
    for(int attempt = 0; attempt < 12; ++attempt){
        auto tetrahedra = bowyer_watson(normalized, super_extent);
        if(covers_convex_hull(tetrahedra, vertices)){
            return {3, std::move(vertices), std::move(tetrahedra)};
        }
        super_extent *= 256.0L;
        if(!std::isfinite(super_extent)) break;
    }
    throw std::overflow_error(
        "could not construct a sufficiently large Delaunay super tetrahedron"
    );
}
