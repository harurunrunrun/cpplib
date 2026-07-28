// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/2d/type/definition/convex_polygon_tangents_result.hpp"
#include "../../src/algorithm/geometry/2d/result/point/convex_polygon_tangents_normalized.hpp"
#include "../../src/algorithm/geometry/2d/result/point_set_point/convex_polygon_tangents_points.hpp"
#include "../../src/algorithm/geometry/2d/aggregate/all/convex_polygon_tangents.hpp"
#include "convex_calipers_test_common.hpp"

namespace{

void self_check(){
    const ConvexPolygonTangentResult invalid;
    if(invalid.left != CONVEX_POLYGON_NPOS
        || invalid.right != CONVEX_POLYGON_NPOS){
        throw std::runtime_error("a default tangent result must be invalid");
    }

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

    const ConvexPolygonTangentResult from_points = convex_polygon_tangents(
        std::vector<Point>{{0, 0}, {4, 0}, {4, 4}, {0, 4}},
        Point{8, 2}
    );
    if(from_points.left >= 4 || from_points.right >= 4
        || from_points.left == from_points.right){
        throw std::runtime_error("the point-sequence tangent overload failed");
    }
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
