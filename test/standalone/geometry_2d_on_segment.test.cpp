// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/predicate/on_segment.hpp"
#include "geometry_2d_test_common.hpp"

int main(){
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        long double ax, ay, bx, by, px, py;
        std::cin >> ax >> ay >> bx >> by >> px >> py;
        std::cout << on_segment({{ax, ay}, {bx, by}}, {px, py}) << '\n';
    }
}
