#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "base.hpp"

struct ClosestPair3DResult{
    std::size_t first;
    std::size_t second;
    long double distance;
};

namespace closest_pair_3d_detail{

inline long double coordinate(const Point3& point, std::size_t axis){
    if(axis == 0) return point.x;
    if(axis == 1) return point.y;
    return point.z;
}

inline bool finite(const Point3& point){
    return std::isfinite(point.x) && std::isfinite(point.y) &&
        std::isfinite(point.z);
}

inline long double point_distance(const Point3& left, const Point3& right){
    return std::hypot(
        left.x - right.x, left.y - right.y, left.z - right.z
    );
}

struct ScaledDistance{
    long double fraction = 0;
    int exponent = 0;
};

inline ScaledDistance axis_distance(long double left, long double right){
    if(left == right) return {};
    int left_exponent = 0;
    int right_exponent = 0;
    std::frexp(left, &left_exponent);
    std::frexp(right, &right_exponent);
    const int scale = std::max(left_exponent, right_exponent);
    const long double scaled = std::abs(
        std::scalbn(left, -scale) - std::scalbn(right, -scale)
    );
    int exponent = 0;
    const long double fraction = std::frexp(scaled, &exponent);
    return {fraction, scale + exponent};
}

inline bool distance_less(
    const ScaledDistance& left,
    const ScaledDistance& right
){
    if(left.fraction == 0) return right.fraction != 0;
    if(right.fraction == 0) return false;
    if(left.exponent != right.exponent){
        return left.exponent < right.exponent;
    }
    return left.fraction < right.fraction;
}

inline bool distance_less_equal(
    const ScaledDistance& left,
    const ScaledDistance& right
){
    return !distance_less(right, left);
}

inline ScaledDistance scaled_point_distance(
    const Point3& left,
    const Point3& right
){
    const std::array<ScaledDistance, 3> component{{
        axis_distance(left.x, right.x),
        axis_distance(left.y, right.y),
        axis_distance(left.z, right.z),
    }};
    int scale = std::numeric_limits<int>::min();
    for(const auto& value: component){
        if(value.fraction != 0) scale = std::max(scale, value.exponent);
    }
    if(scale == std::numeric_limits<int>::min()) return {};
    std::array<long double, 3> normalized{};
    for(std::size_t axis = 0; axis < 3; ++axis){
        if(component[axis].fraction != 0){
            normalized[axis] = std::scalbn(
                component[axis].fraction,
                component[axis].exponent - scale
            );
        }
    }
    int exponent = 0;
    const long double fraction = std::frexp(
        std::hypot(normalized[0], normalized[1], normalized[2]),
        &exponent
    );
    return {fraction, scale + exponent};
}

inline long double unscale(const ScaledDistance& distance){
    return std::scalbn(distance.fraction, distance.exponent);
}

struct Candidate{
    std::size_t first = 0;
    std::size_t second = 0;
    ScaledDistance distance{};
};

inline Candidate make_candidate(
    std::size_t first,
    std::size_t second,
    const std::vector<Point3>& points
){
    if(second < first) std::swap(first, second);
    return {
        first, second,
        scaled_point_distance(points[first], points[second]),
    };
}

inline bool candidate_less(const Candidate& left, const Candidate& right){
    if(distance_less(left.distance, right.distance)) return true;
    if(distance_less(right.distance, left.distance)) return false;
    return std::pair{left.first, left.second}
        < std::pair{right.first, right.second};
}

struct Node{
    std::size_t point_index = 0;
    int left = -1;
    int right = -1;
    Point3 minimum{};
    Point3 maximum{};
};

class Tree{
    const std::vector<Point3>& points;
    std::vector<std::size_t> order;
    std::vector<Node> nodes;

