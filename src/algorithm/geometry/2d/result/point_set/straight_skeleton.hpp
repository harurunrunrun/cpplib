#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_POINT_SET_STRAIGHT_SKELETON_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_POINT_SET_STRAIGHT_SKELETON_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <optional>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../detail/numeric/plus_geometry_detail.hpp"
#include "../../type/definition/straight_skeleton_result.hpp"

namespace straight_skeleton_detail{

using Time = long double;

struct SourceEdge{
    Point direction;
    Point normal;
    Time constant;
};

struct AffinePoint{
    Point base;
    Point velocity;
};

struct WavefrontNode{
    std::size_t left_edge;
    std::size_t right_edge;
    std::size_t trace_vertex;
    std::optional<AffinePoint> collinear_affine = std::nullopt;
};

struct SplitCandidate{
    Time time;
    std::size_t cycle;
    std::size_t vertex;
    std::size_t edge_start;
};

inline Time event_tolerance(Time first, Time second = 0.0L){
    return 4096.0L * std::numeric_limits<Time>::epsilon()
        * std::max({1.0L, std::abs(first), std::abs(second)});
}

inline bool same_event_time(Time first, Time second){
    return std::abs(first - second) <= event_tolerance(first, second);
}

inline int skeleton_orientation(
    const Point& first,
    const Point& second,
    const Point& third
){
    return plus_geometry_detail::orientation(first, second, third);
}

inline bool skeleton_on_segment(
    const Point& first,
    const Point& second,
    const Point& point
){
    if(skeleton_orientation(first, second, point) != 0) return false;
    return plus_geometry_detail::sign(
        dot(point - first, point - second),
        plus_geometry_detail::squared_distance(first, second)
    ) <= 0;
}

inline bool skeleton_segments_intersect(
    const Point& first_a,
    const Point& first_b,
    const Point& second_a,
    const Point& second_b
){
    const int first_side_a =
        skeleton_orientation(first_a, first_b, second_a);
    const int first_side_b =
        skeleton_orientation(first_a, first_b, second_b);
    const int second_side_a =
        skeleton_orientation(second_a, second_b, first_a);
    const int second_side_b =
        skeleton_orientation(second_a, second_b, first_b);
    if(first_side_a == 0
        && skeleton_on_segment(first_a, first_b, second_a)) return true;
    if(first_side_b == 0
        && skeleton_on_segment(first_a, first_b, second_b)) return true;
    if(second_side_a == 0
        && skeleton_on_segment(second_a, second_b, first_a)) return true;
    if(second_side_b == 0
        && skeleton_on_segment(second_a, second_b, first_b)) return true;
    return first_side_a * first_side_b < 0
        && second_side_a * second_side_b < 0;
}

class StraightSkeletonBuilder{
private:
    std::vector<Point> input_polygon_;
    std::vector<Point> polygon_;
    std::vector<std::size_t> contour_index_;
    std::vector<SourceEdge> source_edges_;
    std::vector<std::vector<WavefrontNode>> cycles_;
    StraightSkeletonResult result_;
    std::set<std::pair<std::size_t, std::size_t>> skeleton_edges_;
    std::size_t contour_vertex_count_ = 0;

    static std::vector<Point> clean_polygon(std::vector<Point> polygon){
        if(polygon.size() >= 2
            && plus_geometry_detail::close(polygon.front(), polygon.back())){
            polygon.pop_back();
        }
        if(polygon.size() < 3){
            throw std::invalid_argument(
                "a straight skeleton requires at least three vertices"
            );
        }
        for(const Point& point: polygon){
            plus_geometry_detail::validate_finite(point);
        }
        for(std::size_t index = 0; index < polygon.size(); ++index){
            if(plus_geometry_detail::close(
                polygon[index], polygon[(index + 1) % polygon.size()]
            )){
                throw std::invalid_argument(
                    "a straight-skeleton contour has a zero-length edge"
                );
            }
        }
        return polygon;
    }

