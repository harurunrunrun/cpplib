// competitive-verifier: STANDALONE
#include <cassert>
#include <cstdint>
#include <iostream>
#include "../../src/algorithm/geometry/2d/result/point_set/minimum_enclosing_ellipse.hpp"
int main(){
    int cases = 0;
    if(!(std::cin >> cases)) return 0;
    while(cases-- > 0){
        const std::vector<Point> points{
            {-2, 0}, {2, 0}, {0, -1}, {0, 1},
        };
        const auto result = minimum_enclosing_ellipse(points, 1.0e-8L);
        for(const Point& point: points){
            assert(result.evaluate(point) <= 1.0L + 1.0e-12L);
        }
        std::uint64_t state = 0x9e3779b97f4a7c15ULL;
        std::vector<Point> random_points;
        for(int index = 0; index < 40; ++index){
            state ^= state << 7;
            state ^= state >> 9;
            const long double x = static_cast<long double>(
                static_cast<int>(state % 2001) - 1000
            ) / 37.0L;
            state ^= state << 7;
            state ^= state >> 9;
            const long double y = static_cast<long double>(
                static_cast<int>(state % 2001) - 1000
            ) / 61.0L;
            random_points.push_back({x, y});
        }
        const auto random_result = minimum_enclosing_ellipse(
            random_points, 1.0e-4L, 200000
        );
        for(const Point& point: random_points){
            assert(random_result.evaluate(point) <= 1.0L + 1.0e-12L);
        }
        bool iteration_limit = false;
        try{
            static_cast<void>(minimum_enclosing_ellipse(
                random_points, 1.0e-18L, 1
            ));
        }catch(const std::runtime_error&){
            iteration_limit = true;
        }
        assert(iteration_limit);
        bool degenerate = false;
        try{
            static_cast<void>(minimum_enclosing_ellipse({
                {0, 0}, {1, 0}, {2, 0}, {3, 0}
            }));
        }catch(const std::domain_error&){
            degenerate = true;
        }
        assert(degenerate);
        std::cout << "OK\n";
    }
}
