// competitive-verifier: STANDALONE
#include "../../src/algorithm/geometry/2d/scalar/triangle_area_from_sides.hpp"
#include "geometry_2d_test_common.hpp"
#include <exception>
int main(){ geometry_test_setup_output(); int q; std::cin>>q; while(q--){ long double a,b,c; std::cin>>a>>b>>c; try{ geometry_test_print_real(triangle_area_from_sides(a,b,c)); }catch(const std::exception&){ std::cout<<"ERR\n"; } } }
