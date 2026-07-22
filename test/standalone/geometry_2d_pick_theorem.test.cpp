// competitive-verifier: STANDALONE
#include "../../src/algorithm/geometry/2d/query/pick_theorem.hpp"
#include <exception>
#include <iostream>
#include <vector>
static void print_u128(__uint128_t value){ if(value>=10)print_u128(value/10);std::cout<<char('0'+value%10); }
int main(){int q;std::cin>>q;while(q--){int n;std::cin>>n;std::vector<IntegerPoint<long long>>p(n);for(auto&x:p)std::cin>>x.x>>x.y;try{auto r=pick_theorem(p);print_u128(r.boundary_points);std::cout<<' ';print_u128(r.interior_points);std::cout<<'\n';}catch(const std::exception&){std::cout<<"ERR\n";}}}
