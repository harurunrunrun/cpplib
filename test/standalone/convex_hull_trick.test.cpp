// competitive-verifier: STANDALONE

#define PROBLEM "https://example.com/standalone/convex_hull_trick"

#include <cassert>
#include <limits>
#include <random>
#include <utility>
#include <vector>
#include "../../src/structure/convex_hull_trick/convex_hull_trick.hpp"

int main(){
    ConvexHullTrick<300> cht;
    assert(cht.empty());

    std::vector<std::pair<long long, long long>> lines;
    std::mt19937 rng(135792468);

    long long slope = 100;
    for(int step = 0; step < 200; step++){
        slope -= static_cast<int>(rng() % 3);
        long long intercept = static_cast<int>(rng() % 401) - 200;

        cht.add_line(slope, intercept);
        if(lines.empty() || lines.back().first != slope){
            lines.push_back({slope, intercept});
        }else if(intercept < lines.back().second){
            lines.back().second = intercept;
        }

        for(long long x = -100; x <= 100; x++){
            long long expected = std::numeric_limits<long long>::max();
            for(auto [a, b]: lines){
                expected = std::min(expected, a * x + b);
            }
            assert(cht.query(x) == expected);
        }
    }

    ConvexHullTrick<300> monotone_query;
    for(auto [a, b]: lines) monotone_query.add_line(a, b);
    for(long long x = -100; x <= 100; x++){
        long long expected = std::numeric_limits<long long>::max();
        for(auto [a, b]: lines){
            expected = std::min(expected, a * x + b);
        }
        assert(monotone_query.query_monotone_inc(x) == expected);
    }
}
