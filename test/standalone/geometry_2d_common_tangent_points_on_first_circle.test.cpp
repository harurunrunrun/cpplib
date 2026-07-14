// competitive-verifier: STANDALONE

#include <iomanip>
#include <iostream>
#include <stdexcept>

#include "../../src/algorithm/geometry/2d/common_tangent_points_on_first_circle.hpp"

int main(){
    std::cout << std::fixed << std::setprecision(9);
    int query_count;
    std::cin >> query_count;
    while(query_count--){
        Circle first, second;
        std::cin >> first.center.x >> first.center.y >> first.radius;
        std::cin >> second.center.x >> second.center.y >> second.radius;
        try{
            const auto points =
                common_tangent_points_on_first_circle(first, second);
            std::cout << points.size();
            for(const Point& point: points){
                std::cout << ' ' << point.x << ' ' << point.y;
            }
            std::cout << '\n';
        }catch(const std::domain_error&){
            std::cout << "DOMAIN\n";
        }catch(const std::invalid_argument&){
            std::cout << "INVALID\n";
        }
    }
}
