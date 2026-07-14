// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
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
    bool thrown = false;
    try{
        (void)cht.query(0, 0);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
    thrown = false;
    try{
        (void)cht.line_at(0, 0);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
    thrown = false;
    try{
        (void)cht.size(1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

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
    {
        PersistentConvexHullTrick<4, 8, 128> boundary;
        int v1 = boundary.add_line(0, 3, -8000000000000000000LL);
        int v2 = boundary.add_line(v1, 2, 8000000000000000000LL);
        int v3 = boundary.add_line(v2, 1, -7000000000000000000LL);
        assert(boundary.query(v3, 0) == -8000000000000000000LL);
        assert(boundary.query(v3, 10) == -7999999999999999970LL);
    }
    {
        PersistentConvexHullTrick<2, 8, 64> capacity;
        int v1 = capacity.add_line(0, 2, 0);
        int v2 = capacity.add_line(v1, 1, 100);
        int v3 = capacity.add_line(v2, 0, 0);
        assert(capacity.size(v3) == 2);
        assert(capacity.query(v3, 0) == 0);
    }
    {
        PersistentConvexHullTrick<4, 2, 64> versions;
        int v1 = versions.add_line(0, 1, 3);
        int nodes_before = versions.nodes_used();
        bool thrown = false;
        try{
            (void)versions.add_line(v1, 0, 1);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
        assert(versions.versions() == 2);
        assert(versions.nodes_used() == nodes_before);
        assert(versions.query(v1, 5) == 8);
    }
    {
        PersistentConvexHullTrick<4, 4, 3> nodes;
        bool thrown = false;
        try{
            (void)nodes.add_line(0, 0, 1);
        }catch(const std::runtime_error&){
            thrown = true;
        }
        assert(thrown);
        assert(nodes.versions() == 1);
        assert(nodes.nodes_used() == 1);
        assert(nodes.empty(0));
    }
    {
        using Small = PersistentConvexHullTrick<4, 8, 128>;
        Small lines;
        int first = lines.add_line(0, typename Small::Line{3, 9});
        int second = lines.add_line(first, 2, 4);
        assert(lines.line_at(second, 0).a == 3);
        assert(lines.line_at(second, 0).b == 9);
        assert(lines.line_at(second, 1).a == 2);
        assert(lines.line_at(second, 1).b == 4);
        bool failed = false;
        try{
            (void)lines.line_at(second, 2);
        }catch(const std::runtime_error&){
            failed = true;
        }
        assert(failed);
        failed = false;
        try{
            (void)lines.add_line(second, 4, 0);
        }catch(const std::runtime_error&){
            failed = true;
        }
        assert(failed);
    }
    {
        PersistentConvexHullTrick<1, 4, 32> full;
        int first = full.add_line(0, 1, 0);
        bool failed = false;
        try{
            (void)full.add_line(first, 0, 100);
        }catch(const std::runtime_error&){
            failed = true;
        }
        assert(failed);
        assert(full.size(first) == 1);
    }
}