    void validate_and_orient(){
        const std::size_t count = polygon_.size();
        for(std::size_t index = 0; index < count; ++index){
            if(skeleton_orientation(
                polygon_[(index + count - 1) % count],
                polygon_[index],
                polygon_[(index + 1) % count]
            ) == 0){
                throw std::invalid_argument(
                    "adjacent straight-skeleton contour edges must not be collinear"
                );
            }
        }
        for(std::size_t first = 0; first < count; ++first){
            const std::size_t first_next = (first + 1) % count;
            for(std::size_t second = first + 1; second < count; ++second){
                const std::size_t second_next = (second + 1) % count;
                if(first_next == second || second_next == first) continue;
                if(skeleton_segments_intersect(
                    polygon_[first], polygon_[first_next],
                    polygon_[second], polygon_[second_next]
                )){
                    throw std::invalid_argument(
                        "a straight-skeleton contour must be a simple polygon"
                    );
                }
            }
        }
        const Time area_twice =
            plus_geometry_detail::signed_area_twice(polygon_);
        if(plus_geometry_detail::sign(area_twice) == 0){
            throw std::invalid_argument(
                "a straight-skeleton contour must have nonzero area"
            );
        }
        contour_index_.resize(count);
        if(area_twice > 0.0L){
            for(std::size_t index = 0; index < count; ++index){
                contour_index_[index] = index;
            }
        }else{
            std::reverse(polygon_.begin(), polygon_.end());
            for(std::size_t index = 0; index < count; ++index){
                contour_index_[index] = count - 1 - index;
            }
        }
    }

    void initialize_wavefront(){
        const std::size_t count = polygon_.size();
        source_edges_.reserve(count);
        for(std::size_t index = 0; index < count; ++index){
            const Point direction =
                polygon_[(index + 1) % count] - polygon_[index];
            const Time length = std::sqrt(dot(direction, direction));
            const Point normal{-direction.y / length, direction.x / length};
            source_edges_.push_back({
                direction, normal, dot(normal, polygon_[index]),
            });
        }
        std::vector<WavefrontNode> initial_cycle;
        initial_cycle.reserve(count);
        for(std::size_t index = 0; index < count; ++index){
            initial_cycle.push_back({
                (index + count - 1) % count,
                index,
                contour_index_[index],
            });
        }
        cycles_.push_back(std::move(initial_cycle));
    }

    std::optional<AffinePoint> affine_point(
        const WavefrontNode& node
    ) const{
        if(node.collinear_affine.has_value()){
            return node.collinear_affine;
        }
        const SourceEdge& left = source_edges_[node.left_edge];
        const SourceEdge& right = source_edges_[node.right_edge];
        const Time determinant = cross(left.normal, right.normal);
        const Time magnitude =
            std::abs(left.normal.x * right.normal.y)
            + std::abs(left.normal.y * right.normal.x);
        if(plus_geometry_detail::sign(determinant, magnitude) == 0){
            return std::nullopt;
        }
        return AffinePoint{
            {
                (left.constant * right.normal.y
                    - left.normal.y * right.constant) / determinant,
                (left.normal.x * right.constant
                    - left.constant * right.normal.x) / determinant,
            },
            {
                (right.normal.y - left.normal.y) / determinant,
                (left.normal.x - right.normal.x) / determinant,
            },
        };
    }

    Point point_at(const WavefrontNode& node, Time time) const{
        const std::optional<AffinePoint> affine = affine_point(node);
        if(affine.has_value()){
            return affine->base + affine->velocity * time;
        }
        if(node.trace_vertex < result_.vertices.size()
            && same_event_time(result_.times[node.trace_vertex], time)){
            return result_.vertices[node.trace_vertex];
        }
        throw std::runtime_error(
            "parallel wavefront edges do not determine a finite vertex"
        );
    }

    bool is_reflex(const WavefrontNode& node) const{
        const Point& left = source_edges_[node.left_edge].direction;
        const Point& right = source_edges_[node.right_edge].direction;
        return plus_geometry_detail::sign(
            cross(left, right),
            std::abs(left.x * right.y) + std::abs(left.y * right.x)
        ) < 0;
    }

