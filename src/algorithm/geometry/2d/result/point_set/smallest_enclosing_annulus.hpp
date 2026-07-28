#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_POINT_SET_SMALLEST_ENCLOSING_ANNULUS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_POINT_SET_SMALLEST_ENCLOSING_ANNULUS_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <map>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../detail/numeric/plus_geometry_detail.hpp"
#include "../../result/point_set/voronoi_diagram_points.hpp"
#include "../../type/definition/enclosing_annulus_result.hpp"

namespace smallest_enclosing_annulus_detail{

inline Point circumcenter(
    const Point& first,
    const Point& second,
    const Point& third
){
    return voronoi_diagram_detail::delaunay_triangle_circumcenter(
        first, second, third
    );
}

inline void consider(
    const std::vector<Point>& points,
    const Point& center,
    EnclosingAnnulusResult& best
){
    if(!std::isfinite(center.x) || !std::isfinite(center.y)) return;
    long double minimum_squared =
        std::numeric_limits<long double>::infinity();
    long double maximum_squared = 0.0L;
    for(const Point& point: points){
        const long double distance =
            plus_geometry_detail::squared_distance(point, center);
        minimum_squared = std::min(minimum_squared, distance);
        maximum_squared = std::max(maximum_squared, distance);
    }
    const long double inner =
        std::sqrt(std::max(0.0L, minimum_squared));
    const long double outer =
        std::sqrt(std::max(0.0L, maximum_squared));
    if(outer - inner < best.outer_radius - best.inner_radius){
        best = {center, inner, outer};
    }
}

inline std::vector<std::size_t> strict_convex_hull_indices(
    const std::vector<Point>& points
){
    std::vector<std::size_t> order(points.size());
    std::iota(order.begin(), order.end(), std::size_t{0});
    std::sort(order.begin(), order.end(), [&](std::size_t left,
                                              std::size_t right){
        if(points[left] < points[right]) return true;
        if(points[right] < points[left]) return false;
        return left < right;
    });
    order.erase(std::unique(
        order.begin(), order.end(),
        [&](std::size_t left, std::size_t right){
            return plus_geometry_detail::close(
                points[left], points[right]
            );
        }
    ), order.end());
    if(order.size() <= 1) return order;
    std::vector<std::size_t> hull;
    for(std::size_t index: order){
        while(hull.size() >= 2
            && plus_geometry_detail::orientation(
                points[hull[hull.size() - 2]],
                points[hull.back()],
                points[index]
            ) <= 0){
            hull.pop_back();
        }
        hull.push_back(index);
    }
    const std::size_t lower_size = hull.size();
    for(auto iterator = order.rbegin() + 1;
        iterator != order.rend();
        ++iterator){
        while(hull.size() > lower_size
            && plus_geometry_detail::orientation(
                points[hull[hull.size() - 2]],
                points[hull.back()],
                points[*iterator]
            ) <= 0){
            hull.pop_back();
        }
        hull.push_back(*iterator);
    }
    hull.pop_back();
    return hull;
}

struct FarthestDiagram{
    std::vector<Point> vertices;
    std::vector<VoronoiEdge> edges;
};

inline FarthestDiagram farthest_voronoi(
    const std::vector<Point>& points
){
    FarthestDiagram result;
    std::vector<std::size_t> polygon =
        strict_convex_hull_indices(points);
    if(polygon.size() == 2){
        VoronoiEdge edge;
        edge.kind = VoronoiEdgeKind::LINE;
        edge.first_site = polygon[0];
        edge.second_site = polygon[1];
        edge.origin =
            (points[polygon[0]] + points[polygon[1]]) / 2.0L;
        const Point direction =
            points[polygon[1]] - points[polygon[0]];
        edge.endpoint_or_direction = Point{
            -direction.y, direction.x
        } / std::sqrt(dot(direction, direction));
        result.edges.push_back(edge);
        return result;
    }
    if(polygon.size() < 3) return result;

    std::vector<std::array<std::size_t, 3>> triangles;
    while(polygon.size() > 3){
        bool removed = false;
        for(std::size_t position = 0;
            position < polygon.size();
            ++position){
            const std::size_t previous = polygon[
                (position + polygon.size() - 1) % polygon.size()
            ];
            const std::size_t current = polygon[position];
            const std::size_t next =
                polygon[(position + 1) % polygon.size()];
            if(plus_geometry_detail::orientation(
                points[previous], points[current], points[next]
            ) <= 0) continue;
            bool contains_all = true;
            for(const Point& point: points){
                if(delaunay_triangulation_detail::circumcircle_sign(
                    points[previous], points[current], points[next], point
                ) < 0){
                    contains_all = false;
                    break;
                }
            }
            if(!contains_all) continue;
            triangles.push_back({previous, current, next});
            polygon.erase(
                polygon.begin()
                    + static_cast<std::ptrdiff_t>(position)
            );
            removed = true;
            break;
        }
        if(!removed){
            triangles.push_back({
                polygon[0], polygon[1], polygon[2]
            });
            polygon.erase(polygon.begin() + 1);
        }
    }
    triangles.push_back({polygon[0], polygon[1], polygon[2]});
    std::vector<std::size_t> triangle_vertex;
    for(const auto& triangle: triangles){
        const Point center = circumcenter(
            points[triangle[0]],
            points[triangle[1]],
            points[triangle[2]]
        );
        std::size_t vertex = result.vertices.size();
        for(std::size_t index = 0; index < result.vertices.size(); ++index){
            if(plus_geometry_detail::close(
                result.vertices[index], center
            )){
                vertex = index;
                break;
            }
        }
        if(vertex == result.vertices.size()){
            result.vertices.push_back(center);
        }
        triangle_vertex.push_back(vertex);
    }
    std::map<
        std::pair<std::size_t, std::size_t>,
        std::vector<std::size_t>
    > adjacent;
    for(std::size_t triangle = 0;
        triangle < triangles.size();
        ++triangle){
        for(std::size_t edge = 0; edge < 3; ++edge){
            std::size_t first = triangles[triangle][edge];
            std::size_t second =
                triangles[triangle][(edge + 1) % 3];
            if(first > second) std::swap(first, second);
            adjacent[{first, second}].push_back(triangle);
        }
    }
    for(const auto& [sites, incident]: adjacent){
        VoronoiEdge edge;
        edge.first_site = sites.first;
        edge.second_site = sites.second;
        if(incident.size() == 2){
            const std::size_t first_vertex =
                triangle_vertex[incident[0]];
            const std::size_t second_vertex =
                triangle_vertex[incident[1]];
            if(first_vertex == second_vertex) continue;
            edge.kind = VoronoiEdgeKind::SEGMENT;
            edge.first_vertex = first_vertex;
            edge.second_vertex = second_vertex;
            edge.origin = result.vertices[first_vertex];
            edge.endpoint_or_direction =
                result.vertices[second_vertex];
        }else{
            const std::size_t triangle = incident.front();
            std::size_t opposite = triangles[triangle][0];
            for(std::size_t vertex: triangles[triangle]){
                if(vertex != sites.first && vertex != sites.second){
                    opposite = vertex;
                }
            }
            const Point midpoint =
                (points[sites.first] + points[sites.second]) / 2.0L;
            Point direction{
                -(points[sites.second] - points[sites.first]).y,
                (points[sites.second] - points[sites.first]).x,
            };
            direction /= std::sqrt(dot(direction, direction));
            if(dot(direction, points[opposite] - midpoint) < 0.0L){
                direction = -direction;
            }
            edge.kind = VoronoiEdgeKind::RAY;
            edge.first_vertex = triangle_vertex[triangle];
            edge.origin = result.vertices[edge.first_vertex];
            edge.endpoint_or_direction = direction;
        }
        result.edges.push_back(edge);
    }
    return result;
}

struct ParametricEdge{
    Point origin;
    Point direction;
    long double minimum;
    long double maximum;
};

inline ParametricEdge parametric(const VoronoiEdge& edge){
    if(edge.kind == VoronoiEdgeKind::SEGMENT){
        return {
            edge.origin,
            edge.endpoint_or_direction - edge.origin,
            0.0L,
            1.0L,
        };
    }
    if(edge.kind == VoronoiEdgeKind::RAY){
        return {
            edge.origin,
            edge.endpoint_or_direction,
            0.0L,
            std::numeric_limits<long double>::infinity(),
        };
    }
    return {
        edge.origin,
        edge.endpoint_or_direction,
        -std::numeric_limits<long double>::infinity(),
        std::numeric_limits<long double>::infinity(),
    };
}

inline bool intersection(
    const VoronoiEdge& first,
    const VoronoiEdge& second,
    Point& answer
){
    const ParametricEdge left = parametric(first);
    const ParametricEdge right = parametric(second);
    const long double denominator =
        cross(left.direction, right.direction);
    if(plus_geometry_detail::sign(
        denominator,
        std::abs(left.direction.x * right.direction.y)
            + std::abs(left.direction.y * right.direction.x)
    ) == 0){
        return false;
    }
    const Point offset = right.origin - left.origin;
    const long double left_parameter =
        cross(offset, right.direction) / denominator;
    const long double right_parameter =
        cross(offset, left.direction) / denominator;
    if(left_parameter < left.minimum || left.maximum < left_parameter
        || right_parameter < right.minimum
        || right.maximum < right_parameter){
        return false;
    }
    answer = left.origin + left.direction * left_parameter;
    return true;
}

}  // namespace smallest_enclosing_annulus_detail

