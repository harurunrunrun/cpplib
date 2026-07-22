// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/graph/spanning_tree/fast_matrix_tree_theorem.hpp"
#include "../../src/structure/modint/fast_modint.hpp"

using Mint = FastModint998244353;
using Edge = MatrixTreeEdge<Mint>;

void self_test(){
    assert(fast_count_weighted_undirected_spanning_trees<Mint>(0, {}) ==
        Mint(1));
    assert(fast_count_weighted_undirected_spanning_trees<Mint>(
        1, {{0, 0, 19}}
    ) == Mint(1));
    assert(fast_count_weighted_directed_out_arborescences<Mint>(
        1, 0, {{0, 0, 19}}
    ) == Mint(1));
    assert(fast_count_weighted_directed_in_arborescences<Mint>(
        1, 0, {{0, 0, 19}}
    ) == Mint(1));

    bool thrown = false;
    try{
        (void)fast_count_weighted_undirected_spanning_trees<Mint>(
            -1, {}
        );
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
    thrown = false;
    try{
        (void)fast_count_weighted_undirected_spanning_trees<Mint>(
            2, {{0, 2, 1}}
        );
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
    thrown = false;
    try{
        (void)fast_count_weighted_directed_out_arborescences<Mint>(
            0, 0, {}
        );
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    self_test();

    int mode;
    int n;
    int root;
    int m;
    std::cin >> mode >> n >> root >> m;
    std::vector<Edge> edges;
    edges.reserve(static_cast<std::size_t>(m));
    for(int index = 0; index < m; ++index){
        int from;
        int to;
        long long weight;
        std::cin >> from >> to >> weight;
        edges.push_back({from, to, Mint(weight)});
    }

    Mint result;
    if(mode == 0){
        result = fast_count_weighted_undirected_spanning_trees<Mint>(
            n, edges
        );
        if(n <= 32){
            assert((result ==
                count_weighted_undirected_spanning_trees<Mint, 32>(n, edges)));
        }
    }else if(mode == 1){
        result = fast_count_weighted_directed_out_arborescences<Mint>(
            n, root, edges
        );
        if(n <= 32){
            assert((result ==
                count_weighted_directed_out_arborescences<Mint, 32>(
                    n, root, edges
                )));
        }
    }else{
        assert(mode == 2);
        result = fast_count_weighted_directed_in_arborescences<Mint>(
            n, root, edges
        );
        if(n <= 32){
            assert((result ==
                count_weighted_directed_in_arborescences<Mint, 32>(
                    n, root, edges
                )));
        }
    }
    if(n >= 257){
        assert(
            static_cast<std::size_t>(128 * 128 * 128) >
            fast_matrix_multiply_internal::strassen_scalar_product_budget(128)
        );
    }
    std::cout << result << '\n';
}
