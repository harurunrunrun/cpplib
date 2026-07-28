// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/type/definition/pick_theorem_result.hpp"
#include "../../src/algorithm/geometry/2d/result/scalar/pick_theorem_counts.hpp"
#include "../../src/algorithm/geometry/2d/aggregate/all/pick_theorem.hpp"

#include <cassert>
#include <exception>
#include <iostream>
#include <vector>

static void print_u128(__uint128_t value){
    if(value >= 10) print_u128(value / 10);
    std::cout << static_cast<char>('0' + value % 10);
}

int main(){
    const PickTheoremResult sample{3, 4};
    assert(sample.boundary_points == 3);
    assert(sample.interior_points == 4);

    int query_count;
    std::cin >> query_count;
    while(query_count-- > 0){
        int size;
        std::cin >> size;
        std::vector<IntegerPoint<long long>> polygon(
            static_cast<std::size_t>(size)
        );
        for(auto& point: polygon) std::cin >> point.x >> point.y;
        try{
            const PickTheoremResult result = pick_theorem(polygon);
            print_u128(result.boundary_points);
            std::cout << ' ';
            print_u128(result.interior_points);
            std::cout << '\n';
        }catch(const std::exception&){
            std::cout << "ERR\n";
        }
    }
}
