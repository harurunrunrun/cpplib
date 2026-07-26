// competitive-verifier: STANDALONE
#include "../../src/algorithm/geometry/2d/polygon_perimeter.hpp"
#include "geometry_2d_test_common.hpp"
#include <vector>
int main(){ geometry_test_setup_output(); int q; std::cin>>q; while(q--){ int n; std::cin>>n; std::vector<Point> p(n); for(auto&x:p)std::cin>>x.x>>x.y; geometry_test_print_real(polygon_perimeter(p)); } }
