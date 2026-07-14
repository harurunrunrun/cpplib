// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../../src/structure/other/kd_tree_2d.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    KdTree2D<int, 4> empty;
    assert(!empty.nearest_neighbor(0, 0).exists());

    KdTree2D<double, 4> floating({{0.0, 0.0, -1}});
    const KdTreeNearestResult2D negative_id =
        floating.nearest_neighbor(0.0, 0.0);
    assert(negative_id.exists() && negative_id.id == -1);
    bool invalid_thrown = false;
    try{
        static_cast<void>(floating.nearest_neighbor(
            std::numeric_limits<double>::quiet_NaN(), 0.0
        ));
    }catch(const std::invalid_argument&){
        invalid_thrown = true;
    }
    assert(invalid_thrown);

    int point_count, query_count;
    std::cin >> point_count >> query_count;
    std::vector<KdTreePoint2D<int>> points(
        static_cast<std::size_t>(point_count)
    );
    for(auto& point: points){
        std::cin >> point.x >> point.y >> point.id;
    }
    KdTree2D<int, 200000> tree(points);
    while(query_count-- > 0){
        int x, y;
        std::cin >> x >> y;
        const KdTreeNearestResult2D result = tree.nearest_neighbor(x, y);
        if(!result.exists()){
            std::cout << "-1 -1\n";
        }else{
            std::cout << result.id << ' '
                      << std::llround(result.squared_distance) << '\n';
        }
    }
}
