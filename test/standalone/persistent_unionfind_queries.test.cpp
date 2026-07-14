// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>

#include "../../src/structure/dsu/persistent_unionfind_queries.hpp"
static_assert(
    sizeof(PersistentUnionFindQueries<200000, 200000>) <=
    4 * sizeof(void*)
);


void self_test(){
    PersistentUnionFindQueries<8, 8> dsu(4);
    assert(dsu.processed_queries() == 0);

    dsu.merge(-1, 0, 1);
    assert(dsu.processed_queries() == 1);
    assert(dsu.same(0, 0, 1));
    assert(!dsu.same(-1, 0, 1));
    dsu.merge(0, 1, 2);
    assert(dsu.same(3, 0, 2));
    assert(!dsu.same(2, 0, 2));

    const int processed = dsu.processed_queries();
    bool thrown = false;
    try{
        (void)dsu.same(processed, 0, 1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
    assert(dsu.processed_queries() == processed);

    PersistentUnionFindQueries<4, 1> limited(4);
    assert(!limited.same(-1, 0, 1));
    thrown = false;
    try{
        limited.merge(0, 0, 1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
    assert(limited.processed_queries() == 1);
}

int main(){
    self_test();

    int size, query_count;
    if(!(std::cin >> size >> query_count)) return 0;
    PersistentUnionFindQueries<64, 512> dsu(size);
    for(int query = 0; query < query_count; ++query){
        int type, base_query, left, right;
        std::cin >> type >> base_query >> left >> right;
        if(type == 0){
            dsu.merge(base_query, left, right);
        }else{
            std::cout << dsu.same(base_query, left, right) << '\n';
        }
    }
}
