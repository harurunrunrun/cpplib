// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/intersect.hpp"
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
            if(kind == "SS"){
                long double ax, ay, bx, by, cx, cy, dx, dy;
                std::cin >> ax >> ay >> bx >> by >> cx >> cy >> dx >> dy;
                std::cout << intersect(Segment{{ax, ay}, {bx, by}},
                                       Segment{{cx, cy}, {dx, dy}}) << '\n';
            }else{
                long double ax, ay, bx, by, px, py;
                std::cin >> ax >> ay >> bx >> by >> px >> py;
                std::cout << intersect(Line{{ax, ay}, {bx, by}}, Point{px, py}) << '\n';
            }
        }catch(const std::invalid_argument&){
            std::cout << "ERR\n";
        }
    }
}
