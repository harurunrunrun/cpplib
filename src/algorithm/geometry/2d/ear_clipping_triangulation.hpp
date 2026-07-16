#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <map>
#include <numeric>
#include <set>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

#include "advanced/detail.hpp"
#include "bentley_ottmann_intersections.hpp"

using EarClippingTriangle = std::array<std::size_t, 3>;

namespace ear_clipping_detail{

struct IndexedVertex{
    Point point;
    std::size_t original_index;
};

inline int signed_area_sign(const std::vector<IndexedVertex>& vertices){
    if(vertices.empty()) return 0;
    const Point origin = vertices.front().point;
    long double value = 0.0L;
    long double roundoff_scale = 0.0L;
    long double coordinate_scale = 0.0L;
    for(std::size_t index = 0; index < vertices.size(); ++index){
        const Point first = vertices[index].point - origin;
        const Point second =
            vertices[(index + 1) % vertices.size()].point - origin;
        value += cross(first, second);
        roundoff_scale +=
            std::abs(first.x * second.y)
            + std::abs(first.y * second.x);
        coordinate_scale = std::max({
            coordinate_scale,
            advanced_geometry_detail::length(first),
            advanced_geometry_detail::length(second),
        });
    }
    return geometry_scaled_sign(
        value,
        coordinate_scale * coordinate_scale,
        roundoff_scale
    );
}

}  // namespace ear_clipping_detail


