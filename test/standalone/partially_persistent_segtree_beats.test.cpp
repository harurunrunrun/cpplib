// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include "../../src/structure/segtree/partially_persistent_segtree_beats.hpp"

using Beats = PartiallyPersistentSegmentTreeBeats<long long, 64, 2000, 200000>;

int main(){
    int n, q;
    if(std::cin >> n >> q){
        std::vector<long long> initial(static_cast<std::size_t>(n));
        for(long long& value: initial) std::cin >> value;
        Beats seg(initial);
        while(q--){
            std::string type;
            std::cin >> type;
            if(type == "CHMIN" || type == "CHMAX" || type == "ADD"){
                int left, right;
                long long value;
                std::cin >> left >> right >> value;
                if(type == "CHMIN") std::cout << seg.range_chmin(left, right, value) << '\n';
                else if(type == "CHMAX") std::cout << seg.range_chmax(left, right, value) << '\n';
                else std::cout << seg.range_add(left, right, value) << '\n';
            }else if(type == "SET"){
                int position;
                long long value;
                std::cin >> position >> value;
                std::cout << seg.set(position, value) << '\n';
            }else if(type == "GET"){
                int version, position;
                std::cin >> version >> position;
                std::cout << seg.get(version, position) << '\n';
            }else if(type == "SUM" || type == "MIN" || type == "MAX"){
                int version, left, right;
                std::cin >> version >> left >> right;
                if(type == "SUM") std::cout << seg.range_sum(version, left, right) << '\n';
                else if(type == "MIN") std::cout << seg.range_min(version, left, right) << '\n';
                else std::cout << seg.range_max(version, left, right) << '\n';
            }else if(type == "ALL"){
                int version;
                std::cin >> version;
                std::cout << seg.all_sum(version) << ' ' << seg.all_min(version) << ' '
                    << seg.all_max(version) << '\n';
            }
        }
        return 0;
    }

    PartiallyPersistentSegmentTreeBeats<long long, 4, 16, 256> seg(
        std::vector<long long>{1, 9, 3, 7}
    );
    assert(seg.size() == 4 && seg.versions() == 1 && seg.latest_version() == 0);
    assert(seg.changes_used() == 0);
    int added = seg.range_add(0, 4, 5);
    int capped = seg.range_chmin(1, 4, 10);
    int raised = seg.range_chmax(0, 2, 20);
    assert(added == 1 && capped == 2 && raised == 3);
    assert(seg.size() == 4 && seg.versions() == 4 && seg.latest_version() == raised);
    assert(seg.changes_used() > 0);
    assert(seg.range_sum(0, 0, 4) == 20);
    assert(seg.range_sum(1, 0, 4) == 40);
    assert(seg.get(2, 1) == 10 && seg.get(2, 3) == 10);
    assert(seg.get(3, 0) == 20 && seg.get(3, 1) == 20);
    int query_changes = seg.changes_used();
    assert(seg.get(1, 0) == 6 && seg.get(1, 1) == 14);
    assert(seg.range_min(2, 0, 4) == 6 && seg.range_max(3, 0, 4) == 20);
    assert(seg.changes_used() == query_changes);

    PartiallyPersistentSegmentTreeBeats<long long, 2, 4, 1> limited(
        std::vector<long long>{1, 2}
    );
    int first = limited.range_add(0, 2, 1);
    int before_changes = limited.changes_used();
    int before_versions = limited.versions();
    bool thrown = false;
    try{ (void)limited.set(0, 100); }
    catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    assert(limited.changes_used() == before_changes && limited.versions() == before_versions);
    assert(first == 1 && limited.range_sum(1, 0, 2) == 5);
    PartiallyPersistentSegmentTreeBeats<long long, 2, 0, 8> no_versions(
        std::vector<long long>{3, 4}
    );
    thrown = false;
    try{ (void)no_versions.range_add(0, 2, 1); }
    catch(const std::runtime_error&){ thrown = true; }
    assert(thrown && no_versions.versions() == 1 && no_versions.changes_used() == 0);
    assert(no_versions.range_sum(0, 0, 2) == 7);
    assert(no_versions.size() == 2 && no_versions.latest_version() == 0);
}
