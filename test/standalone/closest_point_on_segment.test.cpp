// competitive-verifier: STANDALONE
#include "../../src/algorithm/geometry/2d/point/closest_point_on_segment.hpp"
#include "geometry_2d_test_common.hpp"
int main(){geometry_test_setup_output();int q;std::cin>>q;while(q--){Segment s;Point p;std::cin>>s.a.x>>s.a.y>>s.b.x>>s.b.y>>p.x>>p.y;geometry_test_print_point(closest_point_on_segment(s,p));}}