namespace simple_polygon_triangulation_detail{

using IndexedVertex = ear_clipping_detail::IndexedVertex;

enum class VertexKind{
    start,
    split,
    finish,
    merge,
    regular,
};

inline std::vector<IndexedVertex> collapse_consecutive_vertices(
    const std::vector<Point>& polygon
){
    using advanced_geometry_detail::point_equal;
    std::vector<IndexedVertex> result;
    result.reserve(polygon.size());
    for(std::size_t index = 0; index < polygon.size(); ++index){
        if(!std::isfinite(polygon[index].x)
            || !std::isfinite(polygon[index].y)){
            throw std::invalid_argument(
                "polygon coordinates must be finite"
            );
        }
        if(result.empty() || !point_equal(
            result.back().point, polygon[index]
        )){
            result.push_back({polygon[index], index});
        }
    }
    if(result.size() >= 2 && point_equal(
        result.front().point, result.back().point
    )){
        result.pop_back();
    }
    return result;
}

inline bool boundary_edges_are_adjacent(
    std::size_t first,
    std::size_t second,
    std::size_t count
){
    return (first + 1) % count == second
        || (second + 1) % count == first;
}

inline void validate_simple_sweep(
    const std::vector<IndexedVertex>& vertices
){
    using namespace advanced_geometry_detail;
    const std::size_t count = vertices.size();
    std::vector<Segment> edges(count);
    for(std::size_t index = 0; index < count; ++index){
        edges[index] = {
            vertices[index].point,
            vertices[(index + 1) % count].point,
        };
    }

    for(std::size_t index = 0; index < count; ++index){
        const std::size_t next = (index + 1) % count;
        const std::vector<Point> intersection =
            segment_intersection_set(edges[index], edges[next]);
        if(intersection.size() != 1 || !point_equal(
            intersection.front(), vertices[next].point
        )){
            throw std::invalid_argument(
                "adjacent polygon edges must meet only at their endpoint"
            );
        }
    }

    if(count == std::numeric_limits<std::size_t>::max()){
        throw std::length_error("polygon has too many edges");
    }
    const auto intersections = bentley_ottmann_detail::intersections(
        edges, count + 1
    );
    for(const SegmentIntersectionRecord& record: intersections){
        if(!boundary_edges_are_adjacent(
            record.first, record.second, count
        )){
            throw std::invalid_argument("the polygon is not simple");
        }
    }
}

inline std::vector<IndexedVertex> remove_collinear_vertices(
    std::vector<IndexedVertex> vertices
){
    using namespace advanced_geometry_detail;
    const std::size_t count = vertices.size();
    if(count < 3) return vertices;
    std::vector<std::size_t> previous(count), next(count), queue(count);
    std::vector<bool> active(count, true);
    std::iota(queue.begin(), queue.end(), std::size_t{0});
    for(std::size_t index = 0; index < count; ++index){
        previous[index] = (index + count - 1) % count;
        next[index] = (index + 1) % count;
    }

    std::size_t remaining = count;
    for(std::size_t read = 0; read < queue.size() && remaining >= 3; ++read){
        const std::size_t vertex = queue[read];
        if(!active[vertex]) continue;
        const std::size_t before = previous[vertex];
        const std::size_t after = next[vertex];
        if(cross_sign(
            vertices[vertex].point - vertices[before].point,
            vertices[after].point - vertices[vertex].point
        ) != 0 || !on_segment_scaled(
            {vertices[before].point, vertices[after].point},
            vertices[vertex].point
        )){
            continue;
        }
        active[vertex] = false;
        --remaining;
        next[before] = after;
        previous[after] = before;
        queue.push_back(before);
        queue.push_back(after);
    }

    std::vector<IndexedVertex> result;
    result.reserve(remaining);
    std::size_t first = 0;
    while(first < count && !active[first]) ++first;
    if(first == count) return result;
    std::size_t vertex = first;
    do{
        result.push_back(vertices[vertex]);
        vertex = next[vertex];
    }while(vertex != first);
    return result;
}

inline bool above(const Point& first, const Point& second){
    if(first.y != second.y) return first.y > second.y;
    return first.x < second.x;
}

inline std::vector<Point> make_sweep_points(
    const std::vector<IndexedVertex>& vertices
){
    const std::size_t count = vertices.size();
    std::vector<Segment> swapped_edges(count);
    for(std::size_t index = 0; index < count; ++index){
        const Point first = vertices[index].point;
        const Point second = vertices[(index + 1) % count].point;
        swapped_edges[index] = {
            {first.y, first.x},
            {second.y, second.x},
        };
    }
    const long double shear =
        bentley_ottmann_detail::choose_shear(swapped_edges);
    std::vector<Point> result(count);
    for(std::size_t index = 0; index < count; ++index){
        const Point point = vertices[index].point;
        result[index] = {point.x, point.y + shear * point.x};
    }
    return result;
}

inline std::vector<VertexKind> classify_vertices(
    const std::vector<IndexedVertex>& vertices,
    const std::vector<Point>& sweep_points
){
    using advanced_geometry_detail::cross_sign;
    const std::size_t count = vertices.size();
    std::vector<VertexKind> result(count, VertexKind::regular);
    for(std::size_t vertex = 0; vertex < count; ++vertex){
        const std::size_t before = (vertex + count - 1) % count;
        const std::size_t after = (vertex + 1) % count;
        const bool both_below =
            above(sweep_points[vertex], sweep_points[before])
            && above(sweep_points[vertex], sweep_points[after]);
        const bool both_above =
            above(sweep_points[before], sweep_points[vertex])
            && above(sweep_points[after], sweep_points[vertex]);
        const int turn = cross_sign(
            vertices[vertex].point - vertices[before].point,
            vertices[after].point - vertices[vertex].point
        );
        if(both_below){
            result[vertex] =
                turn > 0 ? VertexKind::start : VertexKind::split;
        }else if(both_above){
            result[vertex] =
                turn > 0 ? VertexKind::finish : VertexKind::merge;
        }
    }
    return result;
}

struct SweepStatusContext{
    const std::vector<Point>* points = nullptr;
    long double y = 0.0L;
    std::size_t probe_id = 0;
    long double probe_x = 0.0L;
};

inline std::pair<long double, long double> sweep_status_key(
    const SweepStatusContext& context,
    std::size_t edge
){
    if(edge == context.probe_id){
        return {
            context.probe_x,
            -std::numeric_limits<long double>::infinity(),
        };
    }
    const std::vector<Point>& points = *context.points;
    const std::size_t count = points.size();
    Point upper = points[edge];
    Point lower = points[(edge + 1) % count];
    if(above(lower, upper)) std::swap(upper, lower);
    const long double height = upper.y - lower.y;
    if(height <= 0.0L){
        throw std::logic_error(
            "sweep transformation left a horizontal boundary edge"
        );
    }
    const long double ratio = (upper.y - context.y) / height;
    const long double x = upper.x + (lower.x - upper.x) * ratio;
    const long double derivative =
        (upper.x - lower.x) / height;
    return {x, -derivative};
}

struct SweepStatusComparator{
    const SweepStatusContext* context = nullptr;

