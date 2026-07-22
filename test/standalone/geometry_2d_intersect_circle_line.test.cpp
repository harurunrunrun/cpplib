// competitive-verifier: STANDALONE

#include <iostream>
#include <stdexcept>

#include "../../src/algorithm/geometry/2d/predicate/intersect_circle_line.hpp"

int main(){
    int query_count;
    std::cin >> query_count;
    while(query_count--){
        Circle circle;
        Line line;
        std::cin >> circle.center.x >> circle.center.y >> circle.radius
                 >> line.a.x >> line.a.y >> line.b.x >> line.b.y;
        try{
            std::cout << static_cast<int>(intersect_circle_line(circle, line))
                      << '\n';
        }catch(const std::invalid_argument&){
            std::cout << "ERROR\n";
        }
    }
}
