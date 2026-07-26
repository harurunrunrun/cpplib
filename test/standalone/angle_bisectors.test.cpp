// competitive-verifier: STANDALONE
#include "../../src/algorithm/geometry/2d/angle_bisectors.hpp"
#include "../../src/algorithm/geometry/2d/normalized_line.hpp"
#include "geometry_2d_test_common.hpp"
#include <exception>
int main(){geometry_test_setup_output();int q;std::cin>>q;while(q--){Line a,b;std::cin>>a.a.x>>a.a.y>>a.b.x>>a.b.y>>b.a.x>>b.a.y>>b.b.x>>b.b.y;try{for(const Line&l:angle_bisectors(a,b)){const auto n=normalized_line(l);geometry_test_print_real(n.a);geometry_test_print_real(n.b);geometry_test_print_real(n.c);}}catch(const std::exception&){std::cout<<"ERR\n";}}}