    bool operator()(std::size_t first, std::size_t second) const{
        if(first == second) return false;
        const auto first_key = sweep_status_key(*context, first);
        const auto second_key = sweep_status_key(*context, second);
        if(first_key.first != second_key.first){
            return first_key.first < second_key.first;
        }
        if(first_key.second != second_key.second){
            return first_key.second < second_key.second;
        }
        return first < second;
    }
};

inline std::vector<std::pair<std::size_t, std::size_t>>
monotone_partition_diagonals(
    const std::vector<IndexedVertex>& vertices,
    const std::vector<Point>& sweep_points
){
    const std::size_t count = vertices.size();
    const std::vector<VertexKind> kind =
        classify_vertices(vertices, sweep_points);
    std::vector<std::size_t> order(count);
    std::iota(order.begin(), order.end(), std::size_t{0});
    std::sort(order.begin(), order.end(), [&](std::size_t first, std::size_t second){
        if(above(sweep_points[first], sweep_points[second])) return true;
        if(above(sweep_points[second], sweep_points[first])) return false;
        return first < second;
    });

    SweepStatusContext context{
        &sweep_points, 0.0L, count, 0.0L
    };
    using Status = std::set<std::size_t, SweepStatusComparator>;
    Status status(SweepStatusComparator{&context});
    std::vector<std::size_t> helper(count, count);
    std::set<std::pair<std::size_t, std::size_t>> diagonal_set;

    const auto add_diagonal = [&](std::size_t first, std::size_t second){
        if(first == second
            || (first + 1) % count == second
            || (second + 1) % count == first){
            return;
        }
        if(second < first) std::swap(first, second);
        diagonal_set.insert({first, second});
    };
    const auto insert_edge = [&](std::size_t edge, std::size_t vertex){
        helper[edge] = vertex;
        if(!status.insert(edge).second){
            throw std::logic_error(
                "monotone partition inserted an active edge twice"
            );
        }
    };
    const auto erase_edge = [&](std::size_t edge){
        const auto iterator = status.find(edge);
        if(iterator == status.end()){
            throw std::logic_error(
                "monotone partition could not find an active edge"
            );
        }
        status.erase(iterator);
    };
    const auto add_merge_helper_diagonal =
        [&](std::size_t edge, std::size_t vertex){
            if(helper[edge] < count
                && kind[helper[edge]] == VertexKind::merge){
                add_diagonal(vertex, helper[edge]);
            }
        };
    const auto edge_immediately_left_of = [&](std::size_t vertex){
        context.probe_x = sweep_points[vertex].x;
        auto iterator = status.lower_bound(context.probe_id);
        if(iterator == status.begin()){
            throw std::logic_error(
                "monotone partition found no boundary edge to the left"
            );
        }
        --iterator;
        return *iterator;
    };

    for(const std::size_t vertex: order){
        context.y = sweep_points[vertex].y;
        const std::size_t before = (vertex + count - 1) % count;
        const std::size_t after = (vertex + 1) % count;
        switch(kind[vertex]){
        case VertexKind::start:
            insert_edge(vertex, vertex);
            break;
        case VertexKind::finish:
            add_merge_helper_diagonal(before, vertex);
            erase_edge(before);
            break;
        case VertexKind::split:{
            const std::size_t left =
                edge_immediately_left_of(vertex);
            if(helper[left] >= count){
                throw std::logic_error(
                    "monotone partition left edge has no helper"
                );
            }
            add_diagonal(vertex, helper[left]);
            helper[left] = vertex;
            insert_edge(vertex, vertex);
            break;
        }
        case VertexKind::merge:{
            add_merge_helper_diagonal(before, vertex);
            erase_edge(before);
            const std::size_t left =
                edge_immediately_left_of(vertex);
            add_merge_helper_diagonal(left, vertex);
            helper[left] = vertex;
            break;
        }
        case VertexKind::regular:
            if(above(sweep_points[vertex], sweep_points[after])){
                add_merge_helper_diagonal(before, vertex);
                erase_edge(before);
                insert_edge(vertex, vertex);
            }else{
                const std::size_t left =
                    edge_immediately_left_of(vertex);
                add_merge_helper_diagonal(left, vertex);
                helper[left] = vertex;
            }
            break;
        }
    }
    if(!status.empty()){
        throw std::logic_error(
            "monotone partition left active edges after the sweep"
        );
    }
    return {
        diagonal_set.begin(),
        diagonal_set.end(),
    };
}

inline int direction_half(const Point& direction){
    return direction.y > 0.0L
        || (direction.y == 0.0L && direction.x >= 0.0L)
        ? 0
        : 1;
}

inline std::vector<std::vector<std::size_t>> partition_faces(
    const std::vector<IndexedVertex>& vertices,
    const std::vector<std::pair<std::size_t, std::size_t>>& diagonals
){
    using advanced_geometry_detail::cross_sign;
    const std::size_t count = vertices.size();
    std::vector<std::vector<std::size_t>> adjacency(count);
    for(std::size_t vertex = 0; vertex < count; ++vertex){
        adjacency[vertex].push_back((vertex + count - 1) % count);
        adjacency[vertex].push_back((vertex + 1) % count);
    }
    for(const auto& [first, second]: diagonals){
        adjacency[first].push_back(second);
        adjacency[second].push_back(first);
    }

    std::map<std::pair<std::size_t, std::size_t>, std::size_t>
        angular_position;
    for(std::size_t vertex = 0; vertex < count; ++vertex){
        auto& neighbors = adjacency[vertex];
        std::sort(neighbors.begin(), neighbors.end(),
            [&](std::size_t first, std::size_t second){
                const Point first_direction =
                    vertices[first].point - vertices[vertex].point;
                const Point second_direction =
                    vertices[second].point - vertices[vertex].point;
                const int first_half = direction_half(first_direction);
                const int second_half = direction_half(second_direction);
                if(first_half != second_half){
                    return first_half < second_half;
                }
                const int turn =
                    cross_sign(first_direction, second_direction);
                if(turn != 0) return turn > 0;
                const long double first_length =
                    dot(first_direction, first_direction);
                const long double second_length =
                    dot(second_direction, second_direction);
                if(first_length != second_length){
                    return first_length < second_length;
                }
                return first < second;
            }
        );
        neighbors.erase(
            std::unique(neighbors.begin(), neighbors.end()),
            neighbors.end()
        );
        for(std::size_t position = 0;
            position < neighbors.size(); ++position){
            angular_position[{vertex, neighbors[position]}] = position;
        }
    }

    std::set<std::pair<std::size_t, std::size_t>> visited;
    std::vector<std::vector<std::size_t>> result;
    const std::size_t directed_edge_count =
        2 * (count + diagonals.size());
    for(std::size_t first = 0; first < count; ++first){
        for(const std::size_t second: adjacency[first]){
            if(visited.count({first, second}) != 0) continue;
            const std::size_t start_first = first;
            const std::size_t start_second = second;
            std::size_t from = first;
            std::size_t to = second;
            std::vector<std::size_t> face;
            for(std::size_t step = 0;
                step <= directed_edge_count; ++step){
                if(!visited.insert({from, to}).second){
                    throw std::logic_error(
                        "partition boundary did not form disjoint faces"
                    );
                }
                face.push_back(from);
                const auto position_iterator =
                    angular_position.find({to, from});
                if(position_iterator == angular_position.end()){
                    throw std::logic_error(
                        "partition adjacency is not symmetric"
                    );
                }
                const auto& neighbors = adjacency[to];
                const std::size_t position =
                    position_iterator->second;
                const std::size_t next_position =
                    (position + neighbors.size() - 1)
                    % neighbors.size();
                const std::size_t next = neighbors[next_position];
                from = to;
                to = next;
                if(from == start_first && to == start_second) break;
                if(step == directed_edge_count){
                    throw std::logic_error(
                        "partition face walk did not close"
                    );
                }
            }
            if(face.size() < 3) continue;
            std::vector<IndexedVertex> face_vertices;
            face_vertices.reserve(face.size());
            for(const std::size_t vertex: face){
                face_vertices.push_back(vertices[vertex]);
            }
            const int orientation =
                ear_clipping_detail::signed_area_sign(face_vertices);
            if(orientation > 0){
                result.push_back(std::move(face));
            }else if(orientation == 0){
                throw std::logic_error(
                    "monotone partition produced a zero-area face"
                );
            }
        }
    }
    if(result.size() != diagonals.size() + 1){
        throw std::logic_error(
            "monotone partition produced an unexpected face count"
        );
    }
    return result;
}

inline void append_counterclockwise_triangle(
    const std::vector<IndexedVertex>& vertices,
    std::size_t first,
    std::size_t second,
    std::size_t third,
    std::vector<EarClippingTriangle>& result
){
    const int orientation = advanced_geometry_detail::cross_sign(
        vertices[second].point - vertices[first].point,
        vertices[third].point - vertices[first].point
    );
    if(orientation == 0){
        throw std::logic_error(
            "monotone triangulation produced a zero-area triangle"
        );
    }
    if(orientation < 0) std::swap(second, third);
    result.push_back({
        vertices[first].original_index,
        vertices[second].original_index,
        vertices[third].original_index,
    });
}

inline void triangulate_monotone_face(
    const std::vector<IndexedVertex>& vertices,
    const std::vector<Point>& sweep_points,
    const std::vector<std::size_t>& face,
    std::vector<int>& chain,
    std::vector<EarClippingTriangle>& result
){
    const std::size_t size = face.size();
    if(size < 3){
        throw std::logic_error(
            "a partition face needs at least three vertices"
        );
    }
    std::size_t top_position = 0;
    std::size_t bottom_position = 0;
    for(std::size_t position = 1; position < size; ++position){
        if(above(
            sweep_points[face[position]],
            sweep_points[face[top_position]]
        )){
            top_position = position;
        }
        if(above(
            sweep_points[face[bottom_position]],
            sweep_points[face[position]]
        )){
            bottom_position = position;
        }
    }

    for(const std::size_t vertex: face){
        chain[vertex] = -1;
    }
    chain[face[top_position]] = 0;
    chain[face[bottom_position]] = 0;
    for(std::size_t position = top_position;
        position != bottom_position;){
        const std::size_t next_position = (position + 1) % size;
        if(!above(
            sweep_points[face[position]],
            sweep_points[face[next_position]]
        )){
            throw std::logic_error(
                "partition face is not sweep-monotone"
            );
        }
        position = next_position;
        if(position != bottom_position) chain[face[position]] = 1;
    }
    for(std::size_t position = top_position;
        position != bottom_position;){
        const std::size_t next_position =
            (position + size - 1) % size;
        if(!above(
            sweep_points[face[position]],
            sweep_points[face[next_position]]
        )){
            throw std::logic_error(
                "partition face is not sweep-monotone"
            );
        }
        position = next_position;
        if(position != bottom_position) chain[face[position]] = 2;
    }

    std::vector<std::size_t> order = face;
    std::sort(order.begin(), order.end(),
        [&](std::size_t first, std::size_t second){
            if(above(sweep_points[first], sweep_points[second])) return true;
            if(above(sweep_points[second], sweep_points[first])) return false;
            return first < second;
        }
    );

    const std::size_t result_start = result.size();
    std::vector<std::size_t> stack{order[0], order[1]};
    for(std::size_t position = 2; position + 1 < size; ++position){
        const std::size_t vertex = order[position];
        if(chain[vertex] != chain[stack.back()]){
            while(stack.size() > 1){
                const std::size_t last = stack.back();
                stack.pop_back();
                append_counterclockwise_triangle(
                    vertices, vertex, last, stack.back(), result
                );
            }
            stack.clear();
            stack.push_back(order[position - 1]);
            stack.push_back(vertex);
        }else{
            std::size_t last = stack.back();
            stack.pop_back();
            while(!stack.empty()){
                const int turn = advanced_geometry_detail::cross_sign(
                    vertices[last].point - vertices[vertex].point,
                    vertices[stack.back()].point
                        - vertices[vertex].point
                );
                const bool diagonal_is_inside =
                    (chain[vertex] == 1 && turn < 0)
                    || (chain[vertex] == 2 && turn > 0);
                if(!diagonal_is_inside) break;
                append_counterclockwise_triangle(
                    vertices, vertex, last, stack.back(), result
                );
                last = stack.back();
                stack.pop_back();
            }
            stack.push_back(last);
            stack.push_back(vertex);
        }
    }

    const std::size_t bottom = order.back();
    for(std::size_t position = 0;
        position + 1 < stack.size(); ++position){
        append_counterclockwise_triangle(
            vertices,
            bottom,
            stack[position],
            stack[position + 1],
            result
        );
    }
    if(result.size() - result_start != size - 2){
        throw std::logic_error(
            "monotone face triangulation produced a wrong triangle count"
        );
    }
}

inline std::vector<EarClippingTriangle> triangulate(
    const std::vector<Point>& polygon
){
    std::vector<IndexedVertex> vertices =
        collapse_consecutive_vertices(polygon);
    if(vertices.size() < 3){
        throw std::invalid_argument(
            "a polygon needs three noncollinear vertices"
        );
    }
    validate_simple_sweep(vertices);
    vertices = remove_collinear_vertices(std::move(vertices));
    if(vertices.size() < 3){
        throw std::invalid_argument(
            "a polygon needs three noncollinear vertices"
        );
    }
    const int orientation =
        ear_clipping_detail::signed_area_sign(vertices);
    if(orientation == 0){
        throw std::invalid_argument(
            "a polygon must have positive area"
        );
    }
    if(orientation < 0) std::reverse(vertices.begin(), vertices.end());

    const std::vector<Point> sweep_points =
        make_sweep_points(vertices);
    const auto diagonals =
        monotone_partition_diagonals(vertices, sweep_points);
    const auto faces = partition_faces(vertices, diagonals);
    std::vector<EarClippingTriangle> result;
    result.reserve(vertices.size() - 2);
    std::vector<int> chain(vertices.size(), -1);
    for(const auto& face: faces){
        triangulate_monotone_face(
            vertices, sweep_points, face, chain, result
        );
    }
    if(result.size() != vertices.size() - 2){
        throw std::logic_error(
            "polygon triangulation produced a wrong triangle count"
        );
    }
    return result;
}

}  // namespace simple_polygon_triangulation_detail

inline std::vector<EarClippingTriangle> ear_clipping_triangulation(
    const std::vector<Point>& polygon
){
    return simple_polygon_triangulation_detail::triangulate(polygon);
}
