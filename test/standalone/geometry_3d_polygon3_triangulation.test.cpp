// competitive-verifier: STANDALONE

#include "../../src/algorithm/geometry/3d/index_set/polygon/polygon3_triangulation_indices.hpp"
#include "../../src/algorithm/geometry/3d/triangle/polygon/polygon3_triangulation_triangles.hpp"
#include "../../src/algorithm/geometry/3d/aggregate/all/polygon3_triangulation.hpp"
#include "../../src/algorithm/geometry/3d/scalar/triangle/area.hpp"
#include "../../src/algorithm/geometry/3d/scalar/polygon/polygon3_area.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](std::mt19937_64&, std::size_t){
        const Polygon3 polygon{
            {0, 0, 2}, {3, 0, 2}, {3, 1, 2},
            {1, 1, 2}, {1, 3, 2}, {0, 3, 2}
        };
        const auto indices = polygon3_triangulation_indices(polygon);
        const auto triangles = polygon3_triangulation(polygon);
        if(indices.size() != polygon.size() - 2 ||
            triangles.size() != indices.size()){
            return false;
        }
        long double sum = 0;
        for(std::size_t index = 0; index < indices.size(); ++index){
            const auto& vertices = indices[index];
            if(vertices[0] >= polygon.size() || vertices[1] >= polygon.size() ||
                vertices[2] >= polygon.size()){
                return false;
            }
            const Triangle3& triangle = triangles[index];
            if(!geometry3d_api_close(triangle.a, polygon[vertices[0]]) ||
                !geometry3d_api_close(triangle.b, polygon[vertices[1]]) ||
                !geometry3d_api_close(triangle.c, polygon[vertices[2]])){
                return false;
            }
            sum += area(triangle);
        }
        return geometry3d_api_close(sum, polygon3_area(polygon));
    });
}
