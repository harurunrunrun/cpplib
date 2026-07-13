// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/convex_polygon_farthest_vertices.hpp"
#include "convex_calipers_test_common.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int query_count;
    if(!(std::cin >> query_count)) return 0;
    while(query_count-- > 0){
        NormalizedConvexPolygon polygon(read_calipers_polygon());
        const auto farthest = convex_polygon_farthest_vertices(polygon);
        if(farthest.size() != polygon.size()){
            throw std::runtime_error("invalid farthest-vertex result size");
        }
        std::cout << farthest.size();
        for(std::size_t index = 0; index < farthest.size(); ++index){
            if(farthest[index] >= polygon.size()){
                throw std::runtime_error("farthest-vertex index out of range");
            }
            const long double distance = norm(
                polygon.vertices()[index] - polygon.vertices()[farthest[index]]
            );
            std::cout << ' ' << rounded_calipers_value(distance);
        }
        std::cout << '\n';
    }
}
