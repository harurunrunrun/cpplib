// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/convex_polygon_tangents.hpp"
#include "convex_calipers_test_common.hpp"

namespace{

void self_check(){
    const NormalizedConvexPolygon square({
        {0, 0}, {4, 0}, {4, 4}, {0, 4},
    });
    bool thrown = false;
    try{
        (void)convex_polygon_tangents(square, {2, 2});
    }catch(const std::invalid_argument&){
        thrown = true;
    }
    if(!thrown) throw std::runtime_error("an interior tangent source was accepted");
}

}  // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    self_check();

    int query_count;
    if(!(std::cin >> query_count)) return 0;
    while(query_count-- > 0){
        std::vector<Point> input = read_calipers_polygon();
        Point point;
        std::cin >> point.x >> point.y;
        NormalizedConvexPolygon polygon(std::move(input));
        const ConvexPolygonTangentResult result = convex_polygon_tangents(
            polygon, point
        );
        if(polygon.empty()){
            if(result.left != CONVEX_POLYGON_NPOS
                || result.right != CONVEX_POLYGON_NPOS){
                throw std::runtime_error("an empty polygon has tangent points");
            }
            std::cout << "-1 -1\n";
            continue;
        }
        if(result.left >= polygon.size() || result.right >= polygon.size()){
            throw std::runtime_error("tangent index out of range");
        }
        for(const Point& vertex: polygon.vertices()){
            if(advanced_geometry_detail::cross_sign(
                polygon.vertices()[result.left] - point, vertex - point
            ) < 0){
                throw std::runtime_error("the left tangent is not supporting");
            }
            if(advanced_geometry_detail::cross_sign(
                polygon.vertices()[result.right] - point, vertex - point
            ) > 0){
                throw std::runtime_error("the right tangent is not supporting");
            }
        }
        std::cout << result.left << ' ' << result.right << '\n';
    }
}
