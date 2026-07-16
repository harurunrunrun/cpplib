#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

#include "abs.hpp"
#include "cross.hpp"
#include "cross_point.hpp"
#include "dot.hpp"

struct PolygonLineCutResult{
    std::vector<std::vector<Point>> left;
    std::vector<std::vector<Point>> right;
};

namespace polygon_line_cut_detail{

inline int signed_area_sign(const std::vector<Point>& polygon){
    if(polygon.empty()) return 0;
    const Point origin = polygon.front();
    long double value = 0.0L;
    long double coordinate_scale = 0.0L;
    long double roundoff_scale = 0.0L;
    for(std::size_t index = 0; index < polygon.size(); ++index){
        const Point first = polygon[index] - origin;
        const Point second =
            polygon[(index + 1) % polygon.size()] - origin;
        value += cross(first, second);
        coordinate_scale = std::max({
            coordinate_scale,
            abs(first),
            abs(second),
        });
        roundoff_scale +=
            std::abs(first.x * second.y)
            + std::abs(first.y * second.x);
    }
    return geometry_scaled_sign(
        value,
        coordinate_scale * coordinate_scale,
        roundoff_scale
    );
}

struct BoundaryGraph{
    const std::vector<Point>& vertices;
    std::vector<std::pair<std::size_t, std::size_t>> edges;
    std::set<std::pair<std::size_t, std::size_t>> edge_set;

    explicit BoundaryGraph(const std::vector<Point>& vertices_)
        : vertices(vertices_){}

    void add_edge(std::size_t first, std::size_t second){
        if(first == second || vertices[first] == vertices[second]) return;
        if(second < first) std::swap(first, second);
        const std::pair edge{first, second};
        if(edge_set.insert(edge).second) edges.push_back(edge);
    }

    std::vector<std::vector<Point>> polygons() const{
        struct AdjacentEdge{
            std::size_t to;
            std::size_t edge;
        };
        std::vector<std::vector<AdjacentEdge>> adjacency(vertices.size());
        for(std::size_t edge = 0; edge < edges.size(); ++edge){
            const auto [first, second] = edges[edge];
            adjacency[first].push_back({second, edge});
            adjacency[second].push_back({first, edge});
        }
        for(std::size_t vertex = 0; vertex < vertices.size(); ++vertex){
            std::sort(
                adjacency[vertex].begin(), adjacency[vertex].end(),
                [&](const AdjacentEdge& first, const AdjacentEdge& second){
                    const Point first_direction =
                        vertices[first.to] - vertices[vertex];
                    const Point second_direction =
                        vertices[second.to] - vertices[vertex];
                    const long double first_angle = std::atan2(
                        first_direction.y, first_direction.x
                    );
                    const long double second_angle = std::atan2(
                        second_direction.y, second_direction.x
                    );
                    if(first_angle != second_angle){
                        return first_angle < second_angle;
                    }
                    return abs(first_direction) < abs(second_direction);
                }
            );
        }

        const auto directed_id = [&](std::size_t edge, std::size_t from){
            return edge * 2 + (edges[edge].first == from ? 0U : 1U);
        };
        const auto directed_from = [&](std::size_t id){
            const auto& edge = edges[id / 2];
            return id % 2 == 0 ? edge.first : edge.second;
        };
        std::vector<std::size_t> next(
            edges.size() * 2, edges.size() * 2
        );
        for(std::size_t vertex = 0; vertex < adjacency.size(); ++vertex){
            const auto& around = adjacency[vertex];
            for(std::size_t index = 0; index < around.size(); ++index){
                const AdjacentEdge& incoming = around[index];
                const AdjacentEdge& outgoing = around[
                    (index + around.size() - 1) % around.size()
                ];
                next[directed_id(incoming.edge, incoming.to)] =
                    directed_id(outgoing.edge, vertex);
            }
        }

        std::vector<bool> used(edges.size() * 2, false);
        std::vector<std::vector<Point>> result;
        for(std::size_t initial = 0; initial < used.size(); ++initial){
            if(used[initial]) continue;
            std::size_t current = initial;
            std::vector<Point> polygon;
            bool closed = false;
            for(std::size_t step = 0; step <= used.size(); ++step){
                if(current >= used.size() || used[current]) break;
                used[current] = true;
                polygon.push_back(vertices[directed_from(current)]);
                current = next[current];
                if(current == initial){
                    closed = true;
                    break;
                }
            }
            if(!closed || polygon.size() < 3) continue;
            if(signed_area_sign(polygon) <= 0) continue;
            result.push_back(std::move(polygon));
        }
        return result;
    }
};

inline void add_clipped_edge(
    BoundaryGraph& graph,
    std::size_t first,
    std::size_t second,
    std::size_t intersection,
    int first_side,
    int second_side,
    int kept_sign
){
    const bool first_inside = first_side * kept_sign >= 0;
    const bool second_inside = second_side * kept_sign >= 0;
    if(first_inside && second_inside){
        graph.add_edge(first, second);
    }else if(first_inside != second_inside){
        if(intersection == std::numeric_limits<std::size_t>::max())
            [[unlikely]]{
            throw std::logic_error("polygon cut lost an edge intersection");
        }
        graph.add_edge(
            first_inside ? first : intersection,
            second_inside ? second : intersection
        );
    }
}

}  // namespace polygon_line_cut_detail

