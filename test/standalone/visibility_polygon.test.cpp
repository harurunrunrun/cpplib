// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/geometry/2d/visibility_polygon.hpp"

namespace{

long double ray_distance(
    const std::vector<Point>& polygon,
    const Point& origin,
    long double angle
){
    const Point direction{std::cos(angle), std::sin(angle)};
    long double best = std::numeric_limits<long double>::infinity();
    for(std::size_t index = 0; index < polygon.size(); ++index){
        const Point first = polygon[index];
        const Point edge = polygon[(index + 1) % polygon.size()] - first;
        const Point offset = first - origin;
        const long double denominator = cross(direction, edge);
        if(std::fabs(denominator) < 1e-18L) continue;
        const long double distance = cross(offset, edge) / denominator;
        const long double position = cross(offset, direction) / denominator;
        if(distance >= -1e-9L
            && position >= -1e-9L && position <= 1.0L + 1e-9L){
            best = std::min(best, std::max(0.0L, distance));
        }
    }
    return best;
}

bool on_boundary(
    const std::vector<Point>& polygon,
    const Point& point,
    long double tolerance
){
    for(std::size_t index = 0; index < polygon.size(); ++index){
        const Point first = polygon[index];
        const Point second = polygon[(index + 1) % polygon.size()];
        const Point edge = second - first;
        if(std::fabs(cross(edge, point - first))
                <= tolerance * std::max(1.0L, std::sqrt(norm(edge)))
            && dot(point - first, point - second) <= tolerance){
            return true;
        }
    }
    return false;
}

void validate_result(
    const std::vector<Point>& polygon,
    const Point& observer,
    const std::vector<Point>& result
){
    assert(result.size() >= 3);
    long double scale = 1.0L;
    for(const Point& point: polygon){
        scale = std::max(
            scale, std::max(std::fabs(point.x), std::fabs(point.y))
        );
    }
    const long double tolerance = 2e-6L * scale;
    long double twice_area = 0.0L;
    for(std::size_t index = 0; index < result.size(); ++index){
        const Point& point = result[index];
        assert(std::isfinite(point.x) && std::isfinite(point.y));
        assert(on_boundary(polygon, point, tolerance));
        assert(contains(polygon, (observer + point) / 2.0L) != 0);
        const Point first = result[index] - observer;
        const Point second = result[(index + 1) % result.size()] - observer;
        assert(cross(first, second) >= -tolerance);
        twice_area += cross(point, result[(index + 1) % result.size()]);
    }
    assert(twice_area > 0.0L);

    std::vector<long double> samples;
    const long double turn = 2.0L * std::acos(-1.0L);
    for(int index = 0; index < 144; ++index){
        samples.push_back(turn * index / 144.0L);
    }
    for(const Point& vertex: polygon){
        samples.push_back(std::atan2(
            vertex.y - observer.y, vertex.x - observer.x
        ));
    }
    for(const long double angle: samples){
        const long double expected = ray_distance(polygon, observer, angle);
        const long double actual = ray_distance(result, observer, angle);
        assert(std::isfinite(expected) && std::isfinite(actual));
        assert(std::fabs(expected - actual)
            <= tolerance * std::max({1.0L, expected, actual}));
    }
}

}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    std::cin >> test_count;
    while(test_count-- > 0){
        int valid, vertex_count;
        Point observer;
        std::cin >> valid >> vertex_count >> observer.x >> observer.y;
        std::vector<Point> polygon(static_cast<std::size_t>(vertex_count));
        for(Point& point: polygon) std::cin >> point.x >> point.y;
        try{
            const auto result = visibility_polygon(polygon, observer);
            assert(valid == 1);
            validate_result(polygon, observer, result);
            std::cout << "OK\n";
        }catch(const std::invalid_argument&){
            assert(valid == 0);
            std::cout << "ERR\n";
        }
    }
}
