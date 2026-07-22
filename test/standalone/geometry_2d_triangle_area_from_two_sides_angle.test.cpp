// competitive-verifier: STANDALONE
#include "../../src/algorithm/geometry/2d/scalar/triangle_area_from_two_sides_angle.hpp"
#include "geometry_2d_test_common.hpp"
#include <exception>
int main(){ geometry_test_setup_output(); int q; std::cin>>q; while(q--){ long double a,b,t; std::cin>>a>>b>>t; try{ geometry_test_print_real(triangle_area_from_two_sides_angle(a,b,t)); }catch(const std::exception&){ std::cout<<"ERR\n"; } } }
