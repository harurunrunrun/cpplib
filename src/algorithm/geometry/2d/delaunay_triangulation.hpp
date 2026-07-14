#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <map>
#include <set>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

#include "cross.hpp"

struct DelaunayTriangulationResult{
    std::vector<std::array<std::size_t, 3>> triangles;
    std::vector<std::pair<std::size_t, std::size_t>> edges;
    std::vector<std::size_t> representative;
    std::vector<std::size_t> sites;
};

namespace delaunay_triangulation_detail{

inline int robust_sign(long double value, long double magnitude){
    const long double tolerance = 64.0L * std::numeric_limits<long double>::epsilon() * magnitude;
    return (value > tolerance) - (value < -tolerance);
}

inline int orientation(const Point& first, const Point& second, const Point& third){
    const Point left = second - first;
    const Point right = third - first;
    const long double value = cross(left, right);
    const long double magnitude = std::abs(left.x * right.y)
        + std::abs(left.y * right.x);
    return robust_sign(value, magnitude);
}

inline bool point_equal(const Point& first, const Point& second){
    return geometry_sign(first.x - second.x) == 0
        && geometry_sign(first.y - second.y) == 0;
}

inline bool in_circumcircle(
    Point first,
    Point second,
    Point third,
    const Point& query
){
    first -= query;
    second -= query;
    third -= query;
    const long double first_cross = cross(second, third);
    const long double second_cross = cross(third, first);
    const long double third_cross = cross(first, second);
    const long double first_norm = first.x * first.x + first.y * first.y;
    const long double second_norm = second.x * second.x + second.y * second.y;
    const long double third_norm = third.x * third.x + third.y * third.y;
    const long double first_term = first_cross * first_norm;
    const long double second_term = second_cross * second_norm;
    const long double third_term = third_cross * third_norm;
    const long double value = first_term + second_term + third_term;
    const long double magnitude = std::abs(first_term)
        + std::abs(second_term) + std::abs(third_term);
    return robust_sign(value, magnitude) > 0;
}

class Builder{
    struct DirectedEdge{
        int to = 0;
        int counterclockwise = 0;
        int clockwise = 0;
        int reverse = 0;
        bool enabled = false;
    };

public:
    explicit Builder(std::vector<Point> positions)
        : positions_(std::move(positions)){
        if(positions_.size() >= 2){
            static_cast<void>(solve_range(0, static_cast<int>(positions_.size())));
        }
    }

    std::vector<std::pair<int, int>> edges() const{
        std::vector<std::pair<int, int>> result;
        for(int edge = 0; edge < static_cast<int>(edges_.size()); ++edge){
            if(!edges_[static_cast<std::size_t>(edge)].enabled) continue;
            const int reverse = edges_[static_cast<std::size_t>(edge)].reverse;
            if(edge < reverse) continue;
            int first = edges_[static_cast<std::size_t>(edge)].to;
            int second = edges_[static_cast<std::size_t>(reverse)].to;
            if(first > second) std::swap(first, second);
            result.push_back({first, second});
        }
        std::sort(result.begin(), result.end());
        result.erase(std::unique(result.begin(), result.end()), result.end());
        return result;
    }

private:
    int allocate_edge(){
        if(free_edges_.empty()){
            edges_.push_back({});
            return static_cast<int>(edges_.size()) - 1;
        }
        const int result = free_edges_.back();
        free_edges_.pop_back();
        return result;
    }

    std::pair<int, int> add_edge(int first, int second){
        const int forward = allocate_edge();
        const int backward = allocate_edge();
        edges_[static_cast<std::size_t>(forward)] = {
            second, forward, forward, backward, true
        };
        edges_[static_cast<std::size_t>(backward)] = {
            first, backward, backward, forward, true
        };
        return {forward, backward};
    }

    void erase_directed_edge(int edge){
        DirectedEdge& current = edges_[static_cast<std::size_t>(edge)];
        edges_[static_cast<std::size_t>(current.counterclockwise)].clockwise =
            current.clockwise;
        edges_[static_cast<std::size_t>(current.clockwise)].counterclockwise =
            current.counterclockwise;
        current.enabled = false;
    }

