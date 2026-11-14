// competitive-verifier: STANDALONE
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>
#include "../../src/algorithm/geometry/2d/result/line_set/line_arrangement.hpp"
int main(){
    int cases = 0;
    if(!(std::cin >> cases)) return 0;
    while(cases-- > 0){
        const auto result = line_arrangement({
            {{0, 0}, {1, 0}},
            {{0, 0}, {0, 1}},
            {{1, 0}, {0, 1}},
        });
        assert(result.rays.size() == 6);
        assert(result.unbounded_lines.empty());
        assert(result.vertices.size() == 3);
        assert(result.bounded_edges.size() == 3);
        std::vector<Line> general_position;
        for(int index = 0; index < 20; ++index){
            const long double slope = index;
            const long double intercept = index * index;
            general_position.push_back({
                {0, intercept}, {1, intercept + slope}
            });
        }
        const auto large = line_arrangement(general_position);
        assert(large.vertices.size() == 190);
        assert(large.bounded_edges.size() == 360);
        std::vector<Line> concurrent;
        for(int index = -10; index <= 10; ++index){
            const Point center{1.25L, -2.75L};
            concurrent.push_back({
                center,
                center + Point{1, static_cast<long double>(index)},
            });
        }
        const auto concurrent_result = line_arrangement(concurrent);
        assert(concurrent_result.vertices.size() == 1);
        assert(concurrent_result.rays.size() == 42);
        const auto isolated = line_arrangement({{{0, 0}, {1, 1}}});
        assert(isolated.unbounded_lines.size() == 1);
        {
            // A vertical line passes one ulp to the right of the crossing
            // of two lines; the far horizontal line puts the point
            // (x, -100) between the close points in lexicographic order.
            const Line first{{0, 0}, {3, 1}};
            const Line second{{0, 1}, {1, 0}};
            const Point crossing =
                line_arrangement({first, second}).vertices.front();
            const long double x = std::nextafter(crossing.x, 10.0L);
            const Line vertical{{x, 0}, {x, 1}};
            const Line horizontal{{-5, -100}, {5, -100}};
            const auto merged =
                line_arrangement({vertical, first, second});
            assert(merged.vertices.size() == 1);
            const auto separated =
                line_arrangement({vertical, first, second, horizontal});
            assert(separated.vertices.size() == 4);
            assert(separated.bounded_edges.size() == 5);
        }
        std::cout << "OK\n";
    }
}
