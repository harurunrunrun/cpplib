// competitive-verifier: STANDALONE

#include <concepts>
#include <iostream>

#include "../../src/algorithm/geometry/2d/geometry_integer_wide.hpp"
#include "../../src/algorithm/geometry/2d/integer_dot_integer_points.hpp"
#include "../../src/algorithm/geometry/2d/integer_dot.hpp"

int main(){
    static_assert(std::same_as<GeometryIntegerWide, ExactInteger>);
    static_assert(std::same_as<
        decltype(integer_dot(IntegerPoint<int>{}, IntegerPoint<int>{})),
        GeometryIntegerWide
    >);

    int query_count;
    std::cin >> query_count;
    while(query_count--){
        long long first_x;
        long long first_y;
        long long second_x;
        long long second_y;
        std::cin >> first_x >> first_y >> second_x >> second_y;
        std::cout << integer_dot(
            IntegerPoint<long long>{first_x, first_y},
            IntegerPoint<long long>{second_x, second_y}
        ) << '\n';
    }
}
