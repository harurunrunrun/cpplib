// competitive-verifier: PROBLEM https://atcoder.jp/contests/abc287/tasks/abc287_e

#include <iostream>
#include <string>
#include <vector>

#include "../../src/algorithm/string/longest_shared_prefix_lengths.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    std::cin >> n;
    std::vector<std::string> strings(static_cast<std::size_t>(n));
    for(std::string& value: strings) std::cin >> value;
    for(const std::size_t length: longest_shared_prefix_lengths(strings)){
        std::cout << length << '\n';
    }
}
