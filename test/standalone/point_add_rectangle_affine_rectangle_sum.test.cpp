// competitive-verifier: STANDALONE

#include <cassert>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/structure/other/point_add_rectangle_affine_rectangle_sum.hpp"

using PointStructure = PointAddRectangleAffineRectangleSum<long long>;

void run_direct_tests(){
    PointStructure structure;
    assert(structure.size() == 0);
    assert(structure.point_add(0, 0, 2));
    assert(structure.point_add(1, 1, 3));
    assert(!structure.point_add(0, 0, 5));
    assert(structure.rectangle_sum(-10, 10, -10, 10) == 10);
    structure.rectangle_affine(-1, 2, -1, 2, 2, 1);
    assert(structure.rectangle_sum(-10, 10, -10, 10) == 22);
    structure.rectangle_affine(0, 1, 0, 1, -1, 4);
    assert(structure.rectangle_sum(0, 1, 0, 1) == -11);
    assert(structure.rectangle_sum(1, 2, 1, 2) == 7);
    assert(structure.point_add(5, 5, 9));
    assert(structure.rectangle_sum(5, 6, 5, 6) == 9);

    const std::vector<PointStructure::Point> initial{
        {-1'000'000'000'000LL, 1'000'000'000'000LL, 4},
        {1'000'000'000'000LL, -1'000'000'000'000LL, -6},
    };
    PointStructure large(initial);
    large.rectangle_affine(
        -1'000'000'000'001LL,
        -999'999'999'999LL,
        999'999'999'999LL,
        1'000'000'000'001LL,
        3,
        -2
    );
    assert(large.rectangle_sum(
        -1'000'000'000'001LL,
        1'000'000'000'001LL,
        -1'000'000'000'001LL,
        1'000'000'000'001LL
    ) == 4);

    bool invalid_threw = false;
    try{
        structure.rectangle_sum(1, 0, 0, 1);
    }catch(const std::invalid_argument&){
        invalid_threw = true;
    }
    assert(invalid_threw);
    structure.clear();
    assert(structure.size() == 0);
}

int main(){
    int query_count;
    if(!(std::cin >> query_count)){
        run_direct_tests();
        return 0;
    }
    PointStructure structure;
    while(query_count--){
        std::string type;
        std::cin >> type;
        if(type == "P"){
            std::int64_t x, y;
            long long delta;
            std::cin >> x >> y >> delta;
            std::cout << structure.point_add(x, y, delta) << '\n';
        }else if(type == "A"){
            std::int64_t x_left, x_right, y_lower, y_upper;
            long long multiplier, addend;
            std::cin >> x_left >> x_right >> y_lower >> y_upper
                     >> multiplier >> addend;
            structure.rectangle_affine(
                x_left,
                x_right,
                y_lower,
                y_upper,
                multiplier,
                addend
            );
        }else if(type == "Q"){
            std::int64_t x_left, x_right, y_lower, y_upper;
            std::cin >> x_left >> x_right >> y_lower >> y_upper;
            std::cout << structure.rectangle_sum(
                x_left, x_right, y_lower, y_upper
            ) << '\n';
        }else{
            std::cout << structure.size() << '\n';
        }
    }
}
