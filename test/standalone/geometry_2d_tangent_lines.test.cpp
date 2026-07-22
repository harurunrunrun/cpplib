// competitive-verifier: STANDALONE

#include <cmath>
#include <iostream>

#include "../../src/algorithm/geometry/2d/shape/tangent_lines.hpp"

namespace{

long double line_distance(const Line& line, const Point& point){
    const long double dx = line.b.x - line.a.x;
    const long double dy = line.b.y - line.a.y;
    return std::abs(dx * (point.y - line.a.y) -
                    dy * (point.x - line.a.x)) / std::hypot(dx, dy);
}

} // namespace

int main(){
    int query_count;
    std::cin >> query_count;
    while(query_count--){
        Circle circle;
        Point point;
        std::cin >> circle.center.x >> circle.center.y >> circle.radius
                 >> point.x >> point.y;
        const std::vector<Line> lines = tangent_lines(circle, point);
        for(const Line& line: lines){
            if(std::abs(line_distance(line, circle.center) - circle.radius) >
               1e-8L){
                return 1;
            }
        }
        std::cout << lines.size() << '\n';
    }
}
