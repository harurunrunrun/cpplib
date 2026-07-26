// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/ccw_result.hpp"
#include "../../src/algorithm/geometry/2d/ccw_points.hpp"
#include "../../src/algorithm/geometry/2d/ccw.hpp"
#include "geometry_2d_test_common.hpp"

int main(){
    static_assert(CLOCKWISE == -1);
    static_assert(COUNTER_CLOCKWISE == 1);
    static_assert(ONLINE_BACK == 2);
    static_assert(ONLINE_FRONT == -2);
    static_assert(ON_SEGMENT == 0);

    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        long double ax, ay, bx, by, cx, cy;
        std::cin >> ax >> ay >> bx >> by >> cx >> cy;
        std::cout << ccw({ax, ay}, {bx, by}, {cx, cy}) << '\n';
    }
}
