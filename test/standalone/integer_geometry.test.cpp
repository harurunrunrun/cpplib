// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <optional>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/integer_geometry/aggregate/all/convex.hpp"
#include "../../src/algorithm/integer_geometry/aggregate/all/distance.hpp"
#include "../../src/algorithm/integer_geometry/aggregate/all/integer_geometry.hpp"
#include "../../src/algorithm/integer_geometry/aggregate/all/intersection.hpp"
#include "../../src/algorithm/integer_geometry/aggregate/all/lattice.hpp"
#include "../../src/algorithm/integer_geometry/aggregate/all/polygon.hpp"
#include "../../src/algorithm/integer_geometry/aggregate/all/predicates.hpp"
#include "../../src/algorithm/integer_geometry/aggregate/all/types.hpp"
#include "../../src/algorithm/integer_geometry/enumeration/point_set_point/point_in_polygon.hpp"
#include "../../src/algorithm/integer_geometry/enumeration/segment_segment/segment_intersection_kind.hpp"
#include "../../src/algorithm/integer_geometry/integer/integer_point_set/lattice_polygon_metrics.hpp"
#include "../../src/algorithm/integer_geometry/integer/point_point_point/orientation.hpp"
#include "../../src/algorithm/integer_geometry/point/integer_point/rational_point.hpp"
#include "../../src/algorithm/integer_geometry/point/point_point/add_points.hpp"
#include "../../src/algorithm/integer_geometry/point_set/point_set/convex_hull.hpp"
#include "../../src/algorithm/integer_geometry/point_set/point_set_point_set/minkowski_sum.hpp"
#include "../../src/algorithm/integer_geometry/point_set/segment_segment/segment_intersection.hpp"
#include "../../src/algorithm/integer_geometry/predicate/line_line/direction_relations.hpp"
#include "../../src/algorithm/integer_geometry/predicate/line_point/on_line.hpp"
#include "../../src/algorithm/integer_geometry/predicate/point_set/is_convex.hpp"
#include "../../src/algorithm/integer_geometry/predicate/segment_point/on_segment.hpp"
#include "../../src/algorithm/integer_geometry/predicate/segment_segment/direction_relations.hpp"
#include "../../src/algorithm/integer_geometry/predicate/segment_segment/segments_intersect.hpp"
#include "../../src/algorithm/integer_geometry/predicate/vector_vector/direction_relations.hpp"
#include "../../src/algorithm/integer_geometry/result/integer_point_set/lattice_point_count.hpp"
#include "../../src/algorithm/integer_geometry/result/line_line/line_intersection.hpp"
#include "../../src/algorithm/integer_geometry/result/point_set/point_set_squared_metrics.hpp"
#include "../../src/algorithm/integer_geometry/result/segment_segment/segment_intersection_point.hpp"
#include "../../src/algorithm/integer_geometry/scalar/point_point/squared_distance.hpp"
#include "../../src/algorithm/integer_geometry/scalar/point_point_point/cross_points.hpp"
#include "../../src/algorithm/integer_geometry/scalar/point_set/polygon_area.hpp"
#include "../../src/algorithm/integer_geometry/scalar/vector/squared_norm.hpp"
#include "../../src/algorithm/integer_geometry/scalar/vector_vector/vector_products.hpp"
#include "../../src/algorithm/integer_geometry/type/definition/geometry_types.hpp"
#include "../../src/algorithm/integer_geometry/type/definition/lattice_types.hpp"
#include "../../src/algorithm/integer_geometry/type/definition/rational.hpp"
#include "../../src/algorithm/integer_geometry/vector/line/direction_line.hpp"
#include "../../src/algorithm/integer_geometry/vector/point_point/vector_from.hpp"
#include "../../src/algorithm/integer_geometry/vector/segment/direction_segment.hpp"

