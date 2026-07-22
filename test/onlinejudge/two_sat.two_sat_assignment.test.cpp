// competitive-verifier: PROBLEM https://judge.yosupo.jp/problem/two_sat

#include <cstdlib>
#include <iostream>
#include "../../src/algorithm/graph/constraints/two_sat_assignment.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    char p;
    std::string format;
    int n, m;
    std::cin >> p >> format >> n >> m;
    (void)p;
    (void)format;
    TwoSat solver(n);
    while(m--){
        int a, b, zero;
        std::cin >> a >> b >> zero;
        (void)zero;
        solver.add_clause(std::abs(a) - 1, a > 0, std::abs(b) - 1, b > 0);
    }
    const auto assignment = two_sat_assignment(solver);
    if(!assignment){
        std::cout << "s UNSATISFIABLE\n";
        return 0;
    }
    std::cout << "s SATISFIABLE\nv";
    for(int i = 0; i < n; ++i){
        std::cout << ' ' << ((*assignment)[static_cast<std::size_t>(i)] ? i + 1 : -i - 1);
    }
    std::cout << " 0\n";
}
