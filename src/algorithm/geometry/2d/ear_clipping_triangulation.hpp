#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <set>
#include <stdexcept>
#include <vector>

#include "advanced/detail.hpp"

using EarClippingTriangle = std::array<std::size_t, 3>;

namespace ear_clipping_detail{

struct IndexedVertex{
    Point point;
    std::size_t original_index;
};

inline std::vector<IndexedVertex> normalize_polygon(
    const std::vector<Point>& polygon
){
    using namespace advanced_geometry_detail;
    std::vector<IndexedVertex> vertices;
    vertices.reserve(polygon.size());
    for(std::size_t index = 0; index < polygon.size(); ++index){
        if(vertices.empty() || !point_equal(vertices.back().point, polygon[index])){
            vertices.push_back({polygon[index], index});
        }
    }
    if(vertices.size() >= 2
        && point_equal(vertices.front().point, vertices.back().point)){
        vertices.pop_back();
    }

    for(std::size_t first = 0; first < vertices.size(); ++first){
        for(std::size_t second = first + 1; second < vertices.size(); ++second){
            if(point_equal(vertices[first].point, vertices[second].point)){
                throw std::invalid_argument(
                    "a simple polygon cannot repeat a nonconsecutive vertex"
                );
            }
        }
    }

    bool changed = true;
    while(changed && vertices.size() >= 3){
        changed = false;
        for(std::size_t index = 0; index < vertices.size(); ++index){
            const std::size_t previous =
                (index + vertices.size() - 1) % vertices.size();
            const std::size_t next = (index + 1) % vertices.size();
            if(cross_sign(
                vertices[index].point - vertices[previous].point,
                vertices[next].point - vertices[index].point
            ) == 0 && on_segment_scaled(
                {vertices[previous].point, vertices[next].point},
                vertices[index].point
            )){
                vertices.erase(vertices.begin() + static_cast<std::ptrdiff_t>(index));
                changed = true;
                break;
            }
        }
    }
    return vertices;
}

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

inline void validate_simple(const std::vector<IndexedVertex>& vertices){
    using namespace advanced_geometry_detail;
    const std::size_t count = vertices.size();
    for(std::size_t first = 0; first < count; ++first){
        const Segment first_edge{
            vertices[first].point, vertices[(first + 1) % count].point
        };
        for(std::size_t second = first + 1; second < count; ++second){
            if((first + 1) % count == second
                || (second + 1) % count == first){
                continue;
            }
            const Segment second_edge{
                vertices[second].point,
                vertices[(second + 1) % count].point
            };
            if(!segment_intersection_set(first_edge, second_edge).empty()){
                throw std::invalid_argument("the polygon is not simple");
            }
        }
    }
}

inline bool point_in_triangle_closed(
    const Point& point,
    const Point& first,
    const Point& second,
    const Point& third
){
    using namespace advanced_geometry_detail;
    return cross_sign(second - first, point - first) >= 0
        && cross_sign(third - second, point - second) >= 0
        && cross_sign(first - third, point - third) >= 0;
}

}  // namespace ear_clipping_detail

inline std::vector<EarClippingTriangle> ear_clipping_triangulation(
    const std::vector<Point>& polygon
){
    using namespace advanced_geometry_detail;
    using ear_clipping_detail::IndexedVertex;
    std::vector<IndexedVertex> vertices =
        ear_clipping_detail::normalize_polygon(polygon);
    if(vertices.size() < 3){
        throw std::invalid_argument("a polygon needs three noncollinear vertices");
    }
    const int orientation = ear_clipping_detail::signed_area_sign(vertices);
    if(orientation == 0){
        throw std::invalid_argument("a polygon must have positive area");
    }
    if(orientation < 0) std::reverse(vertices.begin(), vertices.end());
    ear_clipping_detail::validate_simple(vertices);

    const std::size_t count = vertices.size();
    std::vector<std::size_t> previous(count), next(count);
    std::vector<bool> active(count, true), reflex(count, false);
    for(std::size_t index = 0; index < count; ++index){
        previous[index] = (index + count - 1) % count;
        next[index] = (index + 1) % count;
    }

    const auto update_reflex = [&](std::size_t index){
        reflex[index] = cross_sign(
            vertices[index].point - vertices[previous[index]].point,
            vertices[next[index]].point - vertices[index].point
        ) < 0;
    };
    for(std::size_t index = 0; index < count; ++index) update_reflex(index);

    const auto is_ear = [&](std::size_t index){
        if(!active[index] || reflex[index]) return false;
        const std::size_t first = previous[index];
        const std::size_t third = next[index];
        if(cross_sign(
            vertices[index].point - vertices[first].point,
            vertices[third].point - vertices[index].point
        ) <= 0){
            return false;
        }
        for(std::size_t candidate = 0; candidate < count; ++candidate){
            if(!active[candidate] || !reflex[candidate]
                || candidate == first || candidate == index
                || candidate == third){
                continue;
            }
            if(ear_clipping_detail::point_in_triangle_closed(
                vertices[candidate].point,
                vertices[first].point,
                vertices[index].point,
                vertices[third].point
            )){
                return false;
            }
        }
        return true;
    };

    std::set<std::size_t> ears;
    for(std::size_t index = 0; index < count; ++index){
        if(is_ear(index)) ears.insert(index);
    }

    std::vector<EarClippingTriangle> result;
    result.reserve(count - 2);
    std::size_t remaining = count;
    while(remaining > 3){
        if(ears.empty()){
            throw std::invalid_argument("failed to find an ear in the polygon");
        }
        const std::size_t ear = *ears.begin();
        ears.erase(ears.begin());
        if(!is_ear(ear)) continue;

        const std::size_t first = previous[ear];
        const std::size_t third = next[ear];
        result.push_back({
            vertices[first].original_index,
            vertices[ear].original_index,
            vertices[third].original_index,
        });
        active[ear] = false;
        --remaining;
        next[first] = third;
        previous[third] = first;

        ears.erase(first);
        ears.erase(third);
        update_reflex(first);
        update_reflex(third);
        if(is_ear(first)) ears.insert(first);
        if(is_ear(third)) ears.insert(third);
    }

    std::array<std::size_t, 3> last{};
    std::size_t write = 0;
    for(std::size_t index = 0; index < count; ++index){
        if(active[index]) last[write++] = index;
    }
    if(write != 3){
        throw std::logic_error("ear clipping active vertex count mismatch");
    }
    if(cross_sign(
        vertices[last[1]].point - vertices[last[0]].point,
        vertices[last[2]].point - vertices[last[1]].point
    ) < 0){
        std::swap(last[1], last[2]);
    }
    result.push_back({
        vertices[last[0]].original_index,
        vertices[last[1]].original_index,
        vertices[last[2]].original_index,
    });
    return result;
}
