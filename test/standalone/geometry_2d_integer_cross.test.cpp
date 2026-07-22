// competitive-verifier: STANDALONE
#include "../../src/algorithm/geometry/2d/scalar/integer_cross.hpp"
#include <iostream>
int main(){ int q; std::cin>>q; while(q--){ long long ax,ay,bx,by; std::cin>>ax>>ay>>bx>>by; std::cout<<integer_cross(IntegerPoint<long long>{ax,ay},IntegerPoint<long long>{bx,by})<<'\n'; } }
