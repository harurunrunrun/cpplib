// competitive-verifier: STANDALONE
#include "../../src/algorithm/geometry/2d/scalar/integer_dot.hpp"
#include <iostream>
int main(){ int q; std::cin>>q; while(q--){ long long ax,ay,bx,by; std::cin>>ax>>ay>>bx>>by; std::cout<<integer_dot(IntegerPoint<long long>{ax,ay},IntegerPoint<long long>{bx,by})<<'\n'; } }
