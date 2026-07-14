// competitive-verifier: STANDALONE
#include "../../src/algorithm/geometry/2d/integer_orientation.hpp"
#include <iostream>
int main(){ int q; std::cin>>q; while(q--){ long long ax,ay,bx,by,cx,cy; std::cin>>ax>>ay>>bx>>by>>cx>>cy; std::cout<<integer_orientation(IntegerPoint<long long>{ax,ay},IntegerPoint<long long>{bx,by},IntegerPoint<long long>{cx,cy})<<'\n'; } }
