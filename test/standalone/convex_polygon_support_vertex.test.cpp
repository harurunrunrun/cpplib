// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/query/convex_polygon_support_vertex.hpp"
#include "convex_calipers_test_common.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int query_count;
    if(!(std::cin >> query_count)) return 0;
    while(query_count-- > 0){
        std::vector<Point> input = read_calipers_polygon();
        Point direction;
        std::cin >> direction.x >> direction.y;
        NormalizedConvexPolygon polygon(std::move(input));
        const std::size_t index = convex_polygon_support_vertex(polygon, direction);
        if(polygon.empty()){
            if(index != CONVEX_POLYGON_NPOS){
                throw std::runtime_error("an empty polygon has a support vertex");
            }
            std::cout << "-1 0\n";
        }else{
            if(index >= polygon.size()){
                throw std::runtime_error("support index out of range");
            }
            std::cout << index << ' '
                      << rounded_calipers_value(dot(
                          polygon.vertices()[index], direction
                      )) << '\n';
        }
    }
}
