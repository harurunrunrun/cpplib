#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_POINT_SET_INDEX_PAIR_SET_CONSTRAINED_DELAUNAY_TRIANGULATION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_POINT_SET_INDEX_PAIR_SET_CONSTRAINED_DELAUNAY_TRIANGULATION_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cstddef>
#include <map>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../detail/numeric/plus_geometry_detail.hpp"
#include "../../result/point_set/delaunay_triangulation_points.hpp"
#include "../../type/definition/constrained_delaunay_triangulation_result.hpp"

namespace constrained_delaunay_detail{

using Edge = std::pair<std::size_t, std::size_t>;
using Triangle = std::array<std::size_t, 3>;

inline Edge ordered_edge(std::size_t first, std::size_t second){
    if(first > second) std::swap(first, second);
    return {first, second};
}

inline bool proper_cross(
    const std::vector<Point>& points,
    const Edge& first,
    const Edge& second
){
    if(first.first == second.first || first.first == second.second
        || first.second == second.first || first.second == second.second){
        return false;
    }
    const int first_side = plus_geometry_detail::orientation(
        points[first.first], points[first.second], points[second.first]
    );
    const int second_side = plus_geometry_detail::orientation(
        points[first.first], points[first.second], points[second.second]
    );
    const int third_side = plus_geometry_detail::orientation(
        points[second.first], points[second.second], points[first.first]
    );
    const int fourth_side = plus_geometry_detail::orientation(
        points[second.first], points[second.second], points[first.second]
    );
    return first_side * second_side < 0 && third_side * fourth_side < 0;
}

inline void orient_triangle(
    const std::vector<Point>& points,
    Triangle& triangle
){
    if(plus_geometry_detail::orientation(
        points[triangle[0]], points[triangle[1]], points[triangle[2]]
    ) < 0){
        std::swap(triangle[1], triangle[2]);
    }
}

inline std::map<Edge, std::vector<std::size_t>> adjacency(
    const std::vector<Triangle>& triangles
){
    std::map<Edge, std::vector<std::size_t>> result;
    for(std::size_t triangle = 0; triangle < triangles.size(); ++triangle){
        for(std::size_t edge = 0; edge < 3; ++edge){
            result[ordered_edge(
                triangles[triangle][edge],
                triangles[triangle][(edge + 1) % 3]
            )].push_back(triangle);
        }
    }
    return result;
}

inline std::size_t opposite(
    const Triangle& triangle,
    const Edge& edge
){
    for(std::size_t vertex: triangle){
        if(vertex != edge.first && vertex != edge.second) return vertex;
    }
    return static_cast<std::size_t>(-1);
}

inline bool flip(
    const std::vector<Point>& points,
    std::vector<Triangle>& triangles,
    const Edge& edge,
    const std::vector<std::size_t>& adjacent
){
    if(adjacent.size() != 2) return false;
    const std::size_t first_opposite =
        opposite(triangles[adjacent[0]], edge);
    const std::size_t second_opposite =
        opposite(triangles[adjacent[1]], edge);
    if(first_opposite == second_opposite) return false;
    if(plus_geometry_detail::orientation(
        points[first_opposite], points[second_opposite], points[edge.first]
    ) * plus_geometry_detail::orientation(
        points[first_opposite], points[second_opposite], points[edge.second]
    ) >= 0){
        return false;
    }
    Triangle first{first_opposite, second_opposite, edge.first};
    Triangle second{second_opposite, first_opposite, edge.second};
    orient_triangle(points, first);
    orient_triangle(points, second);
    triangles[adjacent[0]] = first;
    triangles[adjacent[1]] = second;
    return true;
}

inline bool edge_exists(
    const std::map<Edge, std::vector<std::size_t>>& adjacent,
    const Edge& edge
){
    return adjacent.contains(edge);
}

}  // namespace constrained_delaunay_detail