    int build(std::size_t begin, std::size_t end){
        if(begin == end) return -1;
        std::array<long double, 3> minimum{{
            std::numeric_limits<long double>::infinity(),
            std::numeric_limits<long double>::infinity(),
            std::numeric_limits<long double>::infinity(),
        }};
        std::array<long double, 3> maximum{{
            -std::numeric_limits<long double>::infinity(),
            -std::numeric_limits<long double>::infinity(),
            -std::numeric_limits<long double>::infinity(),
        }};
        for(std::size_t position = begin; position < end; ++position){
            for(std::size_t axis = 0; axis < 3; ++axis){
                const long double value = coordinate(points[order[position]], axis);
                minimum[axis] = std::min(minimum[axis], value);
                maximum[axis] = std::max(maximum[axis], value);
            }
        }
        std::size_t split_axis = 0;
        for(std::size_t axis = 1; axis < 3; ++axis){
            if(maximum[axis] - minimum[axis] >
               maximum[split_axis] - minimum[split_axis]){
                split_axis = axis;
            }
        }
        const std::size_t middle = begin + (end - begin) / 2;
        std::nth_element(
            order.begin() + static_cast<std::ptrdiff_t>(begin),
            order.begin() + static_cast<std::ptrdiff_t>(middle),
            order.begin() + static_cast<std::ptrdiff_t>(end),
            [&](std::size_t left, std::size_t right){
                const long double left_value = coordinate(points[left], split_axis);
                const long double right_value = coordinate(points[right], split_axis);
                if(left_value != right_value) return left_value < right_value;
                return left < right;
            }
        );
        const int node_index = static_cast<int>(nodes.size());
        nodes.push_back(Node{});
        const int left = build(begin, middle);
        const int right = build(middle + 1, end);
        Node& node = nodes[static_cast<std::size_t>(node_index)];
        node.point_index = order[middle];
        node.left = left;
        node.right = right;
        node.minimum = {
            minimum[0], minimum[1], minimum[2],
        };
        node.maximum = {
            maximum[0], maximum[1], maximum[2],
        };
        return node_index;
    }

    long double box_distance(int node_index, const Point3& point) const{
        if(node_index < 0) return std::numeric_limits<long double>::infinity();
        const Node& node = nodes[static_cast<std::size_t>(node_index)];
        std::array<long double, 3> delta{};
        for(std::size_t axis = 0; axis < 3; ++axis){
            const long double value = coordinate(point, axis);
            const long double low = coordinate(node.minimum, axis);
            const long double high = coordinate(node.maximum, axis);
            if(value < low) delta[axis] = low - value;
            else if(high < value) delta[axis] = value - high;
        }
        return std::hypot(delta[0], delta[1], delta[2]);
    }

    void query(
        int node_index,
        std::size_t query_index,
        long double& best_distance,
        std::size_t& best_index
    ) const{
        if(node_index < 0) return;
        const long double lower_bound = box_distance(
            node_index, points[query_index]
        );
        const long double tolerance = 32 * std::numeric_limits<long double>::epsilon() *
            std::max({1.0L, lower_bound, best_distance});
        if(lower_bound > best_distance + tolerance) return;

        const Node& node = nodes[static_cast<std::size_t>(node_index)];
        if(node.point_index != query_index){
            const long double current_distance = point_distance(
                points[query_index], points[node.point_index]
            );
            if(current_distance < best_distance ||
               (current_distance == best_distance && node.point_index < best_index)){
                best_distance = current_distance;
                best_index = node.point_index;
            }
        }
        int first_child = node.left;
        int second_child = node.right;
        if(box_distance(second_child, points[query_index]) <
           box_distance(first_child, points[query_index])){
            std::swap(first_child, second_child);
        }
        query(first_child, query_index, best_distance, best_index);
        query(second_child, query_index, best_distance, best_index);
    }

public:
    explicit Tree(const std::vector<Point3>& points_): points(points_){
        order.resize(points.size());
        std::iota(order.begin(), order.end(), std::size_t{0});
        nodes.reserve(points.size());
        build(0, points.size());
    }

