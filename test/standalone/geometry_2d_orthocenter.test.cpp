// competitive-verifier: STANDALONE
#include "../../src/algorithm/geometry/2d/point/orthocenter.hpp"
#include "geometry_2d_test_common.hpp"
#include <exception>
int main(){ geometry_test_setup_output(); int q; std::cin>>q; while(q--){ Point a,b,c; std::cin>>a.x>>a.y>>b.x>>b.y>>c.x>>c.y; try{ geometry_test_print_point(orthocenter(a,b,c)); }catch(const std::exception&){ std::cout<<"ERR\n"; } } }
