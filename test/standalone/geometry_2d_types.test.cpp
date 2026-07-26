// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/circle.hpp"
#include "../../src/algorithm/geometry/2d/line.hpp"
#include "../../src/algorithm/geometry/2d/point.hpp"
#include "../../src/algorithm/geometry/2d/segment.hpp"
#include "../../src/algorithm/geometry/2d/types.hpp"
#include "geometry_2d_test_common.hpp"

#include <string>

int main(){
    geometry_test_setup_output();
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        std::string operation;
        std::cin >> operation;
        if(operation == "DEFAULT"){
            const Point point;
            geometry_test_print_point(point);
        }else if(operation == "PLUS" || operation == "NEG"){
            long double x, y;
            std::cin >> x >> y;
            const Point point{x, y};
            geometry_test_print_point(operation == "PLUS" ? +point : -point);
        }else if(operation == "ADD" || operation == "SUB"){
            long double ax, ay, bx, by;
            std::cin >> ax >> ay >> bx >> by;
            const Point first{ax, ay};
            const Point second{bx, by};
            geometry_test_print_point(operation == "ADD" ? first + second : first - second);
        }else if(operation == "SCALE_LEFT" || operation == "SCALE_RIGHT" ||
                 operation == "DIV"){
            long double x, y, scalar;
            std::cin >> x >> y >> scalar;
            const Point point{x, y};
            if(operation == "SCALE_LEFT") geometry_test_print_point(scalar * point);
            else if(operation == "SCALE_RIGHT") geometry_test_print_point(point * scalar);
            else geometry_test_print_point(point / scalar);
        }else if(operation == "MUTATE"){
            long double ax, ay, bx, by, scalar;
            std::cin >> ax >> ay >> bx >> by >> scalar;
            Point point{ax, ay};
            point += Point{bx, by};
            geometry_test_print_point(point);
            point -= Point{bx, by};
            geometry_test_print_point(point);
            point *= scalar;
            geometry_test_print_point(point);
            point /= scalar;
            geometry_test_print_point(point);
        }else if(operation == "LESS" || operation == "EQUAL"){
            long double ax, ay, bx, by;
            std::cin >> ax >> ay >> bx >> by;
            const Point first{ax, ay};
            const Point second{bx, by};
            std::cout << (operation == "LESS" ? first < second : first == second) << '\n';
        }else if(operation == "LINE" || operation == "SEGMENT"){
            long double ax, ay, bx, by;
            std::cin >> ax >> ay >> bx >> by;
            if(operation == "LINE"){
                const Line line{{ax, ay}, {bx, by}};
                geometry_test_print_point(line.a);
                geometry_test_print_point(line.b);
            }else{
                const Segment segment{{ax, ay}, {bx, by}};
                geometry_test_print_point(segment.a);
                geometry_test_print_point(segment.b);
                const Line line = supporting_line(segment);
                if(line.a.x != segment.a.x || line.a.y != segment.a.y ||
                   line.b.x != segment.b.x || line.b.y != segment.b.y){
                    return 1;
                }
            }
        }else{
            long double x, y, radius;
            std::cin >> x >> y >> radius;
            const Circle circle{{x, y}, radius};
            geometry_test_print_point(circle.center);
            geometry_test_print_real(circle.radius);
        }
    }
}
