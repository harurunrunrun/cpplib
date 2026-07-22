#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_TETRAHEDRON_TETRAHEDRON_INTERSECTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_TETRAHEDRON_TETRAHEDRON_INTERSECTION_HPP_INCLUDED

#include <array>
#include <variant>
#include <vector>

#include "../core/geometry_primitives.hpp"
#include "../predicate/contains.hpp"
#include "../shape/convex_hull_3d.hpp"
#include "../core/convex_polyhedron3.hpp"
#include "segment_triangle_intersection.hpp"
#include "../predicate/tetrahedron_orientation.hpp"

inline ConvexPolyhedron3 tetrahedron_tetrahedron_intersection(
    const Tetrahedron3& first,
    const Tetrahedron3& second
){
    if(tetrahedron_orientation(first) == 0 ||
        tetrahedron_orientation(second) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate tetrahedron");
    }
    const auto vertices = [](const Tetrahedron3& tetrahedron){
        return std::array<Point3, 4>{
            tetrahedron.a, tetrahedron.b, tetrahedron.c, tetrahedron.d
        };
    };
    const auto edges = [&](const Tetrahedron3& tetrahedron){
        const auto value = vertices(tetrahedron);
        return std::array<Segment3, 6>{{
            {value[0], value[1]}, {value[0], value[2]}, {value[0], value[3]},
            {value[1], value[2]}, {value[1], value[3]}, {value[2], value[3]},
        }};
    };
    const auto faces = [](const Tetrahedron3& tetrahedron){
        return std::array<Triangle3, 4>{{
            {tetrahedron.b, tetrahedron.c, tetrahedron.d},
            {tetrahedron.a, tetrahedron.c, tetrahedron.d},
            {tetrahedron.a, tetrahedron.b, tetrahedron.d},
            {tetrahedron.a, tetrahedron.b, tetrahedron.c},
        }};
    };
    std::vector<Point3> points;
    const auto add = [&](const Point3& point){
        for(const Point3& existing: points) if(existing == point) return;
        points.push_back(point);
    };
    for(const Point3& point: vertices(first)) if(contains(second, point)) add(point);
    for(const Point3& point: vertices(second)) if(contains(first, point)) add(point);
    const auto add_edge_face_intersections = [&](const Tetrahedron3& edge_source,
                                                  const Tetrahedron3& face_source){
        for(const Segment3& edge: edges(edge_source)){
            for(const Triangle3& face: faces(face_source)){
                const LinearIntersection3 intersection =
                    segment_triangle_intersection(edge, face);
                if(const auto* point = std::get_if<Point3>(&intersection)) add(*point);
                if(const auto* segment = std::get_if<Segment3>(&intersection)){
                    add(segment->a);
                    add(segment->b);
                }
            }
        }
    };
    add_edge_face_intersections(first, second);
    add_edge_face_intersections(second, first);
    return convex_hull_3d(std::move(points));
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_TETRAHEDRON_TETRAHEDRON_INTERSECTION_HPP_INCLUDED
