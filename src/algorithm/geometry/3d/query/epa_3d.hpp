#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_EPA_3D_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_EPA_3D_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../point/cross.hpp"
#include "../scalar/dot.hpp"
#include "epa_result3.hpp"
#include "gjk_query_3d.hpp"
#include "separating_axis_theorem_3d.hpp"

namespace epa_3d_detail{

struct Face{
    std::array<std::size_t, 3> vertices{};
    Point3 normal{};
    long double distance = 0.0L;
    bool alive = true;
};

inline void validate_full_dimensional(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second
){
    gjk_3d_detail::validate(first);
    gjk_3d_detail::validate(second);
    if(first.affine_dimension != 3
        || second.affine_dimension != 3)[[unlikely]]{
        throw std::invalid_argument(
            "EPA requires full-dimensional convex polyhedra"
        );
    }
}

inline bool same_point(
    const Point3& first,
    const Point3& second,
    long double tolerance
){
    const long double scale = std::max({
        1.0L, gjk_3d_detail::max_coordinate(first),
        gjk_3d_detail::max_coordinate(second)
    });
    return gjk_3d_detail::max_coordinate(first - second) <= tolerance * scale;
}

inline void add_unique(
    std::vector<GJKSupportPoint3>& points,
    const GJKSupportPoint3& candidate,
    long double tolerance
){
    for(const GJKSupportPoint3& point: points){
        if(same_point(point.point, candidate.point, tolerance)) return;
    }
    points.push_back(candidate);
}

inline std::vector<GJKSupportPoint3> initial_tetrahedron(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    const std::vector<GJKSupportPoint3>& simplex,
    long double tolerance
){
    std::vector<GJKSupportPoint3> candidates;
    for(const GJKSupportPoint3& point: simplex){
        add_unique(candidates, point, tolerance);
    }
    for(int x = -1; x <= 1; ++x){
        for(int y = -1; y <= 1; ++y){
            for(int z = -1; z <= 1; ++z){
                if(x == 0 && y == 0 && z == 0) continue;
                add_unique(candidates, gjk_3d_detail::support(
                    first, second, {
                        static_cast<long double>(x),
                        static_cast<long double>(y),
                        static_cast<long double>(z)
                    }
                ), tolerance);
            }
        }
    }
    for(std::size_t first_index = 0; first_index < candidates.size();
        ++first_index){
        for(std::size_t second_index = first_index + 1;
            second_index < candidates.size(); ++second_index){
            for(std::size_t third_index = second_index + 1;
                third_index < candidates.size(); ++third_index){
                for(std::size_t fourth_index = third_index + 1;
                    fourth_index < candidates.size(); ++fourth_index){
                    std::vector<GJKSupportPoint3> tetrahedron{
                        candidates[first_index], candidates[second_index],
                        candidates[third_index], candidates[fourth_index]
                    };
                    const auto closest = gjk_3d_detail::closest_simplex(
                        tetrahedron, tolerance
                    );
                    const long double scale = std::max(
                        1.0L,
                        gjk_3d_detail::max_coordinate(closest.point)
                    );
                    if(closest.simplex.size() == 4 &&
                        gjk_3d_detail::max_coordinate(closest.point)
                            <= tolerance * scale){
                        return tetrahedron;
                    }
                }
            }
        }
    }
    return {};
}

inline Face make_face(
    std::size_t first,
    std::size_t second,
    std::size_t third,
    const std::vector<GJKSupportPoint3>& vertices
){
    Point3 first_edge = vertices[second].point - vertices[first].point;
    Point3 second_edge = vertices[third].point - vertices[first].point;
    const long double scale = std::max(
        gjk_3d_detail::max_coordinate(first_edge),
        gjk_3d_detail::max_coordinate(second_edge)
    );
    if(scale == 0.0L) return {{first, second, third}, {}, 0.0L, false};
    Point3 normal = cross(first_edge / scale, second_edge / scale);
    const long double length = std::sqrt(dot(normal, normal));
    if(length == 0.0L || !std::isfinite(length)){
        return {{first, second, third}, {}, 0.0L, false};
    }
    normal /= length;
    long double distance = dot(normal, vertices[first].point);
    if(distance < 0.0L){
        std::swap(second, third);
        normal = -normal;
        distance = -distance;
    }
    return {{first, second, third}, normal, distance, true};
}

inline EPAResult3 face_result(
    const Face& face,
    const std::vector<GJKSupportPoint3>& vertices,
    std::size_t iterations,
    bool converged
){
    const GJKSupportPoint3& first = vertices[face.vertices[0]];
    const GJKSupportPoint3& second = vertices[face.vertices[1]];
    const GJKSupportPoint3& third = vertices[face.vertices[2]];
    const Point3 target = face.normal * face.distance;
    const Point3 edge0 = second.point - first.point;
    const Point3 edge1 = third.point - first.point;
    const Point3 offset = target - first.point;
    const long double d00 = dot(edge0, edge0);
    const long double d01 = dot(edge0, edge1);
    const long double d11 = dot(edge1, edge1);
    const long double d20 = dot(offset, edge0);
    const long double d21 = dot(offset, edge1);
    const long double denominator = d00 * d11 - d01 * d01;
    long double second_weight = 0.0L;
    long double third_weight = 0.0L;
    if(denominator != 0.0L){
        second_weight = (d11 * d20 - d01 * d21) / denominator;
        third_weight = (d00 * d21 - d01 * d20) / denominator;
    }
    long double first_weight = 1.0L - second_weight - third_weight;
    first_weight = std::max(0.0L, first_weight);
    second_weight = std::max(0.0L, second_weight);
    third_weight = std::max(0.0L, third_weight);
    const long double sum = first_weight + second_weight + third_weight;
    if(sum > 0.0L){
        first_weight /= sum;
        second_weight /= sum;
        third_weight /= sum;
    }
    EPAResult3 result;
    result.intersects = true;
    result.penetration_depth = std::max(0.0L, face.distance);
    result.normal = face.normal;
    result.point_on_first = first.point_on_first * first_weight
        + second.point_on_first * second_weight
        + third.point_on_first * third_weight;
    result.point_on_second = first.point_on_second * first_weight
        + second.point_on_second * second_weight
        + third.point_on_second * third_weight;
    result.iterations = iterations;
    result.converged = converged;
    return result;
}

inline EPAResult3 sat_fallback(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    std::size_t iterations,
    long double tolerance
){
    const SATResult3 sat = sat_3d_detail::separating_axis_theorem_core(
        first, second, tolerance
    );
    EPAResult3 result;
    result.intersects = sat.intersects;
    result.penetration_depth = sat.penetration_depth;
    result.normal = sat.axis;
    result.iterations = iterations;
    result.converged = true;
    if(sat.intersects){
        const GJKSupportPoint3 contact = gjk_3d_detail::support(
            first, second, sat.axis
        );
        result.point_on_first = contact.point_on_first;
        result.point_on_second = contact.point_on_second;
    }
    return result;
}

inline EPAResult3 epa_core(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    const GJKResult3& gjk_result,
    long double tolerance,
    std::size_t max_iterations
){
    validate_full_dimensional(first, second);
    if(!(tolerance > 0.0L) || !std::isfinite(tolerance))[[unlikely]]{
        throw std::invalid_argument("EPA tolerance must be finite and positive");
    }
    if(max_iterations == 0)[[unlikely]]{
        throw std::invalid_argument("EPA iteration limit must be positive");
    }
    if(!gjk_result.intersects) return {};
    std::vector<GJKSupportPoint3> vertices = initial_tetrahedron(
        first, second, gjk_result.simplex, tolerance
    );
    if(vertices.size() != 4){
        return sat_fallback(first, second, 0, tolerance);
    }
    std::vector<Face> faces{
        make_face(0, 2, 1, vertices), make_face(0, 1, 3, vertices),
        make_face(0, 3, 2, vertices), make_face(1, 2, 3, vertices)
    };

    for(std::size_t iteration = 0; iteration < max_iterations; ++iteration){
        std::size_t closest_index = faces.size();
        long double closest_distance =
            std::numeric_limits<long double>::infinity();
        for(std::size_t index = 0; index < faces.size(); ++index){
            if(faces[index].alive && faces[index].distance < closest_distance){
                closest_distance = faces[index].distance;
                closest_index = index;
            }
        }
        if(closest_index == faces.size())[[unlikely]]{
            return sat_fallback(first, second, iteration, tolerance);
        }
        const Face closest = faces[closest_index];
        const GJKSupportPoint3 next = gjk_3d_detail::support(
            first, second, closest.normal
        );
        const long double support_distance = dot(next.point, closest.normal);
        const long double threshold = tolerance * std::max({
            1.0L, std::abs(closest.distance), std::abs(support_distance)
        });
        bool duplicate = false;
        for(const GJKSupportPoint3& point: vertices){
            duplicate = duplicate || same_point(
                point.point, next.point, tolerance
            );
        }
        if(duplicate || support_distance - closest.distance <= threshold){
            return face_result(closest, vertices, iteration + 1, true);
        }

        const std::size_t next_index = vertices.size();
        vertices.push_back(next);
        std::vector<std::pair<std::size_t, std::size_t>> boundary;
        auto add_edge = [&](std::size_t first_index, std::size_t second_index){
            const auto reverse = std::find(
                boundary.begin(), boundary.end(),
                std::pair{second_index, first_index}
            );
            if(reverse == boundary.end()){
                boundary.emplace_back(first_index, second_index);
            }else{
                boundary.erase(reverse);
            }
        };
        for(Face& face: faces){
            if(!face.alive) continue;
            const Point3 offset = next.point
                - vertices[face.vertices[0]].point;
            if(dot(face.normal, offset) <= threshold) continue;
            face.alive = false;
            add_edge(face.vertices[0], face.vertices[1]);
            add_edge(face.vertices[1], face.vertices[2]);
            add_edge(face.vertices[2], face.vertices[0]);
        }
        if(boundary.empty()){
            return face_result(closest, vertices, iteration + 1, true);
        }
        for(const auto& [first_index, second_index]: boundary){
            Face face = make_face(
                first_index, second_index, next_index, vertices
            );
            if(face.alive) faces.push_back(face);
        }
    }

    std::size_t closest_index = faces.size();
    long double distance = std::numeric_limits<long double>::infinity();
    for(std::size_t index = 0; index < faces.size(); ++index){
        if(faces[index].alive && faces[index].distance < distance){
            distance = faces[index].distance;
            closest_index = index;
        }
    }
    if(closest_index == faces.size()){
        return sat_fallback(first, second, max_iterations, tolerance);
    }
    return face_result(
        faces[closest_index], vertices, max_iterations, false
    );
}

inline EPAResult3 restore_result(
    EPAResult3 result,
    const gjk_3d_detail::CollisionNormalization3& normalization
){
    if(!result.intersects) return result;
    result.penetration_depth = gjk_3d_detail::restore_length(
        result.penetration_depth, normalization
    );
    result.normal = gjk_3d_detail::finite_unit_normal(result.normal);
    result.point_on_first = gjk_3d_detail::restore_point(
        result.point_on_first, normalization
    );
    result.point_on_second = gjk_3d_detail::restore_point(
        result.point_on_second, normalization
    );
    return result;
}

}  // namespace epa_3d_detail

