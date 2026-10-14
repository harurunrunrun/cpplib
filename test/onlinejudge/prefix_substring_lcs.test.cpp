// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/prefix_substring_lcs
#include <iostream>
#include <string>
#include <vector>
#include "../../src/algorithm/string/sequence/prefix_substring_lcs.hpp"
int main(){
    std::ios::sync_with_stdio(false);std::cin.tie(nullptr);
    int q;std::string s,t;std::cin>>q>>s>>t;
    std::vector<PrefixSubstringLcsQuery> queries(q);
    for(auto& x:queries)std::cin>>x.prefix_length>>x.left>>x.right;
    for(int value:prefix_substring_lcs(s,t,queries))std::cout<<value<<'\n';
}
