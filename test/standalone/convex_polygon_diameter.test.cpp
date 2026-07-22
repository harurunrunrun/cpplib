// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/detail/convex_calipers_detail.hpp"
#include "../../src/algorithm/geometry/2d/scalar/convex_polygon_diameter.hpp"
#include "../../src/algorithm/geometry/2d/core/rotating_calipers.hpp"
#include "convex_calipers_test_common.hpp"

namespace{

void self_check(){
    const NormalizedConvexPolygon empty({});
    const auto empty_result = convex_polygon_diameter(empty);
    if(empty_result.first != CONVEX_POLYGON_NPOS
        || empty_result.second != CONVEX_POLYGON_NPOS){
        throw std::runtime_error("an empty polygon has a diameter pair");
    }

    const auto small = convex_polygon_diameter(std::vector<Point>{
        {0.0L, 0.0L}, {1e-6L, 0.0L}, {0.0L, 1e-6L},
    });
    if(small.squared_distance <= 1e-12L
        || small.first == small.second){
        throw std::runtime_error("small-scale distances were treated as zero");
    }

    bool thrown = false;
    try{
        (void)convex_polygon_diameter(
            std::vector<Point>{{0, 0}, {3, 0}, {1, 1}, {3, 3}, {0, 3}}
        );
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    if(!thrown) throw std::runtime_error("a non-convex input was accepted");
}

}  // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    self_check();

    int query_count;
    if(!(std::cin >> query_count)) return 0;
    while(query_count-- > 0){
        NormalizedConvexPolygon polygon(read_calipers_polygon());
        const auto result = convex_polygon_diameter(polygon);
        if(polygon.empty()){
            if(result.first != CONVEX_POLYGON_NPOS
                || result.second != CONVEX_POLYGON_NPOS){
                throw std::runtime_error("invalid empty diameter result");
            }
        }else{
            if(result.first >= polygon.size() || result.second >= polygon.size()){
                throw std::runtime_error("diameter index out of range");
            }
            check_calipers_equal(
                result.squared_distance,
                norm(polygon.vertices()[result.first] - polygon.vertices()[result.second])
            );
        }
        std::cout << rounded_calipers_value(result.squared_distance) << '\n';
    }
}
