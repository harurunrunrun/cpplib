// competitive-verifier: STANDALONE
#include "../../src/algorithm/geometry/2d/point/polygon_centroid.hpp"
#include "geometry_2d_test_common.hpp"
#include <exception>
#include <vector>
int main(){ geometry_test_setup_output(); int q; std::cin>>q; while(q--){ int n; std::cin>>n; std::vector<Point> p(n); for(auto&x:p)std::cin>>x.x>>x.y; try{geometry_test_print_point(polygon_centroid(p));}catch(const std::exception&){std::cout<<"ERR\n";} } }
