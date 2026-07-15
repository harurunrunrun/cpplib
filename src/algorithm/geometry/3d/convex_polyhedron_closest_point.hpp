#pragma once

#include <limits>
#include <stdexcept>

#include "closest_point.hpp"
#include "convex_polyhedron_contains.hpp"
#include "convex_polyhedron_edges.hpp"
#include "distance.hpp"

inline Point3 convex_polyhedron_closest_point(
    const ConvexPolyhedron3& polyhedron,
    const Point3& point
){
    if(polyhedron.vertices.empty())[[unlikely]]{
        throw std::invalid_argument("closest point on empty convex polyhedron");
    }
    if(convex_polyhedron_contains(polyhedron, point)) return point;
    Point3 answer = polyhedron.vertices.front();
    long double best = distance(answer, point);
    if(polyhedron.affine_dimension <= 0) return answer;
    if(polyhedron.affine_dimension == 1){
        return closest_point(Segment3{
            polyhedron.vertices.front(), polyhedron.vertices.back()
        }, point);
    }
    for(const auto& face: polyhedron.faces){
        for(std::size_t vertex: face){
            if(vertex >= polyhedron.vertices.size())[[unlikely]]{
                throw std::out_of_range("convex polyhedron face index");
            }
        }
        const Point3 candidate = closest_point(Triangle3{
            polyhedron.vertices[face[0]], polyhedron.vertices[face[1]],
            polyhedron.vertices[face[2]]
        }, point);
        const long double candidate_distance = distance(candidate, point);
        if(candidate_distance < best){
            best = candidate_distance;
            answer = candidate;
        }
    }
    return answer;
}
