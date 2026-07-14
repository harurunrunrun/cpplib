// competitive-verifier: STANDALONE

#include <iostream>
#include <memory>
#include <vector>

#include "../../src/structure/segtree/persistent_point_add_range_sum.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int size;
    int operation_count;
    if(!(std::cin >> size >> operation_count)) return 0;
    std::vector<long long> values(static_cast<std::size_t>(size));
    for(long long& value: values) std::cin >> value;

    using Tree = PersistentPointAddRangeSum<long long, 256, 1024>;
    auto tree = std::make_unique<Tree>(values);
    for(int operation = 0; operation < operation_count; ++operation){
        char type;
        int version;
        int left;
        long long right;
        std::cin >> type >> version >> left >> right;
        if(type == 'A'){
            std::cout << tree->add(version, left, right) << '\n';
        }else{
            std::cout << tree->sum(
                version,
                left,
                static_cast<int>(right)
            ) << '\n';
        }
    }
}
