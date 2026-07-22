// competitive-verifier: PROBLEM https://www.spoj.com/problems/EDIST/
#include <iostream>
#include <string>
#include "../../src/algorithm/string/sequence/edit_distance.hpp"
int main(){ std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int tests; std::cin>>tests; while(tests--){ std::string a,b; std::cin>>a>>b; std::cout<<edit_distance(a,b)<<'\n'; }
}
