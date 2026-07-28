// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/geometry/2d/detail/numeric/plus_geometry_detail.hpp"
#include "../../src/algorithm/geometry/2d/point_set/point_set_scalar/polygon_offset.hpp"
#include "../../src/algorithm/geometry/2d/result/point_set_point_set/polygon_boolean.hpp"
#include "../../src/algorithm/geometry/2d/result/circle_set/circle_arrangement.hpp"
#include "../../src/algorithm/geometry/2d/result/line_set/line_arrangement.hpp"
#include "../../src/algorithm/geometry/2d/result/point_set/minimum_enclosing_ellipse.hpp"
#include "../../src/algorithm/geometry/2d/result/point_set/power_diagram.hpp"
#include "../../src/algorithm/geometry/2d/result/point_set/smallest_enclosing_annulus.hpp"
#include "../../src/algorithm/geometry/2d/result/point_set/straight_skeleton.hpp"
#include "../../src/algorithm/geometry/2d/result/point_set_index_pair_set/constrained_delaunay_triangulation.hpp"
#include "../../src/algorithm/geometry/2d/type/definition/apollonius_diagram.hpp"
#include "../../src/algorithm/geometry/2d/type/definition/circle_arrangement_result.hpp"
#include "../../src/algorithm/geometry/2d/type/definition/constrained_delaunay_triangulation_result.hpp"
#include "../../src/algorithm/geometry/2d/type/definition/enclosing_annulus_result.hpp"
#include "../../src/algorithm/geometry/2d/type/definition/fully_dynamic_convex_hull.hpp"
#include "../../src/algorithm/geometry/2d/type/definition/line_arrangement_result.hpp"
#include "../../src/algorithm/geometry/2d/type/definition/minimum_enclosing_ellipse_result.hpp"
#include "../../src/algorithm/geometry/2d/type/definition/planar_point_location.hpp"
#include "../../src/algorithm/geometry/2d/type/definition/polygon_medial_axis.hpp"
#include "../../src/algorithm/geometry/2d/type/definition/power_diagram_result.hpp"
#include "../../src/algorithm/geometry/2d/type/definition/segment_voronoi_diagram.hpp"
#include "../../src/algorithm/geometry/2d/type/definition/straight_skeleton_result.hpp"
#include "../../src/algorithm/geometry/2d/type/definition/trapezoidal_map.hpp"
#include "../../src/algorithm/geometry/2d/type/definition/weighted_point.hpp"

namespace{

bool close(long double first, long double second){
    return std::abs(first - second) <= 1.0e-8L;
}

void run_all(){
    FullyDynamicConvexHull hull;
    assert(hull.insert(4, {0, 0}));
    assert(hull.insert(1, {4, 0}));
    assert(hull.insert(2, {0, 4}));
    assert(hull.contains({1, 1}) == 2);
    assert(hull.erase(2));
    assert(hull.vertices().size() == 2);

    const auto boolean_result = polygon_boolean(
        {{0, 0}, {3, 0}, {3, 2}, {0, 2}},
        {{1, -1}, {2, -1}, {2, 3}, {1, 3}},
        PolygonBooleanOperation::intersection
    );
    assert(boolean_result.size() == 1);

    PlanarPointLocation location({
        {{0, 0}, {2, 0}, {2, 2}, {0, 2}},
        {{3, 0}, {5, 0}, {5, 2}, {3, 2}},
    });
    assert(location.locate({1, 1}).face == 0);
    assert(location.locate({4, 1}).face == 1);

    TrapezoidalMap trapezoids({
        {{0, 0}, {4, 0}},
        {{0, 2}, {4, 2}},
    });
    const auto trapezoid_location = trapezoids.locate({2, 1});
    assert(trapezoid_location.below == 0);
    assert(trapezoid_location.above == 1);

    const auto lines = line_arrangement({
        {{0, 0}, {1, 0}},
        {{0, 0}, {0, 1}},
        {{1, 0}, {0, 1}},
    });
    assert(lines.vertices.size() == 3);
    assert(lines.bounded_edges.size() == 3);

    const auto circles = circle_arrangement({
        {{0, 0}, 2},
        {{2, 0}, 2},
    });
    assert(circles.vertices.size() == 2);
    assert(circles.arcs.size() == 4);

    const auto power = power_diagram(
        {{{-1, 0}, 0}, {{1, 0}, 0}},
        {{-3, -2}, {3, -2}, {3, 2}, {-3, 2}}
    );
    assert(power.cells.size() == 2);
    assert(power.cells[0].size() == 4);

    ApolloniusDiagram apollonius({
        {{0, 0}, 0},
        {{10, 0}, 3},
    });
    assert(apollonius.locate({8, 0}).site == 1);

    const auto skeleton = straight_skeleton({
        {0, 0}, {4, 0}, {0, 3},
    });
    assert(!skeleton.edges.empty());

    PolygonMedialAxis medial_axis({
        {0, 0}, {4, 0}, {4, 4}, {0, 4},
    });
    assert(medial_axis.contains({2, 2}));
    assert(!medial_axis.contains({1, 2}));

    const auto annulus = smallest_enclosing_annulus({
        {1, 0}, {0, 1}, {-1, 0}, {0, -1},
    });
    assert(close(annulus.inner_radius, 1));
    assert(close(annulus.outer_radius, 1));

    const auto ellipse = minimum_enclosing_ellipse({
        {-2, 0}, {2, 0}, {0, -1}, {0, 1},
    }, 1.0e-8L);
    for(const Point point: std::vector<Point>{
        {-2, 0}, {2, 0}, {0, -1}, {0, 1},
    }){
        assert(ellipse.evaluate(point) <= 1.0L + 1.0e-6L);
    }

    const auto offset = polygon_offset(
        {{0, 0}, {2, 0}, {2, 2}, {0, 2}}, 1
    );
    assert(offset.edges.size() == 8);
    assert(offset.contours.size() == 1);

    const auto constrained = constrained_delaunay_triangulation(
        {{0, 0}, {2, 0}, {2, 2}, {0, 2}},
        {{0, 2}}
    );
    assert(std::find(
        constrained.edges.begin(), constrained.edges.end(),
        std::pair<std::size_t, std::size_t>{0, 2}
    ) != constrained.edges.end());

    SegmentVoronoiDiagram segment_voronoi({
        {{0, 0}, {4, 0}},
        {{0, 5}, {4, 5}},
    });
    assert(segment_voronoi.locate({2, 1}).segment == 0);
}

}  // namespace

int main(){
    std::size_t cases = 0;
    if(!(std::cin >> cases)) return 0;
    while(cases-- > 0){
        run_all();
        std::cout << "OK\n";
    }
}
