#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SHAPE_EUCLIDEAN_MST_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SHAPE_EUCLIDEAN_MST_HPP_INCLUDED

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <tuple>
#include <utility>
#include <vector>

struct EuclideanMstEdge{
    std::size_t from;
    std::size_t to;
    __uint128_t squared_distance;
};

struct EuclideanMstResult{
    std::vector<EuclideanMstEdge> edges;
};

namespace euclidean_mst_detail{

template<std::integral Coordinate>
struct Point{
    __int128_t x{};
    __int128_t y{};

    Point operator-(const Point& other) const{
        return {
            x - other.x,
            y - other.y
        };
    }

    bool operator==(const Point& other) const{
        return x == other.x && y == other.y;
    }
};

template<std::integral Coordinate>
__int128_t cross(const Point<Coordinate>& first, const Point<Coordinate>& second){
    return static_cast<__int128_t>(first.x) * static_cast<__int128_t>(second.y)
        - static_cast<__int128_t>(first.y) * static_cast<__int128_t>(second.x);
}

template<std::integral Coordinate>
__int128_t norm(const Point<Coordinate>& point){
    return static_cast<__int128_t>(point.x) * static_cast<__int128_t>(point.x)
        + static_cast<__int128_t>(point.y) * static_cast<__int128_t>(point.y);
}

template<std::integral Coordinate>
class DelaunayTriangulation{
    using GeometryPoint = Point<Coordinate>;

    struct DirectedEdge{
        int to = 0;
        int counterclockwise = 0;
        int clockwise = 0;
        int reverse = 0;
        bool enabled = false;
    };

public:
    explicit DelaunayTriangulation(std::vector<GeometryPoint> points)
        : positions_(std::move(points)){
        build();
    }

    std::vector<std::pair<int, int>> edges() const{
        std::vector<std::pair<int, int>> result;
        for(int edge = 0; edge < static_cast<int>(edges_.size()); ++edge){
            if(!edges_[static_cast<std::size_t>(edge)].enabled) continue;
            const int reverse = edges_[static_cast<std::size_t>(edge)].reverse;
            if(edge < reverse) continue;
            result.push_back({
                edges_[static_cast<std::size_t>(edge)].to,
                edges_[static_cast<std::size_t>(reverse)].to
            });
        }
        for(int vertex = 0; vertex < static_cast<int>(duplicate_to_.size()); ++vertex){
            if(duplicate_to_[static_cast<std::size_t>(vertex)] != vertex){
                result.push_back({vertex, duplicate_to_[static_cast<std::size_t>(vertex)]});
            }
        }
        return result;
    }

private:
    static bool in_circumcircle(
        GeometryPoint first,
        GeometryPoint second,
        GeometryPoint third,
        GeometryPoint query
    ){
        first = first - query;
        second = second - query;
        third = third - query;
        const __int128_t value = cross(second, third) * norm(first)
            + cross(third, first) * norm(second)
            + cross(first, second) * norm(third);
        return value > 0;
    }

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
        DirectedEdge& first_edge = edges_[static_cast<std::size_t>(forward)];
        DirectedEdge& second_edge = edges_[static_cast<std::size_t>(backward)];
        first_edge = {second, forward, forward, backward, true};
        second_edge = {first, backward, backward, forward, true};
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
        const __int128_t value = cross(
            positions_[static_cast<std::size_t>(second)]
                - positions_[static_cast<std::size_t>(first)],
            positions_[static_cast<std::size_t>(third)]
                - positions_[static_cast<std::size_t>(first)]
        );
        if(value > 0) return 1;
        if(value < 0) return -1;
        return 0;
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
            if(minimum){
                result = std::min(result, std::pair{current_vertex, current_edge});
            }else{
                result = std::max(result, std::pair{current_vertex, current_edge});
            }
        }while(current_edge != edge);
        return result;
    }

    bool in_circumcircle(int first, int second, int third, int query) const{
        return in_circumcircle(
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

    void build(){
        const int size = static_cast<int>(positions_.size());
        if(size <= 1){
            duplicate_to_.resize(static_cast<std::size_t>(size));
            std::iota(duplicate_to_.begin(), duplicate_to_.end(), 0);
            return;
        }

        std::vector<int> permutation(static_cast<std::size_t>(size));
        std::iota(permutation.begin(), permutation.end(), 0);
        std::stable_sort(permutation.begin(), permutation.end(), [&](int left, int right){
            const GeometryPoint& first = positions_[static_cast<std::size_t>(left)];
            const GeometryPoint& second = positions_[static_cast<std::size_t>(right)];
            if(first.x != second.x) return first.x < second.x;
            return first.y < second.y;
        });

        const std::vector<GeometryPoint> original_positions = positions_;
        duplicate_to_.assign(static_cast<std::size_t>(size), 0);
        int unique_count = 0;
        for(int index = 0; index < size; ++index){
            const int vertex = permutation[static_cast<std::size_t>(index)];
            if(index == 0 || !(original_positions[static_cast<std::size_t>(
                permutation[static_cast<std::size_t>(unique_count - 1)]
            )] == original_positions[static_cast<std::size_t>(vertex)])){
                permutation[static_cast<std::size_t>(unique_count)] = vertex;
                positions_[static_cast<std::size_t>(unique_count)] =
                    original_positions[static_cast<std::size_t>(vertex)];
                duplicate_to_[static_cast<std::size_t>(vertex)] = vertex;
                ++unique_count;
            }else{
                duplicate_to_[static_cast<std::size_t>(vertex)] =
                    permutation[static_cast<std::size_t>(unique_count - 1)];
            }
        }

        if(unique_count >= 2) static_cast<void>(solve_range(0, unique_count));
        positions_ = original_positions;
        for(DirectedEdge& edge: edges_){
            edge.to = permutation[static_cast<std::size_t>(edge.to)];
        }
    }

    std::vector<int> free_edges_;
    std::vector<GeometryPoint> positions_;
    std::vector<DirectedEdge> edges_;
    std::vector<int> duplicate_to_;
};

class Dsu{
public:
    explicit Dsu(std::size_t size): parent_(size), size_(size, 1){
        std::iota(parent_.begin(), parent_.end(), std::size_t{0});
    }

    std::size_t leader(std::size_t vertex){
        while(parent_[vertex] != vertex){
            parent_[vertex] = parent_[parent_[vertex]];
            vertex = parent_[vertex];
        }
        return vertex;
    }

    bool merge(std::size_t first, std::size_t second){
        first = leader(first);
        second = leader(second);
        if(first == second) return false;
        if(size_[first] < size_[second]) std::swap(first, second);
        parent_[second] = first;
        size_[first] += size_[second];
        return true;
    }

private:
    std::vector<std::size_t> parent_;
    std::vector<std::size_t> size_;
};

template<std::integral Coordinate>
__uint128_t squared_distance(
    const std::pair<Coordinate, Coordinate>& first,
    const std::pair<Coordinate, Coordinate>& second
){
    const __int128_t dx = static_cast<__int128_t>(first.first)
        - static_cast<__int128_t>(second.first);
    const __int128_t dy = static_cast<__int128_t>(first.second)
        - static_cast<__int128_t>(second.second);
    const __uint128_t abs_dx = dx < 0
        ? static_cast<__uint128_t>(-dx)
        : static_cast<__uint128_t>(dx);
    const __uint128_t abs_dy = dy < 0
        ? static_cast<__uint128_t>(-dy)
        : static_cast<__uint128_t>(dy);
    return abs_dx * abs_dx + abs_dy * abs_dy;
}

}  // namespace euclidean_mst_detail

