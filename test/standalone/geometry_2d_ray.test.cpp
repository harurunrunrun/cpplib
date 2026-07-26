// competitive-verifier: STANDALONE
#include "../../src/algorithm/geometry/2d/ray.hpp"
#include "geometry_2d_test_common.hpp"
int main(){ geometry_test_setup_output(); int q; std::cin>>q; while(q--){ Ray r; std::cin>>r.origin.x>>r.origin.y>>r.through.x>>r.through.y; geometry_test_print_point(r.origin); geometry_test_print_point(r.through); } }
