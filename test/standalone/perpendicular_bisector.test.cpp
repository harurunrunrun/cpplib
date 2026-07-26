// competitive-verifier: STANDALONE
#include "../../src/algorithm/geometry/2d/perpendicular_bisector.hpp"
#include "geometry_2d_test_common.hpp"
#include <exception>
int main(){geometry_test_setup_output();int q;std::cin>>q;while(q--){Point a,b;std::cin>>a.x>>a.y>>b.x>>b.y;try{const auto l=perpendicular_bisector(a,b);geometry_test_print_point(l.a);geometry_test_print_point(l.b);}catch(const std::exception&){std::cout<<"ERR\n";}}}
