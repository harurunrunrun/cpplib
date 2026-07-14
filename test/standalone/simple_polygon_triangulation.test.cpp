// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/simple_polygon_triangulation.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>

int main(){
    int queries;
    std::cin >> queries;
    while(queries-- > 0){
        int count;
        std::cin >> count;
        std::vector<Point> polygon(static_cast<std::size_t>(count));
        for(Point& point: polygon) std::cin >> point.x >> point.y;
        try{
            const auto triangles = simple_polygon_triangulation(polygon);
            long double polygon_twice_area = 0.0L;
            for(std::size_t index = 0; index < polygon.size(); ++index){
                polygon_twice_area += cross(
                    polygon[index], polygon[(index + 1) % polygon.size()]
                );
            }
            long double triangle_twice_area = 0.0L;
            bool valid = true;
            for(const EarClippingTriangle& triangle: triangles){
                for(const std::size_t index: triangle){
                    valid = valid && index < polygon.size();
                }
                if(!valid) continue;
                const long double value = cross(
                    polygon[triangle[1]] - polygon[triangle[0]],
                    polygon[triangle[2]] - polygon[triangle[0]]
                );
                valid = valid && geometry_sign(value) > 0;
                triangle_twice_area += value;
            }
            const long double expected = std::abs(polygon_twice_area);
            valid = valid && std::abs(triangle_twice_area - expected)
                <= 1e-8L * std::max(1.0L, expected);
            std::cout << triangles.size() << ' ' << valid << '\n';
        }catch(const std::invalid_argument&){
            std::cout << "ERROR\n";
        }
    }
}
