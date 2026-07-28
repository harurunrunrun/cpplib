// competitive-verifier: STANDALONE
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <numeric>
#include <set>
#include <utility>
#include <vector>

#include "../../src/algorithm/geometry/2d/result/point_set/straight_skeleton.hpp"

namespace{

class DisjointSet{
private:
    std::vector<std::size_t> parent_;
public:
    explicit DisjointSet(std::size_t size) : parent_(size){
        std::iota(parent_.begin(), parent_.end(), 0);
    }
    std::size_t root(std::size_t vertex){
        while(parent_[vertex] != vertex){
            parent_[vertex] = parent_[parent_[vertex]];
            vertex = parent_[vertex];
        }
        return vertex;
    }
    void unite(std::size_t first, std::size_t second){
        first = root(first);
        second = root(second);
        if(first != second) parent_[second] = first;
    }
};

bool same_time(long double first, long double second){
    const long double scale = std::max({
        1.0L, std::abs(first), std::abs(second),
    });
    return std::abs(first - second)
        <= 8192.0L * std::numeric_limits<long double>::epsilon() * scale;
}

void validate(
    const std::vector<Point>& polygon,
    const StraightSkeletonResult& result,
    int minimum_split_events,
    bool require_distinct_simultaneous_events
){
    const std::size_t count = result.vertices.size();
    assert(count == result.times.size());
    assert(count == result.kinds.size());
    assert(count >= polygon.size() + 1);
    for(std::size_t index = 0; index < polygon.size(); ++index){
        assert(plus_geometry_detail::close(
            result.vertices[index], polygon[index]
        ));
        assert(result.times[index] == 0.0L);
        assert(result.kinds[index]
            == StraightSkeletonVertexKind::contour_vertex);
    }

    std::vector<int> degree(count, 0);
    std::set<std::pair<std::size_t, std::size_t>> unique_edges;
    DisjointSet disjoint_set(count);
    for(auto [first, second]: result.edges){
        assert(first < count && second < count && first != second);
        if(first > second) std::swap(first, second);
        assert(unique_edges.insert({first, second}).second);
        ++degree[first];
        ++degree[second];
        disjoint_set.unite(first, second);
    }
    for(std::size_t first_index = 0;
        first_index < result.edges.size(); ++first_index){
        const auto [first_a, first_b] = result.edges[first_index];
        for(std::size_t second_index = first_index + 1;
            second_index < result.edges.size(); ++second_index){
            const auto [second_a, second_b] = result.edges[second_index];
            if(first_a == second_a || first_a == second_b
                || first_b == second_a || first_b == second_b){
                continue;
            }
            assert(!straight_skeleton_detail::skeleton_segments_intersect(
                result.vertices[first_a], result.vertices[first_b],
                result.vertices[second_a], result.vertices[second_b]
            ));
        }
    }
    for(const auto& [first, second]: result.edges){
        for(int step = 1; step < 8; ++step){
            const Point point = (
                result.vertices[first] * static_cast<long double>(8 - step)
                + result.vertices[second] * static_cast<long double>(step)
            ) / 8.0L;
            assert(plus_geometry_detail::point_in_polygon(polygon, point) != 0);
        }
    }
    if(result.edges.size() + 1 != count){
        std::cerr << "straight-skeleton graph is not a tree: vertices="
            << count << ", edges=" << result.edges.size() << '\n';
        for(std::size_t index = 0; index < count; ++index){
            std::cerr << "vertex " << index << ": "
                << result.vertices[index].x << ' '
                << result.vertices[index].y << ", time="
                << result.times[index] << ", kind="
                << static_cast<int>(result.kinds[index]) << '\n';
        }
        for(const auto& [first, second]: result.edges){
            std::cerr << "edge " << first << ' ' << second << '\n';
        }
    }
    assert(result.edges.size() + 1 == count);
    for(std::size_t index = 1; index < count; ++index){
        assert(disjoint_set.root(index) == disjoint_set.root(0));
    }
    for(std::size_t index = 0; index < polygon.size(); ++index){
        assert(degree[index] == 1);
    }

    int split_events = 0;
    for(std::size_t index = polygon.size(); index < count; ++index){
        assert(std::isfinite(result.vertices[index].x));
        assert(std::isfinite(result.vertices[index].y));
        assert(std::isfinite(result.times[index]));
        assert(result.times[index] > 0.0L);
        assert(plus_geometry_detail::point_in_polygon(
            polygon, result.vertices[index]
        ) != 0);
        if(result.kinds[index] == StraightSkeletonVertexKind::split_event
            || result.kinds[index]
                == StraightSkeletonVertexKind::simultaneous_event){
            ++split_events;
        }
    }
    assert(split_events >= minimum_split_events);

    if(require_distinct_simultaneous_events){
        bool found = false;
        for(std::size_t first = polygon.size(); first < count; ++first){
            for(std::size_t second = first + 1; second < count; ++second){
                if(same_time(result.times[first], result.times[second])
                    && !plus_geometry_detail::close(
                        result.vertices[first], result.vertices[second]
                    )){
                    found = true;
                }
            }
        }
        assert(found);
    }
}

}  // namespace

int main(){
    int cases = 0;
    if(!(std::cin >> cases)) return 0;
    while(cases-- > 0){
        int size = 0;
        int minimum_split_events = 0;
        int require_distinct_simultaneous_events = 0;
        std::cin >> size >> minimum_split_events
            >> require_distinct_simultaneous_events;
        std::vector<Point> polygon(static_cast<std::size_t>(size));
        for(Point& point: polygon) std::cin >> point.x >> point.y;
        StraightSkeletonResult result;
        try{
            result = straight_skeleton(polygon);
        }catch(const std::exception& error){
            std::cerr << "straight-skeleton failed for polygon:\n";
            for(const Point& point: polygon){
                std::cerr << point.x << ' ' << point.y << '\n';
            }
            std::cerr << error.what() << '\n';
            throw;
        }
        validate(
            polygon, result, minimum_split_events,
            require_distinct_simultaneous_events != 0
        );
        std::cout << "OK\n";
    }

    const std::vector<Point> closed_square{
        {0, 0}, {5, 0}, {5, 5}, {0, 5}, {0, 0},
    };
    validate(
        std::vector<Point>(closed_square.begin(), closed_square.end() - 1),
        straight_skeleton(closed_square), 0, false
    );

    bool rejected = false;
    try{
        static_cast<void>(straight_skeleton({
            {0, 0}, {3, 3}, {0, 3}, {3, 0},
        }));
    }catch(const std::invalid_argument&){
        rejected = true;
    }
    assert(rejected);
    rejected = false;
    try{
        static_cast<void>(straight_skeleton({
            {0, 0}, {2, 0}, {4, 0}, {4, 3}, {0, 3},
        }));
    }catch(const std::invalid_argument&){
        rejected = true;
    }
    assert(rejected);
}
