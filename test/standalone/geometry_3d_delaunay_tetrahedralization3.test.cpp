// competitive-verifier: STANDALONE

#include <array>
#include <cstddef>

#include "../../src/algorithm/geometry/3d/shape/delaunay_tetrahedralization_result3.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](auto&, std::size_t){
        DelaunayTetrahedralization3 value;
        if(value.affine_dimension != -1 || !value.vertices.empty()
            || !value.tetrahedra.empty()) return false;
        value.affine_dimension = 3;
        value.vertices = {{0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
        value.tetrahedra.push_back({0, 1, 2, 3});
        return value.vertices.size() == 4 && value.tetrahedra.front()[3] == 3;
    });
}
