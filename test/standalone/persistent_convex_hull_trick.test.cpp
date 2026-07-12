// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <utility>
#include <vector>
#include "../../src/structure/convex_hull_trick/persistent_convex_hull_trick.hpp"

long long naive_query(const std::vector<std::pair<long long, long long>>& lines, long long x){
    long long result = std::numeric_limits<long long>::max();
    for(auto [a, b]: lines) result = std::min(result, a * x + b);
    return result;
}

int main(){
    int input_q;
    if(std::cin >> input_q){
        PersistentConvexHullTrick<512, 2048, 40000> cht;
        while(input_q--){
            std::string type;
            std::cin >> type;
            if(type == "ADD"){
                int version;
                long long a, b;
                std::cin >> version >> a >> b;
                std::cout << cht.add_line(version, a, b) << '\n';
            }else if(type == "FORK"){
                int version;
                std::cin >> version;
                std::cout << cht.fork(version) << '\n';
            }else if(type == "QUERY"){
                int version;
                long long x;
                std::cin >> version >> x;
                std::cout << cht.query(version, x) << '\n';
            }else if(type == "EMPTY"){
                int version;
                std::cin >> version;
                std::cout << cht.empty(version) << '\n';
            }
        }
        return 0;
    }

    PersistentConvexHullTrick<256, 512, 6000> cht;
    std::vector<std::vector<std::pair<long long, long long>>> naive(1);
    std::vector<long long> last_slope = {1000};
    std::mt19937 rng(314159265);

    assert(cht.versions() == 1);
    assert(cht.empty(0));
    assert(cht.size(0) == 0);

    for(int step = 0; step < 250; step++){
        int base = static_cast<int>(rng() % naive.size());
        int type = static_cast<int>(rng() % 5);
        int created;
        if(type == 0){
            created = cht.fork(base);
            naive.push_back(naive[static_cast<std::size_t>(base)]);
            last_slope.push_back(last_slope[static_cast<std::size_t>(base)]);
        }else{
            long long a = last_slope[static_cast<std::size_t>(base)] - static_cast<int>(rng() % 4);
            long long b = static_cast<int>(rng() % 401) - 200;
            created = cht.add_line(base, a, b);
            auto next = naive[static_cast<std::size_t>(base)];
            if(next.empty() || next.back().first != a){
                next.push_back({a, b});
            }else if(b < next.back().second){
                next.back().second = b;
            }
            last_slope.push_back(a);
            naive.push_back(next);
        }
        assert(created == static_cast<int>(naive.size()) - 1);

        for(int version = 0; version < static_cast<int>(naive.size()); version++){
            assert(cht.empty(version) == naive[static_cast<std::size_t>(version)].empty());
            if(naive[static_cast<std::size_t>(version)].empty()) continue;
            for(long long x = -50; x <= 50; x += 5){
                assert(cht.query(version, x) == naive_query(naive[static_cast<std::size_t>(version)], x));
            }
        }
    }
}