    void erase_edge(int edge){
        const int reverse = edges_[static_cast<std::size_t>(edge)].reverse;
        erase_directed_edge(edge);
        erase_directed_edge(reverse);
        free_edges_.push_back(edge);
        free_edges_.push_back(reverse);
    }

    void insert_counterclockwise_after(int edge, int after){
        const int next = edges_[static_cast<std::size_t>(after)].counterclockwise;
        edges_[static_cast<std::size_t>(edge)].counterclockwise = next;
        edges_[static_cast<std::size_t>(next)].clockwise = edge;
        edges_[static_cast<std::size_t>(edge)].clockwise = after;
        edges_[static_cast<std::size_t>(after)].counterclockwise = edge;
    }

    void insert_clockwise_after(int edge, int after){
        const int next = edges_[static_cast<std::size_t>(after)].clockwise;
        edges_[static_cast<std::size_t>(edge)].clockwise = next;
        edges_[static_cast<std::size_t>(next)].counterclockwise = edge;
        edges_[static_cast<std::size_t>(edge)].counterclockwise = after;
        edges_[static_cast<std::size_t>(after)].clockwise = edge;
    }

    int orientation(int first, int second, int third) const{
        return delaunay_triangulation_detail::orientation(
            positions_[static_cast<std::size_t>(first)],
            positions_[static_cast<std::size_t>(second)],
            positions_[static_cast<std::size_t>(third)]
        );
    }

    std::pair<int, int> next_vertex(int edge) const{
        const int vertex = edges_[static_cast<std::size_t>(edge)].to;
        const int next = edges_[static_cast<std::size_t>(
            edges_[static_cast<std::size_t>(edge)].reverse
        )].counterclockwise;
        return {vertex, next};
    }

    std::pair<int, int> previous_vertex(int edge) const{
        const int previous_edge = edges_[static_cast<std::size_t>(edge)].clockwise;
        return {
            edges_[static_cast<std::size_t>(previous_edge)].to,
            edges_[static_cast<std::size_t>(previous_edge)].reverse
        };
    }

    std::tuple<int, int, int, int> lower_tangent(
        int first,
        int first_edge,
        int second,
        int second_edge
    ) const{
        while(true){
            const auto [previous, previous_edge] = previous_vertex(first_edge);
            if(orientation(second, first, previous) > 0){
                first = previous;
                first_edge = previous_edge;
                continue;
            }
            const auto [next, next_edge] = next_vertex(second_edge);
            if(orientation(first, second, next) < 0){
                second = next;
                second_edge = next_edge;
                continue;
            }
            break;
        }
        return {first, first_edge, second, second_edge};
    }

    std::pair<int, int> extreme_vertex(int vertex, int edge, bool minimum) const{
        std::pair<int, int> result = {vertex, edge};
        int current_vertex = vertex;
        int current_edge = edge;
        do{
            std::tie(current_vertex, current_edge) = next_vertex(current_edge);
            if(minimum) result = std::min(result, std::pair{current_vertex, current_edge});
            else result = std::max(result, std::pair{current_vertex, current_edge});
        }while(current_edge != edge);
        return result;
    }

    bool in_circumcircle(int first, int second, int third, int query) const{
        return delaunay_triangulation_detail::in_circumcircle(
            positions_[static_cast<std::size_t>(first)],
            positions_[static_cast<std::size_t>(second)],
            positions_[static_cast<std::size_t>(third)],
            positions_[static_cast<std::size_t>(query)]
        );
    }

