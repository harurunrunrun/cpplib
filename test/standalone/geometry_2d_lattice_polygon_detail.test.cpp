// competitive-verifier: STANDALONE
#include "../../src/algorithm/geometry/2d/lattice_polygon_detail.hpp"
#include <iostream>
#include <vector>
static void print_u128(__uint128_t value){ if(value>=10)print_u128(value/10);std::cout<<char('0'+value%10); }
int main(){ long long a,b;std::cin>>a>>b;print_u128(lattice_polygon_detail::unsigned_difference(a,b));std::cout<<'\n';std::vector<IntegerPoint<long long>>p{{0,0},{4,0},{0,3}};std::cout<<lattice_polygon_detail::signed_double_area(p)<<'\n'; }