template<std::integral Coordinate>
EuclideanMstResult euclidean_mst(
    const std::vector<std::pair<Coordinate, Coordinate>>& points
){
    using GeometryPoint = euclidean_mst_detail::Point<Coordinate>;
    std::vector<GeometryPoint> geometry_points;
    geometry_points.reserve(points.size());
    for(const auto& [x, y]: points) geometry_points.push_back({x, y});

    euclidean_mst_detail::DelaunayTriangulation<Coordinate> triangulation(
        std::move(geometry_points)
    );
    std::vector<std::pair<int, int>> candidates = triangulation.edges();
    std::sort(candidates.begin(), candidates.end(), [&](const auto& left, const auto& right){
        const __uint128_t left_distance = euclidean_mst_detail::squared_distance(
            points[static_cast<std::size_t>(left.first)],
            points[static_cast<std::size_t>(left.second)]
        );
        const __uint128_t right_distance = euclidean_mst_detail::squared_distance(
            points[static_cast<std::size_t>(right.first)],
            points[static_cast<std::size_t>(right.second)]
        );
        if(left_distance != right_distance) return left_distance < right_distance;
        return left < right;
    });

    euclidean_mst_detail::Dsu dsu(points.size());
    EuclideanMstResult result;
    if(!points.empty()) result.edges.reserve(points.size() - 1);
    for(const auto& [first_value, second_value]: candidates){
        const std::size_t first = static_cast<std::size_t>(first_value);
        const std::size_t second = static_cast<std::size_t>(second_value);
        if(!dsu.merge(first, second)) continue;
        result.edges.push_back({
            first,
            second,
            euclidean_mst_detail::squared_distance(points[first], points[second])
        });
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SHAPE_EUCLIDEAN_MST_HPP_INCLUDED
