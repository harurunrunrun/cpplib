// competitive-verifier: STANDALONE
#include "../../src/algorithm/geometry/2d/counterclockwise_polygon.hpp"
#include "geometry_2d_test_common.hpp"
#include <vector>
int main(){ geometry_test_setup_output(); int q; std::cin>>q; while(q--){ int n; std::cin>>n; std::vector<Point> p(n); for(auto&x:p)std::cin>>x.x>>x.y; p=counterclockwise_polygon(std::move(p)); for(const auto&x:p)geometry_test_print_point(x); } }
