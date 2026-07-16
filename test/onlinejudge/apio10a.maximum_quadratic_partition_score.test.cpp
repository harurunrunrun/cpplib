// competitive-verifier: PROBLEM https://www.spoj.com/problems/APIO10A/
#include <iostream>
#include <vector>
#include "../../src/algorithm/other/maximum_quadratic_partition_score.hpp"
int main(){ std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    int tests; std::cin>>tests; while(tests--){ int n; long long a,b,c; std::cin>>n>>a>>b>>c;
        std::vector<long long> values(n); for(long long& value:values) std::cin>>value;
        std::cout<<maximum_quadratic_partition_score(values,a,b,c)<<'\n'; }
    return 0;
}