    std::optional<Time> edge_meeting_time(
        const WavefrontNode& first,
        const WavefrontNode& second
    ) const{
        const std::optional<AffinePoint> first_affine = affine_point(first);
        const std::optional<AffinePoint> second_affine = affine_point(second);
        if(!first_affine.has_value() || !second_affine.has_value()){
            return std::nullopt;
        }
        if(first.right_edge != second.left_edge){
            throw std::logic_error(
                "adjacent wavefront vertices do not share an edge"
            );
        }
        const Point& direction =
            source_edges_[first.right_edge].direction;
        const Time initial_length = dot(
            second_affine->base - first_affine->base, direction
        );
        const Time length_velocity = dot(
            second_affine->velocity - first_affine->velocity,
            direction
        );
        if(plus_geometry_detail::sign(
            length_velocity,
            std::abs(length_velocity) + std::abs(initial_length)
        ) == 0){
            return std::nullopt;
        }
        const Time time = -initial_length / length_velocity;
        if(!std::isfinite(time) || time < 0.0L) return std::nullopt;
        return time;
    }
    std::optional<Time> split_time(
        const std::vector<WavefrontNode>& cycle,
        std::size_t vertex,
        std::size_t edge_start
    ) const{
        const std::size_t count = cycle.size();
        const WavefrontNode& node = cycle[vertex];
        if(!is_reflex(node)) return std::nullopt;
        const std::size_t target_edge = cycle[edge_start].right_edge;
        if(target_edge == node.left_edge || target_edge == node.right_edge){
            return std::nullopt;
        }
        const std::optional<AffinePoint> affine = affine_point(node);
        if(!affine.has_value()) return std::nullopt;
        const SourceEdge& target = source_edges_[target_edge];
        const Time denominator = dot(target.normal, affine->velocity) - 1.0L;
        if(plus_geometry_detail::sign(
            denominator, std::abs(dot(target.normal, affine->velocity)) + 1.0L
        ) >= 0){
            return std::nullopt;
        }
        const Time time = (target.constant
            - dot(target.normal, affine->base)) / denominator;
        if(!std::isfinite(time) || time < 0.0L) return std::nullopt;
        const Point collision = affine->base + affine->velocity * time;
        if(!affine_point(cycle[edge_start]).has_value()
            || !affine_point(cycle[(edge_start + 1) % count]).has_value()){
            return std::nullopt;
        }
        const Point first = point_at(cycle[edge_start], time);
        const Point second = point_at(cycle[(edge_start + 1) % count], time);
        if(!skeleton_on_segment(first, second, collision)){
            return std::nullopt;
        }
        return time;
    }

    std::optional<Time> next_event_time(Time current_time) const{
        std::optional<Time> answer;
        const auto consider = [&](Time candidate){
            if(candidate <= current_time
                    + event_tolerance(candidate, current_time)) return;
            if(!answer.has_value() || candidate < *answer){
                answer = candidate;
            }
        };
        for(const std::vector<WavefrontNode>& cycle: cycles_){
            const std::size_t count = cycle.size();
            for(std::size_t index = 0; index < count; ++index){
                const std::optional<Time> event = edge_meeting_time(
                    cycle[index], cycle[(index + 1) % count]
                );
                if(event.has_value()) consider(*event);
            }
            for(std::size_t vertex = 0; vertex < count; ++vertex){
                for(std::size_t edge = 0; edge < count; ++edge){
                    if(edge == vertex || (edge + 1) % count == vertex){
                        continue;
                    }
                    const std::optional<Time> event =
                        split_time(cycle, vertex, edge);
                    if(event.has_value()) consider(*event);
                }
            }
        }
        return answer;
    }

