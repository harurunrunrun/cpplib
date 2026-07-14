// competitive-verifier: PROBLEM https://onlinejudge.u-aizu.ac.jp/problems/CGL_7_D
// competitive-verifier: ERROR 1e-8

#include <iomanip>
#include <iostream>

#include "../../src/algorithm/geometry/2d/geometry.hpp"

int main(){
    Circle circle;
    std::cin >> circle.center.x >> circle.center.y >> circle.radius;
    int query_count;
    std::cin >> query_count;
    std::cout << std::fixed << std::setprecision(10);
    while(query_count--){
        Line line;
        std::cin >> line.a.x >> line.a.y >> line.b.x >> line.b.y;
        auto points = circle_line_cross_points(circle, line);
        if(points.size() == 1) points.push_back(points[0]);
        std::cout << points[0].x << ' ' << points[0].y << ' '
                  << points[1].x << ' ' << points[1].y << '\n';
    }
}
