// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

#include "../../src/algorithm/geometry/2d/area.hpp"
#include "../../src/algorithm/geometry/2d/ccw.hpp"
#include "../../src/algorithm/geometry/2d/convex_cut.hpp"
#include "../../src/algorithm/geometry/2d/cross_point.hpp"
#include "../../src/algorithm/geometry/2d/distance.hpp"
#include "../../src/algorithm/geometry/2d/distance_ray_point.hpp"
#include "../../src/algorithm/geometry/2d/ear_clipping_triangulation.hpp"
#include "../../src/algorithm/geometry/2d/half_plane_intersection_area.hpp"
#include "../../src/algorithm/geometry/2d/intersect.hpp"
#include "../../src/algorithm/geometry/2d/intersect_line_convex_polygon.hpp"
#include "../../src/algorithm/geometry/2d/intersect_line_segment.hpp"
#include "../../src/algorithm/geometry/2d/line_segment_intersection.hpp"
#include "../../src/algorithm/geometry/2d/on_segment.hpp"
#include "../../src/algorithm/geometry/2d/orthogonal.hpp"
#include "../../src/algorithm/geometry/2d/parallel.hpp"
#include "../../src/algorithm/geometry/2d/polygon_cut.hpp"
#include "../../src/algorithm/geometry/2d/polygon_line_intersections.hpp"
#include "../../src/algorithm/geometry/2d/proper_segment_intersection.hpp"
#include "../../src/algorithm/geometry/2d/segment_intersection.hpp"
#include "../../src/algorithm/geometry/2d/segment_overlap.hpp"
#include "../../src/algorithm/geometry/2d/side_of_directed_line.hpp"

namespace{

bool close(long double first, long double second){
    return std::abs(first - second) <= 1.0e-9L;
}

bool close(const Point& first, const Point& second){
    return close(first.x, second.x) && close(first.y, second.y);
}

long double total_area(const std::vector<std::vector<Point>>& polygons){
    long double result = 0.0L;
    for(const std::vector<Point>& polygon: polygons){
        result += std::abs(area(polygon));
    }
    return result;
}

}  // namespace