    std::size_t event_vertex(
        const Point& point,
        Time time,
        StraightSkeletonVertexKind kind
    ){
        if(!std::isfinite(point.x) || !std::isfinite(point.y)
            || !std::isfinite(time) || time < 0.0L){
            throw std::runtime_error(
                "straight-skeleton propagation produced a nonfinite event"
            );
        }
        for(std::size_t index = contour_vertex_count_;
            index < result_.vertices.size(); ++index){
            if(same_event_time(result_.times[index], time)
                && plus_geometry_detail::close(result_.vertices[index], point)){
                if(kind != StraightSkeletonVertexKind::peak_event
                    && result_.kinds[index] != kind
                    && result_.kinds[index]
                        != StraightSkeletonVertexKind::simultaneous_event){
                    result_.kinds[index] =
                        StraightSkeletonVertexKind::simultaneous_event;
                }
                return index;
            }
        }
        const std::size_t index = result_.vertices.size();
        result_.vertices.push_back(point);
        result_.times.push_back(time);
        result_.kinds.push_back(kind);
        return index;
    }

    void add_skeleton_edge(std::size_t first, std::size_t second){
        if(first == second) return;
        if(first > second) std::swap(first, second);
        if(skeleton_edges_.insert({first, second}).second){
            result_.edges.push_back({first, second});
        }
    }

    void terminate_cycle(
        const std::vector<WavefrontNode>& cycle,
        Time time
    ){
        std::vector<std::size_t> endpoints;
        endpoints.reserve(cycle.size());
        for(const WavefrontNode& node: cycle){
            const std::size_t endpoint = event_vertex(
                point_at(node, time), time,
                StraightSkeletonVertexKind::peak_event
            );
            add_skeleton_edge(node.trace_vertex, endpoint);
            endpoints.push_back(endpoint);
        }
        if(endpoints.size() == 2){
            add_skeleton_edge(endpoints[0], endpoints[1]);
        }
    }

    int cycle_area_sign(
        const std::vector<WavefrontNode>& cycle,
        Time time
    ) const{
        Time area_twice = 0.0L;
        Time magnitude = 0.0L;
        for(std::size_t index = 0; index < cycle.size(); ++index){
            const Point first = point_at(cycle[index], time);
            const Point second = point_at(
                cycle[(index + 1) % cycle.size()], time
            );
            const Time contribution = cross(first, second);
            area_twice += contribution;
            magnitude += std::abs(contribution);
        }
        return plus_geometry_detail::sign(area_twice, magnitude);
    }

    bool has_clockwise_cycle(Time time) const{
        for(const std::vector<WavefrontNode>& cycle: cycles_){
            if(cycle_area_sign(cycle, time) < 0) return true;
        }
        return false;
    }

    bool discard_clockwise_cycles(Time time){
        bool changed = false;
        std::vector<std::vector<WavefrontNode>> retained;
        retained.reserve(cycles_.size());
        for(std::vector<WavefrontNode> cycle: cycles_){
            if(cycle_area_sign(cycle, time) >= 0){
                retained.push_back(std::move(cycle));
                continue;
            }

            std::optional<std::size_t> anchor;
            for(const WavefrontNode& node: cycle){
                if(node.trace_vertex < result_.times.size()
                    && same_event_time(
                        result_.times[node.trace_vertex], time
                    )){
                    anchor = node.trace_vertex;
                    break;
                }
            }
            if(!anchor.has_value()){
                throw std::runtime_error(
                    "a reversed wavefront component has no current event"
                );
            }
            for(const WavefrontNode& node: cycle){
                add_skeleton_edge(node.trace_vertex, *anchor);
            }
            changed = true;
        }
        cycles_ = std::move(retained);
        return changed;
    }

