// competitive-verifier: STANDALONE
#include "../../src/algorithm/geometry/2d/core/normalized_line.hpp"
#include "geometry_2d_test_common.hpp"
#include <exception>
int main(){geometry_test_setup_output();int q;std::cin>>q;while(q--){Line l;std::cin>>l.a.x>>l.a.y>>l.b.x>>l.b.y;try{const auto n=normalized_line(l);geometry_test_print_real(n.a);geometry_test_print_real(n.b);geometry_test_print_real(n.c);}catch(const std::exception&){std::cout<<"ERR\n";}}}
