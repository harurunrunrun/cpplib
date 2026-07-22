// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/geometry/2d/abs.hpp"
#include "../../src/algorithm/geometry/2d/angle_bisectors.hpp"
#include "../../src/algorithm/geometry/2d/circumcircle.hpp"
#include "../../src/algorithm/geometry/2d/contains.hpp"
#include "../../src/algorithm/geometry/2d/convex_diameter.hpp"
#include "../../src/algorithm/geometry/2d/convex_hull_detail.hpp"
#include "../../src/algorithm/geometry/2d/convex_polygon_minimum_width.hpp"
#include "../../src/algorithm/geometry/2d/convex_polygon_query.hpp"
#include "../../src/algorithm/geometry/2d/counterclockwise_polygon.hpp"
#include "../../src/algorithm/geometry/2d/incircle.hpp"
#include "../../src/algorithm/geometry/2d/incenter.hpp"
#include "../../src/algorithm/geometry/2d/is_convex.hpp"
#include "../../src/algorithm/geometry/2d/minimum_enclosing_circle.hpp"
#include "../../src/algorithm/geometry/2d/orthocenter.hpp"
#include "../../src/algorithm/geometry/2d/polygon_boundary_points/polygon_boundary_point_query.hpp"
#include "../../src/algorithm/geometry/2d/polygon_centroid.hpp"
#include "../../src/algorithm/geometry/2d/triangle_centroid.hpp"
#include "../../src/algorithm/geometry/2d/triangle_contains.hpp"
#include "../../src/algorithm/geometry/2d/triangle_contains_strict.hpp"
#include "../../src/algorithm/geometry/2d/visibility_polygon.hpp"