    bool resolve_self_intersections(Time time){
        bool changed = false;
        while(true){
            bool found = false;
            for(std::size_t cycle_index = 0;
                cycle_index < cycles_.size() && !found; ++cycle_index){
                const std::vector<WavefrontNode> cycle =
                    cycles_[cycle_index];
                const std::size_t count = cycle.size();
                std::vector<Point> positions(count);
                for(std::size_t index = 0; index < count; ++index){
                    positions[index] = point_at(cycle[index], time);
                }
                for(std::size_t first = 0; first < count && !found; ++first){
                    const std::size_t first_next = (first + 1) % count;
                    for(std::size_t second = first + 1;
                        second < count; ++second){
                        const std::size_t second_next = (second + 1) % count;
                        if(first_next == second || second_next == first){
                            continue;
                        }
                        const int first_side_a = skeleton_orientation(
                            positions[first], positions[first_next],
                            positions[second]
                        );
                        const int first_side_b = skeleton_orientation(
                            positions[first], positions[first_next],
                            positions[second_next]
                        );
                        const int second_side_a = skeleton_orientation(
                            positions[second], positions[second_next],
                            positions[first]
                        );
                        const int second_side_b = skeleton_orientation(
                            positions[second], positions[second_next],
                            positions[first_next]
                        );
                        if(first_side_a * first_side_b >= 0
                            || second_side_a * second_side_b >= 0){
                            continue;
                        }

                        const std::size_t first_edge =
                            cycle[first].right_edge;
                        const std::size_t second_edge =
                            cycle[second].right_edge;
                        const std::optional<AffinePoint> crossing_affine =
                            affine_point({second_edge, first_edge, 0});
                        if(!crossing_affine.has_value()) continue;
                        const Point crossing = crossing_affine->base
                            + crossing_affine->velocity * time;
                        const std::size_t event = event_vertex(
                            crossing, time,
                            StraightSkeletonVertexKind::simultaneous_event
                        );

                        std::vector<WavefrontNode> first_cycle{
                            {second_edge, first_edge, event},
                        };
                        for(std::size_t index = first_next;;
                            index = (index + 1) % count){
                            first_cycle.push_back(cycle[index]);
                            if(index == second) break;
                        }
                        std::vector<WavefrontNode> second_cycle{
                            {first_edge, second_edge, event},
                        };
                        for(std::size_t index = second_next;;
                            index = (index + 1) % count){
                            second_cycle.push_back(cycle[index]);
                            if(index == first) break;
                        }

                        cycles_.erase(cycles_.begin()
                            + static_cast<std::ptrdiff_t>(cycle_index));
                        const auto retain_or_terminate = [&](
                            std::vector<WavefrontNode> component
                        ){
                            if(component.size() > 2
                                && cycle_area_sign(component, time) > 0){
                                cycles_.push_back(std::move(component));
                                return;
                            }
                            for(const WavefrontNode& node: component){
                                add_skeleton_edge(node.trace_vertex, event);
                            }
                        };
                        retain_or_terminate(std::move(first_cycle));
                        retain_or_terminate(std::move(second_cycle));
                        found = true;
                        changed = true;
                        break;
                    }
                }
            }
            if(!found) break;
        }
        return changed;
    }

