#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_VISIBILITY_POLYGON_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_VISIBILITY_POLYGON_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "advanced/detail.hpp"
#include "bentley_ottmann_intersections.hpp"
#include "constants.hpp"
#include "contains.hpp"
#include "cross.hpp"
#include "dot.hpp"
#include "norm.hpp"
#include "types.hpp"

namespace visibility_polygon_detail{

inline bool finite_point(const Point& point){
    return std::isfinite(point.x) && std::isfinite(point.y);
}

inline void validate_simple_polygon(const std::vector<Point>& polygon){
    const std::size_t size = polygon.size();
    if(size < 3)[[unlikely]]{
        throw std::invalid_argument("polygon must have at least three vertices");
    }
    for(std::size_t index = 0; index < size; ++index){
        const Point& first = polygon[index];
        const Point& second = polygon[(index + 1) % size];
        if(!finite_point(first))[[unlikely]]{
            throw std::invalid_argument("polygon vertex must be finite");
        }
        if(norm(second - first)
            <= GEOMETRY_EPS * GEOMETRY_EPS)[[unlikely]]{
            throw std::invalid_argument("polygon has a degenerate edge");
        }
    }

    const Point& origin = polygon[0];
    long double twice_area = 0.0L;
    long double area_scale = 0.0L;
    for(std::size_t index = 1; index + 1 < size; ++index){
        const Point first = polygon[index] - origin;
        const Point second = polygon[index + 1] - origin;
        twice_area += cross(first, second);
        area_scale += advanced_geometry_detail::length(first) *
            advanced_geometry_detail::length(second);
    }
    if(advanced_geometry_detail::scaled_sign(
        twice_area, area_scale
    ) == 0)[[unlikely]]{
        throw std::invalid_argument("polygon area must be nonzero");
    }

    std::vector<Segment> edges;
    edges.reserve(size);
    for(std::size_t index = 0; index < size; ++index){
        edges.push_back({
            polygon[index] - origin,
            polygon[(index + 1) % size] - origin,
        });
    }
    const auto intersections = bentley_ottmann_detail::intersections(
        edges, size + 1
    );
    for(const SegmentIntersectionRecord& intersection: intersections){
        const bool adjacent = intersection.second == intersection.first + 1
            || (intersection.first == 0 && intersection.second + 1 == size);
        if(!adjacent
            || intersection.kind != SEGMENT_INTERSECTION_POINT)[[unlikely]]{
            throw std::invalid_argument("polygon must be simple");
        }
    }
}

inline std::pair<bool, long double> ray_segment_distance(
    const Point& origin,
    const Point& direction,
    const Segment& segment
){
    const Point edge = segment.b - segment.a;
    const Point offset = segment.a - origin;
    const long double denominator = cross(direction, edge);
    const long double scale = advanced_geometry_detail::length(direction) *
        advanced_geometry_detail::length(edge);
    if(std::fabs(denominator) <= 1e-18L * scale){
        const long double collinear_value = cross(offset, direction);
        const long double collinear_roundoff =
            std::abs(offset.x * direction.y) +
            std::abs(offset.y * direction.x);
        if(geometry_scaled_sign(
            collinear_value, 1.0L, collinear_roundoff
        ) != 0){
            return {false, 0.0L};
        }
        const long double divisor = norm(direction);
        long double answer = std::numeric_limits<long double>::infinity();
        for(const Point& point: {segment.a, segment.b}){
            const long double value = dot(point - origin, direction) / divisor;
            if(value >= -GEOMETRY_EPS) answer = std::min(answer, value);
        }
        return {std::isfinite(answer), std::max(0.0L, answer)};
    }
    const long double distance = cross(offset, edge) / denominator;
    const long double position = cross(offset, direction) / denominator;
    if(distance < -GEOMETRY_EPS
        || position < -GEOMETRY_EPS
        || position > 1.0L + GEOMETRY_EPS){
        return {false, 0.0L};
    }
    return {true, std::max(0.0L, distance)};
}

class AngularEnvelope{
    static constexpr std::size_t no_edge =
        std::numeric_limits<std::size_t>::max();

    const std::vector<Point>& polygon_;
    const Point& observer_;
    const std::vector<Point>& cell_directions_;
    std::vector<std::size_t> best_;

    bool nearer(
        std::size_t first,
        std::size_t second,
        const Point& direction
    ) const{
        if(second == no_edge) return true;
        const auto [first_hit, first_distance] = ray_segment_distance(
            observer_, direction,
            {polygon_[first], polygon_[(first + 1) % polygon_.size()]}
        );
        const auto [second_hit, second_distance] = ray_segment_distance(
            observer_, direction,
            {polygon_[second], polygon_[(second + 1) % polygon_.size()]}
        );
        if(first_hit != second_hit) return first_hit;
        if(!first_hit) return first < second;
        if(first_distance != second_distance){
            return first_distance < second_distance;
        }
        return first < second;
    }