namespace{

using namespace integer_geometry;

Rational brute_closest(const std::vector<Point>& points){
    std::optional<Rational> best;
    for(std::size_t first = 0; first < points.size(); ++first){
        for(std::size_t second = first + 1; second < points.size(); ++second){
            const Rational candidate = squared_distance(
                points[first], points[second]
            );
            if(!best || candidate < *best) best = candidate;
        }
    }
    return *best;
}

Rational brute_diameter(const std::vector<Point>& points){
    Rational best = 0;
    for(const Point& first: points){
        for(const Point& second: points){
            best = std::max(best, squared_distance(first, second));
        }
    }
    return best;
}

std::vector<Point> brute_minkowski(
    const std::vector<Point>& first,
    const std::vector<Point>& second
){
    std::vector<Point> sums;
    for(const Point& left: first){
        for(const Point& right: second){
            sums.push_back(add_points(left, right));
        }
    }
    return convex_hull(std::move(sums));
}

template<class Exception, class Function>
bool throws(Function function){
    try{
        function();
    }catch(const Exception&){
        return true;
    }
    return false;
}

void fixed_tests(){
    assert(Rational(12, -18) == Rational(-2, 3));
    assert(Rational(-12, -18) == Rational(2, 3));
    assert(Rational(0, -99) == Rational(0));
    assert(Rational(1, 2) + Rational(1, 3) == Rational(5, 6));
    assert(Rational(7, 9) - Rational(5, 6) == Rational(-1, 18));
    assert(Rational(21, 10) * Rational(-15, 14) == Rational(-9, 4));
    assert(Rational(21, 10) / Rational(-15, 14) == Rational(-49, 25));
    assert(Rational(-3, 5) < Rational(-1, 2));
    assert(abs(Rational(-12, 35)) == Rational(12, 35));
    assert(throws<std::invalid_argument>([]{ (void)Rational(1, 0); }));
    assert(throws<std::domain_error>([]{
        (void)(Rational(1) / Rational(0));
    }));

    Integer huge = 1;
    huge <<= 300;
    const Rational large(huge + 1, huge - 1);
    assert(large.numerator() == huge + 1);
    assert(large.denominator() == huge - 1);
    assert(large * large.reciprocal() == Rational(1));
    assert(large - large == Rational(0));
    const Rational reducible((huge + 1) * 37, (huge - 1) * 37);
    assert(reducible == large);

    const Point first{Rational(0), Rational(0)};
    const Point second{Rational(3, 2), Rational(3, 2)};
    const Point third{Rational(0), Rational(2)};
    assert(orientation(first, second, third) > 0);
    assert(dot(Vector{2, -3}, Vector{3, 2}).is_zero());
    assert(cross(Vector{2, 0}, Vector{0, 3}) == Rational(6));
    assert(on_segment({first, second}, Point{Rational(1), Rational(1)}));
    assert(segment_intersection_kind(
        {{0, 0}, {3, 3}}, {{0, 2}, {3, 0}}
    ) == SegmentIntersectionKind::proper);
    const auto crossing = segment_intersection_point(
        {{0, 0}, {3, 3}}, {{0, 2}, {3, 0}}
    );
    assert((crossing == std::optional<Point>{Point{Rational(6, 5), Rational(6, 5)}}));
    const std::vector<Point> overlap = segment_intersection(
        {{0, 0}, {4, 0}}, {{2, 0}, {6, 0}}
    );
    assert((overlap == std::vector<Point>{{2, 0}, {4, 0}}));
    assert(!line_intersection(
        {{0, 0}, {1, 0}}, {{0, 2}, {1, 2}}
    ));

    const std::vector<Point> rectangle = {
        {0, 0}, {Rational(7, 2), 0},
        {Rational(7, 2), Rational(5, 3)}, {0, Rational(5, 3)},
    };
    assert(polygon_twice_signed_area(rectangle) == Rational(35, 3));
    assert(polygon_area(rectangle) == Rational(35, 6));
    assert(point_in_polygon(rectangle, {1, 1}) == PointLocation::inside);
    assert(point_in_polygon(rectangle, {Rational(7, 2), 1})
        == PointLocation::boundary);
    assert(point_in_polygon(rectangle, {4, 1}) == PointLocation::outside);

    const std::vector<Point> points = {
        {0, 0}, {2, 0}, {2, 2}, {0, 2}, {1, 0}, {1, 1}, {0, 0},
    };
    const std::vector<Point> hull = convex_hull(points);
    assert(hull.size() == 4);
    assert(is_convex(hull, true));
    assert(closest_pair_squared(points) == std::optional<Rational>{0});
    assert(convex_diameter_squared(hull) == std::optional<Rational>{8});
    assert(point_set_diameter_squared(points) == std::optional<Rational>{8});
    assert(minkowski_sum({{0, 0}, {2, 0}}, {{0, 0}, {0, 3}})
        == brute_minkowski({{0, 0}, {2, 0}}, {{0, 0}, {0, 3}}));

    Integer width = 1;
    width <<= 240;
    const std::vector<IntegerPoint> lattice_rectangle = {
        {0, 0}, {width, 0}, {width, 3}, {0, 3},
    };
    assert(lattice_polygon_twice_area(lattice_rectangle) == width * 6);
    assert(lattice_boundary_points(lattice_rectangle) == width * 2 + 6);
    assert(lattice_interior_points(lattice_rectangle) == (width - 1) * 2);
    const LatticePointCount count = lattice_point_count(lattice_rectangle);
    assert(count.boundary == width * 2 + 6);
    assert(count.interior == (width - 1) * 2);
    assert((rational_point(IntegerPoint{huge, -huge})
        == Point{Rational(huge), Rational(-huge)}));

    assert(convex_hull({}).empty());
    assert(!closest_pair_squared({}));
    assert(!closest_pair_squared({{0, 0}}));
    assert(!point_set_diameter_squared({}));
    assert(!convex_diameter_squared({}));
    assert(throws<std::invalid_argument>([]{
        (void)lattice_interior_points({{0, 0}, {1, 0}});
    }));
}

void randomized_tests(std::uint64_t seed, int point_count){
    std::mt19937_64 engine(seed);
    std::uniform_int_distribution<std::int64_t> coordinate(-1000, 1000);
    std::vector<Point> points(static_cast<std::size_t>(point_count));
    for(Point& point: points){
        point = Point{coordinate(engine), coordinate(engine)};
    }
    assert(closest_pair_squared(points) == brute_closest(points));
    assert(point_set_diameter_squared(points) == brute_diameter(points));

    std::vector<Point> hull = convex_hull(points);
    if(seed % 2 == 0) std::reverse(hull.begin(), hull.end());
    assert(convex_diameter_squared(hull) == brute_diameter(points));

    std::vector<Point> first(
        points.begin(), points.begin() + point_count / 2
    );
    std::vector<Point> second(
        points.begin() + point_count / 2, points.end()
    );
    first = convex_hull(std::move(first));
    second = convex_hull(std::move(second));
    std::vector<Point> expected = brute_minkowski(first, second);
    if(seed % 3 == 0) std::reverse(first.begin(), first.end());
    if(seed % 5 == 0) std::reverse(second.begin(), second.end());
    std::vector<Point> actual = minkowski_sum_convex(first, second);
    std::sort(expected.begin(), expected.end());
    std::sort(actual.begin(), actual.end());
    assert(actual == expected);
}

}  // namespace

int main(){
    int test_count = 0;
    if(!(std::cin >> test_count)) return 0;
    fixed_tests();
    while(test_count-- > 0){
        std::uint64_t seed = 0;
        int point_count = 0;
        std::cin >> seed >> point_count;
        randomized_tests(seed, point_count);
        std::cout << "OK\n";
    }
}
