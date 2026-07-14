// competitive-verifier: PROBLEM https://www.spoj.com/problems/METEORS/

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <vector>

#include "../../src/algorithm/other/parallel_binary_search.hpp"

struct MeteorShower{
    int left = 0;
    int right = 0;
    long long amount = 0;
};

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int country_count;
    int sector_count;
    std::cin >> country_count >> sector_count;

    std::vector<std::vector<int>> sectors(static_cast<std::size_t>(country_count));
    for(int sector = 0; sector < sector_count; ++sector){
        int owner;
        std::cin >> owner;
        sectors[static_cast<std::size_t>(owner - 1)].push_back(sector);
    }

    std::vector<long long> requirement(static_cast<std::size_t>(country_count));
    for(long long& value: requirement) std::cin >> value;

    int shower_count;
    std::cin >> shower_count;
    std::vector<MeteorShower> showers(static_cast<std::size_t>(shower_count));
    for(auto& shower: showers){
        std::cin >> shower.left >> shower.right >> shower.amount;
        --shower.left;
        --shower.right;
    }

    std::vector<long long> fenwick(static_cast<std::size_t>(sector_count + 2), 0);
    const auto add_difference = [&](int index, long long value){
        for(int i = index + 1; i <= sector_count + 1; i += i & -i){
            fenwick[static_cast<std::size_t>(i)] += value;
        }
    };
    const auto point_get = [&](int index){
        long long result = 0;
        for(int i = index + 1; i > 0; i -= i & -i){
            result += fenwick[static_cast<std::size_t>(i)];
        }
        return result;
    };

    const auto answer = parallel_binary_search(
        country_count,
        shower_count,
        [&]{ std::fill(fenwick.begin(), fenwick.end(), 0); },
        [&](int operation){
            const auto& shower = showers[static_cast<std::size_t>(operation)];
            if(shower.left <= shower.right){
                add_difference(shower.left, shower.amount);
                add_difference(shower.right + 1, -shower.amount);
            }else{
                add_difference(shower.left, shower.amount);
                add_difference(sector_count, -shower.amount);
                add_difference(0, shower.amount);
                add_difference(shower.right + 1, -shower.amount);
            }
        },
        [&](int country){
            long long remaining = requirement[static_cast<std::size_t>(country)];
            if(remaining <= 0) return true;
            for(int sector: sectors[static_cast<std::size_t>(country)]){
                const long long gathered = point_get(sector);
                if(gathered >= remaining) return true;
                remaining -= gathered;
            }
            return false;
        }
    );

    for(int result: answer){
        if(result == shower_count + 1){
            std::cout << "NIE\n";
        }else{
            std::cout << result << '\n';
        }
    }
}