    bool collapse_edges(Time time){
        bool changed = false;
        std::vector<std::vector<WavefrontNode>> next_cycles;
        next_cycles.reserve(cycles_.size());
        for(const std::vector<WavefrontNode>& cycle: cycles_){
            const std::size_t count = cycle.size();
            std::vector<Point> positions(count);
            std::vector<bool> collapsed(count, false);
            std::size_t collapsed_count = 0;
            for(std::size_t index = 0; index < count; ++index){
                positions[index] = point_at(cycle[index], time);
            }
            for(std::size_t index = 0; index < count; ++index){
                const std::optional<Time> event = edge_meeting_time(
                    cycle[index], cycle[(index + 1) % count]
                );
                collapsed[index] = plus_geometry_detail::close(
                    positions[index], positions[(index + 1) % count]
                ) || (event.has_value() && same_event_time(*event, time));
                collapsed_count += static_cast<std::size_t>(collapsed[index]);
            }
            if(collapsed_count == 0){
                next_cycles.push_back(cycle);
                continue;
            }
            changed = true;
            if(collapsed_count == count){
                Point event_point{};
                for(const Point& point: positions) event_point += point;
                event_point /= static_cast<Time>(count);
                const std::size_t event = event_vertex(
                    event_point, time,
                    StraightSkeletonVertexKind::edge_event
                );
                for(const WavefrontNode& node: cycle){
                    add_skeleton_edge(node.trace_vertex, event);
                }
                continue;
            }

            std::size_t start = 0;
            while(collapsed[(start + count - 1) % count]) ++start;
            std::vector<WavefrontNode> reduced;
            std::size_t current = start;
            do{
                const std::size_t first = current;
                Point event_point = positions[current];
                std::size_t group_size = 1;
                while(collapsed[current]){
                    current = (current + 1) % count;
                    event_point += positions[current];
                    ++group_size;
                }
                if(group_size == 1){
                    reduced.push_back(cycle[first]);
                }else{
                    event_point /= static_cast<Time>(group_size);
                    const std::size_t event = event_vertex(
                        event_point, time,
                        StraightSkeletonVertexKind::edge_event
                    );
                    std::size_t member = first;
                    while(true){
                        add_skeleton_edge(cycle[member].trace_vertex, event);
                        if(member == current) break;
                        member = (member + 1) % count;
                    }
                    reduced.push_back({
                        cycle[first].left_edge,
                        cycle[current].right_edge,
                        event,
                    });
                }
                current = (current + 1) % count;
            }while(current != start);

            if(reduced.size() <= 2){
                terminate_cycle(reduced, time);
            }else{
                next_cycles.push_back(std::move(reduced));
            }
        }
        cycles_ = std::move(next_cycles);
        return changed;
    }