    std::pair<int, int> merge_triangulations(
        int first,
        int first_edge,
        int second,
        int second_edge
    ){
        std::tie(first, first_edge) = extreme_vertex(first, first_edge, false);
        std::tie(second, second_edge) = extreme_vertex(second, second_edge, true);
        auto [lower_first, lower_first_edge, lower_second, lower_second_edge] =
            lower_tangent(first, first_edge, second, second_edge);
        auto [upper_second, upper_second_edge, upper_first, upper_first_edge] =
            lower_tangent(second, second_edge, first, first_edge);
        lower_second_edge = edges_[static_cast<std::size_t>(lower_second_edge)].clockwise;
        upper_second_edge = edges_[static_cast<std::size_t>(upper_second_edge)].clockwise;

        const auto [base_forward, base_backward] = add_edge(lower_first, lower_second);
        insert_clockwise_after(base_forward, lower_first_edge);
        insert_counterclockwise_after(base_backward, lower_second_edge);
        if(lower_first == upper_first) upper_first_edge = base_forward;
        if(lower_second == upper_second) upper_second_edge = base_backward;

        int first_vertex = lower_first;
        int current_first_edge = lower_first_edge;
        int second_vertex = lower_second;
        int current_second_edge = lower_second_edge;
        while(first_vertex != upper_first || second_vertex != upper_second){
            const int next_first_vertex =
                edges_[static_cast<std::size_t>(current_first_edge)].to;
            const int next_second_vertex =
                edges_[static_cast<std::size_t>(current_second_edge)].to;
            int next_first_edge =
                edges_[static_cast<std::size_t>(current_first_edge)].counterclockwise;
            int next_second_edge =
                edges_[static_cast<std::size_t>(current_second_edge)].clockwise;

            if(current_first_edge != upper_first_edge && next_first_edge != base_forward){
                const int candidate = edges_[static_cast<std::size_t>(next_first_edge)].to;
                if(in_circumcircle(
                    first_vertex, second_vertex, next_first_vertex, candidate
                )){
                    erase_edge(current_first_edge);
                    current_first_edge = next_first_edge;
                    continue;
                }
            }
            if(current_second_edge != upper_second_edge
                && next_second_edge != base_backward){
                const int candidate = edges_[static_cast<std::size_t>(next_second_edge)].to;
                if(in_circumcircle(
                    next_second_vertex, first_vertex, second_vertex, candidate
                )){
                    erase_edge(current_second_edge);
                    current_second_edge = next_second_edge;
                    continue;
                }
            }

            bool choose_first = current_second_edge == upper_second_edge;
            if(current_first_edge != upper_first_edge
                && current_second_edge != upper_second_edge){
                if(orientation(first_vertex, second_vertex, next_second_vertex) < 0){
                    choose_first = true;
                }else if(orientation(next_first_vertex, first_vertex, second_vertex) < 0){
                    choose_first = false;
                }else{
                    choose_first = in_circumcircle(
                        first_vertex,
                        second_vertex,
                        next_second_vertex,
                        next_first_vertex
                    );
                }
            }

            if(choose_first){
                next_first_edge = edges_[static_cast<std::size_t>(
                    edges_[static_cast<std::size_t>(current_first_edge)].reverse
                )].counterclockwise;
                const auto [forward, backward] = add_edge(
                    next_first_vertex, second_vertex
                );
                insert_clockwise_after(forward, next_first_edge);
                insert_counterclockwise_after(backward, current_second_edge);
                current_first_edge = next_first_edge;
                first_vertex = next_first_vertex;
            }else{
                next_second_edge = edges_[static_cast<std::size_t>(
                    edges_[static_cast<std::size_t>(current_second_edge)].reverse
                )].clockwise;
                const auto [backward, forward] = add_edge(
                    next_second_vertex, first_vertex
                );
                insert_counterclockwise_after(backward, next_second_edge);
                insert_clockwise_after(forward, current_first_edge);
                current_second_edge = next_second_edge;
                second_vertex = next_second_vertex;
            }
        }
        return {lower_first, base_forward};
    }

    std::pair<int, int> solve_range(int left, int right){
        if(right - left == 2){
            const auto [forward, backward] = add_edge(left, left + 1);
            static_cast<void>(backward);
            return {left, forward};
        }
        if(right - left == 3){
            const int first = left;
            const int second = left + 1;
            const int third = left + 2;
            const auto [first_second, second_first] = add_edge(first, second);
            const auto [second_third, third_second] = add_edge(second, third);
            const int direction = orientation(first, second, third);
            if(direction == 0){
                insert_counterclockwise_after(second_first, second_third);
                return {first, first_second};
            }
            const auto [first_third, third_first] = add_edge(first, third);
            if(direction > 0){
                insert_clockwise_after(first_second, first_third);
                insert_clockwise_after(second_third, second_first);
                insert_clockwise_after(third_first, third_second);
                return {first, first_second};
            }
            insert_counterclockwise_after(first_second, first_third);
            insert_counterclockwise_after(second_third, second_first);
            insert_counterclockwise_after(third_first, third_second);
            return {second, second_first};
        }
        const int middle = (left + right) / 2;
        const auto [first, first_edge] = solve_range(left, middle);
        const auto [second, second_edge] = solve_range(middle, right);
        return merge_triangulations(first, first_edge, second, second_edge);
    }

