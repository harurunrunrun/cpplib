// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/geometry/2d/circle_circle_cross_points.hpp"
#include "../../src/algorithm/geometry/2d/circle_circle_intersection_count.hpp"
#include "../../src/algorithm/geometry/2d/circle_line_cross_points.hpp"
#include "../../src/algorithm/geometry/2d/circle_line_intersection_count.hpp"
#include "../../src/algorithm/geometry/2d/circle_polygon_intersection_area.hpp"
#include "../../src/algorithm/geometry/2d/circle_segment_cross_points.hpp"
#include "../../src/algorithm/geometry/2d/circle_segment_intersection_count.hpp"
#include "../../src/algorithm/geometry/2d/circle_triangle_intersection_area.hpp"
#include "../../src/algorithm/geometry/2d/circles_through_two_points.hpp"
#include "../../src/algorithm/geometry/2d/common_tangents.hpp"
#include "../../src/algorithm/geometry/2d/intersect_circle_circle.hpp"
#include "../../src/algorithm/geometry/2d/intersect_circle_line.hpp"
#include "../../src/algorithm/geometry/2d/intersect_circle_segment.hpp"
#include "../../src/algorithm/geometry/2d/tangent_points.hpp"

namespace{

bool near(long double first, long double second, long double scale){
    return std::fabs(first - second) <= 2e-7L * scale;
}

void check_point(
    const Point& point,
    long double x,
    long double y,
    long double scale
){
    assert(near(point.x, x, scale));
    assert(near(point.y, y, scale));
}

void check_scale(long double scale){
    assert(std::isfinite(scale) && scale > 0.0L);
    const long double root_three = std::sqrt(3.0L);
    const Circle unit_first{{0.0L, 0.0L}, scale};
    const Circle unit_second{{scale, 0.0L}, scale};

    assert(circle_circle_intersection_count(unit_first, unit_second) == 2);
    const auto circle_points =
        circle_circle_cross_points(unit_first, unit_second);
    assert(circle_points.size() == 2);
    check_point(circle_points[0], scale / 2.0L,
                -root_three * scale / 2.0L, scale);
    check_point(circle_points[1], scale / 2.0L,
                root_three * scale / 2.0L, scale);
    assert(circle_circle_intersection_count(
        unit_first, {{2.0L * scale, 0.0L}, scale}
    ) == 1);
    assert(circle_circle_intersection_count(
        unit_first, {{2.1L * scale, 0.0L}, scale}
    ) == 0);

    const Circle point_circle{{3.0L * scale, -2.0L * scale}, 0.0L};
    assert(circle_circle_intersection_count(point_circle, point_circle) == 1);
    const auto point_intersection =
        circle_circle_cross_points(point_circle, point_circle);
    assert(point_intersection.size() == 1);
    check_point(
        point_intersection[0],
        point_circle.center.x,
        point_circle.center.y,
        scale
    );
    assert(intersect_circle_circle(point_circle, point_circle));
    assert(circle_circle_intersection_count(unit_first, unit_first)
           == CIRCLE_INTERSECTION_INFINITE);
    bool coincident_threw = false;
    try{
        static_cast<void>(circle_circle_cross_points(unit_first, unit_first));
    }catch(const std::domain_error&){
        coincident_threw = true;
    }
    assert(coincident_threw);

    const Line secant{
        {-2.0L * scale, scale / 2.0L},
        {2.0L * scale, scale / 2.0L}
    };
    const auto line_points = circle_line_cross_points(unit_first, secant);
    assert(line_points.size() == 2);
    assert(circle_line_intersection_count(unit_first, secant) == 2);
    check_point(line_points[0], -root_three * scale / 2.0L,
                scale / 2.0L, scale);
    check_point(line_points[1], root_three * scale / 2.0L,
                scale / 2.0L, scale);
    assert(circle_line_intersection_count(
        unit_first,
        {{-2.0L * scale, scale}, {2.0L * scale, scale}}
    ) == 1);
    assert(circle_line_intersection_count(
        unit_first,
        {{-2.0L * scale, 1.1L * scale},
         {2.0L * scale, 1.1L * scale}}
    ) == 0);
    assert(intersect_circle_line(unit_first, secant));

    const Segment diameter{
        {-2.0L * scale, 0.0L},
        {2.0L * scale, 0.0L}
    };
    const Segment half_segment{
        {-2.0L * scale, 0.0L},
        {0.0L, 0.0L}
    };
    assert(circle_segment_cross_points(unit_first, diameter).size() == 2);
    assert(circle_segment_intersection_count(unit_first, diameter) == 2);
    assert(circle_segment_intersection_count(unit_first, half_segment) == 1);
    assert(circle_segment_intersection_count(
        unit_first,
        {{scale, 0.0L}, {scale, 0.0L}}
    ) == 1);
    assert(intersect_circle_segment(unit_first, diameter));
    assert(!intersect_circle_segment(
        unit_first,
        {{-scale / 2.0L, 0.0L}, {scale / 2.0L, 0.0L}}
    ));

    const auto two_circles = circles_through_two_points(
        {-scale / 2.0L, 0.0L},
        {scale / 2.0L, 0.0L},
        scale
    );
    assert(two_circles.size() == 2);
    check_point(two_circles[0].center, 0.0L,
                -root_three * scale / 2.0L, scale);
    check_point(two_circles[1].center, 0.0L,
                root_three * scale / 2.0L, scale);
    assert(circles_through_two_points(
        {-scale, 0.0L}, {scale, 0.0L}, scale
    ).size() == 1);
    assert(circles_through_two_points(
        {-1.1L * scale, 0.0L}, {1.1L * scale, 0.0L}, scale
    ).empty());

    const std::vector<Point> tiny_triangle = {
        {0.10L * scale, 0.10L * scale},
        {0.20L * scale, 0.10L * scale},
        {0.10L * scale, 0.20L * scale},
    };
    const long double expected_area = 0.005L * scale * scale;
    assert(near(
        circle_polygon_intersection_area(unit_first, tiny_triangle),
        expected_area,
        scale * scale
    ));
    assert(near(
        circle_triangle_intersection_area(
            unit_first,
            tiny_triangle[0],
            tiny_triangle[1],
            tiny_triangle[2]
        ),
        expected_area,
        scale * scale
    ));
    assert(near(
        signed_circle_polygon_intersection_area(unit_first, tiny_triangle),
        expected_area,
        scale * scale
    ));
    const std::vector<Point> reversed_triangle = {
        tiny_triangle[0], tiny_triangle[2], tiny_triangle[1]
    };
    assert(near(
        signed_circle_polygon_intersection_area(
            unit_first, reversed_triangle
        ),
        -expected_area,
        scale * scale
    ));
    const std::vector<Point> containing_triangle = {
        {-3.0L * scale, -3.0L * scale},
        {3.0L * scale, -3.0L * scale},
        {0.0L, 3.0L * scale},
    };
    assert(near(
        circle_polygon_intersection_area(
            unit_first, containing_triangle
        ),
        std::acos(-1.0L) * scale * scale,
        scale * scale
    ));
    assert(circle_polygon_intersection_area(
        {{0.0L, 0.0L}, 0.0L}, tiny_triangle
    ) == 0.0L);

    const auto tangent = tangent_points(
        unit_first, {2.0L * scale, 0.0L}
    );
    assert(tangent.size() == 2);
    assert(common_tangents(
        unit_first, {{3.0L * scale, 0.0L}, scale}
    ).size() == 4);
}

}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    bool non_finite_threw = false;
    try{
        static_cast<void>(circle_circle_intersection_count(
            {{0.0L, 0.0L}, std::numeric_limits<long double>::infinity()},
            {{0.0L, 0.0L}, 1.0L}
        ));
    }catch(const std::invalid_argument&){
        non_finite_threw = true;
    }
    assert(non_finite_threw);

    int scale_count;
    std::cin >> scale_count;
    while(scale_count-- > 0){
        long double scale;
        std::cin >> scale;
        check_scale(scale);
        std::cout << "OK\n";
    }
}
