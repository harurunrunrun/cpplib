// competitive-verifier: PROBLEM https://www.spoj.com/problems/TRT/
#include <iostream>
#include <vector>
#include "../../src/algorithm/other/maximum_time_weighted_endpoint_removal_score.hpp"
int main(){ std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int n; if(!(std::cin>>n)) return 0;
    std::vector<long long> values(n);
    for(long long& value:values) std::cin>>value;
    std::cout<<maximum_time_weighted_endpoint_removal_score(values)<<'\n';
}
