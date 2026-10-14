// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/runenumerate
#include <algorithm>
#include <iostream>
#include <string>
#include <tuple>
#include "../../src/algorithm/string/sequence/run_enumeration.hpp"
int main(){
    std::ios::sync_with_stdio(false); std::cin.tie(nullptr);
    std::string text; std::cin >> text;
    auto runs = enumerate_runs(text);
    std::sort(runs.begin(), runs.end(), [](const auto& a, const auto& b){
        return std::tie(a.period,a.left,a.right) < std::tie(b.period,b.left,b.right);
    });
    std::cout << runs.size() << '\n';
    for(const auto& run: runs)
        std::cout << run.period << ' ' << run.left << ' ' << run.right << '\n';
}
