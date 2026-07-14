// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/structure/dsu/offline_persistent_dsu.hpp"

void self_test(){
    OfflinePersistentDSU<8, 8, 8> dsu(5);
    const int first = dsu.merge(0, 0, 1);
    const int second = dsu.merge(first, 1, 2);
    const int forked = dsu.fork(first);
    assert(dsu.size() == 5);
    assert(dsu.versions() == 4);
    assert(dsu.query_same(0, 0, 1) == 0);
    assert(dsu.query_same(first, 0, 1) == 1);
    assert(dsu.query_same(second, 0, 2) == 2);
    assert(dsu.query_same(forked, 0, 2) == 3);
    assert((dsu.solve() == std::vector<int>{0, 1, 1, 0}));
    assert((dsu.solve() == std::vector<int>{0, 1, 1, 0}));

    bool thrown = false;
    try{ OfflinePersistentDSU<4, 2, 2> invalid(5); }
    catch(const std::out_of_range&){ thrown = true; }
    assert(thrown);
}

int main(){
    self_test();
    int size, operation_count;
    if(!(std::cin >> size >> operation_count)) return 0;
    OfflinePersistentDSU<256, 1024, 1024> dsu(size);
    std::vector<int> version_at(static_cast<std::size_t>(operation_count + 1));
    for(int operation = 0; operation < operation_count; ++operation){
        int type, base_operation, left, right;
        std::cin >> type >> base_operation >> left >> right;
        const int base = version_at[
            static_cast<std::size_t>(base_operation + 1)
        ];
        if(type == 0){
            version_at[static_cast<std::size_t>(operation + 1)] =
                dsu.merge(base, left, right);
        }else{
            dsu.query_same(base, left, right);
            version_at[static_cast<std::size_t>(operation + 1)] = base;
        }
    }
    for(int answer: dsu.solve()) std::cout << answer << '\n';
}
