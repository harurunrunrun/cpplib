// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/geometry/2d/circle_union_area.hpp"

static bool close(long double actual, long double expected, long double tolerance){
    return std::abs(actual - expected)
        <= tolerance * std::max({1.0L, std::abs(actual), std::abs(expected)});
}

static void test_exact_configurations(){
    const long double pi = std::acos(-1.0L);
    assert(circle_union_area({}) == 0.0L);
    assert(close(circle_union_area({{{0, 0}, 2}}), 4 * pi, 1e-14L));
    assert(close(circle_union_area({{{0, 0}, 2}, {{0, 0}, 1}}), 4 * pi, 1e-14L));
    assert(close(circle_union_area({{{0, 0}, 2}, {{0, 0}, 2}}), 4 * pi, 1e-14L));
    assert(close(circle_union_area({{{-3, 0}, 1}, {{3, 0}, 2}}), 5 * pi, 1e-14L));
    const long double two_unit =
        4.0L * pi / 3.0L + std::sqrt(3.0L) / 2.0L;
    assert(close(circle_union_area({{{0, 0}, 1}, {{1, 0}, 1}}), two_unit, 1e-13L));
}

static void test_invariance_and_large(){
    std::mt19937_64 rng(2026071621);
    std::vector<Circle> circles;
    for(int index = 0; index < 250; ++index){
        circles.push_back({
            {static_cast<long double>(static_cast<long long>(rng() % 2001) - 1000),
             static_cast<long double>(static_cast<long long>(rng() % 2001) - 1000)},
            static_cast<long double>(1 + rng() % 100)
        });
    }
    const long double area = circle_union_area(circles);
    assert(area >= 0.0L);
    long double sum = 0.0L;
    for(const Circle& circle: circles){
        sum += std::acos(-1.0L) * circle.radius * circle.radius;
    }
    assert(area <= sum * (1.0L + 1e-12L));

    std::shuffle(circles.begin(), circles.end(), rng);
    assert(close(circle_union_area(circles), area, 1e-12L));
    for(Circle& circle: circles){
        circle.center.x += 123.25L;
        circle.center.y -= 456.5L;
    }
    assert(close(circle_union_area(circles), area, 1e-12L));
    for(Circle& circle: circles){
        circle.center.x *= 3.0L;
        circle.center.y *= 3.0L;
        circle.radius *= 3.0L;
    }
    assert(close(circle_union_area(circles), area * 9.0L, 1e-12L));
}

static void test_invalid(){
    bool thrown = false;
    try{ (void)circle_union_area({{{0, 0}, -1}}); }
    catch(const std::invalid_argument&){ thrown = true; }
    assert(thrown);
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t case_count;
    if(!(std::cin >> case_count)){
        test_exact_configurations();
        test_invariance_and_large();
        test_invalid();
        return 0;
    }
    while(case_count-- > 0){
        std::size_t circle_count;
        std::cin >> circle_count;
        std::vector<Circle> circles(circle_count);
        for(Circle& circle: circles){
            std::cin >> circle.center.x >> circle.center.y >> circle.radius;
        }
        long double expected, tolerance;
        std::cin >> expected >> tolerance;
        std::cout << close(circle_union_area(circles), expected, tolerance) << '\n';
    }
}

