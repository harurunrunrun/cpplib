// competitive-verifier: PROBLEM https://www.spoj.com/problems/IOIPALIN/
#include <iostream>
#include <string>
#include "../../src/algorithm/string/minimum_insertions_to_palindrome.hpp"
int main(){ std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n; std::string text; if(!(std::cin>>n>>text)) return 0; (void)n;
    std::cout<<minimum_insertions_to_palindrome(text)<<'\n';
}