inline ConstrainedDelaunayTriangulationResult
constrained_delaunay_triangulation(
    const std::vector<Point>& points,
    std::vector<std::pair<std::size_t, std::size_t>> constraints
){
    using namespace constrained_delaunay_detail;
    for(const Point& point: points) plus_geometry_detail::validate_finite(point);
    for(Edge& constraint: constraints){
        if(constraint.first >= points.size() || constraint.second >= points.size()
            || constraint.first == constraint.second){
            throw std::invalid_argument("invalid constrained Delaunay edge");
        }
        if(plus_geometry_detail::close(
            points[constraint.first], points[constraint.second]
        )){
            throw std::invalid_argument(
                "a constrained Delaunay edge requires distinct positions"
            );
        }
    }
    std::vector<Edge> split_constraints;
    for(const Edge& constraint: constraints){
        const Point direction =
            points[constraint.second] - points[constraint.first];
        const long double squared_length = dot(direction, direction);
        std::vector<std::pair<long double, std::size_t>> on_constraint;
        for(std::size_t point = 0; point < points.size(); ++point){
            if(plus_geometry_detail::orientation(
                points[constraint.first],
                points[constraint.second],
                points[point]
            ) != 0){
                continue;
            }
            const long double parameter =
                dot(points[point] - points[constraint.first], direction)
                / squared_length;
            if(plus_geometry_detail::sign(parameter) >= 0
                && plus_geometry_detail::sign(parameter - 1.0L) <= 0){
                on_constraint.push_back({parameter, point});
            }
        }
        std::sort(on_constraint.begin(), on_constraint.end());
        for(std::size_t index = 1;
            index < on_constraint.size();
            ++index){
            split_constraints.push_back(ordered_edge(
                on_constraint[index - 1].second,
                on_constraint[index].second
            ));
        }
    }
    constraints = std::move(split_constraints);
    std::sort(constraints.begin(), constraints.end());
    constraints.erase(
        std::unique(constraints.begin(), constraints.end()),
        constraints.end()
    );
    for(std::size_t first = 0; first < constraints.size(); ++first){
        for(std::size_t second = first + 1; second < constraints.size(); ++second){
            if(proper_cross(points, constraints[first], constraints[second])){
                throw std::invalid_argument(
                    "constrained Delaunay edges must not cross"
                );
            }
        }
    }

    const DelaunayTriangulationResult initial = delaunay_triangulation(points);
    for(std::size_t index = 0; index < points.size(); ++index){
        if(initial.representative[index] != index){
            throw std::invalid_argument(
                "constrained Delaunay sites must be pairwise distinct"
            );
        }
    }
    if(initial.triangles.empty()){
        ConstrainedDelaunayTriangulationResult result;
        result.edges = initial.edges;
        result.constraints = std::move(constraints);
        for(const Edge& constraint: result.constraints){
            if(!std::binary_search(
                result.edges.begin(), result.edges.end(), constraint
            )){
                throw std::invalid_argument(
                    "a collinear constraint is outside the site chain"
                );
            }
        }
        return result;
    }
    std::vector<Triangle> triangles = initial.triangles;
    std::set<Edge> protected_edges;
    for(const Edge& constraint: constraints){
        std::size_t flips = 0;
        while(true){
            const auto adjacent = adjacency(triangles);
            if(edge_exists(adjacent, constraint)) break;
            bool changed = false;
            for(const auto& [edge, incident]: adjacent){
                if(protected_edges.contains(edge)
                    || !proper_cross(points, edge, constraint)){
                    continue;
                }
                if(flip(points, triangles, edge, incident)){
                    changed = true;
                    ++flips;
                    break;
                }
            }
            if(!changed || flips > triangles.size() * triangles.size() + 1){
                throw std::invalid_argument(
                    "a constraint passes through a site or outside the triangulated hull"
                );
            }
        }
        protected_edges.insert(constraint);
    }

    bool changed = true;
    std::size_t passes = 0;
    while(changed && passes <= triangles.size() * triangles.size() + 1){
        changed = false;
        ++passes;
        const auto adjacent = adjacency(triangles);
        for(const auto& [edge, incident]: adjacent){
            if(incident.size() != 2 || protected_edges.contains(edge)) continue;
            const std::size_t first_opposite =
                opposite(triangles[incident[0]], edge);
            const std::size_t second_opposite =
                opposite(triangles[incident[1]], edge);
            const Edge replacement =
                ordered_edge(first_opposite, second_opposite);
            bool crosses_constraint = false;
            for(const Edge& constraint: protected_edges){
                if(proper_cross(points, replacement, constraint)){
                    crosses_constraint = true;
                    break;
                }
            }
            if(crosses_constraint) continue;
            Point first = points[edge.first];
            Point second = points[first_opposite];
            Point third = points[edge.second];
            if(plus_geometry_detail::orientation(first, second, third) < 0){
                std::swap(second, third);
            }
            if(delaunay_triangulation_detail::circumcircle_sign(
                first, second, third, points[second_opposite]
            ) > 0 && flip(points, triangles, edge, incident)){
                changed = true;
                break;
            }
        }
    }

    ConstrainedDelaunayTriangulationResult result;
    result.triangles = std::move(triangles);
    result.constraints = std::move(constraints);
    const auto adjacent = adjacency(result.triangles);
    for(const auto& [edge, incident]: adjacent){
        static_cast<void>(incident);
        result.edges.push_back(edge);
    }
    std::sort(result.triangles.begin(), result.triangles.end());
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_POINT_SET_INDEX_PAIR_SET_CONSTRAINED_DELAUNAY_TRIANGULATION_HPP_INCLUDED