namespace{

bool near(long double first, long double second, long double scale){
    return std::abs(first - second) <= 2e-7L * scale;
}

void check_point(
    const Point& point,
    const Point& expected,
    long double scale
){
    assert(near(point.x, expected.x, scale));
    assert(near(point.y, expected.y, scale));
}

long double anchored_twice_area(const std::vector<Point>& polygon){
    if(polygon.size() < 3) return 0.0L;
    long double result = 0.0L;
    for(std::size_t index = 1; index + 1 < polygon.size(); ++index){
        result += cross(
            polygon[index] - polygon[0],
            polygon[index + 1] - polygon[0]
        );
    }
    return result;
}

bool polygon_validation_rejects(const std::vector<Point>& polygon){
    try{
        visibility_polygon_detail::validate_simple_polygon(polygon);
    }catch(const std::invalid_argument&){
        return true;
    }
    return false;
}

void check_visibility_polygon_validation(){
    const long double offset = 1e18L;
    const std::vector<Point> translated_thin_rectangle = {
        {offset, offset},
        {offset + 1e9L, offset},
        {offset + 1e9L, offset + 1.0L},
        {offset, offset + 1.0L},
    };
    assert(!polygon_validation_rejects(translated_thin_rectangle));

    const std::vector<Point> proper_crossing = {
        {0.0L, 0.0L},
        {4.0L, 4.0L},
        {0.0L, 4.0L},
        {4.0L, 0.0L},
        {5.0L, 2.0L},
    };
    assert(polygon_validation_rejects(proper_crossing));

    const std::vector<Point> nonadjacent_touch = {
        {0.0L, 0.0L},
        {4.0L, 0.0L},
        {4.0L, 4.0L},
        {2.0L, 0.0L},
        {0.0L, 4.0L},
    };
    assert(polygon_validation_rejects(nonadjacent_touch));
}

void check_scale(long double scale){
    assert(std::isfinite(scale) && scale > 0.0L);
    const Point shift{7.0L * scale, -11.0L * scale};
    const auto transform = [scale, shift](const Point& point){
        return shift + point * scale;
    };

    const Point first = transform({0.0L, 0.0L});
    const Point second = transform({4.0L, 0.0L});
    const Point third = transform({0.0L, 3.0L});
    const Point inside = transform({0.5L, 0.5L});
    const Point boundary = transform({2.0L, 0.0L});
    const Point outside = transform({3.0L, 3.0L});
    assert(triangle_contains(first, second, third, inside));
    assert(triangle_contains(first, second, third, boundary));
    assert(!triangle_contains(first, second, third, outside));
    assert(triangle_contains_strict(first, second, third, inside));
    assert(!triangle_contains_strict(first, second, third, boundary));

    const std::vector<Point> rectangle = {
        transform({0.0L, 0.0L}),
        transform({4.0L, 0.0L}),
        transform({4.0L, 3.0L}),
        transform({0.0L, 3.0L}),
    };
    assert(contains(rectangle, transform({1.0L, 1.0L})) == 2);
    assert(contains(rectangle, transform({4.0L, 1.0L})) == 1);
    assert(contains(rectangle, transform({5.0L, 1.0L})) == 0);
    assert(is_convex(rectangle));
    assert(!is_convex({
        transform({0.0L, 0.0L}),
        transform({2.0L, 0.0L}),
        transform({1.0L, 0.5L}),
        transform({2.0L, 2.0L}),
        transform({0.0L, 2.0L}),
    }));

    std::vector<Point> hull_input = rectangle;
    hull_input.push_back(transform({1.0L, 1.0L}));
    hull_input.push_back(rectangle[0]);
    const auto hull = convex_hull_detail::build_convex_hull(
        std::move(hull_input), false
    );
    assert(hull.size() == 4);
    assert(is_convex(hull));

    const ConvexPolygonQuery query(rectangle);
    assert(query.size() == 4);
    assert(query.contains(transform({1.0L, 1.0L})) == 2);
    assert(query.contains(transform({4.0L, 1.0L})) == 1);
    assert(query.contains(transform({5.0L, 1.0L})) == 0);

    const PolygonBoundaryPointQuery boundary_index(rectangle);
    assert(boundary_index.uses_convex_fast_path());
    const std::vector<Point> boundary_points = {
        rectangle[0],
        transform({2.0L, 0.0L}),
        rectangle[2],
        transform({2.0L, 1.5L}),
    };
    auto boundary_indices = boundary_index.enumerate(boundary_points);
    std::sort(boundary_indices.begin(), boundary_indices.end());
    assert(boundary_indices == std::vector<int>({0, 1, 2}));
    auto fallback_indices = enumerate_points_on_polygon_boundary(
        rectangle, boundary_points
    );
    std::sort(fallback_indices.begin(), fallback_indices.end());
    assert(fallback_indices == std::vector<int>({0, 1, 2}));

    const auto bisectors = angle_bisectors(
        {transform({-1.0L, 0.0L}), transform({1.0L, 0.0L})},
        {transform({-1.0L, -1.0L}), transform({1.0L, 1.0L})}
    );
    check_point(bisectors[0].a, shift, scale);
    check_point(bisectors[1].a, shift, scale);

    assert(near(convex_diameter(rectangle), 5.0L * scale, scale));
    assert(near(
        convex_polygon_minimum_width(rectangle),
        3.0L * scale,
        scale
    ));

    const Point expected_circumcenter = transform({2.0L, 1.5L});
    const Point expected_incenter = transform({1.0L, 1.0L});
    check_point(
        circumcenter(first, second, third),
        expected_circumcenter,
        scale
    );
    check_point(incenter(first, second, third), expected_incenter, scale);
    check_point(orthocenter(first, second, third), first, scale);
    const Circle triangle_circumcircle = circumcircle(first, second, third);
    check_point(triangle_circumcircle.center, expected_circumcenter, scale);
    assert(near(triangle_circumcircle.radius, 2.5L * scale, scale));
    const Circle triangle_incircle = incircle(first, second, third);
    check_point(triangle_incircle.center, expected_incenter, scale);
    assert(near(triangle_incircle.radius, scale, scale));

    bool collinear_threw = false;
    try{
        static_cast<void>(circumcenter(
            transform({0.0L, 0.0L}),
            transform({1.0L, 1.0L}),
            transform({2.0L, 2.0L})
        ));
    }catch(const std::invalid_argument&){
        collinear_threw = true;
    }
    assert(collinear_threw);

    const MinimumEnclosingCircleResult rectangle_circle =
        minimum_enclosing_circle(rectangle);
    const Circle rectangle_value = rectangle_circle.circle();
    check_point(rectangle_value.center, expected_circumcenter, scale);
    assert(near(rectangle_value.radius, 2.5L * scale, scale));
    for(const Point& point: rectangle){
        assert(rectangle_circle.on_boundary(point));
    }
    assert(!rectangle_circle.contains(transform({8.0L, 1.5L})));

    const auto point_circle = minimum_enclosing_circle(
        std::vector<Point>{first}
    );
    assert(point_circle.on_boundary(first));
    assert(!point_circle.contains(transform({1.0L, 0.0L})));

    const long double root_three = std::sqrt(3.0L);
    const std::vector<Point> acute_triangle = {
        transform({0.0L, 0.0L}),
        transform({2.0L, 0.0L}),
        transform({1.0L, root_three}),
    };
    const MinimumEnclosingCircleResult acute_circle =
        minimum_enclosing_circle(acute_triangle);
    assert(acute_circle.support_size == 3);
    const Point acute_center = transform({1.0L, root_three / 3.0L});
    check_point(acute_circle.circle().center, acute_center, scale);
    assert(acute_circle.contains(acute_center));
    assert(!acute_circle.contains(transform({4.0L, root_three / 3.0L})));
    assert(!acute_circle.contains(transform({1e12L, 0.0L})));

    const std::vector<Point> clockwise = {
        transform({0.0L, 0.0L}),
        transform({0.0L, 3.0L}),
        transform({4.0L, 3.0L}),
        transform({4.0L, 0.0L}),
    };
    const auto counterclockwise = counterclockwise_polygon(clockwise);
    assert(counterclockwise.front() == clockwise.front());
    assert(anchored_twice_area(counterclockwise) > 0.0L);
    check_point(
        polygon_centroid(rectangle),
        transform({2.0L, 1.5L}),
        scale
    );

    const auto visible = visibility_polygon(
        rectangle, transform({2.0L, 1.5L})
    );
    assert(visible.size() >= 4);
    for(const Point& point: visible){
        assert(std::isfinite(point.x) && std::isfinite(point.y));
    }
}

void check_elongated_geometry(){
    const std::vector<Point> wide_rectangle = {
        {0.0L, 0.0L},
        {1e12L, 0.0L},
        {1e12L, 100.0L},
        {0.0L, 100.0L},
    };
    const Point inside{5e11L, 10.0L};
    const Point outside{5e11L, -10.0L};
    assert(contains(wide_rectangle, inside) == 2);
    assert(contains(wide_rectangle, outside) == 0);

    const ConvexPolygonQuery query(wide_rectangle);
    assert(query.contains(inside) == 2);
    assert(query.contains(outside) == 0);

    const Point triangle_first{0.0L, 0.0L};
    const Point triangle_second{1e12L, 0.0L};
    const Point triangle_third{0.0L, 100.0L};
    assert(triangle_contains(
        triangle_first, triangle_second, triangle_third, inside
    ));
    assert(triangle_contains_strict(
        triangle_first, triangle_second, triangle_third, inside
    ));
    assert(!triangle_contains(
        triangle_first, triangle_second, triangle_third, outside
    ));
    assert(!triangle_contains_strict(
        triangle_first, triangle_second, triangle_third, outside
    ));

    const PolygonBoundaryPointQuery boundary_query(wide_rectangle);
    assert(boundary_query.uses_convex_fast_path());
    const std::vector<Point> candidates = {
        {5e11L, 0.0L},
        {1e12L, 50.0L},
        inside,
    };
    auto indices = boundary_query.enumerate(candidates);
    std::sort(indices.begin(), indices.end());
    assert(indices == std::vector<int>({0, 1}));

    const std::vector<Point> vertical_crossing = {
        {1e12L, -1.0L},
        {1e12L, 1.0L},
        {-1.0L, 1.0L},
        {-1.0L, -1.0L},
    };
    assert(contains(vertical_crossing, {0.0L, 0.0L}) == 2);

    const auto visible = visibility_polygon(
        wide_rectangle, {5e11L, 50.0L}
    );
    assert(visible.size() >= 4);
    assert(std::abs(anchored_twice_area(visible)) > 1e13L);

    const auto [parallel_hit, parallel_distance] =
        visibility_polygon_detail::ray_segment_distance(
            {0.0L, 0.0L},
            {1.0L, 0.0L},
            {{1e12L, 10.0L}, {1e12L + 100.0L, 10.0L}}
        );
    static_cast<void>(parallel_distance);
    assert(!parallel_hit);

    const auto [small_edge_hit, small_edge_distance] =
        visibility_polygon_detail::ray_segment_distance(
            {0.0L, 0.0L},
            {1.0L, 0.0L},
            {{1.0L, -5e-20L}, {1.0L + 1e-6L, 5e-20L}}
        );
    assert(small_edge_hit);
    assert(near(small_edge_distance, 1.0L + 5e-7L, 1.0L));
    const auto [unit_edge_hit, unit_edge_distance] =
        visibility_polygon_detail::ray_segment_distance(
            {0.0L, 0.0L},
            {1.0L, 0.0L},
            {{1.0L, -5e-14L}, {2.0L, 5e-14L}}
        );
    assert(unit_edge_hit);
    assert(near(unit_edge_distance, 1.5L, 1.0L));

    const std::vector<Point> short_rectangle = {
        {0.0L, 0.0L},
        {1e-6L, 0.0L},
        {1e-6L, 1.0L},
        {0.0L, 1.0L},
    };
    const Point endpoint_excess{1e-6L + 5e-11L, 0.0L};
    const PolygonBoundaryPointQuery short_query(short_rectangle);
    assert(short_query.uses_convex_fast_path());
    const auto short_fast = short_query.enumerate(
        {endpoint_excess}, false
    );
    const auto short_fallback = enumerate_points_on_polygon_boundary(
        short_rectangle, {endpoint_excess}, false
    );
    assert(short_fast == short_fallback);
    assert(short_fast.size() == 2);

    assert(!triangle_contains_strict(
        {0.0L, 0.0L},
        {1.0L, 0.0L},
        {1.0L, 1e-12L},
        {0.5L, 0.25L}
    ));
}

void check_large_translation(){
    const long double offset = 1e12L;
    const std::vector<Point> rectangle = {
        {offset, offset},
        {offset + 3.0L, offset},
        {offset + 3.0L, offset + 2.0L},
        {offset, offset + 2.0L},
    };
    check_point(
        polygon_centroid(rectangle),
        {offset + 1.5L, offset + 1.0L},
        1.0L
    );
    const std::vector<Point> clockwise = {
        rectangle[0], rectangle[3], rectangle[2], rectangle[1]
    };
    const auto counterclockwise = counterclockwise_polygon(clockwise);
    assert(counterclockwise.front() == clockwise.front());
    assert(anchored_twice_area(counterclockwise) > 0.0L);

    const auto visible = visibility_polygon(
        rectangle, {offset + 1.5L, offset + 1.0L}
    );
    assert(visible.size() >= 4);
    assert(std::abs(anchored_twice_area(visible)) > 1.0L);
}

void check_translated_concave_centroid(){
    const long double offset = 1e18L;
    const long double width = 1e9L;
    const long double height = 1e9L;
    const std::vector<Point> polygon = {
        {offset, offset},
        {offset + width, offset},
        {offset + width, offset + height},
        {offset + width - 1.0L, offset + height},
        {offset + width - 1.0L, offset + 1.0L},
        {offset + 1.0L, offset + 1.0L},
        {offset + 1.0L, offset + height},
        {offset, offset + height},
    };
    const long double polygon_area = width + 2.0L * height - 2.0L;
    const long double expected_y = (
        height * height + (width - 2.0L) / 2.0L
    ) / polygon_area;
    check_point(
        polygon_centroid(polygon),
        {offset + width / 2.0L, offset + expected_y},
        width
    );
}

void check_large_finite_values(){
    const long double maximum = std::numeric_limits<long double>::max();
    const long double base = maximum * 0.55L;
    const long double delta = base * 1e-4L;
    const Point first{base, base};
    const Point second{base + delta, base};
    const Point third{base, base + delta};
    const Point expected_circumcenter{
        base + delta / 2.0L, base + delta / 2.0L
    };
    const long double comparison_scale = base * 1e-10L;

    const long double huge_length = abs(Point{base, base});
    assert(std::isfinite(huge_length));
    assert(near(huge_length, base * std::sqrt(2.0L), comparison_scale));

    check_point(
        circumcenter(first, second, third),
        expected_circumcenter,
        comparison_scale
    );
    const long double inradius =
        delta * (2.0L - std::sqrt(2.0L)) / 2.0L;
    check_point(
        incenter(first, second, third),
        {base + inradius, base + inradius},
        comparison_scale
    );
    check_point(
        orthocenter(first, second, third),
        first,
        comparison_scale
    );
    check_point(
        triangle_centroid(first, second, third),
        {base + delta / 3.0L, base + delta / 3.0L},
        comparison_scale
    );
    const Circle outer = circumcircle(first, second, third);
    assert(std::isfinite(outer.radius));
    assert(near(
        outer.radius, delta / std::sqrt(2.0L), delta
    ));
    const Circle inner = incircle(first, second, third);
    assert(std::isfinite(inner.radius));
    assert(near(inner.radius, inradius, delta));

    MinimumEnclosingCircleResult diameter_circle;
    diameter_circle.support_size = 2;
    diameter_circle.support[0] = first;
    diameter_circle.support[1] = second;
    const Point diameter_center{base + delta / 2.0L, base};
    const Circle diameter_value = diameter_circle.circle();
    check_point(diameter_value.center, diameter_center, comparison_scale);
    assert(near(diameter_value.radius, delta / 2.0L, delta));
    assert(diameter_circle.contains(diameter_center));
    assert(!diameter_circle.on_boundary(diameter_center));
    assert(diameter_circle.on_boundary(first));
    assert(diameter_circle.on_boundary(second));

    MinimumEnclosingCircleResult point_circle;
    point_circle.support_size = 1;
    point_circle.support[0] = first;
    assert(point_circle.containment({-base, base}) ==
        MinimumEnclosingCircleContainment::OUTSIDE);

    MinimumEnclosingCircleResult triple_circle;
    triple_circle.support_size = 3;
    triple_circle.support[0] = first;
    triple_circle.support[1] = second;
    triple_circle.support[2] = third;
    assert(triple_circle.contains(expected_circumcenter));
    assert(triple_circle.containment({-base, -base}) ==
        MinimumEnclosingCircleContainment::OUTSIDE);
}

}  // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    check_visibility_polygon_validation();
    check_large_translation();
    check_elongated_geometry();
    int scale_count;
    std::cin >> scale_count;
    check_translated_concave_centroid();
    check_large_finite_values();
    while(scale_count-- > 0){
        long double scale;
        std::cin >> scale;
        check_scale(scale);
        std::cout << "OK\n";
    }
}
