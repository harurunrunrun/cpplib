// competitive-verifier: STANDALONE

#define PROBLEM "https://example.com/standalone/convex_hull_trick"

#include <cassert>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>
#include "../../src/structure/convex_hull_trick/convex_hull_trick.hpp"

int main(){
    int line_count, query_count, monotone_count;
    if(std::cin >> line_count >> query_count >> monotone_count){
        std::vector<std::pair<long long, long long>> input_lines(
            static_cast<std::size_t>(line_count)
        );
        ConvexHullTrick<512> arbitrary;
        ConvexHullTrick<512> monotone;
        for(auto& [a, b]: input_lines){
            std::cin >> a >> b;
            arbitrary.add_line(a, b);
            monotone.add_line(a, b);
        }
        while(query_count--){
            long long x;
            std::cin >> x;
            std::cout << arbitrary.query(x) << '\n';
        }
        while(monotone_count--){
            long long x;
            std::cin >> x;
            std::cout << monotone.query_monotone_inc(x) << '\n';
        }
        return 0;
    }

    ConvexHullTrick<300> cht;
    assert(cht.empty());
    assert(cht.size() == 0);
    bool thrown = false;
    try{
        (void)cht.query(0);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
    thrown = false;
    try{
        (void)cht.query_monotone_inc(0);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
    assert((ConvexHullTrick<1>::Line{
        std::numeric_limits<long long>::max(), 1
    }.eval(2) == std::numeric_limits<long long>::max()));

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
    {
        ConvexHullTrick<4> boundary;
        std::vector<std::pair<long long, long long>> boundary_lines = {
            {3, -8000000000000000000LL},
            {2, 8000000000000000000LL},
            {1, -7000000000000000000LL},
        };
        for(auto [a, b]: boundary_lines) boundary.add_line(a, b);
        for(long long x: {-10LL, 0LL, 10LL}){
            long long expected = std::numeric_limits<long long>::max();
            for(auto [a, b]: boundary_lines){
                expected = std::min(expected, static_cast<long long>(
                    static_cast<__int128>(a) * x + b));
            }
            assert(boundary.query(x) == expected);
        }
    }
    {
        ConvexHullTrick<2> capacity;
        capacity.add_line(2, 0);
        capacity.add_line(1, 100);
        capacity.add_line(0, 0);
        assert(capacity.size() == 2);
        assert(capacity.query(0) == 0);
    }
    {
        ConvexHullTrick<1> large;
        large.add_line(0, 3000000000000000000LL);
        assert(large.query(0) == 3000000000000000000LL);
    }
    {
        ConvexHullTrick<2> monotonicity;
        monotonicity.add_line(0, 0);
        bool failed = false;
        try{
            monotonicity.add_line(1, -1);
        }catch(const std::runtime_error&){
            failed = true;
        }
        assert(failed);
        assert(monotonicity.size() == 1);
    }
    {
        ConvexHullTrick<1> capacity;
        capacity.add_line(1, 0);
        bool failed = false;
        try{
            capacity.add_line(0, 100);
        }catch(const std::runtime_error&){
            failed = true;
        }
        assert(failed);
        assert(capacity.size() == 1);
    }
}