    void add(
        std::size_t query_left,
        std::size_t query_right,
        std::size_t edge,
        std::size_t node,
        std::size_t left,
        std::size_t right
    ){
        if(query_right <= left || right <= query_left) return;
        if(query_left <= left && right <= query_right){
            const std::size_t probe = left + (right - left) / 2;
            if(nearer(edge, best_[node], cell_directions_[probe])){
                best_[node] = edge;
            }
            return;
        }
        const std::size_t middle = left + (right - left) / 2;
        add(query_left, query_right, edge, node * 2, left, middle);
        add(query_left, query_right, edge, node * 2 + 1, middle, right);
    }

    void query(
        std::size_t position,
        std::size_t node,
        std::size_t left,
        std::size_t right,
        std::size_t& answer
    ) const{
        if(best_[node] != no_edge
            && nearer(best_[node], answer, cell_directions_[position])){
            answer = best_[node];
        }
        if(right - left == 1) return;
        const std::size_t middle = left + (right - left) / 2;
        if(position < middle){
            query(position, node * 2, left, middle, answer);
        }else{
            query(position, node * 2 + 1, middle, right, answer);
        }
    }

public:
    AngularEnvelope(
        const std::vector<Point>& polygon,
        const Point& observer,
        const std::vector<Point>& cell_directions
    ):
        polygon_(polygon),
        observer_(observer),
        cell_directions_(cell_directions)
    {
        if(cell_directions.empty())[[unlikely]]{
            throw std::invalid_argument("visibility sweep needs an angular cell");
        }
        if(cell_directions.size()
            > (std::numeric_limits<std::size_t>::max() - 4) / 4)[[unlikely]]{
            throw std::length_error("too many visibility sweep events");
        }
        best_.assign(cell_directions.size() * 4 + 4, no_edge);
    }

    void add_circular_range(
        std::size_t first,
        std::size_t last,
        std::size_t edge
    ){
        const std::size_t size = cell_directions_.size();
        if(first == last) return;
        if(first < last){
            add(first, last, edge, 1, 0, size);
        }else{
            add(first, size, edge, 1, 0, size);
            add(0, last, edge, 1, 0, size);
        }
    }

    std::size_t nearest(std::size_t position) const{
        std::size_t answer = no_edge;
        query(position, 1, 0, cell_directions_.size(), answer);
        if(answer == no_edge)[[unlikely]]{
            throw std::runtime_error("angular cell did not hit polygon boundary");
        }
        return answer;
    }
};

inline Point boundary_point(
    const std::vector<Point>& polygon,
    const Point& observer,
    const Point& direction,
    std::size_t edge
){
    const auto [hit, distance] = ray_segment_distance(
        observer, direction,
        {polygon[edge], polygon[(edge + 1) % polygon.size()]}
    );
    if(!hit)[[unlikely]]{
        throw std::runtime_error("visibility event did not hit active edge");
    }
    return observer + direction * distance;
}

inline std::size_t nearest_event_edge(
    const std::vector<Point>& polygon,
    const Point& observer,
    const Point& direction,
    const std::vector<std::size_t>& candidates
){
    std::size_t answer = std::numeric_limits<std::size_t>::max();
    long double best = std::numeric_limits<long double>::infinity();
    for(std::size_t edge: candidates){
        const auto [hit, distance] = ray_segment_distance(
            observer, direction,
            {polygon[edge], polygon[(edge + 1) % polygon.size()]}
        );
        if(hit && (distance < best
            || (distance == best && edge < answer))){
            best = distance;
            answer = edge;
        }
    }
    if(answer == std::numeric_limits<std::size_t>::max())[[unlikely]]{
        throw std::runtime_error("visibility event did not hit polygon boundary");
    }
    return answer;
}

}  // namespace visibility_polygon_detail

