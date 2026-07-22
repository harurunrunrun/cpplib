// competitive-verifier: STANDALONE
#include "../../src/algorithm/geometry/2d/core/integer_point.hpp"
#include <iostream>
int main(){ int q; std::cin>>q; while(q--){ long long ax,ay,bx,by; std::cin>>ax>>ay>>bx>>by; IntegerPoint<long long>a{ax,ay},b{bx,by}; std::cout<<(a==b)<<' '<<(a<b)<<'\n'; } }