inline EPAResult3 epa_3d(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    const GJKResult3& gjk_result,
    long double tolerance = 1.0e-10L,
    std::size_t max_iterations = 128
){
    if(!(tolerance > 0.0L) || !std::isfinite(tolerance))[[unlikely]]{
        throw std::invalid_argument("EPA tolerance must be finite and positive");
    }
    if(max_iterations == 0)[[unlikely]]{
        throw std::invalid_argument("EPA iteration limit must be positive");
    }
    epa_3d_detail::validate_full_dimensional(first, second);
    const gjk_3d_detail::CollisionNormalization3 normalization =
        gjk_3d_detail::normalize_pair(first, second);
    if(!gjk_result.intersects) return {};
    GJKResult3 normalized_gjk = gjk_result;
    normalized_gjk.simplex = gjk_3d_detail::normalize_simplex(
        gjk_result.simplex, normalization
    );
    if(normalized_gjk.simplex.empty()){
        normalized_gjk = gjk_3d_detail::gjk_query_core(
            normalization.first, normalization.second,
            tolerance * 0.01L, 96
        );
    }
    return epa_3d_detail::restore_result(
        epa_3d_detail::epa_core(
            normalization.first, normalization.second,
            normalized_gjk, tolerance, max_iterations
        ),
        normalization
    );
}

inline EPAResult3 epa_3d(
    const ConvexPolyhedron3& first,
    const ConvexPolyhedron3& second,
    long double tolerance = 1.0e-10L,
    std::size_t max_iterations = 128
){
    if(!(tolerance > 0.0L) || !std::isfinite(tolerance))[[unlikely]]{
        throw std::invalid_argument("EPA tolerance must be finite and positive");
    }
    if(max_iterations == 0)[[unlikely]]{
        throw std::invalid_argument("EPA iteration limit must be positive");
    }
    epa_3d_detail::validate_full_dimensional(first, second);
    const gjk_3d_detail::CollisionNormalization3 normalization =
        gjk_3d_detail::normalize_pair(first, second);
    const GJKResult3 gjk_result = gjk_3d_detail::gjk_query_core(
        normalization.first, normalization.second,
        tolerance * 0.01L, 96
    );
    return epa_3d_detail::restore_result(
        epa_3d_detail::epa_core(
            normalization.first, normalization.second,
            gjk_result, tolerance, max_iterations
        ),
        normalization
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_EPA_3D_HPP_INCLUDED
