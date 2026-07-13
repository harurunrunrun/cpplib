#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <utility>
#include <vector>

#include "geometry.hpp"


inline std::vector<int> enumerate_points_on_polygon_boundary(
    const std::vector<Point>& polygon,
    const std::vector<Point>& points,
    bool unique = true
){
    int n = static_cast<int>(polygon.size());
    int m = static_cast<int>(points.size());
    std::vector<int> result;
    std::vector<char> used(static_cast<std::size_t>(m), 0);
    for(int i = 0; i < n; i++){
        Point a = polygon[static_cast<std::size_t>(i)];
        Point b = polygon[static_cast<std::size_t>((i + 1) % n)];
        Point base = b - a;
        long double base_norm = norm(base);
        std::vector<std::pair<long double, int>> current;
        for(int j = 0; j < m; j++){
            if(unique && used[static_cast<std::size_t>(j)]) continue;
            if(!on_segment({a, b}, points[static_cast<std::size_t>(j)])) continue;
            long double parameter = geometry_sign(base_norm) == 0
                                        ? 0.0L
                                        : dot(points[static_cast<std::size_t>(j)] - a, base) / base_norm;
            current.push_back({parameter, j});
        }
        std::sort(current.begin(), current.end(), [](const auto& lhs, const auto& rhs){
            if(lhs.first != rhs.first) return lhs.first < rhs.first;
            return lhs.second < rhs.second;
        });
        for(auto [parameter, index]: current){
            (void)parameter;
            result.push_back(index);
            if(unique) used[static_cast<std::size_t>(index)] = 1;
        }
    }
    return result;
}

class PolygonBoundaryPointQuery{
    struct EdgeInterval{
        long double left;
        long double right;
        int edge;
    };

    struct PointEdge{
        long double parameter;
        int edge;
    };

    struct SideData{
        std::vector<EdgeInterval> intervals;
        std::vector<PointEdge> point_edges;
    };

    struct SideCandidates{
        std::array<int, 4> side = {-1, -1, -1, -1};
        int size = 0;
        bool overflow = false;

        void add(int value){
            for(int i = 0; i < size; ++i){
                if(side[static_cast<std::size_t>(i)] == value) return;
            }
            if(size == static_cast<int>(side.size())){
                overflow = true;
                return;
            }
            side[static_cast<std::size_t>(size++)] = value;
        }
    };

    std::vector<Point> polygon_;
    std::vector<Point> convex_vertices_;
    std::vector<SideData> sides_;
    bool convex_fast_path_ = false;

    long double side_parameter(int side, const Point& point) const;
    long double side_parameter_tolerance(int side) const;
    SideCandidates locate_boundary_sides(const Point& point) const;
    bool build_convex_fast_path();
    void append_side_candidates(
        int side,
        const Point& point,
        std::vector<int>& candidate_edges
    ) const;
    std::vector<int> enumerate_convex(
        const std::vector<Point>& points,
        bool unique
    ) const;

public:
    explicit PolygonBoundaryPointQuery(std::vector<Point> polygon);

    bool uses_convex_fast_path() const{ return convex_fast_path_; }
    const std::vector<Point>& polygon() const{ return polygon_; }

    std::vector<int> enumerate(
        const std::vector<Point>& points,
        bool unique = true
    ) const;
};

inline long double PolygonBoundaryPointQuery::side_parameter(
    int side,
    const Point& point
) const{
    const Point& first = convex_vertices_[static_cast<std::size_t>(side)];
    const Point& second = convex_vertices_[
        static_cast<std::size_t>((side + 1) % static_cast<int>(convex_vertices_.size()))
    ];
    const Point base = second - first;
    return dot(point - first, base) / norm(base);
}

inline long double PolygonBoundaryPointQuery::side_parameter_tolerance(int side) const{
    const Point& first = convex_vertices_[static_cast<std::size_t>(side)];
    const Point& second = convex_vertices_[
        static_cast<std::size_t>((side + 1) % static_cast<int>(convex_vertices_.size()))
    ];
    return std::sqrt(GEOMETRY_EPS / norm(second - first)) + GEOMETRY_EPS;
}

