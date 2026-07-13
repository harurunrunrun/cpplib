// competitive-verifier: PROBLEM https://atcoder.jp/contests/abc220/tasks/abc220_f

#include <iostream>
#include <utility>

#include "../../src/algorithm/tree/rerooting_dp.hpp"

struct DistanceSumMonoid{
    using S = std::pair<long long, long long>;

    constexpr S op(const S& lhs, const S& rhs) const{
        return {lhs.first + rhs.first, lhs.second + rhs.second};
    }

    constexpr S e() const{
        return {0, 0};
    }
};

constexpr DistanceSumMonoid distance_sum_monoid{};

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n;
    std::cin >> n;
    const auto add_vertex = [](std::pair<long long, long long> value, int){
        ++value.first;
        return value;
    };
    const auto add_edge = [](std::pair<long long, long long> value, int, int, int){
        value.second += value.first;
        return value;
    };
    RerootingDP<distance_sum_monoid, decltype(add_vertex), decltype(add_edge)> rerooting(
        n, add_vertex, add_edge
    );
    for(int i = 1; i < n; ++i){
        int u, v;
        std::cin >> u >> v;
        rerooting.add_edge(u - 1, v - 1);
    }
    for(const auto& [count, distance_sum]: rerooting.solve()){
        static_cast<void>(count);
        std::cout << distance_sum << '\n';
    }
}
