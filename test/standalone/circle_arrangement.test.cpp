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
        {
            // Two extra circles cross at (1, -9), whose x lies between the
            // rounded copies of the shared point (1, 0) in lexicographic
            // order. The shared point must remain a single vertex.
            std::vector<Circle> extended = coaxial;
            extended.push_back({{0, -10}, std::sqrt(2.0L)});
            extended.push_back({{2, -10}, std::sqrt(2.0L)});
            const auto extended_result = circle_arrangement(extended);
            int near_shared = 0;
            for(const Point& vertex: extended_result.vertices){
                if(std::abs(vertex.x - 1) < 1e-9L
                    && std::abs(vertex.y) < 1e-9L){
                    ++near_shared;
                }
            }
            assert(near_shared == 1);
        }
        std::cout << "OK\n";
    }
}
