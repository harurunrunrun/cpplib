// competitive-verifier: STANDALONE
#include "../../src/algorithm/geometry/2d/proper_segment_intersection.hpp"
#include <iostream>
int main(){ int q; std::cin>>q; while(q--){ Segment a,b; std::cin>>a.a.x>>a.a.y>>a.b.x>>a.b.y>>b.a.x>>b.a.y>>b.b.x>>b.b.y; std::cout<<proper_segment_intersection(a,b)<<'\n'; } }
