#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POINT_SET_POINT_SET_SCALAR_POLYGON_OFFSET_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POINT_SET_POINT_SET_SCALAR_POLYGON_OFFSET_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <map>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../detail/numeric/plus_geometry_detail.hpp"

enum class PolygonOffsetEdgeKind{
    segment,
    circular_arc,
};
enum class PolygonOffsetSourceKind{
    edge,
    vertex,
};

struct PolygonOffsetEdge{
    PolygonOffsetEdgeKind kind = PolygonOffsetEdgeKind::segment;
    Point first;
    Point second;
    Point center;
    long double radius = 0.0L;
    long double first_angle = 0.0L;
    long double second_angle = 0.0L;
    bool counterclockwise = true;
    PolygonOffsetSourceKind source_kind =
        PolygonOffsetSourceKind::edge;
    std::size_t source_index = 0;
};

struct PolygonOffsetResult{
    std::vector<PolygonOffsetEdge> edges;
    std::vector<std::vector<std::size_t>> contours;
};

namespace polygon_offset_detail{

inline constexpr long double pi =
    3.141592653589793238462643383279502884L;

struct LineCandidate{
    Point origin;
    Point direction;
    long double length;
    std::size_t source;
};

struct CircleCandidate{
    Point center;
    long double radius;
    std::size_t source;
};

inline long double normalize_angle(long double angle){
    const long double period = 2.0L * pi;
    angle = std::fmod(angle, period);
    if(angle < 0.0L) angle += period;
    return angle;
}

inline long double distance_to_boundary(
    const std::vector<Point>& polygon,
    const Point& point
){
    long double best = std::numeric_limits<long double>::infinity();
    for(std::size_t edge = 0; edge < polygon.size(); ++edge){
        const Point first = polygon[edge];
        const Point second = polygon[(edge + 1) % polygon.size()];
        const Point direction = second - first;
        const long double parameter = std::clamp(
            dot(point - first, direction) / dot(direction, direction),
            0.0L,
            1.0L
        );
        best = std::min(
            best,
            std::sqrt(plus_geometry_detail::squared_distance(
                point, first + direction * parameter
            ))
        );
    }
    return best;
}

inline long double signed_distance(
    const std::vector<Point>& polygon,
    const Point& point
){
    const long double distance = distance_to_boundary(polygon, point);
    return plus_geometry_detail::point_in_polygon(polygon, point) == 2
        ? -distance
        : distance;
}

inline bool on_offset(
    const std::vector<Point>& polygon,
    const Point& point,
    long double distance
){
    const long double current = signed_distance(polygon, point);
    return plus_geometry_detail::sign(
        current - distance,
        std::max({
            1.0L,
            std::abs(current),
            std::abs(distance),
            plus_geometry_detail::scale(point),
        })
    ) == 0;
}

inline bool selected(
    const std::vector<Point>& polygon,
    const Point& point,
    long double distance
){
    return plus_geometry_detail::sign(
        signed_distance(polygon, point) - distance,
        std::max({
            1.0L,
            std::abs(distance),
            plus_geometry_detail::scale(point),
        })
    ) <= 0;
}

inline void line_line_events(
    const LineCandidate& first,
    const LineCandidate& second,
    std::vector<long double>& events
){
    const long double denominator =
        cross(first.direction, second.direction);
    if(plus_geometry_detail::sign(denominator) == 0) return;
    const long double parameter = cross(
        second.origin - first.origin, second.direction
    ) / denominator;
    if(plus_geometry_detail::sign(parameter) >= 0
        && plus_geometry_detail::sign(parameter - first.length) <= 0){
        events.push_back(std::clamp(
            parameter, 0.0L, first.length
        ));
    }
}

inline void line_circle_events(
    const LineCandidate& line,
    const CircleCandidate& circle,
    std::vector<long double>& events
){
    const Point offset = line.origin - circle.center;
    const long double linear = dot(offset, line.direction);
    const long double constant =
        dot(offset, offset) - circle.radius * circle.radius;
    const long double discriminant =
        linear * linear - constant;
    const int discriminant_sign = plus_geometry_detail::sign(
        discriminant, std::abs(linear * linear) + std::abs(constant)
    );
    if(discriminant_sign < 0) return;
    const long double root = discriminant_sign == 0
        ? 0.0L : std::sqrt(discriminant);
    for(const long double parameter: {-linear - root, -linear + root}){
        if(plus_geometry_detail::sign(parameter) >= 0
            && plus_geometry_detail::sign(parameter - line.length) <= 0){
            events.push_back(std::clamp(
                parameter, 0.0L, line.length
            ));
        }
    }
}

inline void circle_line_events(
    const CircleCandidate& circle,
    const LineCandidate& line,
    std::vector<long double>& events
){
    std::vector<long double> line_parameters;
    line_circle_events(line, circle, line_parameters);
    for(long double parameter: line_parameters){
        const Point point =
            line.origin + line.direction * parameter;
        events.push_back(normalize_angle(std::atan2(
            point.y - circle.center.y,
            point.x - circle.center.x
        )));
    }
}

inline void circle_circle_events(
    const CircleCandidate& first,
    const CircleCandidate& second,
    std::vector<long double>& events
){
    const Point difference = second.center - first.center;
    const long double distance = std::sqrt(dot(difference, difference));
    if(plus_geometry_detail::sign(distance) == 0
        || distance > first.radius + second.radius
        || distance < std::abs(first.radius - second.radius)){
        return;
    }
    const long double along = (
        first.radius * first.radius
        - second.radius * second.radius
        + distance * distance
    ) / (2.0L * distance);
    const long double height = std::sqrt(std::max(
        0.0L,
        first.radius * first.radius - along * along
    ));
    const Point axis = difference / distance;
    const Point normal{-axis.y, axis.x};
    const Point base = first.center + axis * along;
    for(const Point point: {
        base + normal * height,
        base - normal * height,
    }){
        events.push_back(normalize_angle(std::atan2(
            point.y - first.center.y,
            point.x - first.center.x
        )));
    }
}

inline void orient_edge(
    const std::vector<Point>& polygon,
    long double distance,
    PolygonOffsetEdge& edge
){
    const Point middle = edge.kind == PolygonOffsetEdgeKind::segment
        ? (edge.first + edge.second) / 2.0L
        : edge.center + Point{
            std::cos((edge.first_angle + edge.second_angle) / 2.0L),
            std::sin((edge.first_angle + edge.second_angle) / 2.0L),
        } * edge.radius;
    Point tangent;
    if(edge.kind == PolygonOffsetEdgeKind::segment){
        tangent = edge.second - edge.first;
    }else{
        const Point radial = middle - edge.center;
        tangent = {-radial.y, radial.x};
    }
    tangent /= std::sqrt(dot(tangent, tangent));
    const Point left_normal{-tangent.y, tangent.x};
    const long double epsilon =
        1.0e-9L * std::max(
            1.0L, plus_geometry_detail::scale(middle)
        );
    if(selected(polygon, middle + left_normal * epsilon, distance)){
        return;
    }
    std::swap(edge.first, edge.second);
    std::swap(edge.first_angle, edge.second_angle);
    edge.counterclockwise = !edge.counterclockwise;
}

inline Point start_tangent(const PolygonOffsetEdge& edge){
    if(edge.kind == PolygonOffsetEdgeKind::segment){
        const Point direction = edge.second - edge.first;
        return direction / std::sqrt(dot(direction, direction));
    }
    const Point radial = edge.first - edge.center;
    Point tangent{-radial.y, radial.x};
    if(!edge.counterclockwise) tangent = -tangent;
    return tangent / std::sqrt(dot(tangent, tangent));
}

inline Point end_tangent(const PolygonOffsetEdge& edge){
    if(edge.kind == PolygonOffsetEdgeKind::segment){
        const Point direction = edge.second - edge.first;
        return direction / std::sqrt(dot(direction, direction));
    }
    const Point radial = edge.second - edge.center;
    Point tangent{-radial.y, radial.x};
    if(!edge.counterclockwise) tangent = -tangent;
    return tangent / std::sqrt(dot(tangent, tangent));
}

inline std::vector<std::vector<std::size_t>> stitch(
    const std::vector<PolygonOffsetEdge>& edges
){
    long double coordinate_scale = 1.0L;
    for(const PolygonOffsetEdge& edge: edges){
        coordinate_scale = std::max({
            coordinate_scale,
            plus_geometry_detail::scale(edge.first),
            plus_geometry_detail::scale(edge.second),
        });
    }
    const long double cell_width = 1024.0L
        * std::numeric_limits<long double>::epsilon()
        * coordinate_scale;
    std::vector<Point> nodes;
    std::map<
        std::pair<long double, long double>,
        std::vector<std::size_t>
    > buckets;
    const auto node_id = [&](const Point& point){
        const long double cell_x = std::floor(point.x / cell_width);
        const long double cell_y = std::floor(point.y / cell_width);
        for(int offset_x = -1; offset_x <= 1; ++offset_x){
            for(int offset_y = -1; offset_y <= 1; ++offset_y){
                const auto iterator = buckets.find({
                    cell_x + offset_x, cell_y + offset_y
                });
                if(iterator == buckets.end()) continue;
                for(std::size_t node: iterator->second){
                    if(plus_geometry_detail::close(nodes[node], point)){
                        return node;
                    }
                }
            }
        }
        const std::size_t node = nodes.size();
        nodes.push_back(point);
        buckets[{cell_x, cell_y}].push_back(node);
        return node;
    };
    std::vector<std::size_t> first_node(edges.size());
    std::vector<std::size_t> second_node(edges.size());
    for(std::size_t edge = 0; edge < edges.size(); ++edge){
        first_node[edge] = node_id(edges[edge].first);
        second_node[edge] = node_id(edges[edge].second);
    }
    std::vector<std::vector<std::pair<long double, std::size_t>>>
        outgoing(nodes.size());
    for(std::size_t edge = 0; edge < edges.size(); ++edge){
        const Point tangent = start_tangent(edges[edge]);
        outgoing[first_node[edge]].push_back({
            normalize_angle(std::atan2(tangent.y, tangent.x)), edge
        });
    }
    for(auto& records: outgoing){
        std::sort(records.begin(), records.end());
    }
    std::vector<std::size_t> successor(edges.size(), edges.size());
    for(std::size_t edge = 0; edge < edges.size(); ++edge){
        const auto& records = outgoing[second_node[edge]];
        if(records.empty()) continue;
        const Point incoming = end_tangent(edges[edge]);
        const long double incoming_angle =
            normalize_angle(std::atan2(incoming.y, incoming.x));
        const long double target =
            normalize_angle(incoming_angle + pi);
        auto iterator = std::lower_bound(
            records.begin(), records.end(),
            std::pair{target, std::size_t{0}}
        );
        std::size_t position = static_cast<std::size_t>(
            iterator - records.begin()
        );
        std::size_t best = records.size();
        long double best_turn =
            -std::numeric_limits<long double>::infinity();
        for(const std::size_t candidate_position: {
            position % records.size(),
            (position + records.size() - 1) % records.size(),
        }){
            const std::size_t candidate =
                records[candidate_position].second;
            const Point direction = start_tangent(edges[candidate]);
            const long double turn = std::atan2(
                cross(incoming, direction), dot(incoming, direction)
            );
            if(best == records.size() || turn > best_turn){
                best = candidate_position;
                best_turn = turn;
            }
        }
        successor[edge] = records[best].second;
    }
    std::vector<bool> used(edges.size(), false);
    std::vector<std::vector<std::size_t>> contours;
    for(std::size_t start = 0; start < edges.size(); ++start){
        if(used[start]) continue;
        std::vector<std::size_t> contour;
        std::size_t current = start;
        while(!used[current]){
            used[current] = true;
            contour.push_back(current);
            const std::size_t next = successor[current];
            if(next == start) break;
            if(next == edges.size()){
                contour.clear();
                break;
            }
            if(used[next]){
                contour.clear();
                break;
            }
            current = next;
        }
        if(!contour.empty()) contours.push_back(std::move(contour));
    }
    return contours;
}

}  // namespace polygon_offset_detail

