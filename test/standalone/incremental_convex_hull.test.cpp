// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <vector>

#include "../../src/algorithm/geometry/2d/query/incremental_convex_hull.hpp"

using Point = IntegerPoint<long long>;
using Wide = GeometryIntegerWide;

static std::vector<Point> naive_hull(std::vector<Point> points){
    std::sort(points.begin(), points.end());
    points.erase(std::unique(points.begin(), points.end()), points.end());
    if(points.size() <= 1) return points;
    std::vector<Point> lower;
    std::vector<Point> upper;
    for(const Point& point: points){
        while(lower.size() >= 2 && integer_orientation(
            lower[lower.size() - 2], lower.back(), point
        ) <= 0) lower.pop_back();
        lower.push_back(point);
    }
    for(auto iterator = points.rbegin(); iterator != points.rend(); ++iterator){
        while(upper.size() >= 2 && integer_orientation(
            upper[upper.size() - 2], upper.back(), *iterator
        ) <= 0) upper.pop_back();
        upper.push_back(*iterator);
    }
    lower.pop_back();
    upper.pop_back();
    lower.insert(lower.end(), upper.begin(), upper.end());
    return lower;
}

static Wide cross_origin(const Point& first, const Point& second){
    return Wide(static_cast<__int128_t>(first.x))
            * Wide(static_cast<__int128_t>(second.y))
        - Wide(static_cast<__int128_t>(first.y))
            * Wide(static_cast<__int128_t>(second.x));
}

static Wide naive_area(const std::vector<Point>& hull){
    Wide result = 0;
    for(std::size_t index = 0; index < hull.size(); ++index){
        result += cross_origin(hull[index], hull[(index + 1) % hull.size()]);
    }
    return result.absolute();
}

static Wide dot(const Point& point, const Point& direction){
    return Wide(static_cast<__int128_t>(point.x))
            * Wide(static_cast<__int128_t>(direction.x))
        + Wide(static_cast<__int128_t>(point.y))
            * Wide(static_cast<__int128_t>(direction.y));
}

static int naive_contains(const std::vector<Point>& hull, const Point& point){
    if(hull.empty()) return 0;
    if(hull.size() == 1) return hull[0] == point ? 1 : 0;
    if(hull.size() == 2){
        if(integer_orientation(hull[0], hull[1], point) != 0) return 0;
        return std::min(hull[0].x, hull[1].x) <= point.x &&
            point.x <= std::max(hull[0].x, hull[1].x) &&
            std::min(hull[0].y, hull[1].y) <= point.y &&
            point.y <= std::max(hull[0].y, hull[1].y) ? 1 : 0;
    }
    bool boundary = false;
    for(std::size_t index = 0; index < hull.size(); ++index){
        const int side = integer_orientation(
            hull[index], hull[(index + 1) % hull.size()], point
        );
        if(side < 0) return 0;
        if(side == 0) boundary = true;
    }
    return boundary ? 1 : 2;
}

static void verify(
    const IncrementalConvexHull<long long>& hull,
    const std::vector<Point>& all,
    std::mt19937_64& random
){
    const std::vector<Point> expected = naive_hull(all);
    assert(hull.vertices() == expected);
    assert(hull.size() == expected.size());
    assert(hull.empty() == expected.empty());
    assert(hull.doubled_area() == naive_area(expected));
    for(int iteration = 0; iteration < 40; ++iteration){
        const Point query{
            static_cast<long long>(random() % 61) - 30,
            static_cast<long long>(random() % 61) - 30
        };
        assert(hull.contains(query) == naive_contains(expected, query));
        const Point direction{
            static_cast<long long>(random() % 21) - 10,
            static_cast<long long>(random() % 21) - 10
        };
        const auto actual = hull.support(direction);
        if(expected.empty()){
            assert(!actual.has_value());
        }else{
            Point best = expected.front();
            for(const Point& candidate: expected){
                if(dot(candidate, direction) > dot(best, direction) ||
                    (dot(candidate, direction) == dot(best, direction) &&
                     candidate < best)){
                    best = candidate;
                }
            }
            assert(actual.has_value() && *actual == best);
        }
    }
}

static void test_random(){
    std::mt19937_64 random(2026071651);
    for(int testcase = 0; testcase < 100; ++testcase){
        IncrementalConvexHull<long long> hull;
        std::vector<Point> points;
        verify(hull, points, random);
        for(int step = 0; step < 180; ++step){
            Point point{
                static_cast<long long>(random() % 51) - 25,
                static_cast<long long>(random() % 51) - 25
            };
            const std::vector<Point> before = naive_hull(points);
            points.push_back(point);
            const std::vector<Point> after = naive_hull(points);
            assert(hull.insert(point) == (before != after));
            if(step % 9 == 0) verify(hull, points, random);
        }
        verify(hull, points, random);
    }
}

static void test_boundaries(){
    IncrementalConvexHull<long long> hull;
    for(const Point point: std::vector<Point>{
        {0, 0}, {0, 10}, {0, 5}, {0, -10}, {0, -10},
        {-10, -10}, {-10, 10}, {10, -10}, {10, 10}, {0, 0}
    }) hull.insert(point);
    assert(hull.size() == 4);
    assert(hull.doubled_area() == 800);
    assert(hull.contains({0, 0}) == 2);
    assert(hull.contains({10, 0}) == 1);
    assert(hull.contains({11, 0}) == 0);

    IncrementalConvexHull<long long> extreme;
    const long long low = (std::numeric_limits<long long>::min)();
    const long long high = (std::numeric_limits<long long>::max)();
    assert(extreme.insert({low, low}));
    assert(extreme.insert({high, low}));
    assert(extreme.insert({high, high}));
    assert(extreme.insert({low, high}));
    assert(extreme.size() == 4);
    assert(extreme.contains({0, 0}) == 2);
    assert(extreme.doubled_area().bit_length() >= 128);
}

static void test_large(){
    IncrementalConvexHull<long long> hull;
    constexpr int count = 100000;
    for(int value = 0; value < count; ++value){
        const long long x = value;
        assert(hull.insert({x, x * x}));
    }
    assert(hull.size() == count);
    assert(hull.vertices().size() == count);
    assert(hull.support({0, -1})->x == 0);
    assert(hull.support({1, 0})->x == count - 1);
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int query_count;
    if(!(std::cin >> query_count)){
        test_random();
        test_boundaries();
        test_large();
        return 0;
    }

    IncrementalConvexHull<long long> hull;
    while(query_count-- > 0){
        std::string operation;
        std::cin >> operation;
        if(operation == "ADD"){
            Point point;
            std::cin >> point.x >> point.y;
            hull.insert(point);
        }else if(operation == "CONTAINS"){
            Point point;
            std::cin >> point.x >> point.y;
            std::cout << hull.contains(point) << '\n';
        }else if(operation == "AREA"){
            std::cout << hull.doubled_area() << '\n';
        }else if(operation == "SIZE"){
            std::cout << hull.size() << '\n';
        }else if(operation == "SUPPORT"){
            Point direction;
            std::cin >> direction.x >> direction.y;
            const auto answer = hull.support(direction);
            if(!answer.has_value()) std::cout << "EMPTY\n";
            else std::cout << answer->x << ' ' << answer->y << '\n';
        }else if(operation == "VERTICES"){
            const std::vector<Point> vertices = hull.vertices();
            std::cout << vertices.size();
            for(const Point& point: vertices){
                std::cout << ' ' << point.x << ' ' << point.y;
            }
            std::cout << '\n';
        }
    }
}
