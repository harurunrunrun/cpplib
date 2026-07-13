// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/orthogonal.hpp"
#include "geometry_2d_test_common.hpp"

#include <stdexcept>
#include <string>

int main(){
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        std::string kind;
        std::cin >> kind;
        try{
            if(kind == "P"){
                long double ax, ay, bx, by;
                std::cin >> ax >> ay >> bx >> by;
                std::cout << orthogonal(Point{ax, ay}, Point{bx, by}) << '\n';
            }else{
                long double ax, ay, bx, by, cx, cy, dx, dy;
                std::cin >> ax >> ay >> bx >> by >> cx >> cy >> dx >> dy;
                std::cout << orthogonal(Line{{ax, ay}, {bx, by}},
                                         Line{{cx, cy}, {dx, dy}}) << '\n';
            }
        }catch(const std::invalid_argument&){
            std::cout << "ERR\n";
        }
    }
}