inline PolygonBoundaryPointQuery::SideCandidates
PolygonBoundaryPointQuery::locate_boundary_sides(const Point& point) const{
    SideCandidates result;
    const int n = static_cast<int>(convex_vertices_.size());
    if(n < 3) return result;
    const auto add_if_on_side = [&](int side){
        side = (side + n) % n;
        if(on_segment(
            {
                convex_vertices_[static_cast<std::size_t>(side)],
                convex_vertices_[static_cast<std::size_t>((side + 1) % n)]
            },
            point
        )) result.add(side);
    };

    const Point& origin = convex_vertices_[0];
    const int first_side = geometry_sign(cross(convex_vertices_[1] - origin, point - origin));
    const int last_side = geometry_sign(cross(
        convex_vertices_[static_cast<std::size_t>(n - 1)] - origin,
        point - origin
    ));
    if(first_side < 0 || last_side > 0) return result;
    if(first_side == 0){
        if(!on_segment({origin, convex_vertices_[1]}, point)) return result;
        for(int side: {n - 1, 0, 1}) add_if_on_side(side);
        return result;
    }
    if(last_side == 0){
        if(!on_segment({origin, convex_vertices_[static_cast<std::size_t>(n - 1)]}, point)){
            return result;
        }
        for(int side: {n - 2, n - 1, 0}) add_if_on_side(side);
        return result;
    }

    int low = 1;
    int high = n - 1;
    while(high - low > 1){
        const int middle = low + (high - low) / 2;
        if(geometry_sign(cross(
            convex_vertices_[static_cast<std::size_t>(middle)] - origin,
            point - origin
        )) >= 0){
            low = middle;
        }else{
            high = middle;
        }
    }
    const Segment side{
        convex_vertices_[static_cast<std::size_t>(low)],
        convex_vertices_[static_cast<std::size_t>(low + 1)]
    };
    if(!on_segment(side, point)) return result;
    for(int candidate: {low - 1, low, low + 1}) add_if_on_side(candidate);
    return result;
}

inline bool PolygonBoundaryPointQuery::build_convex_fast_path(){
    if(polygon_.size() < 3 || !is_convex(polygon_)) return false;

    ConvexPolygonQuery convex(polygon_);
    convex_vertices_ = convex.vertices();
    const int side_count = static_cast<int>(convex_vertices_.size());
    if(side_count < 3) return false;
    sides_.assign(static_cast<std::size_t>(side_count), {});

    const int edge_count = static_cast<int>(polygon_.size());
    for(int edge = 0; edge < edge_count; ++edge){
        const Point& first = polygon_[static_cast<std::size_t>(edge)];
        const Point& second = polygon_[static_cast<std::size_t>((edge + 1) % edge_count)];
        const long double edge_norm = norm(second - first);
        const bool point_edge = geometry_sign(edge_norm) == 0;
        if(point_edge && !(first == second)) return false;

        const Point middle = point_edge ? first : (first + second) / 2.0L;
        const SideCandidates candidates = locate_boundary_sides(middle);
        int selected_side = -1;
        if(candidates.overflow) return false;
        for(int i = 0; i < candidates.size; ++i){
            const int side = candidates.side[static_cast<std::size_t>(i)];
            const Segment support{
                convex_vertices_[static_cast<std::size_t>(side)],
                convex_vertices_[static_cast<std::size_t>((side + 1) % side_count)]
            };
            if(on_segment(support, first) && on_segment(support, second)){
                selected_side = side;
                break;
            }
        }
        if(selected_side == -1) return false;

        if(point_edge){
            sides_[static_cast<std::size_t>(selected_side)].point_edges.push_back({
                side_parameter(selected_side, first),
                edge
            });
        }else{
            long double left = side_parameter(selected_side, first);
            long double right = side_parameter(selected_side, second);
            if(left > right) std::swap(left, right);
            sides_[static_cast<std::size_t>(selected_side)].intervals.push_back({
                left,
                right,
                edge
            });
        }
    }

    for(SideData& side: sides_){
        std::sort(side.intervals.begin(), side.intervals.end(), [](const auto& lhs, const auto& rhs){
            if(lhs.left != rhs.left) return lhs.left < rhs.left;
            if(lhs.right != rhs.right) return lhs.right < rhs.right;
            return lhs.edge < rhs.edge;
        });
        std::sort(side.point_edges.begin(), side.point_edges.end(), [](const auto& lhs, const auto& rhs){
            if(lhs.parameter != rhs.parameter) return lhs.parameter < rhs.parameter;
            return lhs.edge < rhs.edge;
        });
        if(side.intervals.empty()) return false;
        if(geometry_sign(side.intervals.front().left) != 0) return false;
        if(geometry_sign(side.intervals.back().right - 1.0L) != 0) return false;
        for(std::size_t i = 1; i < side.intervals.size(); ++i){
            if(geometry_sign(side.intervals[i].left - side.intervals[i - 1].right) != 0){
                return false;
            }
            if(side.intervals[i].right < side.intervals[i - 1].right) return false;
        }
    }
    return true;
}

