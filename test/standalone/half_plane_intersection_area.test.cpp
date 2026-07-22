// competitive-verifier: STANDALONE
#include "../../src/algorithm/geometry/2d/scalar/half_plane_intersection_area.hpp"
#include "geometry_2d_test_common.hpp"
#include <cmath>
#include <exception>
#include <vector>
int main(){geometry_test_setup_output();int q;std::cin>>q;while(q--){int n;std::cin>>n;std::vector<Line>h(n);for(auto&l:h)std::cin>>l.a.x>>l.a.y>>l.b.x>>l.b.y;try{const auto a=half_plane_intersection_area(h);if(std::isinf(a))std::cout<<"INF\n";else geometry_test_print_real(a);}catch(const std::exception&){std::cout<<"ERR\n";}}}
