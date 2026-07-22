// competitive-verifier: PROBLEM https://www.spoj.com/problems/DSUBSEQ/
#include <iostream>
#include <string>
#include "../../src/algorithm/string/sequence/count_distinct_subsequences.hpp"
int main(){ std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int tests; std::cin>>tests; while(tests--){ std::string text; std::cin>>text;
        std::cout<<count_distinct_subsequences(text)<<'\n'; }
}
