// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/core/geometry.hpp"
#include "../../src/algorithm/geometry/2d/core/geometry.hpp"
#include "../../src/algorithm/geometry/2d/shape/minimum_enclosing_circle.hpp"
#include "../../src/algorithm/geometry/2d/point_collection/sort_points_by_argument.hpp"
#include "../../src/algorithm/geometry/2d/core/constants.hpp"
#include "../../src/algorithm/geometry/2d/scalar/geometry_sign.hpp"
#include "../../src/algorithm/geometry/2d/core/types.hpp"
#include "../../src/algorithm/geometry/2d/scalar/dot.hpp"
#include "../../src/algorithm/geometry/2d/scalar/cross.hpp"
#include "../../src/algorithm/geometry/2d/scalar/norm.hpp"
#include "../../src/algorithm/geometry/2d/scalar/abs.hpp"
#include "../../src/algorithm/geometry/2d/scalar/arg.hpp"
#include "../../src/algorithm/geometry/2d/point/unit.hpp"
#include "../../src/algorithm/geometry/2d/point/rotate90.hpp"
#include "../../src/algorithm/geometry/2d/point/rotate.hpp"
#include "../../src/algorithm/geometry/2d/point/polar.hpp"
#include "../../src/algorithm/geometry/2d/predicate/ccw.hpp"
#include "../../src/algorithm/geometry/2d/predicate/orthogonal.hpp"
#include "../../src/algorithm/geometry/2d/predicate/parallel.hpp"
#include "../../src/algorithm/geometry/2d/point/projection.hpp"
#include "../../src/algorithm/geometry/2d/point/reflection.hpp"
#include "../../src/algorithm/geometry/2d/predicate/on_segment.hpp"
#include "../../src/algorithm/geometry/2d/predicate/intersect.hpp"
#include "../../src/algorithm/geometry/2d/predicate/same_line.hpp"
#include "../../src/algorithm/geometry/2d/predicate/intersect_line_line.hpp"
#include "../../src/algorithm/geometry/2d/predicate/intersect_line_segment.hpp"
#include "../../src/algorithm/geometry/2d/scalar/distance.hpp"
#include "../../src/algorithm/geometry/2d/point/cross_point.hpp"
#include "../../src/algorithm/geometry/2d/point/line_intersection.hpp"
#include "../../src/algorithm/geometry/2d/point/segment_intersection.hpp"
#include "../../src/algorithm/geometry/2d/scalar/area.hpp"
#include "../../src/algorithm/geometry/2d/predicate/contains.hpp"
#include "../../src/algorithm/geometry/2d/query/convex_polygon_query.hpp"
#include "../../src/algorithm/geometry/2d/predicate/contains_convex.hpp"
#include "../../src/algorithm/geometry/2d/predicate/is_convex.hpp"
#include "../../src/algorithm/geometry/2d/point_collection/convex_hull.hpp"
#include "../../src/algorithm/geometry/2d/point_collection/convex_cut.hpp"
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