    std::pair<std::size_t, long double> nearest(std::size_t query_index) const{
        std::size_t best_index = points.size();
        long double best_distance = std::numeric_limits<long double>::infinity();
        query(0, query_index, best_distance, best_index);
        return {best_index, best_distance};
    }
};

class DivideAndConquerSolver{
    static constexpr std::size_t no_band =
        std::numeric_limits<std::size_t>::max();

    struct Band{
        long double first_y = 0;
        std::vector<std::size_t> points_by_z;
    };

    const std::vector<Point3>& points;
    const std::vector<std::size_t>& x_order;
    std::vector<std::size_t> x_rank;
    std::vector<std::size_t> band_of;

    void improve(
        Candidate& best,
        std::size_t first,
        std::size_t second
    ) const{
        const Candidate candidate = make_candidate(first, second, points);
        if(candidate_less(candidate, best)) best = candidate;
    }

    void compare_same_band(
        const std::vector<std::size_t>& band,
        Candidate& best
    ) const{
        for(std::size_t position = 0; position < band.size(); ++position){
            std::size_t previous = position;
            while(previous > 0){
                --previous;
                if(!distance_less_equal(
                    axis_distance(
                        points[band[position]].z,
                        points[band[previous]].z
                    ),
                    best.distance
                )) break;
                improve(best, band[position], band[previous]);
            }
        }
    }

    void compare_adjacent_bands(
        const std::vector<std::size_t>& first_band,
        const std::vector<std::size_t>& second_band,
        Candidate& best
    ) const{
        std::size_t lower = 0;
        for(const std::size_t first: first_band){
            while(lower < second_band.size() &&
                  points[second_band[lower]].z < points[first].z &&
                  !distance_less_equal(
                      axis_distance(points[second_band[lower]].z, points[first].z),
                      best.distance
                  )){
                ++lower;
            }
            for(std::size_t position = lower;
                position < second_band.size(); ++position){
                const std::size_t second = second_band[position];
                if(points[first].z < points[second].z &&
                   !distance_less_equal(
                       axis_distance(points[first].z, points[second].z),
                       best.distance
                   )){
                    break;
                }
                if(distance_less_equal(
                    axis_distance(points[first].y, points[second].y),
                    best.distance
                )){
                    improve(best, first, second);
                }
            }
        }
    }

    Candidate solve_range(
        std::size_t begin,
        std::size_t end,
        const std::vector<std::size_t>& y_order,
        const std::vector<std::size_t>& z_order
    ){
        const std::size_t size = end - begin;
        if(size <= 32){
            Candidate best = make_candidate(y_order[0], y_order[1], points);
            for(std::size_t first = 0; first < size; ++first){
                for(std::size_t second = first + 1; second < size; ++second){
                    improve(best, y_order[first], y_order[second]);
                }
            }
            return best;
        }

        const std::size_t middle = begin + size / 2;
        std::vector<std::size_t> left_y;
        std::vector<std::size_t> right_y;
        std::vector<std::size_t> left_z;
        std::vector<std::size_t> right_z;
        left_y.reserve(middle - begin);
        right_y.reserve(end - middle);
        left_z.reserve(middle - begin);
        right_z.reserve(end - middle);
        for(const std::size_t point: y_order){
            (x_rank[point] < middle ? left_y : right_y).push_back(point);
        }
        for(const std::size_t point: z_order){
            (x_rank[point] < middle ? left_z : right_z).push_back(point);
        }

        Candidate best = solve_range(begin, middle, left_y, left_z);
        const Candidate right_best =
            solve_range(middle, end, right_y, right_z);
        if(candidate_less(right_best, best)) best = right_best;
        const ScaledDistance slab_distance = best.distance;
        const long double dividing_x = points[x_order[middle]].x;

        std::vector<std::size_t> slab_y;
        slab_y.reserve(size);
        for(const std::size_t point: y_order){
            band_of[point] = no_band;
            if(distance_less_equal(
                axis_distance(points[point].x, dividing_x), slab_distance
            )){
                slab_y.push_back(point);
            }
        }

        std::vector<Band> bands;
        for(const std::size_t point: slab_y){
            if(bands.empty() || !distance_less_equal(
                axis_distance(points[point].y, bands.back().first_y),
                slab_distance
            )){
                bands.push_back({points[point].y, {}});
            }
            band_of[point] = bands.size() - 1;
        }
        for(const std::size_t point: z_order){
            if(band_of[point] != no_band){
                bands[band_of[point]].points_by_z.push_back(point);
            }
        }

        for(const Band& band: bands){
            compare_same_band(band.points_by_z, best);
        }
        for(std::size_t band = 1; band < bands.size(); ++band){
            compare_adjacent_bands(
                bands[band - 1].points_by_z,
                bands[band].points_by_z,
                best
            );
        }
        return best;
    }

public:
    DivideAndConquerSolver(
        const std::vector<Point3>& points_,
        const std::vector<std::size_t>& x_order_
    ): points(points_), x_order(x_order_),
       x_rank(points.size()), band_of(points.size(), no_band){
        for(std::size_t rank = 0; rank < x_order.size(); ++rank){
            x_rank[x_order[rank]] = rank;
        }
    }

