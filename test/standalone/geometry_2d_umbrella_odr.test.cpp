// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/aggregate/all/geometry.hpp"
#include "../../src/algorithm/geometry/2d/aggregate/all/geometry.hpp"
#include "../../src/algorithm/geometry/2d/aggregate/all/minimum_enclosing_circle.hpp"
#include "../../src/algorithm/geometry/2d/point_set/point_set/sort_points_by_argument.hpp"
#include "../../src/algorithm/geometry/2d/constant/definition/constants.hpp"
#include "../../src/algorithm/geometry/2d/aggregate/all/geometry_sign.hpp"
#include "../../src/algorithm/geometry/2d/aggregate/all/types.hpp"
#include "../../src/algorithm/geometry/2d/scalar/point_point/dot.hpp"
#include "../../src/algorithm/geometry/2d/scalar/point_point/cross.hpp"
#include "../../src/algorithm/geometry/2d/scalar/point/norm.hpp"
#include "../../src/algorithm/geometry/2d/scalar/point/abs.hpp"
#include "../../src/algorithm/geometry/2d/scalar/point/arg.hpp"
#include "../../src/algorithm/geometry/2d/point/point/unit.hpp"
#include "../../src/algorithm/geometry/2d/point/point/rotate90.hpp"
#include "../../src/algorithm/geometry/2d/point/point/rotate.hpp"
#include "../../src/algorithm/geometry/2d/point/scalar/polar.hpp"
#include "../../src/algorithm/geometry/2d/aggregate/all/ccw.hpp"
#include "../../src/algorithm/geometry/2d/aggregate/all/orthogonal.hpp"
#include "../../src/algorithm/geometry/2d/aggregate/all/parallel.hpp"
#include "../../src/algorithm/geometry/2d/point/line_point/projection.hpp"
#include "../../src/algorithm/geometry/2d/point/line_point/reflection.hpp"
#include "../../src/algorithm/geometry/2d/predicate/segment_point/on_segment.hpp"
#include "../../src/algorithm/geometry/2d/aggregate/all/intersect.hpp"
#include "../../src/algorithm/geometry/2d/predicate/line_line/same_line.hpp"
#include "../../src/algorithm/geometry/2d/predicate/line_line/intersect_line_line.hpp"
#include "../../src/algorithm/geometry/2d/predicate/line_segment/intersect_line_segment.hpp"
#include "../../src/algorithm/geometry/2d/aggregate/all/distance.hpp"
#include "../../src/algorithm/geometry/2d/point/line_line/cross_point.hpp"
#include "../../src/algorithm/geometry/2d/point/line_line/line_intersection.hpp"
#include "../../src/algorithm/geometry/2d/point/segment_segment/segment_intersection.hpp"
#include "../../src/algorithm/geometry/2d/scalar/point_set/area.hpp"
#include "../../src/algorithm/geometry/2d/integer/point_set_point/contains.hpp"
#include "../../src/algorithm/geometry/2d/type/definition/convex_polygon_query.hpp"
#include "../../src/algorithm/geometry/2d/integer/point_set_point/contains_convex.hpp"
#include "../../src/algorithm/geometry/2d/predicate/point_set/is_convex.hpp"
#include "../../src/algorithm/geometry/2d/point_set/point_set/convex_hull.hpp"
#include "../../src/algorithm/geometry/2d/point_set/point_set_line/convex_cut.hpp"
#include "geometry_2d_test_common.hpp"

long double geometry_2d_umbrella_dot_a(const Point&, const Point&);
long double geometry_2d_umbrella_cross_b(const Point&, const Point&);

int main(){
    geometry_test_setup_output();
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        long double ax, ay, bx, by;
        std::cin >> ax >> ay >> bx >> by;
        const Point first{ax, ay};
        const Point second{bx, by};
        geometry_test_print_real(geometry_2d_umbrella_dot_a(first, second));
        geometry_test_print_real(geometry_2d_umbrella_cross_b(first, second));
        std::cout << ccw({0, 0}, first, second) << '\n';
    }
}
