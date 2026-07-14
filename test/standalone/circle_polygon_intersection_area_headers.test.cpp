// competitive-verifier: STANDALONE

#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>
#include "../../src/algorithm/geometry/2d/circle_polygon_intersection_area/detail.hpp"

#include "../../src/algorithm/geometry/2d/circle_polygon_intersection_area/signed_circle_polygon_intersection_area.hpp"
#include "../../src/algorithm/geometry/2d/circle_polygon_intersection_area.hpp"
#include "../../src/algorithm/geometry/2d/circle_polygon_intersection_area/circle_polygon_intersection_area.hpp"

long double circle_polygon_intersection_area_odr_signed(
    const Circle& circle,
    const std::vector<Point>& polygon
);
long double circle_polygon_intersection_area_odr_unsigned(
    const Circle& circle,
    const std::vector<Point>& polygon
);

int main(){
    int n;
    Circle circle;
    if(!(std::cin >> n >> circle.center.x >> circle.center.y >> circle.radius)) return 0;
    std::vector<Point> polygon(static_cast<std::size_t>(n));
    for(Point& point: polygon) std::cin >> point.x >> point.y;

    const long double signed_area =
        signed_circle_polygon_intersection_area(circle, polygon);
    const long double unsigned_area =
        circle_polygon_intersection_area(circle, polygon);
    if(std::abs(circle_polygon_intersection_area_odr_signed(circle, polygon) -
                signed_area) > 1e-15L){
        return 2;
    }
    if(std::abs(circle_polygon_intersection_area_odr_unsigned(circle, polygon) -
                unsigned_area) > 1e-15L){
        return 3;
    }

    std::cout << std::fixed << std::setprecision(8)
              << signed_area << '\n'
              << unsigned_area << '\n';
}
