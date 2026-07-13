// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

#include "../../src/approximate/randomized/freivalds.hpp"

int main(){
    int rows, inner, columns;
    if(!(std::cin >> rows >> inner >> columns)) return 0;
    std::vector<std::vector<std::int64_t>> first(
        static_cast<std::size_t>(rows), std::vector<std::int64_t>(static_cast<std::size_t>(inner)));
    std::vector<std::vector<std::int64_t>> second(
        static_cast<std::size_t>(inner), std::vector<std::int64_t>(static_cast<std::size_t>(columns)));
    for(auto& row: first) for(auto& value: row) std::cin >> value;
    for(auto& row: second) for(auto& value: row) std::cin >> value;
    std::vector<std::vector<std::int64_t>> product(
        static_cast<std::size_t>(rows), std::vector<std::int64_t>(static_cast<std::size_t>(columns)));
    for(int i = 0; i < rows; ++i){
        for(int k = 0; k < inner; ++k){
            for(int j = 0; j < columns; ++j){
                product[static_cast<std::size_t>(i)][static_cast<std::size_t>(j)] +=
                    first[static_cast<std::size_t>(i)][static_cast<std::size_t>(k)]
                    * second[static_cast<std::size_t>(k)][static_cast<std::size_t>(j)];
            }
        }
    }
    assert(freivalds_verify(first, second, product, 24, 777));
    if(rows != 0 && columns != 0){
        ++product[0][0];
        assert(!freivalds_verify(first, second, product, 64, 777));
    }
    const std::array<std::array<std::int64_t, 2>, 2> fixed_first{{{{1, 2}}, {{3, 4}}}};
    const std::array<std::array<std::int64_t, 2>, 2> fixed_second{{{{5, 6}}, {{7, 8}}}};
    const std::array<std::array<std::int64_t, 2>, 2> fixed_product{{{{19, 22}}, {{43, 50}}}};
    assert(freivalds_verify(fixed_first, fixed_second, fixed_product, 16, 1));
    std::cout << "OK\n";
}
