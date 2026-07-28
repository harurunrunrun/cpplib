// competitive-verifier: STANDALONE
#include <cassert>
#include <iostream>
#include <limits>
#include "../../src/algorithm/geometry/2d/type/definition/polygon_medial_axis.hpp"
int main(){
    int cases = 0;
    if(!(std::cin >> cases)) return 0;
    while(cases-- > 0){
        PolygonMedialAxis axis({
            {0, 0}, {4, 0}, {4, 4}, {0, 4},
        });
        assert(axis.contains({2, 2}));
        assert(!axis.contains({1, 2}));
        assert(axis.locate({2, 2}).nearest_edges.size() == 4);
        std::cout << "OK\n";
        assert(!axis.branches().empty());
        bool center_branch = false;
        for(std::size_t branch = 0; branch < axis.branches().size(); ++branch){
            center_branch = center_branch
                || axis.on_branch(branch, {2, 2});
        }
        assert(center_branch);
        for(const Point point: {
            Point{1, 1}, Point{3, 1}, Point{3, 3}, Point{1, 3}
        }){
            assert(axis.on_axis(point));
            bool on_constructed_branch = false;
            for(std::size_t branch = 0;
                branch < axis.branches().size();
                ++branch){
                on_constructed_branch = on_constructed_branch
                    || axis.on_branch(branch, point);
            }
            assert(on_constructed_branch);
        }
        for(const MedialAxisBranch& branch: axis.branches()){
            assert(branch.curve < axis.curves().size());
            assert(branch.first_boundary_edge < axis.polygon().size());
            assert(branch.second_boundary_edge < axis.polygon().size());
        }
        PolygonMedialAxis concave({
            {0, 0}, {3, 0}, {3, 1}, {1, 1}, {1, 3}, {0, 3},
        });
        bool rejected_nonfinite = false;
        try{
            static_cast<void>(axis.on_branch(
                0,
                {std::numeric_limits<long double>::quiet_NaN(), 0}
            ));
        }catch(const std::invalid_argument&){
            rejected_nonfinite = true;
        }
        assert(rejected_nonfinite);
        bool rejected_degenerate = false;
        try{
            static_cast<void>(PolygonMedialAxis({
                {0, 0}, {1, 0}, {2, 0}
            }));
        }catch(const std::invalid_argument&){
            rejected_degenerate = true;
        }
        assert(rejected_degenerate);
        assert(!concave.on_axis({0.8L, 0.8L}));
        assert(concave.locate({0.8L, 0.8L}).nearest_points.size() == 1);
    }
}
