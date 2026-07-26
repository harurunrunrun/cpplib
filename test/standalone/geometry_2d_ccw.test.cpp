// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/ccw.hpp"
#include "geometry_2d_test_common.hpp"

int main(){
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        long double ax, ay, bx, by, cx, cy;
        std::cin >> ax >> ay >> bx >> by >> cx >> cy;
        std::cout << ccw({ax, ay}, {bx, by}, {cx, cy}) << '\n';
    }
}
