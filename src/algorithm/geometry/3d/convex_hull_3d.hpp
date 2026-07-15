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

#include "../2d/convex_hull.hpp"
#include "adaptive_orient3d.hpp"
#include "convex_polyhedron3.hpp"
#include "cross.hpp"
#include "dot.hpp"
#include "geometry3d_sign.hpp"
#include "is_finite.hpp"
#include "norm.hpp"
#include "plane_coordinate_system.hpp"
#include "to_plane_coordinates.hpp"

namespace convex_hull_3d_detail{

struct Face{
    std::array<std::size_t, 3> vertices{};
    bool alive = true;
};

inline std::vector<Point3> unique_points(std::vector<Point3> points){
    for(const Point3& point: points){
        if(!geometry3d_is_finite(point))[[unlikely]]{
            throw std::invalid_argument("convex_hull_3d requires finite points");
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

inline bool exactly_non_collinear(
    const Point3& first,
    const Point3& second,
    const Point3& third
){
    using namespace geometry3d_adaptive_detail;
    const std::array<ExactDyadic, 3> first_direction{
        subtract(exact_dyadic(second.x), exact_dyadic(first.x)),
        subtract(exact_dyadic(second.y), exact_dyadic(first.y)),
        subtract(exact_dyadic(second.z), exact_dyadic(first.z)),
    };
    const std::array<ExactDyadic, 3> second_direction{
        subtract(exact_dyadic(third.x), exact_dyadic(first.x)),
        subtract(exact_dyadic(third.y), exact_dyadic(first.y)),
        subtract(exact_dyadic(third.z), exact_dyadic(first.z)),
    };
    for(std::size_t coordinate = 0; coordinate < 3; ++coordinate){
        const std::size_t next = (coordinate + 1) % 3;
        const std::size_t last = (coordinate + 2) % 3;
        if(sign(subtract(
            multiply(first_direction[next], second_direction[last]),
            multiply(first_direction[last], second_direction[next])
        )) != 0) return true;
    }
    return false;
}

inline std::vector<Point3> locally_normalized_points(
    const std::vector<Point3>& points,
    const Point3& anchor
){
    std::vector<Geometry3DNormalizedDifference> differences;
    differences.reserve(points.size());
    long double local_scale = 0.0L;
    for(const Point3& point: points){
        differences.push_back(geometry3d_normalized_difference(point, anchor));
        const Point3& value = differences.back().value;
        if(value.x != 0.0L || value.y != 0.0L || value.z != 0.0L){
            local_scale = std::max(local_scale, differences.back().scale);
        }
    }
    if(local_scale == 0.0L) local_scale = 1.0L;
    std::vector<Point3> result;
    result.reserve(points.size());
    for(const Geometry3DNormalizedDifference& difference: differences){
        result.push_back(
            difference.value * (difference.scale / local_scale)
        );
    }
    return result;
}

inline Face outward_face(
    std::size_t first,
    std::size_t second,
    std::size_t third,
    const std::array<std::size_t, 4>& interior_witness,
    const std::vector<Point3>& points
){
    for(const std::size_t witness: interior_witness){
        const int side = adaptive_orient3d(
            points[first], points[second], points[third], points[witness]
        );
        if(side != 0){
            if(side > 0) std::swap(second, third);
            return {{{first, second, third}}, true};
        }
    }
    throw std::logic_error("convex hull produced a degenerate face");
}

inline ConvexPolyhedron3 collinear_hull(const std::vector<Point3>& points){
    if(points.size() <= 1) return {
        points.empty() ? -1 : 0, points, {}
    };
    // Lexicographic order is the line-parameter order, possibly reversed.
    return {1, {points.front(), points.back()}, {}};
}

inline ConvexPolyhedron3 coplanar_hull(
    const std::vector<Point3>& points,
    const std::vector<Point3>& scaled,
    std::size_t first,
    std::size_t second,
    std::size_t third
){
    const Point3 normal = cross(
        scaled[second] - scaled[first], scaled[third] - scaled[first]
    );
    const PlaneCoordinateSystem3 system = plane_coordinate_system({
        scaled[first], normal
    });
    std::vector<Point> projected;
    projected.reserve(points.size());
    for(const Point3& point: scaled){
        projected.push_back(to_plane_coordinates(system, point));
    }
    const std::vector<Point> boundary = convex_hull(projected);
    std::vector<Point3> vertices;
    vertices.reserve(boundary.size());
    for(const Point& point: boundary){
        const auto iterator = std::find_if(
            projected.begin(), projected.end(),
            [&](const Point& candidate){
                return candidate.x == point.x && candidate.y == point.y;
            }
        );
        if(iterator == projected.end())[[unlikely]]{
            throw std::logic_error("coplanar hull projection mismatch");
        }
        vertices.push_back(points[static_cast<std::size_t>(
            iterator - projected.begin()
        )]);
    }
    std::vector<std::array<std::size_t, 3>> faces;
    for(std::size_t index = 1; index + 1 < vertices.size(); ++index){
        faces.push_back({0, index, index + 1});
    }
    return {2, std::move(vertices), std::move(faces)};
}

}  // namespace convex_hull_3d_detail

inline ConvexPolyhedron3 convex_hull_3d(std::vector<Point3> input){
    using namespace convex_hull_3d_detail;
    std::vector<Point3> points = unique_points(std::move(input));
    if(points.size() <= 1){
        return {points.empty() ? -1 : 0, std::move(points), {}};
    }

    const std::size_t first = 0;
    const std::size_t second = 1;
    std::size_t third = points.size();
    for(std::size_t index = 0; index < points.size(); ++index){
        if(index != first && index != second && exactly_non_collinear(
            points[first], points[second], points[index]
        )){
            third = index;
            break;
        }
    }
    if(third == points.size()) return collinear_hull(points);

    std::size_t fourth = points.size();
    for(std::size_t index = 0; index < points.size(); ++index){
        if(index != first && index != second && index != third
            && adaptive_orient3d(
                points[first], points[second], points[third], points[index]
            ) != 0){
            fourth = index;
            break;
        }
    }
    if(fourth == points.size()){
        return coplanar_hull(
            points, locally_normalized_points(points, points[first]),
            first, second, third
        );
    }

    const std::array<std::size_t, 4> interior_witness{
        first, second, third, fourth,
    };
    std::vector<Face> faces{
        outward_face(first, second, third, interior_witness, points),
        outward_face(first, fourth, second, interior_witness, points),
        outward_face(first, third, fourth, interior_witness, points),
        outward_face(second, fourth, third, interior_witness, points),
    };
    const std::set<std::size_t> initial{first, second, third, fourth};

    for(std::size_t point_index = 0; point_index < points.size(); ++point_index){
        if(initial.contains(point_index)) continue;
        std::vector<std::size_t> visible;
        for(std::size_t face_index = 0; face_index < faces.size(); ++face_index){
            const Face& face = faces[face_index];
            if(face.alive && adaptive_orient3d(
                points[face.vertices[0]], points[face.vertices[1]],
                points[face.vertices[2]], points[point_index]
            ) > 0){
                visible.push_back(face_index);
            }
        }
        if(visible.empty()) continue;

        struct HorizonEdge{
            std::size_t first = 0;
            std::size_t second = 0;
            int count = 0;
        };
        std::map<std::pair<std::size_t, std::size_t>, HorizonEdge> horizon;
        for(std::size_t face_index: visible){
            Face& face = faces[face_index];
            face.alive = false;
            for(std::size_t edge = 0; edge < 3; ++edge){
                const std::size_t edge_first = face.vertices[edge];
                const std::size_t edge_second = face.vertices[(edge + 1) % 3];
                const auto key = std::minmax(edge_first, edge_second);
                HorizonEdge& stored = horizon[key];
                if(stored.count == 0){
                    stored.first = edge_first;
                    stored.second = edge_second;
                }
                ++stored.count;
            }
        }
        for(const auto& [key, edge]: horizon){
            static_cast<void>(key);
            if(edge.count != 1) continue;
            faces.push_back(outward_face(
                edge.first, edge.second, point_index,
                interior_witness, points
            ));
        }
    }

    std::set<std::size_t> used;
    for(const Face& face: faces){
        if(!face.alive) continue;
        used.insert(face.vertices.begin(), face.vertices.end());
    }
    std::vector<std::size_t> remap(points.size(), points.size());
    std::vector<Point3> vertices;
    vertices.reserve(used.size());
    for(std::size_t index: used){
        remap[index] = vertices.size();
        vertices.push_back(points[index]);
    }
    std::vector<std::array<std::size_t, 3>> result_faces;
    for(const Face& face: faces){
        if(!face.alive) continue;
        result_faces.push_back({
            remap[face.vertices[0]], remap[face.vertices[1]],
            remap[face.vertices[2]],
        });
    }
    return {3, std::move(vertices), std::move(result_faces)};
}