    std::vector<Point> positions_;
    std::vector<DirectedEdge> edges_;
    std::vector<int> free_edges_;
};

inline bool direction_less(const Point& first, const Point& second){
    const auto lower_half = [](const Point& direction){
        return direction.y < 0.0L
            || (direction.y == 0.0L && direction.x < 0.0L);
    };
    const bool first_lower = lower_half(first);
    const bool second_lower = lower_half(second);
    if(first_lower != second_lower) return !first_lower;
    const long double direction = cross(first, second);
    if(direction != 0.0L) return direction > 0.0L;
    const long double first_norm = first.x * first.x + first.y * first.y;
    const long double second_norm = second.x * second.x + second.y * second.y;
    if(first_norm != second_norm) return first_norm < second_norm;
    if(first.x != second.x) return first.x < second.x;
    return first.y < second.y;
}

inline std::vector<std::vector<int>> bounded_faces(
    const std::vector<Point>& points,
    const std::vector<std::pair<int, int>>& edges
){
    std::vector<std::vector<int>> adjacency(points.size());
    for(const auto& [first, second]: edges){
        adjacency[static_cast<std::size_t>(first)].push_back(second);
        adjacency[static_cast<std::size_t>(second)].push_back(first);
    }
    for(std::size_t vertex = 0; vertex < points.size(); ++vertex){
        std::sort(
            adjacency[vertex].begin(),
            adjacency[vertex].end(),
            [&](int first, int second){
                return direction_less(
                    points[static_cast<std::size_t>(first)] - points[vertex],
                    points[static_cast<std::size_t>(second)] - points[vertex]
                );
            }
        );
    }
    std::vector<std::map<int, std::size_t>> neighbor_positions(points.size());
    for(std::size_t vertex = 0; vertex < points.size(); ++vertex){
        for(std::size_t index = 0; index < adjacency[vertex].size(); ++index){
            neighbor_positions[vertex].emplace(
                adjacency[vertex][index], index
            );
        }
    }

    std::set<std::pair<int, int>> visited;
    std::vector<std::vector<int>> faces;
    for(const auto& [edge_first, edge_second]: edges){
        for(const auto& [start_first, start_second]: {
            std::pair{edge_first, edge_second},
            std::pair{edge_second, edge_first},
        }){
            if(visited.contains({start_first, start_second})) continue;
            std::vector<int> face;
            int first = start_first;
            int second = start_second;
            while(!visited.contains({first, second})){
                visited.insert({first, second});
                face.push_back(first);
                const std::vector<int>& neighbors =
                    adjacency[static_cast<std::size_t>(second)];
                const std::size_t index = neighbor_positions[
                    static_cast<std::size_t>(second)].at(first);
                const int next = neighbors[
                    (index + neighbors.size() - 1) % neighbors.size()
                ];
                first = second;
                second = next;
            }
            if(first != start_first || second != start_second || face.size() < 3) continue;
            long double twice_area = 0.0L;
            long double magnitude = 0.0L;
            for(std::size_t index = 0; index < face.size(); ++index){
                const Point& current = points[static_cast<std::size_t>(face[index])];
                const Point& next = points[static_cast<std::size_t>(
                    face[(index + 1) % face.size()]
                )];
                const long double term = cross(current, next);
                twice_area += term;
                magnitude += std::abs(term);
            }
            if(robust_sign(twice_area, magnitude) > 0) faces.push_back(std::move(face));
        }
    }
    return faces;
}

inline void canonicalize_triangle(std::array<std::size_t, 3>& triangle){
    const auto minimum = std::min_element(triangle.begin(), triangle.end());
    std::rotate(triangle.begin(), minimum, triangle.end());
}

}  // namespace delaunay_triangulation_detail

