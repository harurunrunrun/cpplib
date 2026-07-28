// competitive-verifier: STANDALONE
#include <cassert>
#include <iostream>
#include "../../src/algorithm/geometry/2d/result/point_set/power_diagram.hpp"
int main(){
    int cases = 0;
    if(!(std::cin >> cases)) return 0;
    while(cases-- > 0){
        const auto result = power_diagram(
            {{{-1, 0}, 0}, {{1, 0}, 0}},
            {{-3, -2}, {3, -2}, {3, 2}, {-3, 2}}
        );
        assert(result.cells.size() == 2);
        assert(result.cells[0].size() == 4 && result.cells[1].size() == 4);
        std::cout << "OK\n";
    }
}
