// competitive-verifier: STANDALONE
#include "../../src/algorithm/geometry/2d/segment_overlap.hpp"
#include "geometry_2d_test_common.hpp"
int main(){ geometry_test_setup_output(); int q; std::cin>>q; while(q--){ Segment a,b; std::cin>>a.a.x>>a.a.y>>a.b.x>>a.b.y>>b.a.x>>b.a.y>>b.b.x>>b.b.y; auto r=segment_overlap(a,b); if(!r) std::cout<<"NONE\n"; else{ geometry_test_print_point(r->a); geometry_test_print_point(r->b); } } }
