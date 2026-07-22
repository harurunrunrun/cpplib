// competitive-verifier: STANDALONE
#include "../../src/algorithm/geometry/2d/point/triangle_centroid.hpp"
#include "geometry_2d_test_common.hpp"
int main(){ geometry_test_setup_output(); int q; std::cin>>q; while(q--){ Point a,b,c; std::cin>>a.x>>a.y>>b.x>>b.y>>c.x>>c.y; geometry_test_print_point(triangle_centroid(a,b,c)); } }