inline PolygonLineCutResult polygon_cut(
    const std::vector<Point>& polygon,
    const Line& line
){
    const Point raw_direction = line.b - line.a;
    const long double direction_length = abs(raw_direction);
    if(geometry_sign(direction_length) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate line");
    }
    const Point direction = raw_direction / direction_length;

    std::vector<Point> vertices = polygon;
    std::vector<int> sides(polygon.size());
    std::vector<unsigned char> on_line_added(polygon.size(), 0);
    std::vector<unsigned> toggle_count(polygon.size(), 0);
    std::vector<std::size_t> on_line;
    on_line.reserve(polygon.size() * 2);
    for(std::size_t index = 0; index < polygon.size(); ++index){
        sides[index] = geometry_sign(cross(
            direction, polygon[index] - line.a
        ));
        if(sides[index] == 0){
            on_line.push_back(index);
            on_line_added[index] = 1;
        }
    }

    polygon_line_cut_detail::BoundaryGraph left_graph(vertices);
    polygon_line_cut_detail::BoundaryGraph right_graph(vertices);
    const std::size_t no_intersection =
        std::numeric_limits<std::size_t>::max();
    for(std::size_t index = 0; index < polygon.size(); ++index){
        const std::size_t next = (index + 1) % polygon.size();
        const int first_side = sides[index];
        const int second_side = sides[next];
        std::size_t intersection = no_intersection;
        if(first_side == 0){
            intersection = index;
        }else if(second_side == 0){
            intersection = next;
        }else if(first_side * second_side < 0){
            intersection = vertices.size();
            vertices.push_back(cross_point(
                Line{polygon[index], polygon[next]}, line
            ));
            on_line_added.push_back(0);
            toggle_count.push_back(0);
        }
        if(intersection != no_intersection
            && !on_line_added[intersection]){
            on_line.push_back(intersection);
            on_line_added[intersection] = 1;
        }

        polygon_line_cut_detail::add_clipped_edge(
            left_graph, index, next, intersection,
            first_side, second_side, 1
        );
        polygon_line_cut_detail::add_clipped_edge(
            right_graph, index, next, intersection,
            first_side, second_side, -1
        );
        if((first_side > 0) != (second_side > 0)){
            if(intersection == no_intersection)[[unlikely]]{
                throw std::logic_error("polygon cut lost a parity event");
            }
            ++toggle_count[intersection];
        }
    }

    std::sort(on_line.begin(), on_line.end(), [&](std::size_t first,
                                                  std::size_t second){
        const long double first_parameter =
            dot(vertices[first] - line.a, direction);
        const long double second_parameter =
            dot(vertices[second] - line.a, direction);
        if(first_parameter != second_parameter){
            return first_parameter < second_parameter;
        }
        return first < second;
    });
    bool inside = false;
    for(std::size_t index = 0; index < on_line.size(); ++index){
        inside = inside != (toggle_count[on_line[index]] % 2 != 0);
        if(inside && index + 1 < on_line.size()){
            left_graph.add_edge(on_line[index], on_line[index + 1]);
            right_graph.add_edge(on_line[index], on_line[index + 1]);
        }
    }
    return {left_graph.polygons(), right_graph.polygons()};
}
