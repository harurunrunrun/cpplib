// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>

#include "../../src/structure/graph/coordinate_product_knight_distances.hpp"

void self_test(){
    CoordinateProductKnightDistances board(8);
    assert(board.board_size() == 8);
    assert(board.distance(2, 5, 5, 2) == 42);
    assert(board.distance(1, 2, 3, 4) == 18);
    assert(board.distance(7, 7, 7, 7) == 0);
    board.clear_cache();
    assert(board.distance(2, 5, 5, 2) == 42);

    CoordinateProductKnightDistances small(2);
    assert(!small.distance(0, 0, 1, 1));

    bool thrown = false;
    try{
        (void)board.distance(-1, 0, 0, 0);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    self_test();

    int board_size, query_count;
    if(!(std::cin >> board_size >> query_count)) return 0;
    CoordinateProductKnightDistances board(board_size);
    while(query_count-- > 0){
        int start_x, start_y, target_x, target_y;
        std::cin >> start_x >> start_y >> target_x >> target_y;
        std::cout << board.distance(start_x, start_y, target_x, target_y)
                         .value_or(-1)
                  << '\n';
    }
}
