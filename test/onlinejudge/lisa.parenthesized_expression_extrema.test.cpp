// competitive-verifier: PROBLEM https://www.spoj.com/problems/LISA/
#include <iostream>
#include <string>
#include "../../src/algorithm/other/parenthesized_expression_extrema.hpp"
int main(){ std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int tests; std::cin>>tests; while(tests--){ std::string expression; std::cin>>expression;
        auto answer=parenthesized_expression_extrema(expression);
        std::cout<<answer.maximum<<' '<<answer.minimum<<'\n'; }
}
