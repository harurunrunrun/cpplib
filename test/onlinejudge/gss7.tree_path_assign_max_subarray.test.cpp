// competitive-verifier: PROBLEM https://www.spoj.com/problems/GSS7/

#include <iostream>
#include <vector>
#include "../../src/structure/tree/tree_path_assign_max_subarray.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n;
    std::cin >> n;
    std::vector<long long> values(static_cast<std::size_t>(n));
    for(long long& value: values) std::cin >> value;
    TreePathAssignMaxSubarray<long long, 100000> tree(values);
    for(int edge = 1; edge < n; ++edge){
        int u, v;
        std::cin >> u >> v;
        (void)tree.add_edge(u - 1, v - 1);
    }
    int q;
    std::cin >> q;
    while(q-- != 0){
        int type, u, v;
        std::cin >> type >> u >> v;
        --u;
        --v;
        if(type == 1) std::cout << tree.path_max_subarray(u, v) << '\n';
        else{
            long long value;
            std::cin >> value;
            tree.assign_path(u, v, value);
        }
    }
}
