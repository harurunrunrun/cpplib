// competitive-verifier: STANDALONE
#include "../../src/algorithm/geometry/2d/line_segment_intersection.hpp"
#include "geometry_2d_test_common.hpp"
#include <exception>
int main(){ geometry_test_setup_output(); int q; std::cin>>q; while(q--){ Line a; Segment b; std::cin>>a.a.x>>a.a.y>>a.b.x>>a.b.y>>b.a.x>>b.a.y>>b.b.x>>b.b.y; try{ geometry_test_print_point(line_segment_intersection(a,b)); }catch(const std::exception&){ std::cout<<"ERR\n"; } } }
