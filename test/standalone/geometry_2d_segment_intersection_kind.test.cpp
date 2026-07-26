// competitive-verifier: STANDALONE
#include "../../src/algorithm/geometry/2d/segment_intersection_kind.hpp"
#include <iostream>
int main(){ int q; std::cin>>q; while(q--){ Segment a,b; std::cin>>a.a.x>>a.a.y>>a.b.x>>a.b.y>>b.a.x>>b.a.y>>b.b.x>>b.b.y; std::cout<<segment_intersection_kind(a,b)<<'\n'; } }
