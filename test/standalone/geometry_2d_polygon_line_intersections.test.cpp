// competitive-verifier: STANDALONE
#include "../../src/algorithm/geometry/2d/point_collection/polygon_line_intersections.hpp"
#include "geometry_2d_test_common.hpp"
#include <exception>
#include <vector>
int main(){ geometry_test_setup_output(); int q; std::cin>>q; while(q--){ int n; std::cin>>n; std::vector<Point> p(n); for(auto&x:p)std::cin>>x.x>>x.y; Line l;std::cin>>l.a.x>>l.a.y>>l.b.x>>l.b.y;try{auto r=polygon_line_intersections(p,l);std::cout<<r.size()<<'\n';for(const auto&x:r)geometry_test_print_point(x);}catch(const std::exception&){std::cout<<"ERR\n";} } }
