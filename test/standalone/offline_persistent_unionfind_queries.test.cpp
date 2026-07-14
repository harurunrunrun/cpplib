// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/structure/dsu/offline_persistent_unionfind_queries.hpp"

using LargeOfflinePersistentUnionFindQueries =
    OfflinePersistentUnionFindQueries<200000, 200000, 200000>;
static_assert(
    sizeof(LargeOfflinePersistentUnionFindQueries) <= 16 * sizeof(void*)
);

void self_test(){
    {
        OfflinePersistentUnionFindQueries<8, 8, 8> queries(4);
        assert(queries.size() == 4);
        queries.merge(-1, 0, 1);
        assert(queries.same(0, 0, 1) == 0);
        queries.merge(1, 1, 2);
        assert(queries.same(1, 0, 2) == 1);
        assert(queries.same(2, 0, 2) == 2);
        assert(queries.operations() == 5);
        assert(queries.query_count() == 3);
        const std::vector<int> expected = {1, 0, 1};
        assert(queries.solve() == expected);
        assert(queries.solve() == expected);
    }
    {
        OfflinePersistentUnionFindQueries<4, 0, 0> queries(2);
        bool thrown = false;
        try{
            queries.merge(-1, 0, 1);
        }catch(const std::length_error&){
            thrown = true;
        }
        assert(thrown);
        assert(queries.operations() == 0);
        assert(queries.query_count() == 0);
        assert(queries.solve().empty());
        assert(queries.solve().empty());
    }
    {
        OfflinePersistentUnionFindQueries<4, 4, 1> queries(3);
        assert(queries.same(-1, 0, 0) == 0);
        const int old_operations = queries.operations();
        const int old_queries = queries.query_count();

        bool thrown = false;
        try{
            (void)queries.same(-1, 0, 1);
        }catch(const std::length_error&){
            thrown = true;
        }
        assert(thrown);
        assert(queries.operations() == old_operations);
        assert(queries.query_count() == old_queries);

        thrown = false;
        try{
            queries.merge(queries.operations(), 0, 1);
        }catch(const std::out_of_range&){
            thrown = true;
        }
        assert(thrown);
        assert(queries.operations() == old_operations);

        thrown = false;
        try{
            queries.merge(-1, -1, 0);
        }catch(const std::out_of_range&){
            thrown = true;
        }
        assert(thrown);
        assert(queries.operations() == old_operations);
        thrown = false;
        try{
            (void)queries.same(-1, -1, 0);
        }catch(const std::out_of_range&){
            thrown = true;
        }
        assert(thrown);
        assert(queries.operations() == old_operations);
        assert(queries.query_count() == old_queries);
    }
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size, operation_count;
    if(!(std::cin >> size >> operation_count)){
        self_test();
        return 0;
    }

    OfflinePersistentUnionFindQueries<64, 256, 256> queries(size);
    for(int operation = 0; operation < operation_count; ++operation){
        int type, base_operation, left, right;
        std::cin >> type >> base_operation >> left >> right;
        if(type == 0){
            queries.merge(base_operation, left, right);
        }else{
            queries.same(base_operation, left, right);
        }
    }
    for(const int answer: queries.solve()){
        std::cout << answer << '\n';
    }
}
