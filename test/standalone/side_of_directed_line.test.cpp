// competitive-verifier: STANDALONE
#include "../../src/algorithm/geometry/2d/side_of_directed_line.hpp"
#include <exception>
#include <iostream>
int main(){int q;std::cin>>q;while(q--){Line l;Point p;std::cin>>l.a.x>>l.a.y>>l.b.x>>l.b.y>>p.x>>p.y;try{std::cout<<side_of_directed_line(l,p)<<'\n';}catch(const std::exception&){std::cout<<"ERR\n";}}}
