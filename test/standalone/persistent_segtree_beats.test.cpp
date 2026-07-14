// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include "../../src/structure/segtree/detail/segtree_beats_node.hpp"
#include "../../src/structure/segtree/persistent_segtree_beats.hpp"

using Beats = PersistentSegmentTreeBeats<long long, 64, 200000, 2000>;

int main(){
    int n, q;
    if(std::cin >> n >> q){
        std::vector<long long> initial(static_cast<std::size_t>(n));
        for(long long& value: initial) std::cin >> value;
        auto seg = std::make_unique<Beats>(initial);
        while(q--){
            std::string type;
            std::cin >> type;
            if(type == "CHMIN" || type == "CHMAX" || type == "ADD"){
                int version, left, right;
                long long value;
                std::cin >> version >> left >> right >> value;
                if(type == "CHMIN") std::cout << seg->range_chmin(version, left, right, value) << '\n';
                else if(type == "CHMAX") std::cout << seg->range_chmax(version, left, right, value) << '\n';
                else std::cout << seg->range_add(version, left, right, value) << '\n';
            }else if(type == "SET"){
                int version, position;
                long long value;
                std::cin >> version >> position >> value;
                std::cout << seg->set(version, position, value) << '\n';
            }else if(type == "FORK"){
                int version;
                std::cin >> version;
                std::cout << seg->fork(version) << '\n';
            }else if(type == "GET"){
                int version, position;
                std::cin >> version >> position;
                std::cout << seg->get(version, position) << '\n';
            }else if(type == "SUM" || type == "MIN" || type == "MAX"){
                int version, left, right;
                std::cin >> version >> left >> right;
                if(type == "SUM") std::cout << seg->range_sum(version, left, right) << '\n';
                else if(type == "MIN") std::cout << seg->range_min(version, left, right) << '\n';
                else std::cout << seg->range_max(version, left, right) << '\n';
            }else if(type == "ALL"){
                int version;
                std::cin >> version;
                std::cout << seg->all_sum(version) << ' ' << seg->all_min(version) << ' '
                    << seg->all_max(version) << '\n';
            }
        }
        return 0;
    }

    std::vector<long long> initial = {1, 9, 3, 7};
    PersistentSegmentTreeBeats<long long, 4, 256, 16> seg(initial);
    assert(seg.size() == 4 && seg.versions() == 1 && seg.latest_version() == 0);
    assert(seg.nodes_used() > 0);
    int added = seg.range_add(0, 0, 4, 5);
    int capped = seg.range_chmin(added, 1, 4, 10);
    int branched = seg.range_chmax(added, 0, 2, 20);
    assert(seg.versions() == 4 && seg.latest_version() == branched);
    int nodes_before_fork = seg.nodes_used();
    int copied = seg.fork(capped);
    assert(copied == 4 && seg.versions() == 5 && seg.latest_version() == copied);
    assert(seg.nodes_used() == nodes_before_fork);
    assert(seg.range_sum(copied, 0, 4) == seg.range_sum(capped, 0, 4));
    assert(seg.range_sum(0, 0, 4) == 20);
    assert(seg.range_sum(added, 0, 4) == 40);
    assert(seg.get(capped, 1) == 10 && seg.get(capped, 3) == 10);
    assert(seg.get(branched, 0) == 20 && seg.get(branched, 1) == 20);
    int query_nodes = seg.nodes_used();
    assert(seg.get(added, 0) == 6 && seg.get(added, 1) == 14);
    assert(seg.range_min(capped, 0, 4) == 6 && seg.range_max(branched, 0, 4) == 20);
    assert(seg.nodes_used() == query_nodes);

    PersistentSegmentTreeBeats<long long, 2, 4, 4> limited(std::vector<long long>{1, 2});
    int first = limited.range_add(0, 0, 2, 1);
    int before_nodes = limited.nodes_used();
    int before_versions = limited.versions();
    bool thrown = false;
    try{ (void)limited.set(first, 0, 100); }
    catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    assert(limited.nodes_used() == before_nodes && limited.versions() == before_versions);
    assert(limited.range_sum(first, 0, 2) == 5);
    PersistentSegmentTreeBeats<long long, 2, 8, 0> no_versions(
        std::vector<long long>{3, 4}
    );
    thrown = false;
    try{ (void)no_versions.fork(0); }
    catch(const std::runtime_error&){ thrown = true; }
    assert(thrown && no_versions.versions() == 1 && no_versions.range_sum(0, 0, 2) == 7);
    assert(no_versions.size() == 2 && no_versions.latest_version() == 0);
}
