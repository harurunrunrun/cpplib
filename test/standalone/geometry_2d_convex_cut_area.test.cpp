// competitive-verifier: STANDALONE

#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/geometry/2d/scalar/convex_cut_area.hpp"

int main(){
    std::cout << std::fixed << std::setprecision(9);
    int query_count;
    std::cin >> query_count;
    while(query_count--){
        int size;
        std::cin >> size;
        std::vector<Point> polygon(static_cast<std::size_t>(size));
        for(Point& point: polygon) std::cin >> point.x >> point.y;
        Line line;
        std::cin >> line.a.x >> line.a.y >> line.b.x >> line.b.y;
        try{
            std::cout << convex_cut_area(polygon, line) << '\n';
        }catch(const std::invalid_argument&){
            std::cout << "ERR\n";
        }
    }
}
