// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/geometry.hpp"
#include "../../src/algorithm/geometry/geometry.hpp"
#include "../../src/algorithm/geometry/2d/minimum_enclosing_circle.hpp"
#include "../../src/algorithm/geometry/2d/sort_points_by_argument.hpp"
#include "../../src/algorithm/geometry/2d/constants.hpp"
#include "../../src/algorithm/geometry/2d/geometry_sign.hpp"
#include "../../src/algorithm/geometry/2d/types.hpp"
#include "../../src/algorithm/geometry/2d/dot.hpp"
#include "../../src/algorithm/geometry/2d/cross.hpp"
#include "../../src/algorithm/geometry/2d/norm.hpp"
#include "../../src/algorithm/geometry/2d/abs.hpp"
#include "../../src/algorithm/geometry/2d/arg.hpp"
#include "../../src/algorithm/geometry/2d/unit.hpp"
#include "../../src/algorithm/geometry/2d/rotate90.hpp"
#include "../../src/algorithm/geometry/2d/rotate.hpp"
#include "../../src/algorithm/geometry/2d/polar.hpp"
#include "../../src/algorithm/geometry/2d/ccw.hpp"
#include "../../src/algorithm/geometry/2d/orthogonal.hpp"
#include "../../src/algorithm/geometry/2d/parallel.hpp"
#include "../../src/algorithm/geometry/2d/projection.hpp"
#include "../../src/algorithm/geometry/2d/reflection.hpp"
#include "../../src/algorithm/geometry/2d/on_segment.hpp"
#include "../../src/algorithm/geometry/2d/intersect.hpp"
#include "../../src/algorithm/geometry/2d/same_line.hpp"
#include "../../src/algorithm/geometry/2d/intersect_line_line.hpp"
#include "../../src/algorithm/geometry/2d/intersect_line_segment.hpp"
#include "../../src/algorithm/geometry/2d/distance.hpp"
#include "../../src/algorithm/geometry/2d/cross_point.hpp"
#include "../../src/algorithm/geometry/2d/line_intersection.hpp"
#include "../../src/algorithm/geometry/2d/segment_intersection.hpp"
#include "../../src/algorithm/geometry/2d/area.hpp"
#include "../../src/algorithm/geometry/2d/contains.hpp"
#include "../../src/algorithm/geometry/2d/convex_polygon_query.hpp"
#include "../../src/algorithm/geometry/2d/contains_convex.hpp"
#include "../../src/algorithm/geometry/2d/is_convex.hpp"
#include "../../src/algorithm/geometry/2d/convex_hull.hpp"
#include "../../src/algorithm/geometry/2d/convex_cut.hpp"
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