inline std::vector<Point> visibility_polygon(
    const std::vector<Point>& polygon,
    const Point& observer
){
    using namespace visibility_polygon_detail;

    validate_simple_polygon(polygon);
    if(!finite_point(observer))[[unlikely]]{
        throw std::invalid_argument("observer must be finite");
    }
    if(contains(polygon, observer) != 2)[[unlikely]]{
        throw std::invalid_argument("observer must be strictly inside polygon");
    }

    struct DirectionEvent{
        long double angle;
        Point direction;
        std::size_t vertex;
    };
    const long double turn = 2.0L * std::acos(-1.0L);
    std::vector<DirectionEvent> events;
    events.reserve(polygon.size());
    for(std::size_t vertex = 0; vertex < polygon.size(); ++vertex){
        Point direction = polygon[vertex] - observer;
        direction = direction / advanced_geometry_detail::length(direction);
        long double angle = std::atan2(direction.y, direction.x);
        if(angle < 0.0L) angle += turn;
        events.push_back({angle, direction, vertex});
    }
    std::sort(events.begin(), events.end(), [](const auto& first, const auto& second){
        if(first.angle != second.angle) return first.angle < second.angle;
        return first.vertex < second.vertex;
    });

    struct DirectionGroup{
        long double angle;
        Point direction;
        std::vector<std::size_t> vertices;
    };
    std::vector<DirectionGroup> groups;
    std::vector<std::size_t> vertex_group(polygon.size());
    for(const DirectionEvent& event: events){
        if(groups.empty() || !advanced_geometry_detail::same_direction(
            groups.back().direction, event.direction
        )){
            groups.push_back({event.angle, event.direction, {}});
        }
        groups.back().vertices.push_back(event.vertex);
        vertex_group[event.vertex] = groups.size() - 1;
    }
    if(groups.size() < 2)[[unlikely]]{
        throw std::invalid_argument("polygon directions are degenerate");
    }

    std::vector<Point> cell_directions(groups.size());
    for(std::size_t group = 0; group < groups.size(); ++group){
        long double next_angle = groups[(group + 1) % groups.size()].angle;
        if(group + 1 == groups.size()) next_angle += turn;
        const long double middle_angle =
            groups[group].angle + (next_angle - groups[group].angle) / 2.0L;
        cell_directions[group] = {
            std::cos(middle_angle), std::sin(middle_angle)
        };
    }

    AngularEnvelope envelope(polygon, observer, cell_directions);
    std::vector<std::vector<std::size_t>> incident(groups.size());
    for(std::size_t edge = 0; edge < polygon.size(); ++edge){
        const std::size_t next = (edge + 1) % polygon.size();
        const std::size_t first_group = vertex_group[edge];
        const std::size_t second_group = vertex_group[next];
        incident[first_group].push_back(edge);
        incident[second_group].push_back(edge);
        if(first_group == second_group) continue;

        const Point first_direction = polygon[edge] - observer;
        const Point second_direction = polygon[next] - observer;
        const int orientation = advanced_geometry_detail::cross_sign(
            first_direction, second_direction
        );
        if(orientation > 0){
            envelope.add_circular_range(first_group, second_group, edge);
        }else if(orientation < 0){
            envelope.add_circular_range(second_group, first_group, edge);
        }else if(!advanced_geometry_detail::same_direction(
            first_direction, second_direction
        ))[[unlikely]]{
            throw std::invalid_argument("observer must be strictly inside polygon");
        }
    }
    for(auto& edges: incident){
        std::sort(edges.begin(), edges.end());
        edges.erase(std::unique(edges.begin(), edges.end()), edges.end());
    }

    std::vector<Point> candidates;
    candidates.reserve(groups.size() * 3);
    for(std::size_t group = 0; group < groups.size(); ++group){
        const std::size_t before = envelope.nearest(
            (group + groups.size() - 1) % groups.size()
        );
        const std::size_t after = envelope.nearest(group);
        const Point& direction = groups[group].direction;
        candidates.push_back(boundary_point(
            polygon, observer, direction, before
        ));

        std::vector<std::size_t> event_edges = incident[group];
        event_edges.push_back(before);
        event_edges.push_back(after);
        std::sort(event_edges.begin(), event_edges.end());
        event_edges.erase(
            std::unique(event_edges.begin(), event_edges.end()),
            event_edges.end()
        );
        const std::size_t exact = nearest_event_edge(
            polygon, observer, direction, event_edges
        );
        candidates.push_back(boundary_point(
            polygon, observer, direction, exact
        ));
        candidates.push_back(boundary_point(
            polygon, observer, direction, after
        ));
    }

    long double local_extent = 1.0L;
    long double coordinate_scale = std::max({
        1.0L,
        std::abs(observer.x),
        std::abs(observer.y),
    });
    for(const Point& point: polygon){
        local_extent = std::max(
            local_extent,
            advanced_geometry_detail::length(point - observer)
        );
        coordinate_scale = std::max(
            coordinate_scale,
            std::max(std::fabs(point.x), std::fabs(point.y))
        );
    }
    const long double merge_distance = 8.0L * GEOMETRY_EPS +
        1024.0L * std::numeric_limits<long double>::epsilon() *
            (coordinate_scale + local_extent);
    std::vector<Point> result;
    result.reserve(candidates.size());
    for(const Point& point: candidates){
        if(result.empty()
            || advanced_geometry_detail::length(point - result.back())
                > merge_distance){
            result.push_back(point);
        }
    }
    if(result.size() > 1
        && advanced_geometry_detail::length(result.front() - result.back())
            <= merge_distance){
        result.pop_back();
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_VISIBILITY_POLYGON_HPP_INCLUDED
