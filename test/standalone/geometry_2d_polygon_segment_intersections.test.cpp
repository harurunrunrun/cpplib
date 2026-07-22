// competitive-verifier: STANDALONE
#include "../../src/algorithm/geometry/2d/point_collection/polygon_segment_intersections.hpp"
#include "geometry_2d_test_common.hpp"
#include <vector>
int main(){ geometry_test_setup_output(); int q; std::cin>>q; while(q--){ int n; std::cin>>n; std::vector<Point> p(n); for(auto&x:p)std::cin>>x.x>>x.y; Segment s;std::cin>>s.a.x>>s.a.y>>s.b.x>>s.b.y;auto r=polygon_segment_intersections(p,s);std::cout<<r.size()<<'\n';for(const auto&x:r)geometry_test_print_point(x); } }
