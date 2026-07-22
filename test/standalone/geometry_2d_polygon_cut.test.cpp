// competitive-verifier: STANDALONE

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <exception>
#include <random>
#include <utility>
#include <vector>

#include "../../src/algorithm/geometry/2d/predicate/contains.hpp"
#include "../../src/algorithm/geometry/2d/scalar/polygon_area.hpp"
#include "../../src/algorithm/geometry/2d/shape/polygon_cut.hpp"
#include "geometry_2d_test_common.hpp"

namespace{

struct SlowBoundaryGraph{
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
        std::vector<bool> used(edges.size() * 2, false);
        std::vector<std::vector<Point>> result;
        for(std::size_t initial_edge = 0;
            initial_edge < edges.size(); ++initial_edge){
            for(std::size_t direction = 0; direction < 2; ++direction){
                const std::size_t initial_id =
                    initial_edge * 2 + direction;
                if(used[initial_id]) continue;
                std::size_t from = direction == 0
                    ? edges[initial_edge].first
                    : edges[initial_edge].second;
                std::size_t to = direction == 0
                    ? edges[initial_edge].second
                    : edges[initial_edge].first;
                std::size_t current_edge = initial_edge;
                const std::size_t start_from = from;
                const std::size_t start_to = to;
                std::vector<Point> polygon;
                bool closed = false;
                for(std::size_t step = 0;
                    step <= edges.size() * 2; ++step){
                    const std::size_t current_id =
                        directed_id(current_edge, from);
                    if(used[current_id]) break;
                    used[current_id] = true;
                    polygon.push_back(vertices[from]);
                    const auto& around = adjacency[to];
                    const auto reverse = std::find_if(
                        around.begin(), around.end(),
                        [&](const AdjacentEdge& adjacent){
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
                if(polygon_line_cut_detail::signed_area_sign(polygon) <= 0){
                    continue;
                }
                result.push_back(std::move(polygon));
            }
        }
        return result;
    }
};

PolygonLineCutResult slow_polygon_cut(
    const std::vector<Point>& polygon,
    const Line& line
){
    const Point raw_direction = line.b - line.a;
    const long double direction_length = abs(raw_direction);
    const Point direction = raw_direction / direction_length;
    SlowBoundaryGraph left_graph;
    SlowBoundaryGraph right_graph;
    std::vector<Point> on_line;
    const auto append_on_line = [&](const Point& point){
        if(std::none_of(
            on_line.begin(), on_line.end(),
            [&](const Point& current){ return current == point; }
        )){
            on_line.push_back(point);
        }
    };
    const auto add_clipped = [&](SlowBoundaryGraph& graph,
                                 const Point& first,
                                 const Point& second,
                                 int first_side,
                                 int second_side,
                                 int kept_sign){
        const bool first_inside = first_side * kept_sign >= 0;
        const bool second_inside = second_side * kept_sign >= 0;
        if(first_inside && second_inside){
            graph.add_edge(first, second);
        }else if(first_inside != second_inside){
            const Point intersection = cross_point(
                Line{first, second}, line
            );
            graph.add_edge(
                first_inside ? first : intersection,
                second_inside ? second : intersection
            );
        }
    };
    for(std::size_t index = 0; index < polygon.size(); ++index){
        const Point& first = polygon[index];
        const Point& second = polygon[(index + 1) % polygon.size()];
        const int first_side = geometry_sign(cross(
            direction, first - line.a
        ));
        const int second_side = geometry_sign(cross(
            direction, second - line.a
        ));
        add_clipped(
            left_graph, first, second, first_side, second_side, 1
        );
        add_clipped(
            right_graph, first, second, first_side, second_side, -1
        );
        if(first_side == 0) append_on_line(first);
        if(second_side == 0) append_on_line(second);
        if(first_side * second_side < 0){
            append_on_line(cross_point(Line{first, second}, line));
        }
    }
    std::sort(
        on_line.begin(), on_line.end(),
        [&](const Point& first, const Point& second){
            return dot(first - line.a, direction)
                < dot(second - line.a, direction);
        }
    );
    for(std::size_t index = 1; index < on_line.size(); ++index){
        const Point midpoint =
            (on_line[index - 1] + on_line[index]) / 2.0L;
        if(contains(polygon, midpoint) != 0){
            left_graph.add_edge(on_line[index - 1], on_line[index]);
            right_graph.add_edge(on_line[index - 1], on_line[index]);
        }
    }
    return {left_graph.polygons(), right_graph.polygons()};
}

std::vector<long double> component_areas(
    const std::vector<std::vector<Point>>& polygons
){
    std::vector<long double> result;
    for(const auto& polygon: polygons){
        result.push_back(polygon_area(polygon));
    }
    std::sort(result.begin(), result.end());
    return result;
}

bool same_components(
    const std::vector<std::vector<Point>>& first,
    const std::vector<std::vector<Point>>& second
){
    const auto left = component_areas(first);
    const auto right = component_areas(second);
    if(left.size() != right.size()) return false;
    for(std::size_t index = 0; index < left.size(); ++index){
        if(std::abs(left[index] - right[index])
            > 1.0e-8L * std::max({
                1.0L, std::abs(left[index]), std::abs(right[index])
            })){
            return false;
        }
    }
    return true;
}

bool internal_tests(){
    std::mt19937_64 random(0xD1B54A32D192ED03ULL);
    const long double turn = 2.0L * std::acos(-1.0L);
    for(std::size_t iteration = 0; iteration < 128; ++iteration){
        const std::size_t size = 3 + random() % 12;
        std::vector<Point> polygon;
        for(std::size_t index = 0; index < size; ++index){
            const long double angle =
                turn * static_cast<long double>(index)
                / static_cast<long double>(size);
            const long double radius =
                5.0L + static_cast<long double>(random() % 11);
            polygon.push_back({
                radius * std::cos(angle),
                radius * std::sin(angle),
            });
        }
        Point first{
            static_cast<long double>(
                static_cast<long long>(random() % 31) - 15
            ),
            static_cast<long double>(
                static_cast<long long>(random() % 31) - 15
            ),
        };
        Point second{
            static_cast<long double>(
                static_cast<long long>(random() % 31) - 15
            ),
            static_cast<long double>(
                static_cast<long long>(random() % 31) - 15
            ),
        };
        if(first == second) second.x += 1.0L;
        const Line line{first, second};
        const auto fast = polygon_cut(polygon, line);
        const auto slow = slow_polygon_cut(polygon, line);
        if(!same_components(fast.left, slow.left)
            || !same_components(fast.right, slow.right)){
            return false;
        }
    }

    constexpr std::size_t teeth = 6000;
    std::vector<Point> comb{{0, 0}, {2 * teeth, 0}, {2 * teeth, 3}};
    comb.reserve(4 * teeth + 2);
    for(std::size_t tooth = teeth; tooth-- > 0;){
        const long double x =
            2.0L * static_cast<long double>(tooth);
        comb.push_back({x + 1.0L, 3});
        comb.push_back({x + 1.0L, 1});
        comb.push_back({x, 1});
        if(tooth != 0) comb.push_back({x, 3});
    }
    const auto cut = polygon_cut(
        comb, {{-1, 2}, {2 * teeth + 1, 2}}
    );
    if(cut.left.size() != teeth || cut.right.size() != 1) return false;
    long double left_area = 0.0L;
    long double right_area = 0.0L;
    for(const auto& polygon: cut.left) left_area += polygon_area(polygon);
    for(const auto& polygon: cut.right) right_area += polygon_area(polygon);
    return left_area == static_cast<long double>(teeth)
        && right_area == 3.0L * static_cast<long double>(teeth);
}

}  // namespace

int main(){
    geometry_test_setup_output();
    if(!internal_tests()) return 1;
    int queries;
    std::cin >> queries;
    while(queries--){
        int size;
        std::cin >> size;
        std::vector<Point> polygon(static_cast<std::size_t>(size));
        for(Point& point: polygon) std::cin >> point.x >> point.y;
        Line line;
        std::cin >> line.a.x >> line.a.y >> line.b.x >> line.b.y;
        try{
            const auto result = polygon_cut(polygon, line);
            long double left_area = 0.0L;
            long double right_area = 0.0L;
            for(const auto& component: result.left){
                left_area += polygon_area(component);
            }
            for(const auto& component: result.right){
                right_area += polygon_area(component);
            }
            std::cout << result.left.size() << ' '
                      << result.right.size() << '\n';
            geometry_test_print_real(left_area);
            geometry_test_print_real(right_area);
        }catch(const std::exception&){
            std::cout << "ERR\n";
        }
    }
}
