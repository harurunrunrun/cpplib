// competitive-verifier: STANDALONE
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>
#include "../../src/algorithm/geometry/2d/result/circle_set/circle_arrangement.hpp"
int main(){
    int cases = 0;
    if(!(std::cin >> cases)) return 0;
    while(cases-- > 0){
        const auto result = circle_arrangement({
            {{0, 0}, 2},
            {{2, 0}, 2},
        });
        assert(result.vertices.size() == 2);
        assert(result.arcs.size() == 4);
        std::vector<Circle> coaxial;
        for(int index = -5; index <= 5; ++index){
            const long double y = static_cast<long double>(index) / 3.0L;
            coaxial.push_back({
                {0, y}, std::sqrt(1.0L + y * y),
            });
        }
        const auto shared = circle_arrangement(coaxial);
        assert(shared.vertices.size() == 2);
        assert(shared.arcs.size() == 2 * coaxial.size());
        std::cout << "OK\n";
    }
}
