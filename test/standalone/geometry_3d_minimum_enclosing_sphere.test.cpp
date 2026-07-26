// competitive-verifier: STANDALONE

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <optional>
#include <random>
#include <vector>

#include "../../src/algorithm/geometry/3d/minimum_enclosing_sphere.hpp"

namespace{

bool close_value(long double left, long double right, long double epsilon = 2e-8L){
    return std::abs(left - right) <= epsilon *
        std::max({1.0L, std::abs(left), std::abs(right)});
}

bool close_point(const Point3& left, const Point3& right, long double epsilon = 2e-8L){
    return close_value(left.x, right.x, epsilon) &&
        close_value(left.y, right.y, epsilon) &&
        close_value(left.z, right.z, epsilon);
}

bool contains_all(const Sphere3& sphere, const std::vector<Point3>& points){
    for(const Point3& point: points){
        const long double distance = std::hypot(
            point.x - sphere.center.x,
            point.y - sphere.center.y,
            point.z - sphere.center.z
        );
        if(distance > sphere.radius + 3e-8L * std::max(1.0L, sphere.radius)){
            return false;
        }
    }
    return true;
}

Sphere3 brute_support(const std::vector<Point3>& points){
    Sphere3 best{{}, std::numeric_limits<long double>::infinity()};
    const auto consider = [&](const Sphere3& sphere){
        if(contains_all(sphere, points) && sphere.radius < best.radius) best = sphere;
    };
    for(std::size_t a = 0; a < points.size(); ++a){
        consider(minimum_enclosing_sphere(points[a]));
        for(std::size_t b = a + 1; b < points.size(); ++b){
            consider(minimum_enclosing_sphere(points[a], points[b]));
            for(std::size_t c = b + 1; c < points.size(); ++c){
                consider(minimum_enclosing_sphere(points[a], points[b], points[c]));
                for(std::size_t d = c + 1; d < points.size(); ++d){
                    consider(minimum_enclosing_sphere(
                        points[a], points[b], points[c], points[d]
                    ));
                }
            }
        }
    }
    return best;
}

} // namespace

int main(){
    std::uint64_t seed;
    std::size_t rounds;
    if(!(std::cin >> seed >> rounds) || rounds == 0 || rounds > 10000) return 2;

    const Sphere3 one = minimum_enclosing_sphere(Point3{3, -2, 5});
    if(!close_point(one.center, {3, -2, 5}) || !close_value(one.radius, 0)) return 1;
    const Sphere3 two = minimum_enclosing_sphere(
        Point3{-2, 0, 0}, Point3{4, 0, 0}
    );
    if(!close_point(two.center, {1, 0, 0}) || !close_value(two.radius, 3)) return 1;

    const Sphere3 obtuse = minimum_enclosing_sphere(
        Point3{0, 0, 0}, Point3{4, 0, 0}, Point3{1, 1, 0}
    );
    if(!close_point(obtuse.center, {2, 0, 0}) || !close_value(obtuse.radius, 2)){
        return 1;
    }
    const Sphere3 tetrahedron = minimum_enclosing_sphere(
        Point3{1, 1, 1}, Point3{1, -1, -1},
        Point3{-1, 1, -1}, Point3{-1, -1, 1}
    );
    if(!close_point(tetrahedron.center, {0, 0, 0}) ||
       !close_value(tetrahedron.radius, std::sqrt(3.0L))) return 1;

    const std::vector<Point3> coplanar{{
        {10, 10, 7}, {14, 10, 7}, {14, 14, 7}, {10, 14, 7}, {12, 12, 7},
    }};
    const Sphere3 square = minimum_enclosing_sphere(coplanar, seed);
    if(!close_point(square.center, {12, 12, 7}) ||
       !close_value(square.radius, std::sqrt(8.0L))) return 1;

    const std::vector<Point3> degenerate{{
        {5, 6, 7}, {5, 6, 7}, {7, 6, 7}, {11, 6, 7}, {9, 6, 7},
    }};
    const Sphere3 line = minimum_enclosing_sphere(degenerate, seed);
    if(!close_point(line.center, {8, 6, 7}) || !close_value(line.radius, 3)) return 1;

    const long double shift = 1e15L;
    const std::vector<Point3> translated{{
        {shift - 10, -shift, shift}, {shift + 10, -shift, shift},
        {shift, -shift + 3, shift + 4}, {shift, -shift, shift},
    }};
    const Sphere3 translated_sphere = minimum_enclosing_sphere(translated, seed);
    if(!close_point(translated_sphere.center, {shift, -shift, shift}, 2e-15L) ||
       !close_value(translated_sphere.radius, 10, 2e-12L)) return 1;

    const long double extreme_radius =
        0.75L * std::numeric_limits<long double>::max();
    const Point3 extreme_left{-extreme_radius, 0, 0};
    const Point3 extreme_right{extreme_radius, 0, 0};
    const Sphere3 extreme_pair = minimum_enclosing_sphere(
        extreme_left, extreme_right
    );
    if(extreme_pair.center.x != 0.0L || extreme_pair.center.y != 0.0L
        || extreme_pair.center.z != 0.0L
        || extreme_pair.radius != extreme_radius) return 1;
    const Sphere3 extreme_vector = minimum_enclosing_sphere(
        std::vector<Point3>{extreme_left, {}, extreme_right}, seed
    );
    if(extreme_vector.center.x != 0.0L || extreme_vector.center.y != 0.0L
        || extreme_vector.center.z != 0.0L
        || extreme_vector.radius != extreme_radius) return 1;

    const long double tiny_radius = 1e-3000L;
    const Sphere3 tiny_vector = minimum_enclosing_sphere(
        std::vector<Point3>{{0, 0, 0}, {2 * tiny_radius, 0, 0}}, seed
    );
    if(tiny_vector.center.x != tiny_radius
        || tiny_vector.center.y != 0.0L || tiny_vector.center.z != 0.0L
        || tiny_vector.radius != tiny_radius) return 1;

    bool empty_threw = false;
    try{
        static_cast<void>(minimum_enclosing_sphere(std::vector<Point3>{}));
    }catch(const std::invalid_argument&){
        empty_threw = true;
    }
    if(!empty_threw) return 1;

    std::mt19937_64 random(seed);
    std::uniform_int_distribution<int> coordinate(-20, 20);
    for(std::size_t iteration = 0; iteration < rounds; ++iteration){
        const std::size_t size = 2 + static_cast<std::size_t>(random() % 9);
        std::vector<Point3> points;
        points.reserve(size);
        for(std::size_t index = 0; index < size; ++index){
            points.push_back({
                static_cast<long double>(coordinate(random)),
                static_cast<long double>(coordinate(random)),
                static_cast<long double>(coordinate(random)),
            });
            if(index > 0 && random() % 7 == 0) points.back() = points[index - 1];
        }
        const Sphere3 actual = minimum_enclosing_sphere(points, random());
        const Sphere3 expected = brute_support(points);
        if(!contains_all(actual, points) ||
           !close_value(actual.radius, expected.radius, 3e-8L)) return 1;
    }

    std::cout << "OK\n";
    return 0;
}
