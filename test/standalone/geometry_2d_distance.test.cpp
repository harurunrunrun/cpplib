// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/distance.hpp"
#include "geometry_2d_test_common.hpp"

#include <stdexcept>
#include <string>

int main(){
    geometry_test_setup_output();
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        std::string kind;
        std::cin >> kind;
        try{
            if(kind == "PP"){
                long double ax, ay, bx, by;
                std::cin >> ax >> ay >> bx >> by;
                geometry_test_print_real(distance(Point{ax, ay}, Point{bx, by}));
            }else if(kind == "LP" || kind == "SP"){
                long double ax, ay, bx, by, px, py;
                std::cin >> ax >> ay >> bx >> by >> px >> py;
                const Line line{{ax, ay}, {bx, by}};
                if(kind == "LP") geometry_test_print_real(distance(line, {px, py}));
                else geometry_test_print_real(distance_segment_point(line, {px, py}));
            }else{
                long double ax, ay, bx, by, cx, cy, dx, dy;
                std::cin >> ax >> ay >> bx >> by >> cx >> cy >> dx >> dy;
                const Line first{{ax, ay}, {bx, by}};
                const Line second{{cx, cy}, {dx, dy}};
                if(kind == "SS") geometry_test_print_real(distance(first, second));
                else if(kind == "LL") geometry_test_print_real(distance_line_line(first, second));
                else geometry_test_print_real(distance_line_segment(first, second));
            }
        }catch(const std::invalid_argument&){
            std::cout << "ERR\n";
        }
    }
}
