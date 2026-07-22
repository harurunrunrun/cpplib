#ifndef CPPLIB_SRC_ALGORITHM_STRING_AUTOMATA_Z_ALGORITHM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_AUTOMATA_Z_ALGORITHM_HPP_INCLUDED

#include <algorithm>
#include <vector>

template<class Sequence>
std::vector<int> z_algorithm(const Sequence& s){
    const int n = (int)s.size();
    std::vector<int> z(n);
    if(n == 0){
        return z;
    }
    z[0] = n;
    int l = 0, r = 0;
    for(int i = 1; i < n; i++){
        if(i <= r){
            z[i] = std::min(r - i + 1, z[i - l]);
        }
        while(i + z[i] < n && s[z[i]] == s[i + z[i]]){
            z[i]++;
        }
        if(r < i + z[i] - 1){
            l = i;
            r = i + z[i] - 1;
        }
    }
    return z;
}

#endif  // CPPLIB_SRC_ALGORITHM_STRING_AUTOMATA_Z_ALGORITHM_HPP_INCLUDED
