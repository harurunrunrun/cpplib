#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SHAPE_VISIBILITY_GRAPH_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SHAPE_VISIBILITY_GRAPH_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <set>
#include <utility>
#include <vector>

#include "../detail/advanced_convex_geometry_detail.hpp"
#include "../core/constants.hpp"
#include "../scalar/cross.hpp"
#include "../scalar/norm.hpp"
#include "../core/types.hpp"
#include "../point_collection/visibility_polygon.hpp"

struct VisibilityGraphEdge{
    std::size_t first;
    std::size_t second;
    long double distance;
};

namespace visibility_graph_detail{

inline long double normalized_angle(const Point& direction){
    long double angle = std::atan2(direction.y, direction.x);
    if(angle < 0.0L) angle += 2.0L * std::acos(-1.0L);
    return angle;
}

inline int polygon_orientation(const std::vector<Point>& polygon){
    const Point origin = polygon.front();
    long double twice_area = 0.0L;
    for(std::size_t index = 1; index + 1 < polygon.size(); ++index){
        twice_area += cross(
            polygon[index] - origin,
            polygon[index + 1] - origin
        );
    }
    return twice_area > 0.0L ? 1 : -1;
}

inline bool direction_is_inside_vertex(
    const std::vector<Point>& polygon,
    std::size_t vertex,
    const Point& direction,
    int orientation
){
    const std::size_t size = polygon.size();
    const Point previous =
        polygon[(vertex + size - 1) % size] - polygon[vertex];
    const Point following =
        polygon[(vertex + 1) % size] - polygon[vertex];
    const int after_following = orientation *
        advanced_geometry_detail::cross_sign(following, direction);
    const int before_previous = orientation *
        advanced_geometry_detail::cross_sign(direction, previous);
    const int corner = orientation *
        advanced_geometry_detail::cross_sign(following, previous);
    if(corner >= 0){
        return after_following >= 0 && before_previous >= 0;
    }
    return after_following >= 0 || before_previous >= 0;
}

struct DirectionVertex{
    std::size_t vertex;
    Point direction;
    long double angle;
    long double squared_distance;
};

struct DirectionGroup{
    std::size_t begin;
    std::size_t end;
    long double angle;
};

inline std::vector<DirectionGroup> direction_groups(
    std::vector<DirectionVertex>& vertices
){
    std::sort(vertices.begin(), vertices.end(),
        [](const DirectionVertex& first, const DirectionVertex& second){
            if(first.angle != second.angle) return first.angle < second.angle;
            if(first.squared_distance != second.squared_distance){
                return first.squared_distance < second.squared_distance;
            }
            return first.vertex < second.vertex;
        }
    );

    if(1 < vertices.size()){
        const long double turn = 2.0L * std::acos(-1.0L);
        std::size_t start = 0;
        long double largest_gap = -1.0L;
        for(std::size_t index = 0; index < vertices.size(); ++index){
            const std::size_t next = (index + 1) % vertices.size();
            long double next_angle = vertices[next].angle;
            if(next == 0) next_angle += turn;
            const long double gap = next_angle - vertices[index].angle;
            if(largest_gap < gap){
                largest_gap = gap;
                start = next;
            }
        }
        std::rotate(vertices.begin(), vertices.begin() + start, vertices.end());
        for(std::size_t index = 1; index < vertices.size(); ++index){
            if(vertices[index].angle < vertices[index - 1].angle){
                vertices[index].angle += turn;
            }
        }
    }

    std::vector<DirectionGroup> groups;
    for(std::size_t begin = 0; begin < vertices.size();){
        std::size_t end = begin + 1;
        while(end < vertices.size()
            && advanced_geometry_detail::same_direction(
                vertices[begin].direction, vertices[end].direction
            )){
            ++end;
        }
        std::sort(vertices.begin() + static_cast<std::ptrdiff_t>(begin),
            vertices.begin() + static_cast<std::ptrdiff_t>(end),
            [](const DirectionVertex& first, const DirectionVertex& second){
                if(first.squared_distance != second.squared_distance){
                    return first.squared_distance < second.squared_distance;
                }
                return first.vertex < second.vertex;
            }
        );
        groups.push_back({begin, end, vertices[begin].angle});
        begin = end;
    }
    return groups;
}

struct ActiveEdgeOrder{
    const std::vector<Point>* polygon;
    Point origin;
    const Point* direction;

    long double distance(std::size_t edge) const{
        const Point first = (*polygon)[edge] - origin;
        const Point side =
            (*polygon)[(edge + 1) % polygon->size()] - (*polygon)[edge];
        return cross(first, side) / cross(*direction, side);
    }