inline PolygonOffsetResult polygon_offset(
    std::vector<Point> polygon,
    long double distance
){
    polygon = plus_geometry_detail::normalized_polygon(std::move(polygon));
    if(polygon.size() < 3){
        throw std::invalid_argument(
            "polygon offset requires a nondegenerate polygon"
        );
    }
    if(!std::isfinite(distance)){
        throw std::invalid_argument(
            "polygon offset distance must be finite"
        );
    }
    for(const Point& point: polygon){
        plus_geometry_detail::validate_finite(point);
    }
    PolygonOffsetResult result;
    if(distance == 0.0L){
        for(std::size_t edge = 0; edge < polygon.size(); ++edge){
            result.edges.push_back({
                PolygonOffsetEdgeKind::segment,
                polygon[edge],
                polygon[(edge + 1) % polygon.size()],
                {},
                0.0L,
                0.0L,
                0.0L,
                true,
                PolygonOffsetSourceKind::edge,
                edge,
            });
        }
        result.contours.push_back({});
        for(std::size_t edge = 0; edge < result.edges.size(); ++edge){
            result.contours.front().push_back(edge);
        }
        return result;
    }

    std::vector<polygon_offset_detail::LineCandidate> lines;
    std::vector<polygon_offset_detail::CircleCandidate> circles;
    const long double radius = std::abs(distance);
    for(std::size_t edge = 0; edge < polygon.size(); ++edge){
        const Point direction =
            polygon[(edge + 1) % polygon.size()] - polygon[edge];
        const long double length = std::sqrt(dot(direction, direction));
        const Point unit = direction / length;
        const Point outward{unit.y, -unit.x};
        lines.push_back({
            polygon[edge] + outward * distance,
            unit,
            length,
            edge,
        });
        circles.push_back({polygon[edge], radius, edge});
    }

    for(std::size_t line = 0; line < lines.size(); ++line){
        std::vector<long double> events{0.0L, lines[line].length};
        for(std::size_t other = 0; other < lines.size(); ++other){
            if(line != other){
                polygon_offset_detail::line_line_events(
                    lines[line], lines[other], events
                );
            }
        }
        for(const auto& circle: circles){
            polygon_offset_detail::line_circle_events(
                lines[line], circle, events
            );
        }
        std::sort(events.begin(), events.end());
        events.erase(std::unique(events.begin(), events.end()), events.end());
        for(std::size_t interval = 1;
            interval < events.size();
            ++interval){
            const long double middle =
                (events[interval - 1] + events[interval]) / 2.0L;
            const Point sample =
                lines[line].origin + lines[line].direction * middle;
            if(!polygon_offset_detail::on_offset(
                polygon, sample, distance
            )) continue;
            PolygonOffsetEdge edge{
                PolygonOffsetEdgeKind::segment,
                lines[line].origin
                    + lines[line].direction * events[interval - 1],
                lines[line].origin
                    + lines[line].direction * events[interval],
                {},
                0.0L,
                0.0L,
                0.0L,
                true,
                PolygonOffsetSourceKind::edge,
                lines[line].source,
            };
            polygon_offset_detail::orient_edge(
                polygon, distance, edge
            );
            result.edges.push_back(std::move(edge));
        }
    }

    for(std::size_t circle = 0; circle < circles.size(); ++circle){
        std::vector<long double> events{
            0.0L, 2.0L * polygon_offset_detail::pi
        };
        for(const auto& line: lines){
            polygon_offset_detail::circle_line_events(
                circles[circle], line, events
            );
        }
        for(std::size_t other = 0; other < circles.size(); ++other){

            if(circle != other){
                polygon_offset_detail::circle_circle_events(
                    circles[circle], circles[other], events
                );
            }
        }
        std::sort(events.begin(), events.end());
        events.erase(std::unique(events.begin(), events.end()), events.end());
        for(std::size_t interval = 1;
            interval < events.size();
            ++interval){
            const long double middle =
                (events[interval - 1] + events[interval]) / 2.0L;
            const Point sample = circles[circle].center + Point{
                std::cos(middle), std::sin(middle)
            } * radius;
            if(!polygon_offset_detail::on_offset(
                polygon, sample, distance
            )) continue;
            PolygonOffsetEdge edge{
                PolygonOffsetEdgeKind::circular_arc,
                circles[circle].center + Point{
                    std::cos(events[interval - 1]),
                    std::sin(events[interval - 1]),
                } * radius,
                circles[circle].center + Point{
                    std::cos(events[interval]),
                    std::sin(events[interval]),
                } * radius,
                circles[circle].center,
                radius,
                events[interval - 1],
                events[interval],
                true,
                PolygonOffsetSourceKind::vertex,
                circles[circle].source,
            };
            polygon_offset_detail::orient_edge(
                polygon, distance, edge
            );
            result.edges.push_back(std::move(edge));
        }
    }
    const long double fragment_tolerance = 4096.0L
        * std::numeric_limits<long double>::epsilon();
    result.edges.erase(std::remove_if(
        result.edges.begin(), result.edges.end(),
        [&](const PolygonOffsetEdge& edge){
            const long double scale = std::max(
                plus_geometry_detail::scale(edge.first),
                plus_geometry_detail::scale(edge.second)
            );
            const bool short_chord =
                plus_geometry_detail::squared_distance(
                    edge.first, edge.second
                ) <= fragment_tolerance * fragment_tolerance
                    * scale * scale;
            const bool short_parameter =
                edge.kind == PolygonOffsetEdgeKind::segment
                || std::abs(edge.second_angle - edge.first_angle)
                    <= fragment_tolerance;
            return short_chord && short_parameter;
        }
    ), result.edges.end());
    result.contours = polygon_offset_detail::stitch(result.edges);
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_POINT_SET_POINT_SET_SCALAR_POLYGON_OFFSET_HPP_INCLUDED
