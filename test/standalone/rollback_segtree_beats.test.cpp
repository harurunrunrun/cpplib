// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include "../../src/structure/segtree/rollback_segtree_beats.hpp"

using Beats = RollbackSegmentTreeBeats<long long, 64, 200000, 2000>;

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
                int left, right;
                long long value;
                std::cin >> left >> right >> value;
                if(type == "CHMIN") seg->range_chmin(left, right, value);
                else if(type == "CHMAX") seg->range_chmax(left, right, value);
                else seg->range_add(left, right, value);
            }else if(type == "SET"){
                int position;
                long long value;
                std::cin >> position >> value;
                seg->set(position, value);
            }else if(type == "ROLLBACK"){
                int snapshot;
                std::cin >> snapshot;
                seg->rollback(snapshot);
            }else if(type == "SNAPSHOT"){
                std::cout << seg->snapshot() << '\n';
            }else if(type == "GET"){
                int position;
                std::cin >> position;
                std::cout << seg->get(position) << '\n';
            }else if(type == "SUM" || type == "MIN" || type == "MAX"){
                int left, right;
                std::cin >> left >> right;
                if(type == "SUM") std::cout << seg->range_sum(left, right) << '\n';
                else if(type == "MIN") std::cout << seg->range_min(left, right) << '\n';
                else std::cout << seg->range_max(left, right) << '\n';
            }else if(type == "ALL"){
                std::cout << seg->all_sum() << ' ' << seg->all_min() << ' '
                    << seg->all_max() << '\n';
            }
        }
        return 0;
    }

    RollbackSegmentTreeBeats<long long, 4, 256, 16> seg(
        std::vector<long long>{1, 9, 3, 7}
    );
    auto initial = seg.snapshot();
    seg.range_add(0, 4, 5);
    auto added = seg.snapshot();
    seg.range_chmin(1, 4, 10);
    seg.range_chmax(0, 2, 20);
    int query_changes = seg.changes_used();
    assert(seg.get(0) == 20 && seg.get(1) == 20);
    assert(seg.range_min(0, 4) == 8 && seg.range_max(0, 4) == 20);
    assert(seg.changes_used() == query_changes);
    seg.rollback(added);
    assert(seg.range_sum(0, 4) == 40);
    assert(seg.get(0) == 6 && seg.get(1) == 14);
    seg.rollback(initial);
    assert(seg.range_sum(0, 4) == 20);

    RollbackSegmentTreeBeats<long long, 2, 1, 4> limited(
        std::vector<long long>{1, 2}
    );
    limited.range_add(0, 2, 1);
    int before_changes = limited.changes_used();
    int before_operations = limited.operations();
    bool thrown = false;
    try{ limited.set(0, 100); }
    catch(const std::runtime_error&){ thrown = true; }
    assert(thrown);
    assert(limited.changes_used() == before_changes && limited.operations() == before_operations);
    assert(limited.range_sum(0, 2) == 5);
    RollbackSegmentTreeBeats<long long, 2, 8, 0> no_operations(
        std::vector<long long>{3, 4}
    );
    thrown = false;
    try{ no_operations.range_add(0, 2, 1); }
    catch(const std::runtime_error&){ thrown = true; }
    assert(thrown && no_operations.operations() == 0 && no_operations.changes_used() == 0);
    assert(no_operations.range_sum(0, 2) == 7);
}
