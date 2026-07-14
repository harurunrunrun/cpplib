// competitive-verifier: PROBLEM https://www.spoj.com/problems/SUBLEX/

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/string/suffix_array.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string text;
    if(!(std::cin >> text)) return 0;

    const int size = static_cast<int>(text.size());
    const std::vector<int> suffixes = suffix_array(text);
    const std::vector<int> lcp = lcp_array(text, suffixes);

    // In suffix-array order, suffix i contributes exactly the prefixes longer
    // than its LCP with the preceding suffix. Those prefixes occur
    // consecutively in increasing lexicographical order.
    std::vector<long long> prefix_count(static_cast<std::size_t>(size));
    for(int index = 0; index < size; index++){
        const int shared = index == 0 ? 0 : lcp[static_cast<std::size_t>(index - 1)];
        const long long added = size - suffixes[static_cast<std::size_t>(index)] - shared;
        prefix_count[static_cast<std::size_t>(index)] =
            added + (index == 0 ? 0 : prefix_count[static_cast<std::size_t>(index - 1)]);
    }

    int query_count;
    std::cin >> query_count;
    while(query_count--){
        long long rank;
        std::cin >> rank;
        const int index = static_cast<int>(
            std::lower_bound(prefix_count.begin(), prefix_count.end(), rank) - prefix_count.begin()
        );
        const long long preceding = index == 0 ? 0 : prefix_count[static_cast<std::size_t>(index - 1)];
        const int shared = index == 0 ? 0 : lcp[static_cast<std::size_t>(index - 1)];
        const int length = shared + static_cast<int>(rank - preceding);
        std::cout << text.substr(
            static_cast<std::size_t>(suffixes[static_cast<std::size_t>(index)]),
            static_cast<std::size_t>(length)
        ) << '\n';
    }
}
