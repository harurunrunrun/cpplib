// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/convex_cut.hpp"
#include "geometry_2d_test_common.hpp"

#include <stdexcept>
#include <vector>

int main(){
    geometry_test_setup_output();
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        int size;
        std::cin >> size;
        std::vector<Point> polygon(static_cast<std::size_t>(size));
        for(Point& point: polygon) std::cin >> point.x >> point.y;
        Line line;
        std::cin >> line.a.x >> line.a.y >> line.b.x >> line.b.y;
        try{
            const std::vector<Point> result = convex_cut(polygon, line);
            std::cout << result.size() << '\n';
            for(const Point& point: result) geometry_test_print_point(point);
        }catch(const std::invalid_argument&){
            std::cout << "ERR\n";
        }
    }
}