inline DelaunayTriangulationResult delaunay_triangulation(
    const std::vector<Point>& points
){
    long double coordinate_bound = 0.0L;
    for(const Point& point: points){
        if(!std::isfinite(point.x) || !std::isfinite(point.y)){
            throw std::invalid_argument("Delaunay sites must be finite");
        }
        coordinate_bound = std::max({
            coordinate_bound,
            std::abs(point.x),
            std::abs(point.y),
        });
    }
    const long double safe_bound = std::pow(
        std::numeric_limits<long double>::max() / 256.0L,
        0.25L
    );
    if(coordinate_bound > safe_bound){
        throw std::overflow_error("Delaunay coordinates are too large");
    }

    DelaunayTriangulationResult result;
    result.representative.resize(points.size());

    std::vector<std::size_t> order(points.size());
    for(std::size_t index = 0; index < points.size(); ++index) order[index] = index;
    std::sort(order.begin(), order.end(), [&](std::size_t first, std::size_t second){
        if(points[first].x != points[second].x) return points[first].x < points[second].x;
        if(points[first].y != points[second].y) return points[first].y < points[second].y;
        return first < second;
    });

    std::vector<Point> unique_points;
    std::set<std::pair<long double, std::size_t>> active_sites;
    std::size_t active_left = 0;
    for(std::size_t index: order){
        while(active_left < unique_points.size()
            && geometry_sign(points[index].x - unique_points[active_left].x) > 0){
            active_sites.erase({unique_points[active_left].y, active_left});
            ++active_left;
        }

        std::size_t site_index = std::numeric_limits<std::size_t>::max();
        auto candidate = active_sites.lower_bound({
            points[index].y - GEOMETRY_EPS, 0
        });
        while(candidate != active_sites.end()
            && geometry_sign(candidate->first - points[index].y) <= 0){
            const std::size_t current_site = candidate->second;
            if(delaunay_triangulation_detail::point_equal(
                unique_points[current_site], points[index]
            )){
                site_index = std::min(site_index, current_site);
            }
            ++candidate;
        }

        if(site_index == std::numeric_limits<std::size_t>::max()){
            site_index = unique_points.size();
            unique_points.push_back(points[index]);
            result.sites.push_back(index);
            active_sites.emplace(points[index].y, site_index);
        }
        result.representative[index] = result.sites[site_index];
    }

    if(unique_points.size() < 2) return result;
    delaunay_triangulation_detail::Builder builder(unique_points);
    std::vector<std::pair<int, int>> unique_edges = builder.edges();
    const std::vector<std::vector<int>> faces =
        delaunay_triangulation_detail::bounded_faces(unique_points, unique_edges);

    for(const std::vector<int>& face: faces){
        for(std::size_t index = 1; index + 1 < face.size(); ++index){
            std::array<std::size_t, 3> triangle = {
                result.sites[static_cast<std::size_t>(face[0])],
                result.sites[static_cast<std::size_t>(face[index])],
                result.sites[static_cast<std::size_t>(face[index + 1])],
            };
            delaunay_triangulation_detail::canonicalize_triangle(triangle);
            result.triangles.push_back(triangle);
            unique_edges.push_back({face[0], face[index + 1]});
        }
    }
    std::sort(result.triangles.begin(), result.triangles.end());
    result.triangles.erase(
        std::unique(result.triangles.begin(), result.triangles.end()),
        result.triangles.end()
    );

    for(auto [first, second]: unique_edges){
        std::size_t first_index = result.sites[static_cast<std::size_t>(first)];
        std::size_t second_index = result.sites[static_cast<std::size_t>(second)];
        if(first_index > second_index) std::swap(first_index, second_index);
        result.edges.push_back({first_index, second_index});
    }
    std::sort(result.edges.begin(), result.edges.end());
    result.edges.erase(std::unique(result.edges.begin(), result.edges.end()), result.edges.end());
    return result;
}
