// competitive-verifier: STANDALONE

#include <cassert>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>

#include "../../src/structure/other/online_rectangle_add_rectangle_sum.hpp"

using RectangleStructure = OnlineRectangleAddRectangleSum<long long>;

void run_direct_tests(){
    RectangleStructure structure(-5, 6);
    assert(structure.coordinate_low() == -5);
    assert(structure.coordinate_high() == 6);
    structure.reserve(1000);
    structure.add_rectangle(-5, 2, -3, 4, 3);
    assert(structure.rectangle_sum(-5, 2, -3, 4) == 147);
    assert(structure.rectangle_sum(-2, 0, 0, 2) == 12);
    structure.add_rectangle(-1, 4, 1, 5, -2);
    assert(structure.rectangle_sum(-1, 2, 1, 4) == 9);
    structure.add_rectangle(6, 6, -5, 6, 100);
    assert(structure.rectangle_sum(-5, 6, -5, 6) == 107);
    assert(structure.stored_node_count() != 0);
    structure.clear();
    assert(structure.rectangle_sum(-5, 6, -5, 6) == 0);

    RectangleStructure large_coordinates(
        1'000'000'000'000LL,
        1'000'000'001'000LL
    );
    large_coordinates.add_rectangle(
        1'000'000'000'010LL,
        1'000'000'000'020LL,
        1'000'000'000'030LL,
        1'000'000'000'050LL,
        -7
    );
    assert(large_coordinates.rectangle_sum(
        1'000'000'000'015LL,
        1'000'000'000'020LL,
        1'000'000'000'040LL,
        1'000'000'000'050LL
    ) == -350);

    bool invalid_threw = false;
    try{
        structure.add_rectangle(2, 1, 0, 1, 3);
    }catch(const std::invalid_argument&){
        invalid_threw = true;
    }
    assert(invalid_threw);
}

int main(){
    std::int64_t coordinate_low, coordinate_high;
    int query_count;
    if(!(std::cin >> coordinate_low >> coordinate_high >> query_count)){
        run_direct_tests();
        return 0;
    }
    RectangleStructure structure(coordinate_low, coordinate_high);
    while(query_count--){
        std::string type;
        std::int64_t x_left, x_right, y_lower, y_upper;
        std::cin >> type >> x_left >> x_right >> y_lower >> y_upper;
        if(type == "A"){
            long long value;
            std::cin >> value;
            structure.add_rectangle(
                x_left, x_right, y_lower, y_upper, value
            );
        }else{
            std::cout << structure.rectangle_sum(
                x_left, x_right, y_lower, y_upper
            ) << '\n';
        }
    }
}
