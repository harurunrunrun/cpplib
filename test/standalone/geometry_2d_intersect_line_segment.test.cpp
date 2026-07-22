// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/predicate/intersect_line_segment.hpp"
#include "geometry_2d_test_common.hpp"

#include <stdexcept>

int main(){
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        long double ax, ay, bx, by, cx, cy, dx, dy;
        std::cin >> ax >> ay >> bx >> by >> cx >> cy >> dx >> dy;
        try{
            std::cout << intersect_line_segment({{ax, ay}, {bx, by}},
                                                 {{cx, cy}, {dx, dy}}) << '\n';
        }catch(const std::invalid_argument&){
            std::cout << "ERR\n";
        }
    }
}
