// competitive-verifier: STANDALONE
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <numbers>
#include <utility>
#include <vector>
#include "../../src/algorithm/geometry/2d/result/point_set_index_pair_set/constrained_delaunay_triangulation.hpp"
int main(){
    int cases = 0;
    if(!(std::cin >> cases)) return 0;
    while(cases-- > 0){
        const auto result = constrained_delaunay_triangulation(
            {{0, 0}, {2, 0}, {2, 2}, {0, 2}},
            {{0, 2}}
        );
        assert(std::find(
            result.edges.begin(), result.edges.end(),
            std::pair<std::size_t, std::size_t>{0, 2}
        ) != result.edges.end());
        for(std::size_t size = 5; size <= 12; ++size){
            std::vector<Point> points;
            for(std::size_t index = 0; index < size; ++index){
                const long double angle = 2.0L
                    * std::numbers::pi_v<long double>
                    * static_cast<long double>(index)
                    / static_cast<long double>(size);
                points.push_back({
                    std::cos(angle) * (10.0L + index * 0.01L),
                    std::sin(angle) * (10.0L + index * 0.01L),
                });
            }
            const auto triangulation = constrained_delaunay_triangulation(
                points, {{0, size / 2}}
            );
            assert(std::find(
                triangulation.edges.begin(), triangulation.edges.end(),
                std::pair<std::size_t, std::size_t>{0, size / 2}
            ) != triangulation.edges.end());
        }
        const auto split = constrained_delaunay_triangulation(
            {{0, 0}, {1, 0}, {2, 0}, {0, 2}, {2, 2}},
            {{0, 2}, {2, 0}, {1, 3}}
        );
        for(const auto edge: {
            std::pair<std::size_t, std::size_t>{0, 1},
            std::pair<std::size_t, std::size_t>{1, 2},
            std::pair<std::size_t, std::size_t>{1, 3},
        }){
            assert(std::find(
                split.constraints.begin(), split.constraints.end(), edge
            ) != split.constraints.end());
            assert(std::find(
                split.edges.begin(), split.edges.end(), edge
            ) != split.edges.end());
        }
        const auto collinear = constrained_delaunay_triangulation(
            {{0, 0}, {1, 0}, {2, 0}}, {{0, 2}}
        );
        assert(collinear.triangles.empty());
        assert(collinear.constraints.size() == 2);
        bool rejected_duplicate_endpoint = false;
        try{
            static_cast<void>(constrained_delaunay_triangulation(
                {{0, 0}, {0, 0}, {1, 1}}, {{0, 1}}
            ));
        }catch(const std::invalid_argument&){
            rejected_duplicate_endpoint = true;
        }
        assert(rejected_duplicate_endpoint);
        std::cout << "OK\n";
    }
}
