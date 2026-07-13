// competitive-verifier: STANDALONE

#include <iomanip>
#include <iostream>
#include <vector>

#include "../../src/algorithm/geometry/2d/circle_polygon_intersection_area/circle_polygon_intersection_area.hpp"

int main(){
    int n;
    Circle circle;
    if(!(std::cin >> n >> circle.center.x >> circle.center.y >> circle.radius)) return 0;
    std::vector<Point> polygon(static_cast<std::size_t>(n));
    for(Point& point: polygon) std::cin >> point.x >> point.y;
    std::cout << std::fixed << std::setprecision(8)
              << circle_polygon_intersection_area(circle, polygon) << '\n';
}