inline EnclosingAnnulusResult smallest_enclosing_annulus(
    const std::vector<Point>& points
){
    if(points.empty()) return {};
    for(const Point& point: points){
        plus_geometry_detail::validate_finite(point);
    }
    std::vector<Point> unique = points;
    std::sort(unique.begin(), unique.end());
    unique.erase(std::unique(
        unique.begin(), unique.end(),
        [](const Point& first, const Point& second){
            return plus_geometry_detail::close(first, second);
        }
    ), unique.end());
    if(unique.size() >= 3){
        bool noncollinear = false;
        for(std::size_t index = 2; index < unique.size(); ++index){
            if(plus_geometry_detail::orientation(
                unique[0], unique[1], unique[index]
            ) != 0){
                noncollinear = true;
                break;
            }
        }
        if(!noncollinear){
            throw std::domain_error(
                "a finite smallest annulus does not exist for three "
                "or more distinct collinear points"
            );
        }
    }
    EnclosingAnnulusResult result{
        points.front(),
        0.0L,
        std::numeric_limits<long double>::infinity(),
    };
    for(const Point& point: points){
        smallest_enclosing_annulus_detail::consider(
            points, point, result
        );
    }
    for(std::size_t first = 0; first < points.size(); ++first){
        for(std::size_t second = first + 1;
            second < points.size();
            ++second){
            smallest_enclosing_annulus_detail::consider(
                points,
                (points[first] + points[second]) / 2.0L,
                result
            );
        }
    }
    const VoronoiDiagramResult nearest = voronoi_diagram(points);
    const auto farthest =
        smallest_enclosing_annulus_detail::farthest_voronoi(points);
    for(const Point& center: nearest.vertices){
        smallest_enclosing_annulus_detail::consider(
            points, center, result
        );
    }
    for(const Point& center: farthest.vertices){
        smallest_enclosing_annulus_detail::consider(
            points, center, result
        );
    }
    for(const VoronoiEdge& near_edge: nearest.edges){
        for(const VoronoiEdge& far_edge: farthest.edges){
            Point center;
            if(smallest_enclosing_annulus_detail::intersection(
                near_edge, far_edge, center
            )){
                smallest_enclosing_annulus_detail::consider(
                    points, center, result
                );
            }
        }
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_POINT_SET_SMALLEST_ENCLOSING_ANNULUS_HPP_INCLUDED
