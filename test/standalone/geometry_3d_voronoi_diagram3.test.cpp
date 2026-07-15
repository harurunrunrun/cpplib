// competitive-verifier: STANDALONE

#include <cstddef>

#include "../../src/algorithm/geometry/3d/voronoi_diagram3.hpp"
#include "geometry_3d_api_test_common.hpp"

int main(){
    return geometry3d_api_test_main([](auto&, std::size_t){
        VoronoiDiagram3 diagram;
        diagram.sites.push_back({1, 2, 3});
        VoronoiCell3 cell;
        cell.site = 0;
        cell.unbounded = true;
        cell.boundary_halfspaces.push_back({{0, 0, 0}, {1, 0, 0}});
        diagram.cells.push_back(cell);
        VoronoiRidge3 ridge;
        ridge.sites = {0, 1};
        ridge.unbounded = true;
        ridge.unbounded_rays.push_back({{0, 0, 0}, {0, 0, 1}});
        diagram.ridges.push_back(ridge);
        VoronoiEdge3 edge;
        edge.sites = {0, 1, 2};
        edge.segment = Segment3{{0, 0, 0}, {1, 0, 0}};
        diagram.edges.push_back(edge);
        return diagram.affine_dimension == -1 && diagram.cells.front().unbounded
            && diagram.ridges.front().sites[1] == 1
            && diagram.edges.front().segment.has_value();
    });
}