    bool resolve_parallel_overlaps(Time time){
        bool changed = false;
        std::vector<std::vector<WavefrontNode>> next_cycles;
        next_cycles.reserve(cycles_.size());
        for(std::vector<WavefrontNode> cycle: cycles_){
            while(cycle.size() >= 3){
                std::size_t singular = cycle.size();
                for(std::size_t index = 0; index < cycle.size(); ++index){
                    if(!affine_point(cycle[index]).has_value()){
                        singular = index;
                        break;
                    }
                }
                if(singular == cycle.size()) break;
                changed = true;
                const std::size_t previous =
                    (singular + cycle.size() - 1) % cycle.size();
                std::rotate(
                    cycle.begin(),
                    cycle.begin() + static_cast<std::ptrdiff_t>(previous),
                    cycle.end()
                );
                const WavefrontNode previous_node = cycle[0];
                const WavefrontNode singular_node = cycle[1];
                const WavefrontNode next_node = cycle[2];
                const SourceEdge& left =
                    source_edges_[singular_node.left_edge];
                const SourceEdge& right =
                    source_edges_[singular_node.right_edge];
                const Time normal_dot = dot(left.normal, right.normal);
                const Point singular_point =
                    result_.vertices[singular_node.trace_vertex];

                if(plus_geometry_detail::sign(normal_dot - 1.0L) == 0){
                    const Time line_difference =
                        left.constant - right.constant;
                    if(plus_geometry_detail::sign(line_difference) != 0){
                        throw std::runtime_error(
                            "distinct parallel wavefront lines became adjacent"
                        );
                    }
                    cycle[1].collinear_affine = AffinePoint{
                        singular_point - left.normal * time,
                        left.normal,
                    };
                    continue;
                }
                if(plus_geometry_detail::sign(normal_dot + 1.0L) != 0){
                    throw std::runtime_error(
                        "a degenerate wavefront vertex has inconsistent normals"
                    );
                }
                const Time left_offset = left.constant + time;
                const Time right_offset = right.constant + time;
                if(plus_geometry_detail::sign(
                    left_offset + right_offset,
                    std::abs(left_offset) + std::abs(right_offset)
                ) != 0){
                    throw std::runtime_error(
                        "opposite wavefront edges are parallel but disjoint"
                    );
                }

                const Point previous_point = point_at(previous_node, time);
                const Point next_point = point_at(next_node, time);
                const Time direction_length =
                    std::sqrt(dot(left.direction, left.direction));
                const Point axis = left.direction / direction_length;
                const Time previous_parameter =
                    dot(previous_point - singular_point, axis);
                const Time next_parameter =
                    dot(next_point - singular_point, axis);
                if(plus_geometry_detail::sign(
                    previous_parameter * next_parameter,
                    std::abs(previous_parameter * next_parameter)
                ) <= 0){
                    throw std::runtime_error(
                        "opposite wavefront edges touch without an overlap interval"
                    );
                }

                const Time previous_length = std::abs(previous_parameter);
                const Time next_length = std::abs(next_parameter);
                const int length_order = plus_geometry_detail::sign(
                    previous_length - next_length,
                    previous_length + next_length
                );
                if(length_order < 0){
                    const std::size_t endpoint = event_vertex(
                        previous_point, time,
                        StraightSkeletonVertexKind::simultaneous_event
                    );
                    add_skeleton_edge(previous_node.trace_vertex, endpoint);
                    add_skeleton_edge(
                        singular_node.trace_vertex, endpoint
                    );
                    cycle[0] = {
                        previous_node.left_edge,
                        singular_node.right_edge,
                        endpoint,
                    };
                    cycle.erase(cycle.begin() + 1);
                }else if(length_order > 0){
                    const std::size_t endpoint = event_vertex(
                        next_point, time,
                        StraightSkeletonVertexKind::simultaneous_event
                    );
                    add_skeleton_edge(next_node.trace_vertex, endpoint);
                    add_skeleton_edge(
                        singular_node.trace_vertex, endpoint
                    );
                    cycle[1] = {
                        singular_node.left_edge,
                        next_node.right_edge,
                        endpoint,
                    };
                    cycle.erase(cycle.begin() + 2);
                }else{
                    const Point endpoint_point =
                        (previous_point + next_point) / 2.0L;
                    const std::size_t endpoint = event_vertex(
                        endpoint_point, time,
                        StraightSkeletonVertexKind::simultaneous_event
                    );
                    add_skeleton_edge(previous_node.trace_vertex, endpoint);
                    add_skeleton_edge(next_node.trace_vertex, endpoint);
                    add_skeleton_edge(
                        singular_node.trace_vertex, endpoint
                    );
                    cycle[0] = {
                        previous_node.left_edge,
                        next_node.right_edge,
                        endpoint,
                    };
                    cycle.erase(cycle.begin() + 1, cycle.begin() + 3);
                }
            }
            if(cycle.size() <= 2){
                terminate_cycle(cycle, time);
            }else{
                next_cycles.push_back(std::move(cycle));
            }
        }
        cycles_ = std::move(next_cycles);
        return changed;
    }
    std::optional<SplitCandidate> split_at_time(Time time) const{
        for(std::size_t cycle_index = 0;
            cycle_index < cycles_.size(); ++cycle_index){
            const std::vector<WavefrontNode>& cycle = cycles_[cycle_index];
            const std::size_t count = cycle.size();
            for(std::size_t vertex = 0; vertex < count; ++vertex){
                for(std::size_t edge = 0; edge < count; ++edge){
                    if(edge == vertex || (edge + 1) % count == vertex){
                        continue;
                    }
                    const std::optional<Time> event =
                        split_time(cycle, vertex, edge);
                    if(event.has_value()
                        && same_event_time(*event, time)){
                        return SplitCandidate{
                            *event, cycle_index, vertex, edge,
                        };
                    }
                }
            }
        }
        return std::nullopt;
    }

