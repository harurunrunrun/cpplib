// competitive-verifier: STANDALONE
#include "../../src/algorithm/geometry/2d/polygon_cut.hpp"
#include "../../src/algorithm/geometry/2d/polygon_area.hpp"
#include "geometry_2d_test_common.hpp"
#include <exception>
#include <vector>
int main(){ geometry_test_setup_output(); int q;std::cin>>q;while(q--){int n;std::cin>>n;std::vector<Point>p(n);for(auto&x:p)std::cin>>x.x>>x.y;Line l;std::cin>>l.a.x>>l.a.y>>l.b.x>>l.b.y;try{auto r=polygon_cut(p,l);long double la=0,ra=0;for(const auto&x:r.left)la+=polygon_area(x);for(const auto&x:r.right)ra+=polygon_area(x);std::cout<<r.left.size()<<' '<<r.right.size()<<'\n';geometry_test_print_real(la);geometry_test_print_real(ra);}catch(const std::exception&){std::cout<<"ERR\n";} } }