int main(){
    int test_count;
    std::cin >> test_count;
    while(test_count-- > 0){
        long double direction_scale;
        long double polygon_scale;
        std::cin >> direction_scale >> polygon_scale;
        assert(direction_scale > GEOMETRY_EPS);
        assert(polygon_scale > GEOMETRY_EPS);

        const Line horizontal{{0.0L, 0.0L}, {1.0L, 0.0L}};
        const Line scaled_horizontal{
            {0.0L, 0.0L}, {direction_scale, 0.0L}
        };
        assert(side_of_directed_line(horizontal, {0.5L, 1.0L}) == 1);
        assert(side_of_directed_line(scaled_horizontal, {0.5L, 1.0L}) == 1);
        assert(intersect(horizontal, Point{0.5L, 0.0L}));
        assert(intersect(scaled_horizontal, Point{0.5L, 0.0L}));
        assert(ccw({0.0L, 0.0L}, {direction_scale, 0.0L},
                   {0.0L, 1.0L}) == COUNTER_CLOCKWISE);

        const Ray ray{{0.0L, 0.0L}, {1.0L, 0.0L}};
        const Ray scaled_ray{
            {0.0L, 0.0L}, {direction_scale, 0.0L}
        };
        assert(close(distance(ray, {0.5L, 1.0L}), 1.0L));
        assert(close(distance(scaled_ray, {0.5L, 1.0L}), 1.0L));
        assert(parallel(Point{1.0L, 0.0L},
                        Point{direction_scale, 0.0L}));
        assert(orthogonal(Point{1.0L, 0.0L},
                          Point{0.0L, direction_scale}));

        const Line diagonal{{0.0L, -1.0L}, {1.0L, 1.0L}};
        const Line scaled_diagonal{
            {0.0L, -1.0L},
            {direction_scale, -1.0L + 2.0L * direction_scale},
        };
        assert(close(cross_point(horizontal, diagonal), {0.5L, 0.0L}));
        assert(close(
            cross_point(scaled_horizontal, scaled_diagonal),
            {0.5L, 0.0L}
        ));

        const Segment vertical{{0.5L, -1.0L}, {0.5L, 1.0L}};
        assert(intersect_line_segment(horizontal, vertical));
        assert(intersect_line_segment(scaled_horizontal, vertical));
        assert(close(line_segment_intersection(horizontal, vertical),
                     {0.5L, 0.0L}));
        assert(close(line_segment_intersection(scaled_horizontal, vertical),
                     {0.5L, 0.0L}));

        const Segment short_horizontal{
            {0.0L, 0.0L}, {direction_scale, 0.0L}
        };
        const Segment short_vertical{
            {direction_scale / 2.0L, -1.0L},
            {direction_scale / 2.0L, 1.0L},
        };
        assert(intersect(short_horizontal, short_vertical));
        if(direction_scale > 4.0L * GEOMETRY_EPS){
            assert(proper_segment_intersection(short_horizontal, short_vertical));
        }
        assert(close(
            segment_intersection(short_horizontal, short_vertical),
            {direction_scale / 2.0L, 0.0L}
        ));
        assert(on_segment(
            short_horizontal, {direction_scale / 2.0L, 0.0L}
        ));
        assert(close(distance_segment_point(
            short_horizontal, {direction_scale / 2.0L, 1.0L}
        ), 1.0L));
        assert(segment_overlap(
            short_horizontal,
            {{direction_scale / 2.0L, 0.0L},
             {2.0L * direction_scale, 0.0L}}
        ).has_value());

        const std::vector<Point> square = {
            {0.0L, 0.0L}, {1.0L, 0.0L},
            {1.0L, 1.0L}, {0.0L, 1.0L},
        };
        const Line cutter{{0.5L, -1.0L}, {0.5L, 2.0L}};
        const Line scaled_cutter{
            {0.5L, -1.0L},
            {0.5L, -1.0L + 3.0L * direction_scale},
        };
        assert(close(std::abs(area(convex_cut(square, cutter))), 0.5L));
        assert(close(std::abs(area(convex_cut(square, scaled_cutter))), 0.5L));
        const PolygonLineCutResult cut = polygon_cut(square, cutter);
        const PolygonLineCutResult scaled_cut =
            polygon_cut(square, scaled_cutter);
        assert(close(total_area(cut.left), total_area(scaled_cut.left)));
        assert(close(total_area(cut.right), total_area(scaled_cut.right)));

        const Line crossing{{0.0L, 0.5L}, {1.0L, 0.5L}};
        const Line scaled_crossing{
            {0.0L, 0.5L}, {direction_scale, 0.5L}
        };
        assert(polygon_line_intersections(square, crossing).size() == 2);
        assert(polygon_line_intersections(square, scaled_crossing).size() == 2);
        assert(intersect_line_convex_polygon(crossing, square));
        assert(intersect_line_convex_polygon(scaled_crossing, square));
        const Line outside{{0.0L, 1.5L}, {1.0L, 1.5L}};
        const Line scaled_outside{
            {0.0L, 1.5L}, {direction_scale, 1.5L}
        };
        assert(!intersect_line_convex_polygon(outside, square));
        assert(!intersect_line_convex_polygon(scaled_outside, square));

        const std::vector<Line> half_planes = {
            {{0.0L, 0.0L}, {direction_scale, 0.0L}},
            {{1.0L, 0.0L}, {1.0L, direction_scale}},
            {{1.0L, 1.0L}, {1.0L - direction_scale, 1.0L}},
            {{0.0L, 1.0L}, {0.0L, 1.0L - direction_scale}},
        };
        assert(close(half_plane_intersection_area(half_planes), 1.0L));

        const Point origin{0.1L, -0.2L};
        const std::vector<Point> tiny_square = {
            origin,
            origin + Point{polygon_scale, 0.0L},
            origin + Point{polygon_scale, polygon_scale},
            origin + Point{0.0L, polygon_scale},
        };
        assert(ear_clipping_triangulation(tiny_square).size() == 2);
        const Line tiny_cutter{
            origin + Point{polygon_scale / 2.0L, -polygon_scale},
            origin + Point{polygon_scale / 2.0L, 2.0L * polygon_scale},
        };
        const PolygonLineCutResult tiny_cut =
            polygon_cut(tiny_square, tiny_cutter);
        assert(tiny_cut.left.size() == 1);
        assert(tiny_cut.right.size() == 1);

        std::cout << "OK\n";
    }
}