    void apply_split(const SplitCandidate& candidate){
        const std::vector<WavefrontNode> cycle = cycles_[candidate.cycle];
        const std::size_t count = cycle.size();
        const std::size_t vertex = candidate.vertex;
        const std::size_t edge = candidate.edge_start;
        const WavefrontNode& reflex = cycle[vertex];
        const std::size_t target_edge = cycle[edge].right_edge;
        const std::size_t event = event_vertex(
            point_at(reflex, candidate.time), candidate.time,
            StraightSkeletonVertexKind::split_event
        );
        add_skeleton_edge(reflex.trace_vertex, event);

        std::vector<WavefrontNode> first_cycle;
        std::vector<WavefrontNode> second_cycle;
        first_cycle.push_back({target_edge, reflex.right_edge, event});
        for(std::size_t index = (vertex + 1) % count;;
            index = (index + 1) % count){
            first_cycle.push_back(cycle[index]);
            if(index == edge) break;
        }
        second_cycle.push_back({reflex.left_edge, target_edge, event});
        for(std::size_t index = (edge + 1) % count;;
            index = (index + 1) % count){
            if(index == vertex) break;
            second_cycle.push_back(cycle[index]);
        }

        cycles_.erase(cycles_.begin()
            + static_cast<std::ptrdiff_t>(candidate.cycle));
        if(first_cycle.size() <= 2){
            terminate_cycle(first_cycle, candidate.time);
        }else{
            cycles_.push_back(std::move(first_cycle));
        }
        if(second_cycle.size() <= 2){
            terminate_cycle(second_cycle, candidate.time);
        }else{
            cycles_.push_back(std::move(second_cycle));
        }
    }

public:
    explicit StraightSkeletonBuilder(std::vector<Point> polygon)
        : input_polygon_(clean_polygon(std::move(polygon))),
          polygon_(input_polygon_){
        validate_and_orient();
        contour_vertex_count_ = input_polygon_.size();
        result_.vertices = input_polygon_;
        result_.times.assign(contour_vertex_count_, 0.0L);
        result_.kinds.assign(
            contour_vertex_count_,
            StraightSkeletonVertexKind::contour_vertex
        );
        initialize_wavefront();
    }

    StraightSkeletonResult run(){
        Time current_time = 0.0L;
        const std::size_t event_limit =
            16 * contour_vertex_count_ * contour_vertex_count_ + 64;
        std::size_t processed_events = 0;
        while(!cycles_.empty()){
            const std::optional<Time> next_time =
                next_event_time(current_time);
            if(!next_time.has_value()){
                throw std::runtime_error(
                    "straight-skeleton wavefront has no finite next event"
                );
            }
            current_time = *next_time;
            bool progressed = false;
            while(true){
                bool local_change = collapse_edges(current_time);
                local_change = resolve_self_intersections(current_time)
                    || local_change;
                const bool parallel_change =
                    resolve_parallel_overlaps(current_time);
                local_change = parallel_change || local_change;
                local_change = resolve_self_intersections(current_time)
                    || local_change;
                local_change = discard_clockwise_cycles(current_time)
                    || local_change;
                if(has_clockwise_cycle(current_time)){
                    throw std::runtime_error(
                        "straight-skeleton wavefront orientation was reversed"
                    );
                }
                if(local_change){
                    progressed = true;
                    ++processed_events;
                }
                const std::optional<SplitCandidate> split =
                    split_at_time(current_time);
                if(split.has_value()){
                    apply_split(*split);
                    static_cast<void>(
                        resolve_self_intersections(current_time)
                    );
                    static_cast<void>(
                        discard_clockwise_cycles(current_time)
                    );
                    if(has_clockwise_cycle(current_time)){
                        throw std::runtime_error(
                            "straight-skeleton split produced a reversed cycle"
                        );
                    }
                    progressed = true;
                    ++processed_events;
                    continue;
                }
                if(!local_change) break;
            }
            if(!progressed){
                throw std::runtime_error(
                    "straight-skeleton event did not change the wavefront"
                );
            }
            if(processed_events > event_limit){
                throw std::runtime_error(
                    "straight-skeleton event sequence did not converge"
                );
            }
        }
        return result_;
    }
};

}  // namespace straight_skeleton_detail

inline StraightSkeletonResult straight_skeleton(std::vector<Point> polygon){
    return straight_skeleton_detail::StraightSkeletonBuilder(
        std::move(polygon)
    ).run();
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_POINT_SET_STRAIGHT_SKELETON_HPP_INCLUDED