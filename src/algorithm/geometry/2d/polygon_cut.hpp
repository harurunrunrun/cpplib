#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "area.hpp"
#include "contains.hpp"
#include "cross_point.hpp"

struct PolygonLineCutResult{
    std::vector<std::vector<Point>> left;
    std::vector<std::vector<Point>> right;
};

namespace polygon_line_cut_detail{

struct BoundaryGraph{
    std::vector<Point> vertices;
    std::vector<std::pair<std::size_t, std::size_t>> edges;

    std::size_t vertex(const Point& point){
        for(std::size_t index = 0; index < vertices.size(); ++index){
            if(vertices[index] == point) return index;
        }
        vertices.push_back(point);
        return vertices.size() - 1;
    }

    void add_edge(const Point& first, const Point& second){
        if(first == second) return;
        std::size_t first_index = vertex(first);
        std::size_t second_index = vertex(second);
        if(second_index < first_index) std::swap(first_index, second_index);
        const std::pair edge{first_index, second_index};
        if(std::find(edges.begin(), edges.end(), edge) == edges.end()){
            edges.push_back(edge);
        }
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
            std::sort(adjacency[vertex].begin(), adjacency[vertex].end(),
                [&](const AdjacentEdge& first, const AdjacentEdge& second){
                    const Point first_direction = vertices[first.to] - vertices[vertex];
                    const Point second_direction = vertices[second.to] - vertices[vertex];
                    const long double first_angle =
                        std::atan2(first_direction.y, first_direction.x);
                    const long double second_angle =
                        std::atan2(second_direction.y, second_direction.x);
                    if(first_angle != second_angle) return first_angle < second_angle;
                    return abs(first_direction) < abs(second_direction);
                });
        }
        const auto directed_id = [&](std::size_t edge, std::size_t from){
            return edge * 2 + (edges[edge].first == from ? 0U : 1U);
        };
        std::vector<bool> used(edges.size() * 2, false);
        std::vector<std::vector<Point>> result;
        for(std::size_t initial_edge = 0; initial_edge < edges.size(); ++initial_edge){
            for(std::size_t direction = 0; direction < 2; ++direction){
                const std::size_t initial_id = initial_edge * 2 + direction;
                if(used[initial_id]) continue;
                std::size_t from = direction == 0
                    ? edges[initial_edge].first : edges[initial_edge].second;
                std::size_t to = direction == 0
                    ? edges[initial_edge].second : edges[initial_edge].first;
                std::size_t current_edge = initial_edge;
                const std::size_t start_from = from;
                const std::size_t start_to = to;
                std::vector<Point> polygon;
                bool closed = false;
                for(std::size_t step = 0; step <= edges.size() * 2; ++step){
                    const std::size_t current_id = directed_id(current_edge, from);
                    if(used[current_id]) break;
                    used[current_id] = true;
                    polygon.push_back(vertices[from]);
                    const auto& around = adjacency[to];
                    const auto reverse = std::find_if(
                        around.begin(), around.end(), [&](const AdjacentEdge& adjacent){
                            return adjacent.edge == current_edge;
                        }
                    );
                    if(reverse == around.end()) break;
                    const std::size_t reverse_index =
                        static_cast<std::size_t>(reverse - around.begin());
                    const AdjacentEdge& next = around[
                        (reverse_index + around.size() - 1) % around.size()
                    ];
                    from = to;
                    to = next.to;
                    current_edge = next.edge;
                    if(from == start_from && to == start_to){
                        closed = true;
                        break;
                    }
                }
                if(!closed || polygon.size() < 3) continue;
                if(geometry_sign(area(polygon)) <= 0) continue;
                result.push_back(std::move(polygon));
            }
        }
        return result;
    }
};

inline void add_clipped_edge(
    BoundaryGraph& graph,
    const Point& first,
    const Point& second,
    int first_side,
    int second_side,
    const Line& line,
    int kept_sign
){
    const bool first_inside = first_side * kept_sign >= 0;
    const bool second_inside = second_side * kept_sign >= 0;
    if(first_inside && second_inside){
        graph.add_edge(first, second);
    }else if(first_inside != second_inside){
        const Point intersection = cross_point(Line{first, second}, line);
        graph.add_edge(first_inside ? first : intersection,
                       second_inside ? second : intersection);
    }
}

} // namespace polygon_line_cut_detail

inline PolygonLineCutResult polygon_cut(
    const std::vector<Point>& polygon,
    const Line& line
){
    const Point direction = line.b - line.a;
    if(geometry_sign(abs(direction)) == 0)[[unlikely]]{
        throw std::invalid_argument("degenerate line");
    }
    polygon_line_cut_detail::BoundaryGraph left_graph;
    polygon_line_cut_detail::BoundaryGraph right_graph;
    std::vector<Point> on_line;
    const auto append_on_line = [&](const Point& point){
        if(std::none_of(on_line.begin(), on_line.end(), [&](const Point& current){
            return current == point;
        })) on_line.push_back(point);
    };
    for(std::size_t index = 0; index < polygon.size(); ++index){
        const Point& first = polygon[index];
        const Point& second = polygon[(index + 1) % polygon.size()];
        const int first_side = geometry_sign(cross(direction, first - line.a));
        const int second_side = geometry_sign(cross(direction, second - line.a));
        polygon_line_cut_detail::add_clipped_edge(
            left_graph, first, second, first_side, second_side, line, 1
        );
        polygon_line_cut_detail::add_clipped_edge(
            right_graph, first, second, first_side, second_side, line, -1
        );
        if(first_side == 0) append_on_line(first);
        if(second_side == 0) append_on_line(second);
        if(first_side * second_side < 0){
            append_on_line(cross_point(Line{first, second}, line));
        }
    }
    std::sort(on_line.begin(), on_line.end(), [&](const Point& first, const Point& second){
        return dot(first - line.a, direction) < dot(second - line.a, direction);
    });
    for(std::size_t index = 1; index < on_line.size(); ++index){
        const Point midpoint = (on_line[index - 1] + on_line[index]) / 2.0L;
        if(contains(polygon, midpoint) != 0){
            left_graph.add_edge(on_line[index - 1], on_line[index]);
            right_graph.add_edge(on_line[index - 1], on_line[index]);
        }
    }
    return {left_graph.polygons(), right_graph.polygons()};
}
