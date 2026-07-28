#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_POINT_SET_POINT_SET_POLYGON_BOOLEAN_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_POINT_SET_POINT_SET_POLYGON_BOOLEAN_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <map>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../detail/numeric/plus_geometry_detail.hpp"

enum class PolygonBooleanOperation{
    set_union,
    intersection,
    difference,
    symmetric_difference,
};

namespace polygon_boolean_detail{

inline bool apply_operation(
    PolygonBooleanOperation operation,
    bool in_first,
    bool in_second
){
    if(operation == PolygonBooleanOperation::set_union){
        return in_first || in_second;
    }
    if(operation == PolygonBooleanOperation::intersection){
        return in_first && in_second;
    }
    if(operation == PolygonBooleanOperation::difference){
        return in_first && !in_second;
    }
    return in_first != in_second;
}

struct Fragment{
    Point first;
    Point second;
};

inline void add_split_parameters(
    const Segment& segment,
    const std::vector<Point>& other,
    std::vector<long double>& parameters
){
    for(std::size_t index = 0; index < other.size(); ++index){
        const Segment other_segment{
            other[index],
            other[(index + 1) % other.size()],
        };
        long double first_parameter = 0.0L;
        long double second_parameter = 0.0L;
        if(plus_geometry_detail::segment_parameter(
            segment, other_segment, first_parameter, second_parameter
        )){
            parameters.push_back(std::clamp(first_parameter, 0.0L, 1.0L));
            continue;
        }
        if(plus_geometry_detail::orientation(
            segment.a, segment.b, other_segment.a
        ) != 0){
            continue;
        }
        const Point direction = segment.b - segment.a;
        const long double denominator = dot(direction, direction);
        for(const Point& point: {other_segment.a, other_segment.b}){
            const long double parameter =
                dot(point - segment.a, direction) / denominator;
            if(plus_geometry_detail::sign(parameter) >= 0
                && plus_geometry_detail::sign(parameter - 1.0L) <= 0){
                parameters.push_back(std::clamp(parameter, 0.0L, 1.0L));
            }
        }
    }
}

inline std::vector<Fragment> boundary_fragments(
    const std::vector<Point>& first,
    const std::vector<Point>& second,
    PolygonBooleanOperation operation
){
    std::vector<Fragment> fragments;
    const auto process = [&](const std::vector<Point>& owner,
                             const std::vector<Point>& other){
        for(std::size_t index = 0; index < owner.size(); ++index){
            const Segment segment{
                owner[index],
                owner[(index + 1) % owner.size()],
            };
            std::vector<long double> parameters{0.0L, 1.0L};
            add_split_parameters(segment, other, parameters);
            std::sort(parameters.begin(), parameters.end());
            parameters.erase(
                std::unique(parameters.begin(), parameters.end(), [](long double left, long double right){
                    return plus_geometry_detail::sign(left - right) == 0;
                }),
                parameters.end()
            );
            for(std::size_t part = 1; part < parameters.size(); ++part){
                const long double left_parameter = parameters[part - 1];
                const long double right_parameter = parameters[part];
                if(plus_geometry_detail::sign(
                    right_parameter - left_parameter
                ) == 0){
                    continue;
                }
                const Point begin =
                    plus_geometry_detail::interpolate(segment, left_parameter);
                const Point end =
                    plus_geometry_detail::interpolate(segment, right_parameter);
                const Point middle = (begin + end) / 2.0L;
                const Point direction = end - begin;
                const long double length = std::sqrt(dot(direction, direction));
                const long double coordinate_scale = std::max(
                    plus_geometry_detail::scale(middle), length
                );
                const long double displacement = std::max(
                    1024.0L * std::numeric_limits<long double>::epsilon()
                        * coordinate_scale,
                    1.0e-12L * coordinate_scale
                );
                const Point normal{
                    -direction.y / length * displacement,
                    direction.x / length * displacement,
                };
                const Point left_sample = middle + normal;
                const Point right_sample = middle - normal;
                const bool left_inside = apply_operation(
                    operation,
                    plus_geometry_detail::point_in_polygon(first, left_sample) != 0,
                    plus_geometry_detail::point_in_polygon(second, left_sample) != 0
                );
                const bool right_inside = apply_operation(
                    operation,
                    plus_geometry_detail::point_in_polygon(first, right_sample) != 0,
                    plus_geometry_detail::point_in_polygon(second, right_sample) != 0
                );
                if(left_inside == right_inside) continue;
                if(left_inside) fragments.push_back({begin, end});
                else fragments.push_back({end, begin});
            }
        }
    };
    process(first, second);
    process(second, first);
    return fragments;
}

inline bool inside_set(
    const std::vector<std::vector<Point>>& polygons,
    const Point& point
){
    bool inside = false;
    for(const std::vector<Point>& polygon: polygons){
        if(plus_geometry_detail::point_in_polygon(polygon, point) != 0){
            inside = !inside;
        }
    }
    return inside;
}

inline std::vector<Fragment> boundary_fragments(
    const std::vector<std::vector<Point>>& first,
    const std::vector<std::vector<Point>>& second,
    PolygonBooleanOperation operation
){
    std::vector<Fragment> fragments;
    const auto process = [&](const std::vector<std::vector<Point>>& owners){
        for(const std::vector<Point>& owner: owners){
            for(std::size_t index = 0; index < owner.size(); ++index){
                const Segment segment{
                    owner[index], owner[(index + 1) % owner.size()]
                };
                std::vector<long double> parameters{0.0L, 1.0L};
                for(const std::vector<Point>& polygon: first){
                    add_split_parameters(segment, polygon, parameters);
                }
                for(const std::vector<Point>& polygon: second){
                    add_split_parameters(segment, polygon, parameters);
                }
                std::sort(parameters.begin(), parameters.end());
                parameters.erase(std::unique(
                    parameters.begin(), parameters.end(),
                    [](long double left, long double right){
                        return plus_geometry_detail::sign(left - right) == 0;
                    }
                ), parameters.end());
                for(std::size_t part = 1; part < parameters.size(); ++part){
                    if(plus_geometry_detail::sign(
                        parameters[part] - parameters[part - 1]
                    ) == 0) continue;
                    Point begin = plus_geometry_detail::interpolate(
                        segment, parameters[part - 1]
                    );
                    Point end = plus_geometry_detail::interpolate(
                        segment, parameters[part]
                    );
                    const Point direction = end - begin;
                    const long double length = std::sqrt(dot(direction, direction));
                    const Point middle = (begin + end) / 2.0L;
                    const long double displacement = std::max(
                        1024.0L * std::numeric_limits<long double>::epsilon()
                            * std::max(plus_geometry_detail::scale(middle), length),
                        1.0e-12L * std::max(plus_geometry_detail::scale(middle), length)
                    );
                    const Point normal{
                        -direction.y / length * displacement,
                        direction.x / length * displacement,
                    };
                    const auto selected = [&](const Point& sample){
                        return apply_operation(
                            operation,
                            inside_set(first, sample),
                            inside_set(second, sample)
                        );
                    };
                    const bool left = selected(middle + normal);
                    const bool right = selected(middle - normal);
                    if(left == right) continue;
                    if(left) fragments.push_back({begin, end});
                    else fragments.push_back({end, begin});
                }
            }
        }
    };
    process(first);
    process(second);
    return fragments;
}

inline std::vector<std::vector<Point>> stitch(std::vector<Fragment> fragments){
    long double coordinate_scale = 1.0L;
    for(const Fragment& fragment: fragments){
        coordinate_scale = std::max({
            coordinate_scale,
            plus_geometry_detail::scale(fragment.first),
            plus_geometry_detail::scale(fragment.second),
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
    std::vector<std::pair<std::size_t, std::size_t>> edges;
    edges.reserve(fragments.size());
    for(const Fragment& fragment: fragments){
        const std::size_t first = node_id(fragment.first);
        const std::size_t second = node_id(fragment.second);
        if(first != second) edges.push_back({first, second});
    }
    std::sort(edges.begin(), edges.end());
    edges.erase(std::unique(edges.begin(), edges.end()), edges.end());

    constexpr long double pi =
        3.141592653589793238462643383279502884L;
    const auto normalized_angle = [&](Point direction){
        long double angle = std::atan2(direction.y, direction.x);
        if(angle < 0.0L) angle += 2.0L * pi;
        return angle;
    };
    std::vector<std::vector<std::pair<long double, std::size_t>>>
        outgoing(nodes.size());
    for(std::size_t edge = 0; edge < edges.size(); ++edge){
        outgoing[edges[edge].first].push_back({
            normalized_angle(
                nodes[edges[edge].second] - nodes[edges[edge].first]
            ),
            edge,
        });
    }
    for(auto& records: outgoing) std::sort(records.begin(), records.end());
    std::vector<std::size_t> successor(edges.size(), edges.size());
    for(std::size_t edge = 0; edge < edges.size(); ++edge){
        const std::size_t next_node = edges[edge].second;
        const auto& records = outgoing[next_node];
        if(records.empty()) continue;
        const Point incoming =
            nodes[next_node] - nodes[edges[edge].first];
        long double target = normalized_angle(incoming) + pi;
        if(target >= 2.0L * pi) target -= 2.0L * pi;
        const auto iterator = std::lower_bound(
            records.begin(), records.end(),
            std::pair{target, std::size_t{0}}
        );
        const std::size_t position = static_cast<std::size_t>(
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
            const Point direction = nodes[edges[candidate].second]
                - nodes[next_node];
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
    std::vector<std::vector<Point>> result;
    for(std::size_t start_edge = 0;
        start_edge < edges.size();
        ++start_edge){
        if(used[start_edge]) continue;
        std::vector<Point> polygon;
        std::size_t edge = start_edge;
        while(!used[edge]){
            used[edge] = true;
            polygon.push_back(nodes[edges[edge].first]);
            const std::size_t next = successor[edge];
            if(next == start_edge) break;
            if(next == edges.size() || used[next]){
                polygon.clear();
                break;
            }
            edge = next;
        }
        if(polygon.size() >= 3
            && plus_geometry_detail::sign(
                plus_geometry_detail::signed_area_twice(polygon)
            ) != 0){
            result.push_back(std::move(polygon));
        }
    }
    return result;
}

}  // namespace polygon_boolean_detail

inline std::vector<std::vector<Point>> polygon_boolean(
    std::vector<std::vector<Point>> first,
    std::vector<std::vector<Point>> second,
    PolygonBooleanOperation operation
){
    const auto normalize = [](std::vector<std::vector<Point>>& polygons){
        for(std::vector<Point>& polygon: polygons){
            polygon = plus_geometry_detail::normalized_polygon(
                std::move(polygon)
            );
            if(polygon.size() < 3){
                throw std::invalid_argument(
                    "polygon Boolean components must be nondegenerate"
                );
            }
            for(const Point& point: polygon){
                plus_geometry_detail::validate_finite(point);
            }
        }
    };
    normalize(first);
    normalize(second);
    return polygon_boolean_detail::stitch(
        polygon_boolean_detail::boundary_fragments(first, second, operation)
    );
}

inline std::vector<std::vector<Point>> polygon_boolean(
    std::vector<Point> first,
    std::vector<Point> second,
    PolygonBooleanOperation operation
){
    first = plus_geometry_detail::normalized_polygon(std::move(first));
    second = plus_geometry_detail::normalized_polygon(std::move(second));
    if(first.size() < 3 || second.size() < 3){
        throw std::invalid_argument(
            "polygon Boolean operations require two nondegenerate polygons"
        );
    }
    for(const Point& point: first) plus_geometry_detail::validate_finite(point);
    for(const Point& point: second) plus_geometry_detail::validate_finite(point);
    return polygon_boolean_detail::stitch(
        polygon_boolean_detail::boundary_fragments(
            first, second, operation
        )
    );
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_2D_RESULT_POINT_SET_POINT_SET_POLYGON_BOOLEAN_HPP_INCLUDED