    bool operator()(std::size_t first, std::size_t second) const{
        if(first == second) return false;
        const long double first_distance = distance(first);
        const long double second_distance = distance(second);
        if(first_distance != second_distance){
            return first_distance < second_distance;
        }
        return first < second;
    }
};

inline bool edge_blocks_direction(
    const std::vector<Point>& polygon,
    const Point& origin,
    std::size_t edge,
    const Point& target_direction
){
    const Point side =
        polygon[(edge + 1) % polygon.size()] - polygon[edge];
    const long double denominator = cross(target_direction, side);
    const long double side_scale = advanced_geometry_detail::length(
        target_direction
    ) * advanced_geometry_detail::length(side);
    if(geometry_scaled_sign(
        denominator,
        side_scale,
        std::abs(target_direction.x * side.y)
            + std::abs(target_direction.y * side.x)
    ) == 0){
        return false;
    }
    const long double parameter =
        cross(polygon[edge] - origin, side) / denominator;
    return geometry_scaled_sign(
        1.0L - parameter,
        1.0L,
        1.0L + std::abs(parameter)
    ) > 0;
}

inline std::vector<char> visible_from_vertex(
    const std::vector<Point>& polygon,
    std::size_t source,
    int orientation
){
    const std::size_t size = polygon.size();
    const Point origin = polygon[source];
    std::vector<DirectionVertex> directions;
    directions.reserve(size - 1);
    for(std::size_t vertex = 0; vertex < size; ++vertex){
        if(vertex == source) continue;
        const Point direction = polygon[vertex] - origin;
        directions.push_back({
            vertex,
            direction,
            normalized_angle(direction),
            norm(direction),
        });
    }
    const std::vector<DirectionGroup> groups = direction_groups(directions);
    const std::size_t group_count = groups.size();

    std::vector<std::size_t> vertex_group(size, group_count);
    for(std::size_t group = 0; group < group_count; ++group){
        for(std::size_t index = groups[group].begin;
            index < groups[group].end; ++index){
            vertex_group[directions[index].vertex] = group;
        }
    }

    std::vector<std::vector<std::size_t>> starting(group_count);
    std::vector<std::vector<std::size_t>> ending(group_count);
    std::vector<char> wraps(size, 0);
    for(std::size_t edge = 0; edge < size; ++edge){
        const std::size_t next = (edge + 1) % size;
        if(edge == source || next == source) continue;
        const std::size_t first_group = vertex_group[edge];
        const std::size_t second_group = vertex_group[next];
        if(first_group == second_group) continue;
        const int side = advanced_geometry_detail::cross_sign(
            polygon[edge] - origin,
            polygon[next] - origin
        );
        if(side == 0) continue;
        const std::size_t start = side > 0 ? first_group : second_group;
        const std::size_t finish = side > 0 ? second_group : first_group;
        starting[start].push_back(edge);
        ending[finish].push_back(edge);
        wraps[edge] = start > finish;
    }

    const long double turn = 2.0L * std::acos(-1.0L);
    Point sweep_direction{
        std::cos((groups.back().angle + groups.front().angle + turn) / 2.0L),
        std::sin((groups.back().angle + groups.front().angle + turn) / 2.0L),
    };
    using ActiveSet = std::set<std::size_t, ActiveEdgeOrder>;
    ActiveSet active(ActiveEdgeOrder{&polygon, origin, &sweep_direction});
    std::vector<ActiveSet::iterator> positions(size, active.end());
    for(std::size_t edge = 0; edge < size; ++edge){
        if(wraps[edge]) positions[edge] = active.insert(edge).first;
    }

    std::vector<char> visible(size, 0);
    visible[(source + 1) % size] = 1;
    visible[(source + size - 1) % size] = 1;
    for(std::size_t group = 0; group < group_count; ++group){
        for(const std::size_t edge: ending[group]){
            if(positions[edge] != active.end()){
                active.erase(positions[edge]);
                positions[edge] = active.end();
            }
        }

        const long double current_angle = groups[group].angle;
        const long double next_angle = group + 1 < group_count
            ? groups[group + 1].angle
            : groups.front().angle + turn;
        const long double middle_angle = (current_angle + next_angle) / 2.0L;
        sweep_direction = {
            std::cos(middle_angle),
            std::sin(middle_angle),
        };
        for(const std::size_t edge: starting[group]){
            positions[edge] = active.insert(edge).first;
        }

        const std::size_t target = directions[groups[group].begin].vertex;
        const bool adjacent = target == (source + 1) % size
            || target == (source + size - 1) % size;
        if(adjacent){
            visible[target] = 1;
            continue;
        }
        const Point target_direction = polygon[target] - origin;
        if(!direction_is_inside_vertex(
            polygon, source, target_direction, orientation
        )){
            continue;
        }
        if(active.empty() || !edge_blocks_direction(
            polygon, origin, *active.begin(), target_direction
        )){
            visible[target] = 1;
        }
    }
    return visible;
}

}

inline std::vector<VisibilityGraphEdge> visibility_graph(
    const std::vector<Point>& polygon
){
    visibility_polygon_detail::validate_simple_polygon(polygon);
    const std::size_t size = polygon.size();
    const int orientation = visibility_graph_detail::polygon_orientation(
        polygon
    );
    std::vector<VisibilityGraphEdge> result;
    result.reserve(size * (size - 1) / 2);
    for(std::size_t first = 0; first < size; ++first){
        const std::vector<char> visible =
            visibility_graph_detail::visible_from_vertex(
                polygon, first, orientation
            );
        for(std::size_t second = first + 1; second < size; ++second){
            if(!visible[second]) continue;
            result.push_back({
                first,
                second,
                std::sqrt(norm(polygon[second] - polygon[first])),
            });
        }
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_SHAPE_VISIBILITY_GRAPH_HPP_INCLUDED
