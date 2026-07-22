// competitive-verifier: STANDALONE

#include <cmath>
#include <iostream>
#include <stdexcept>

#include "../../src/algorithm/geometry/2d/shape/common_tangents.hpp"

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
        Circle first;
        Circle second;
        std::cin >> first.center.x >> first.center.y >> first.radius
                 >> second.center.x >> second.center.y >> second.radius;
        try{
            const std::vector<Line> lines = common_tangents(first, second);
            for(const Line& line: lines){
                if(std::abs(line_distance(line, first.center) - first.radius) >
                       1e-8L ||
                   std::abs(line_distance(line, second.center) - second.radius) >
                       1e-8L){
                    return 1;
                }
            }
            std::cout << lines.size() << '\n';
        }catch(const std::domain_error&){
            std::cout << "ERROR\n";
        }
    }
}
