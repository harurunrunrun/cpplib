// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/line_line_distance.hpp"
#include "../../src/algorithm/geometry/2d/line_point_distance.hpp"
#include "../../src/algorithm/geometry/2d/line_segment_distance.hpp"
#include "../../src/algorithm/geometry/2d/point_point_distance.hpp"
#include "../../src/algorithm/geometry/2d/segment_point_distance.hpp"
#include "../../src/algorithm/geometry/2d/segment_segment_distance.hpp"
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
                if(kind == "LP"){
                    geometry_test_print_real(
                        distance(Line{{ax, ay}, {bx, by}}, {px, py})
                    );
                }else{
                    geometry_test_print_real(distance_segment_point(
                        Segment{{ax, ay}, {bx, by}}, {px, py}
                    ));
                }
            }else{
                long double ax, ay, bx, by, cx, cy, dx, dy;
                std::cin >> ax >> ay >> bx >> by >> cx >> cy >> dx >> dy;
                if(kind == "SS"){
                    geometry_test_print_real(distance(
                        Segment{{ax, ay}, {bx, by}},
                        Segment{{cx, cy}, {dx, dy}}
                    ));
                }else if(kind == "LL"){
                    geometry_test_print_real(distance_line_line(
                        Line{{ax, ay}, {bx, by}},
                        Line{{cx, cy}, {dx, dy}}
                    ));
                }else{
                    geometry_test_print_real(distance_line_segment(
                        Line{{ax, ay}, {bx, by}},
                        Segment{{cx, cy}, {dx, dy}}
                    ));
                }
            }
        }catch(const std::invalid_argument&){
            std::cout << "ERR\n";
        }
    }
}