    Candidate solve(){
        std::vector<std::size_t> y_order = x_order;
        std::vector<std::size_t> z_order = x_order;
        std::sort(y_order.begin(), y_order.end(), [&](std::size_t left, std::size_t right){
            if(points[left].y != points[right].y) return points[left].y < points[right].y;
            if(points[left].z != points[right].z) return points[left].z < points[right].z;
            if(points[left].x != points[right].x) return points[left].x < points[right].x;
            return left < right;
        });
        std::sort(z_order.begin(), z_order.end(), [&](std::size_t left, std::size_t right){
            if(points[left].z != points[right].z) return points[left].z < points[right].z;
            if(points[left].y != points[right].y) return points[left].y < points[right].y;
            if(points[left].x != points[right].x) return points[left].x < points[right].x;
            return left < right;
        });
        return solve_range(0, points.size(), y_order, z_order);
    }
};

} // namespace closest_pair_3d_detail

inline std::optional<ClosestPair3DResult> closest_pair_3d(
    const std::vector<Point3>& points
){
    if(points.size() < 2) return std::nullopt;
    for(const Point3& point: points){
        if(!closest_pair_3d_detail::finite(point))[[unlikely]]{
            throw std::invalid_argument("closest_pair_3d requires finite points");
        }
    }

    std::vector<std::size_t> sorted(points.size());
    std::iota(sorted.begin(), sorted.end(), std::size_t{0});
    std::sort(sorted.begin(), sorted.end(), [&](std::size_t left, std::size_t right){
        if(points[left].x != points[right].x) return points[left].x < points[right].x;
        if(points[left].y != points[right].y) return points[left].y < points[right].y;
        if(points[left].z != points[right].z) return points[left].z < points[right].z;
        return left < right;
    });
    std::optional<std::pair<std::size_t, std::size_t>> duplicate;
    for(std::size_t index = 1; index < sorted.size(); ++index){
        const std::size_t left = sorted[index - 1];
        const std::size_t right = sorted[index];
        if(points[left].x == points[right].x &&
           points[left].y == points[right].y &&
           points[left].z == points[right].z){
            const std::pair<std::size_t, std::size_t> candidate = std::minmax(left, right);
            if(!duplicate || candidate < *duplicate) duplicate = candidate;
        }
    }
    if(duplicate){
        return ClosestPair3DResult{duplicate->first, duplicate->second, 0};
    }

    closest_pair_3d_detail::DivideAndConquerSolver solver(points, sorted);
    const auto result = solver.solve();
    return ClosestPair3DResult{
        result.first,
        result.second,
        closest_pair_3d_detail::unscale(result.distance),
    };
}
