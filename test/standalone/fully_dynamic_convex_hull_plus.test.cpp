// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <map>
#include <vector>

#include "../../src/algorithm/geometry/2d/point_set/point_set/convex_hull.hpp"
#include "../../src/algorithm/geometry/2d/scalar/point_point/dot.hpp"
#include "../../src/algorithm/geometry/2d/type/definition/convex_polygon_query.hpp"
#include "../../src/algorithm/geometry/2d/type/definition/fully_dynamic_convex_hull.hpp"

namespace{

std::uint64_t next_random(std::uint64_t& state){
    state ^= state << 7;
    state ^= state >> 9;
    return state;
}

bool same_point(const Point& first, const Point& second){
    return first.x == second.x && first.y == second.y;
}

std::vector<Point> active_points(const std::map<std::size_t, Point>& active){
    std::vector<Point> points;
    points.reserve(active.size());
    for(const auto& [id, point]: active){
        static_cast<void>(id);
        points.push_back(point);
    }
    return points;
}

void verify_state(
    const FullyDynamicConvexHull& dynamic_hull,
    const std::map<std::size_t, Point>& active,
    std::uint64_t& state
){
    const std::vector<Point> points = active_points(active);
    const std::vector<Point> expected = convex_hull(points);
    const std::vector<Point> actual = dynamic_hull.vertices();
    if(actual.size() != expected.size()){
        std::cerr << "points";
        for(const Point& point: points) std::cerr << " (" << point.x << ',' << point.y << ')';
        std::cerr << "\nexpected";
        for(const Point& point: expected) std::cerr << " (" << point.x << ',' << point.y << ')';
        std::cerr << "\nactual";
        for(const Point& point: actual) std::cerr << " (" << point.x << ',' << point.y << ')';
        std::cerr << '\n';
    }
    assert(actual.size() == expected.size());
    for(std::size_t index = 0; index < actual.size(); ++index){
        assert(same_point(actual[index], expected[index]));
    }

    const Point query{
        static_cast<long double>(static_cast<int>(next_random(state) % 101) - 50)
            / 2.0L,
        static_cast<long double>(static_cast<int>(next_random(state) % 101) - 50)
            / 2.0L,
    };
    const int expected_location = ConvexPolygonQuery(expected).contains(query);
    assert(dynamic_hull.contains(query) == expected_location);

    const Point direction{
        static_cast<long double>(static_cast<int>(next_random(state) % 101) - 50),
        static_cast<long double>(static_cast<int>(next_random(state) % 101) - 50),
    };
    const auto support = dynamic_hull.support(direction);
    if(points.empty()){
        assert(!support.has_value());
    }else{
        assert(support.has_value());
        long double best = -std::numeric_limits<long double>::infinity();
        for(const Point& point: points){
            best = std::max(best, dot(point, direction));
        }
        const long double value = dot(*support, direction);
        assert(std::abs(value - best) <= 1.0e-12L
            * std::max({1.0L, std::abs(value), std::abs(best)}));
    }
}

void run_case(std::uint64_t seed){
    FullyDynamicConvexHull hull;
    std::map<std::size_t, Point> active;

    assert(hull.insert(1000, {7, -3}));
    assert(hull.insert(1001, {7, -3}));
    assert(hull.insert(1002, {7, 5}));
    active.emplace(1000, Point{7, -3});
    active.emplace(1001, Point{7, -3});
    active.emplace(1002, Point{7, 5});
    verify_state(hull, active, seed);
    assert(hull.erase(1000));
    active.erase(1000);
    verify_state(hull, active, seed);
    assert(hull.erase(1002));
    active.erase(1002);
    verify_state(hull, active, seed);

    for(std::size_t operation = 0; operation < 4000; ++operation){
        const std::size_t id = next_random(seed) % 256;
        if(next_random(seed) % 5 < 3){
            const Point point{
                static_cast<long double>(
                    static_cast<int>(next_random(seed) % 61) - 30
                ),
                static_cast<long double>(
                    static_cast<int>(next_random(seed) % 61) - 30
                ),
            };
            const bool inserted = hull.insert(id, point);
            const bool expected = active.emplace(id, point).second;
            assert(inserted == expected);
        }else{
            const bool erased = hull.erase(id);
            const bool expected = active.erase(id) != 0;
            assert(erased == expected);
        }
        assert(hull.size() == active.size());
        assert(hull.empty() == active.empty());
        assert(hull.contains_id(id) == active.contains(id));
        if(operation % 3 == 0) verify_state(hull, active, seed);
    }
}

}  // namespace

int main(){
    int cases = 0;
    if(!(std::cin >> cases)) return 0;
    for(int case_index = 0; case_index < cases; ++case_index){
        run_case(0x3141592653589793ULL
            ^ static_cast<std::uint64_t>(case_index + 1));
        std::cout << "OK\n";
    }
}
