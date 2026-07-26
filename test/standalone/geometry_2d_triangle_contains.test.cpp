// competitive-verifier: STANDALONE
#include "../../src/algorithm/geometry/2d/triangle_contains.hpp"
#include <iostream>
int main(){ int q; std::cin>>q; while(q--){ Point a,b,c,p; std::cin>>a.x>>a.y>>b.x>>b.y>>c.x>>c.y>>p.x>>p.y; std::cout<<triangle_contains(a,b,c,p)<<'\n'; } }
