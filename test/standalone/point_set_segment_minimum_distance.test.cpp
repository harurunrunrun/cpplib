// competitive-verifier: STANDALONE
#include "../../src/algorithm/geometry/2d/point_set_segment_minimum_distance.hpp"
#include "geometry_2d_test_common.hpp"
#include <cmath>
#include <vector>
int main(){geometry_test_setup_output();int q;std::cin>>q;while(q--){int n;std::cin>>n;std::vector<Point>p(n);for(auto&x:p)std::cin>>x.x>>x.y;Segment s;std::cin>>s.a.x>>s.a.y>>s.b.x>>s.b.y;const auto d=point_set_segment_minimum_distance(p,s);if(std::isinf(d))std::cout<<"INF\n";else geometry_test_print_real(d);}}
