// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>

#include "../../src/algorithm/geometry/2d/view_cone_contains.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    bool threw = false;
    try{
        static_cast<void>(view_cone_contains(
            {0.0L, 0.0L}, {0.0L, 0.0L}, 1.0L, {1.0L, 0.0L}
        ));
    }catch(const std::invalid_argument&){
        threw = true;
    }
    assert(threw);
    assert(view_cone_contains(
        {1.0L, 2.0L}, {1.0L, 0.0L}, 0.0L, {1.0L, 2.0L}, false
    ));

    int query_count;
    std::cin >> query_count;
    const long double radians = std::acos(-1.0L) / 180.0L;
    while(query_count-- > 0){
        long double observer_x, observer_y;
        long double direction_x, direction_y;
        long double aperture_degrees;
        long double point_x, point_y;
        int include_boundary;
        std::cin >> observer_x >> observer_y
                 >> direction_x >> direction_y
                 >> aperture_degrees
                 >> point_x >> point_y >> include_boundary;
        std::cout << view_cone_contains(
            {observer_x, observer_y},
            {direction_x, direction_y},
            aperture_degrees * radians,
            {point_x, point_y},
            include_boundary != 0
        ) << '\n';
    }
}
