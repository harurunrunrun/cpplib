// competitive-verifier: STANDALONE
#include "../../src/algorithm/geometry/2d/points_from_two_distances.hpp"
#include "geometry_2d_test_common.hpp"
#include <exception>
int main(){geometry_test_setup_output();int q;std::cin>>q;while(q--){Point a,b;long double x,y;std::cin>>a.x>>a.y>>x>>b.x>>b.y>>y;try{const auto r=points_from_two_distances(a,x,b,y);std::cout<<r.size()<<'\n';for(const auto&p:r)geometry_test_print_point(p);}catch(const std::exception&){std::cout<<"ERR\n";}}}
