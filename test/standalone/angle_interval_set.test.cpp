// competitive-verifier: STANDALONE

#include <cassert>
#include <cmath>
#include <iostream>
#include <string>

#include "../../src/algorithm/geometry/2d/query/angle_interval_set.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    const long double turn = 2.0L * std::acos(-1.0L);
    AngleIntervalSet api;
    api.add(0.0L, turn / 2.0L);
    const auto listed = api.intervals();
    assert(listed.size() == 1);
    assert(std::fabs(listed[0].first) < 1e-12L);
    assert(std::fabs(listed[0].second - turn / 2.0L) < 1e-12L);
    api.clear();

    int tick_count, operation_count;
    std::cin >> tick_count >> operation_count;
    const auto angle = [&](long long tick){
        return static_cast<long double>(tick) * turn / tick_count;
    };
    while(operation_count-- > 0){
        char operation;
        std::cin >> operation;
        if(operation == 'A'){
            long long start, end;
            std::cin >> start >> end;
            api.add(angle(start), angle(end));
        }else if(operation == 'R'){
            long long start, end;
            std::cin >> start >> end;
            api.remove(angle(start), angle(end));
        }else if(operation == 'P'){
            long long point;
            std::cin >> point;
            std::cout << api.contains(angle(point)) << '\n';
        }else if(operation == 'C'){
            long long start, end;
            std::cin >> start >> end;
            std::cout << api.covers(angle(start), angle(end)) << '\n';
        }else if(operation == 'L'){
            std::cout << std::llround(
                api.covered_length() * tick_count / turn
            ) << '\n';
        }else if(operation == 'N'){
            std::cout << api.interval_count() << '\n';
        }else if(operation == 'F'){
            std::cout << api.full() << '\n';
        }else if(operation == 'E'){
            std::cout << api.empty() << '\n';
        }else{
            assert(operation == 'X');
            api.clear();
        }
    }
}
