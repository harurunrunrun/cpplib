// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <random>
#include <limits>
#include <vector>

#include "../../src/algorithm/integer_geometry/convex.hpp"
#include "../../src/algorithm/integer_geometry/distance.hpp"
#include "../../src/algorithm/integer_geometry/integer_geometry.hpp"
#include "../../src/algorithm/integer_geometry/polygon.hpp"
#include "../../src/algorithm/integer_geometry/predicates.hpp"
#include "../../src/algorithm/integer_geometry/rational_intersection.hpp"
#include "../../src/algorithm/integer_geometry/types.hpp"

namespace{

using namespace integer_geometry;

UnsignedWide independent_squared_distance(
    const Point& first,
    const Point& second
){
    const Wide x_difference =
        static_cast<Wide>(first.x) - static_cast<Wide>(second.x);
    const Wide y_difference =
        static_cast<Wide>(first.y) - static_cast<Wide>(second.y);
    return static_cast<UnsignedWide>(x_difference * x_difference)
        + static_cast<UnsignedWide>(y_difference * y_difference);
}

UnsignedWide brute_closest(const std::vector<Point>& points){
    UnsignedWide result = ~UnsignedWide{0};
    for(std::size_t first = 0; first < points.size(); ++first){
        for(std::size_t second = first + 1; second < points.size(); ++second){
            result = std::min(result, independent_squared_distance(
                points[first], points[second]
            ));
        }
    }
    return result;
}

UnsignedWide brute_diameter(const std::vector<Point>& points){
    UnsignedWide result = 0;
    for(const Point& first: points){
        for(const Point& second: points){
            result = std::max(
                result, independent_squared_distance(first, second)
            );
        }
    }
    return result;
}

std::vector<Point> brute_minkowski(
    const std::vector<Point>& first,
    const std::vector<Point>& second
){
    std::vector<Point> sums;
    for(const Point& left: first){
        for(const Point& right: second){
            sums.push_back({
                static_cast<Coordinate>(
                    static_cast<Wide>(left.x) + right.x),
                static_cast<Coordinate>(
                    static_cast<Wide>(left.y) + right.y),
            });
        }
    }
    return convex_hull(std::move(sums));
}

void sort_points(std::vector<Point>& points){
    std::sort(points.begin(), points.end());
}

template<class Function>
bool throws_overflow(Function function){
    try{
        function();
    }catch(const std::overflow_error&){
        return true;
    }
    return false;
}

template<class Function>
bool throws_invalid_argument(Function function){
    try{
        function();
    }catch(const std::invalid_argument&){
        return true;
    }
    return false;
}

void fixed_tests(){
    const Coordinate minimum = std::numeric_limits<Coordinate>::min();
    const Coordinate maximum = std::numeric_limits<Coordinate>::max();
    const Vector full_width = vector_from(
        {std::numeric_limits<Coordinate>::min(), 0},
        {std::numeric_limits<Coordinate>::max(), 0}
    );
    assert(full_width.x == static_cast<Wide>(~std::uint64_t{0}));
    const UnsignedWide maximum_difference =
        static_cast<UnsignedWide>(full_width.x);
    const UnsignedWide maximum_axis_square =
        maximum_difference * maximum_difference;
    assert(squared_distance({minimum, 0}, {maximum, 0})
        == maximum_axis_square);
    assert(closest_pair_squared({{minimum, 0}, {maximum, 0}}).value()
        == maximum_axis_square);
    assert(throws_overflow([&]{
        (void)dot(full_width, full_width);
    }));
    assert(orientation(
        {minimum, minimum},
        {maximum, minimum},
        {minimum, maximum}
    ) == 1);
    assert(ccw(
        {minimum, minimum},
        {maximum, minimum},
        {minimum, maximum}
    ) == 1);
    assert(throws_overflow([&]{
        (void)cross(
            {minimum, minimum},
            {maximum, minimum},
            {minimum, maximum}
        );
    }));
    assert(segment_intersection_kind(
        {{minimum, minimum}, {maximum, maximum}},
        {{minimum, maximum}, {maximum, minimum}}
    ) == SegmentIntersectionKind::proper);
    const std::vector<Point> extreme_square = {
        {minimum, minimum},
        {maximum, minimum},
        {maximum, maximum},
        {minimum, maximum},
    };
    assert(is_convex(extreme_square, true));
    std::vector<Point> extreme_hull = convex_hull({
        {maximum, maximum},
        {minimum, maximum},
        {maximum, minimum},
        {minimum, minimum},
    });
    std::vector<Point> sorted_extreme_square = extreme_square;
    sort_points(extreme_hull);
    sort_points(sorted_extreme_square);
    assert(extreme_hull == sorted_extreme_square);
    auto extreme_translation =
        minkowski_sum_convex(extreme_square, {{0, 0}});
    sort_points(extreme_translation);
    assert(extreme_translation == sorted_extreme_square);
    assert(throws_overflow([&]{
        (void)convex_diameter_squared(extreme_square);
    }));
    assert(throws_overflow([&]{
        (void)squared_distance(
            {minimum, minimum},
            {maximum, maximum}
        );
    }));
    assert(closest_pair_squared({
        {minimum, minimum},
        {minimum + 1, minimum},
        {maximum, maximum},
    }).value() == 1);
    assert(throws_overflow([&]{
        (void)closest_pair_squared({
            {minimum, minimum},
            {maximum, maximum},
        });
    }));
    assert(throws_overflow([]{
        constexpr Coordinate large = 7'000'000'000'000'000'000LL;
        (void)polygon_twice_signed_area({
            {large, 0}, {0, large}, {-large, 0}, {0, -large},
        });
    }));
    constexpr Coordinate large_intersection_coordinate =
        1'000'000'000'000'000LL;
    const auto large_intersection = line_intersection(
        {{-large_intersection_coordinate, -large_intersection_coordinate},
         {large_intersection_coordinate, large_intersection_coordinate}},
        {{-large_intersection_coordinate, large_intersection_coordinate},
         {large_intersection_coordinate, -large_intersection_coordinate}}
    );
    assert(large_intersection.has_value());
    assert(large_intersection->x == Rational(0, 1));
    assert(large_intersection->y == Rational(0, 1));


    const Point low{-4'000'000'000LL, -3'000'000'000LL};
    const Point right{4'000'000'000LL, -3'000'000'000LL};
    const Point high{-4'000'000'000LL, 3'000'000'000LL};
    assert(orientation(low, right, high) == 1);
    assert(ccw(low, right, high) == 1);
    assert(dot(vector_from(low, right), vector_from(low, high)) == 0);
    assert(cross(low, right, high)
        == Wide{48} * Wide{1'000'000'000'000'000'000LL});

    assert(on_segment({{0, 0}, {4, 0}}, {2, 0}));
    assert(!on_segment({{0, 0}, {4, 0}}, {5, 0}));
    assert(segment_intersection_kind(
        {{0, 0}, {4, 4}}, {{0, 4}, {4, 0}}
    ) == SegmentIntersectionKind::proper);
    assert(segment_intersection_kind(
        {{0, 0}, {2, 0}}, {{2, 0}, {3, 2}}
    ) == SegmentIntersectionKind::touch);
    assert(segment_intersection_kind(
        {{0, 0}, {4, 0}}, {{2, 0}, {6, 0}}
    ) == SegmentIntersectionKind::overlap);
    assert(segment_intersection_kind(
        {{1, 1}, {1, 1}}, {{1, 1}, {1, 1}}
    ) == SegmentIntersectionKind::touch);
    assert(segment_intersection_kind(
        {{1, 1}, {1, 1}}, {{2, 2}, {2, 2}}
    ) == SegmentIntersectionKind::none);

    const auto crossing = segment_intersection_point(
        {{0, 0}, {3, 3}}, {{0, 2}, {3, 0}}
    );
    assert(crossing.has_value());
    assert(crossing->x == Rational(6, 5));
    assert(crossing->y == Rational(6, 5));
    assert(Rational(-12, -10) == Rational(6, 5));
    assert(Rational(12, -10) == Rational(-6, 5));
    constexpr Wide wide_minimum = std::numeric_limits<Wide>::min();
    constexpr Wide power_126 = Wide{1} << 126;
    assert(Rational(wide_minimum, -2) == Rational(power_126, 1));
    assert(Rational(2, wide_minimum) == Rational(-1, power_126));
    assert(Rational(0, wide_minimum) == Rational(0, 1));
    assert(throws_overflow([&]{
        (void)Rational(wide_minimum, -1);
    }));
    assert(throws_overflow([&]{
        (void)Rational(1, wide_minimum);
    }));
    assert(throws_invalid_argument([]{
        (void)Rational(1, 0);
    }));
    assert(!segment_intersection_point(
        {{0, 0}, {4, 0}}, {{2, 0}, {6, 0}}
    ).has_value());

    const std::vector<Point> rectangle = {
        {0, 0}, {4, 0}, {4, 3}, {0, 3},
    };
    std::vector<Point> clockwise = rectangle;
    std::reverse(clockwise.begin(), clockwise.end());
    assert(polygon_twice_signed_area(rectangle) == 24);
    assert(polygon_twice_signed_area(clockwise) == -24);
    assert(polygon_twice_area(clockwise) == 24);
    assert(point_in_polygon(clockwise, {2, 1}) == PointLocation::inside);
    assert(point_in_polygon(clockwise, {4, 1}) == PointLocation::boundary);
    assert(point_in_polygon(clockwise, {5, 1}) == PointLocation::outside);
    assert(lattice_boundary_points(rectangle) == 14);
    assert(lattice_interior_points(rectangle) == 6);
    const LatticePointCount count = lattice_point_count(rectangle);
    assert(count.boundary == 14 && count.interior == 6);
    assert(point_in_polygon({{2, 3}}, {2, 3})
        == PointLocation::boundary);
    assert(point_in_polygon({{2, 3}}, {2, 4})
        == PointLocation::outside);
    assert(point_in_polygon({{0, 0}, {4, 0}}, {2, 0})
        == PointLocation::boundary);
    assert(throws_invalid_argument([]{
        (void)lattice_interior_points({{0, 0}, {1, 0}});
    }));

    const std::vector<Point> with_duplicates = {
        {0, 0}, {2, 0}, {2, 2}, {0, 2}, {1, 0}, {0, 0}, {1, 1},
    };
    const std::vector<Point> hull = convex_hull(with_duplicates);
    assert(hull.size() == 4);
    assert(is_convex(hull, true));
    assert(!is_convex({{0, 0}, {2, 2}, {0, 2}, {2, 0}}));
    assert(closest_pair_squared(with_duplicates).value() == 0);
    assert(convex_diameter_squared(hull).value() == 8);
    std::vector<Point> reversed_hull = hull;
    std::reverse(reversed_hull.begin(), reversed_hull.end());
    assert(convex_diameter_squared(reversed_hull).value() == 8);
    assert(point_set_diameter_squared(with_duplicates).value() == 8);
    assert(parallel({{0, 0}, {2, 2}}, {{4, 1}, {7, 4}}));
    assert(orthogonal({{0, 0}, {2, 2}}, {{0, 0}, {-3, 3}}));
    assert(segments_intersect(
        {{0, 0}, {4, 4}}, {{0, 4}, {4, 0}}
    ));
    assert(squared_distance({-4, 5}, {2, -3}) == 100);
    assert((add_points({2, -3}, {-5, 7}) == Point{-3, 4}));
    assert((rational_point({2, -3}) == RationalPoint{
        Rational(2, 1), Rational(-3, 1)
    }));

    const Line parallel_first{{0, 0}, {1, 0}};
    const Line parallel_second{{0, 2}, {1, 2}};
    const Line degenerate{{3, 4}, {3, 4}};
    assert(!line_intersection(parallel_first, parallel_second).has_value());
    assert(!line_intersection(degenerate, parallel_first).has_value());
    const auto line_crossing = line_intersection(
        Line{{0, 0}, {3, 3}},
        Line{{0, 2}, {3, 0}}
    );
    assert((line_crossing == std::optional<RationalPoint>{
        RationalPoint{Rational(6, 5), Rational(6, 5)}
    }));

    assert(convex_hull({}).empty());
    assert((convex_hull({{7, -2}}) == std::vector<Point>{{7, -2}}));
    assert((convex_hull({{0, 0}, {1, 1}, {2, 2}, {1, 1}})
        == std::vector<Point>{{0, 0}, {2, 2}}));
    assert(!closest_pair_squared({}).has_value());
    assert(!closest_pair_squared({{0, 0}}).has_value());
    assert(!point_set_diameter_squared({}).has_value());

    const std::vector<Point> nonconvex_first = {
        {0, 0}, {2, 0}, {0, 2}, {1, 1},
    };
    const std::vector<Point> nonconvex_second = {
        {0, 0}, {1, 0}, {0, 1}, {0, 0},
    };
    assert(minkowski_sum(nonconvex_first, nonconvex_second)
        == brute_minkowski(nonconvex_first, nonconvex_second));

    const std::vector<Point> reversed_segment = {{3, 1}, {-1, 0}};
    const std::vector<Point> small_triangle = {
        {0, 0}, {2, 0}, {0, 2},
    };
    std::vector<Point> expected_segment_sum =
        brute_minkowski(reversed_segment, small_triangle);
    std::vector<Point> actual_segment_sum =
        minkowski_sum_convex(reversed_segment, small_triangle);
    sort_points(expected_segment_sum);
    sort_points(actual_segment_sum);
    assert(actual_segment_sum == expected_segment_sum);

    const std::vector<Point> second_segment = {{0, 4}, {2, -1}};
    expected_segment_sum =
        brute_minkowski(reversed_segment, second_segment);
    actual_segment_sum =
        minkowski_sum_convex(reversed_segment, second_segment);
    sort_points(expected_segment_sum);
    sort_points(actual_segment_sum);
    assert(actual_segment_sum == expected_segment_sum);

    assert(minkowski_sum_convex({{7, -3}}, small_triangle)
        == brute_minkowski({{7, -3}}, small_triangle));

    const Vector vector{2, -3};
    assert((vector + Vector{4, 5} == Vector{6, 2}));
    assert((vector - Vector{4, 5} == Vector{-2, -8}));
    assert((-vector == Vector{-2, 3}));
    assert((vector * Wide{3} == Vector{6, -9}));
    assert(point_in_polygon({}, {0, 0}) == PointLocation::outside);
    assert(polygon_twice_signed_area({}) == 0);
    assert(polygon_twice_area({}) == 0);
    assert(lattice_boundary_points({}) == 0);
    assert(!convex_diameter_squared({}).has_value());
    assert(minkowski_sum_convex({}, {{0, 0}}).empty());
    assert(minkowski_sum({}, {{0, 0}}).empty());
    assert(is_convex(clockwise, true));
}

void randomized_tests(std::uint64_t seed, int point_count){
    assert(point_count >= 2);
    std::mt19937_64 engine(seed);
    std::uniform_int_distribution<std::int64_t> coordinate(-1000, 1000);
    std::vector<Point> points(
        static_cast<std::size_t>(point_count)
    );
    for(Point& point: points){
        point = {coordinate(engine), coordinate(engine)};
    }
    for(std::size_t index = 2; index < points.size(); ++index){
        const Point& first = points[index - 2];
        const Point& second = points[index - 1];
        const Point& third = points[index];
        const Wide first_x = static_cast<Wide>(second.x) - first.x;
        const Wide first_y = static_cast<Wide>(second.y) - first.y;
        const Wide second_x = static_cast<Wide>(third.x) - first.x;
        const Wide second_y = static_cast<Wide>(third.y) - first.y;
        const Wide determinant =
            first_x * second_y - first_y * second_x;
        const int expected_sign =
            (determinant > 0) - (determinant < 0);
        assert(orientation(first, second, third) == expected_sign);
    }
    assert(closest_pair_squared(points).value() == brute_closest(points));
    assert(point_set_diameter_squared(points).value() == brute_diameter(points));
    std::vector<Point> point_hull = convex_hull(points);
    if(seed % 5 == 0) std::reverse(point_hull.begin(), point_hull.end());
    assert(convex_diameter_squared(point_hull).value() == brute_diameter(points));

    std::vector<Point> first(points.begin(), points.begin() + point_count / 2);
    std::vector<Point> second(points.begin() + point_count / 2, points.end());
    first = convex_hull(std::move(first));
    second = convex_hull(std::move(second));
    std::vector<Point> expected = brute_minkowski(first, second);
    if(seed % 2 == 0) std::reverse(first.begin(), first.end());
    if(seed % 3 == 0) std::reverse(second.begin(), second.end());
    std::vector<Point> actual = minkowski_sum_convex(first, second);
    sort_points(expected);
    sort_points(actual);
    assert(actual == expected);

    for(std::size_t degenerate_size = 1;
        degenerate_size <= 2; ++degenerate_size){
        std::vector<Point> degenerate(
            points.begin(),
            points.begin() + static_cast<std::ptrdiff_t>(degenerate_size)
        );
        std::vector<Point> other(
            points.begin() + static_cast<std::ptrdiff_t>(degenerate_size),
            points.end()
        );
        degenerate = convex_hull(std::move(degenerate));
        other = convex_hull(std::move(other));
        std::vector<Point> degenerate_expected =
            brute_minkowski(degenerate, other);
        std::vector<Point> degenerate_actual =
            minkowski_sum_convex(degenerate, other);
        sort_points(degenerate_expected);
        sort_points(degenerate_actual);
        assert(degenerate_actual == degenerate_expected);
    }
}

}  // namespace

int main(){
    int test_count;
    std::cin >> test_count;
    fixed_tests();
    while(test_count-- > 0){
        std::uint64_t seed;
        int point_count;
        std::cin >> seed >> point_count;
        randomized_tests(seed, point_count);
        std::cout << "OK\n";
    }
}