inline void PolygonBoundaryPointQuery::append_side_candidates(
    int side,
    const Point& point,
    std::vector<int>& candidate_edges
) const{
    const SideData& data = sides_[static_cast<std::size_t>(side)];
    const long double parameter = side_parameter(side, point);
    const long double tolerance = side_parameter_tolerance(side);

    auto interval = std::lower_bound(
        data.intervals.begin(),
        data.intervals.end(),
        parameter - tolerance,
        [](const EdgeInterval& value, long double bound){
            return value.right < bound;
        }
    );
    while(interval != data.intervals.end() && interval->left <= parameter + tolerance){
        candidate_edges.push_back(interval->edge);
        ++interval;
    }

    auto point_edge = std::lower_bound(
        data.point_edges.begin(),
        data.point_edges.end(),
        parameter - tolerance,
        [](const PointEdge& value, long double bound){
            return value.parameter < bound;
        }
    );
    while(point_edge != data.point_edges.end() &&
          point_edge->parameter <= parameter + tolerance){
        candidate_edges.push_back(point_edge->edge);
        ++point_edge;
    }
}

inline std::vector<int> PolygonBoundaryPointQuery::enumerate_convex(
    const std::vector<Point>& points,
    bool unique
) const{
    const int edge_count = static_cast<int>(polygon_.size());
    std::vector<std::vector<std::pair<long double, int>>> points_on_edge(
        static_cast<std::size_t>(edge_count)
    );
    std::vector<int> candidate_edges;

    for(int point_index = 0; point_index < static_cast<int>(points.size()); ++point_index){
        const Point& point = points[static_cast<std::size_t>(point_index)];
        const SideCandidates candidate_sides = locate_boundary_sides(point);
        candidate_edges.clear();
        for(int i = 0; i < candidate_sides.size; ++i){
            append_side_candidates(
                candidate_sides.side[static_cast<std::size_t>(i)],
                point,
                candidate_edges
            );
        }
        std::sort(candidate_edges.begin(), candidate_edges.end());
        candidate_edges.erase(
            std::unique(candidate_edges.begin(), candidate_edges.end()),
            candidate_edges.end()
        );
        candidate_edges.erase(
            std::remove_if(
                candidate_edges.begin(),
                candidate_edges.end(),
                [&](int edge){
                    return !on_segment(
                        {
                            polygon_[static_cast<std::size_t>(edge)],
                            polygon_[static_cast<std::size_t>((edge + 1) % edge_count)]
                        },
                        point
                    );
                }
            ),
            candidate_edges.end()
        );
        if(unique && candidate_edges.size() > 1){
            candidate_edges.erase(candidate_edges.begin() + 1, candidate_edges.end());
        }

        for(int edge: candidate_edges){
            const Point& first = polygon_[static_cast<std::size_t>(edge)];
            const Point base =
                polygon_[static_cast<std::size_t>((edge + 1) % edge_count)] - first;
            const long double base_norm = norm(base);
            const long double parameter = geometry_sign(base_norm) == 0
                ? 0.0L
                : dot(point - first, base) / base_norm;
            points_on_edge[static_cast<std::size_t>(edge)].push_back({
                parameter,
                point_index
            });
        }
    }

    std::vector<int> result;
    for(auto& current: points_on_edge){
        std::sort(current.begin(), current.end(), [](const auto& lhs, const auto& rhs){
            if(lhs.first != rhs.first) return lhs.first < rhs.first;
            return lhs.second < rhs.second;
        });
        for(const auto& [parameter, point_index]: current){
            (void)parameter;
            result.push_back(point_index);
        }
    }
    return result;
}

inline PolygonBoundaryPointQuery::PolygonBoundaryPointQuery(std::vector<Point> polygon)
    : polygon_(std::move(polygon)){
    convex_fast_path_ = build_convex_fast_path();
    if(!convex_fast_path_){
        convex_vertices_.clear();
        sides_.clear();
    }
}

inline std::vector<int> PolygonBoundaryPointQuery::enumerate(
    const std::vector<Point>& points,
    bool unique
) const{
    if(!convex_fast_path_){
        return enumerate_points_on_polygon_boundary(polygon_, points, unique);
    }
    return enumerate_convex(points, unique);
}
