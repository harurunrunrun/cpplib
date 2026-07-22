// competitive-verifier: STANDALONE
#include "../../src/algorithm/geometry/2d/scalar/distance_ray_point.hpp"
#include "geometry_2d_test_common.hpp"
#include <exception>
int main(){ geometry_test_setup_output(); int q; std::cin>>q; while(q--){ Ray r; Point p; std::cin>>r.origin.x>>r.origin.y>>r.through.x>>r.through.y>>p.x>>p.y; try{ geometry_test_print_real(distance(r,p)); }catch(const std::exception&){ std::cout<<"ERR\n"; } } }
