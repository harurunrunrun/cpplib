// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <functional>
#include <numeric>
#include <utility>
#include <vector>

#include "../../src/algorithm/range/offline_query/cdq_divide_and_conquer.hpp"
#include "../../src/algorithm/range/offline_query/hilbert_mo.hpp"
#include "../../src/algorithm/range/offline_query/mo_with_modifications.hpp"
#include "../../src/algorithm/range/offline_query/offline_3d_dominance.hpp"
#include "../../src/algorithm/range/offline_query/rollback_mo.hpp"
#include "../../src/algorithm/tree/construction/prufer_code.hpp"
#include "../../src/algorithm/tree/decomposition/ladder_decomposition.hpp"
#include "../../src/algorithm/tree/decomposition/rake_compress_tree_contraction.hpp"
#include "../../src/algorithm/tree/isomorphism/tree_automorphism_count.hpp"
#include "../../src/algorithm/tree/query/level_ancestor.hpp"
#include "../../src/algorithm/tree/query/tree_path_xor_basis.hpp"
#include "../../src/structure/fenwick_tree/fenwick_ordered_sets.hpp"
#include "../../src/structure/fenwick_tree/persistent_fenwick_tree.hpp"
#include "../../src/structure/range_query/aggregation/sparse_table_2d.hpp"
#include "../../src/structure/range_query/extrema/fischer_heun_rmq.hpp"
#include "../../src/structure/range_query/extrema/plus_minus_one_rmq.hpp"
#include "../../src/structure/range_query/value_query/fractional_cascading.hpp"
#include "../../src/structure/range_query/value_query/orthogonal_range_reporting.hpp"
#include "../../src/structure/range_query/value_query/priority_search_tree.hpp"
#include "../../src/structure/range_query/value_query/range_tree.hpp"
#include "../../src/structure/wavelet_matrix/static/wavelet_tree.hpp"

int main(){
    const std::vector<std::vector<int>> tree{{1, 2}, {0, 3, 4}, {0}, {1}, {1}};
    assert(prufer_decode(prufer_encode(tree)).size() == 4);
    LevelAncestor ancestor(tree);
    assert(ancestor.kth_ancestor(4, 2) == 0);
    LadderDecomposition ladders(tree);
    assert(ladders.depth(3) == 2);
    assert(!rake_compress_tree_contraction(tree).empty());
    assert(rooted_tree_automorphism_count<long long>(tree, 0) == 2);
    TreePathXorBasis basis(tree, {1, 2, 4, 8, 16});
    assert(basis.maximum_xor(3, 2) == 15);

    int merges = 0;
    cdq_divide_and_conquer(0, 8, [&](int, int, int){ ++merges; });
    assert(merges == 7);
    assert(hilbert_order(1, 2) != hilbert_order(2, 1));

    PersistentFenwickTree<int> persistent(4);
    const int v1 = persistent.add(1, 5);
    const int v2 = persistent.add(2, 7);
    assert(persistent.sum(v1, 0, 4) == 5);
    assert(persistent.sum(v2, 0, 4) == 12);

    FenwickOrderedSets<int> ordered(3, {{0, 4}, {1, 2}, {2, 7}});
    ordered.add(0, 4); ordered.add(1, 2); ordered.add(2, 7);
    assert(ordered.count_less(0, 3, 5) == 2);

    SparseTable2D<int, std::function<int(int, int)>> sparse(
        {{5, 2, 7}, {3, 4, 1}}, [](int a, int b){ return std::min(a, b); });
    assert(sparse.prod(0, 1, 2, 3) == 1);
    PlusMinusOneRMQ pm({0, 1, 0, -1, 0});
    assert(pm.argmin(0, 5) == 3);
    FischerHeunRMQ<int> rmq({5, 2, 7, 1, 3});
    assert(rmq.argmin(1, 5) == 3);

    WaveletTree<int> wavelet({4, 1, 7, 2, 5});
    assert(wavelet.kth_smallest(1, 5, 2) == 5);
    assert(wavelet.count(0, 5, 2, 6) == 3);

    using RT = RangeTree<int, int>;
    const std::vector<RT::Point> points{{0, 2, 0}, {1, 1, 1}, {2, 3, 2}, {3, 0, 3}};
    RT range_tree(points);
    auto reported = range_tree.report(0, 3, 1, 3);
    std::sort(reported.begin(), reported.end());
    assert((reported == std::vector<int>{0, 1}));
    OrthogonalRangeReporting<int> orthogonal({{0, 2}, {1, 1}, {2, 3}});
    assert(orthogonal.report(0, 2, 0, 3).size() == 2);
    PrioritySearchTree<int, int> priority({{0, 2, 0}, {1, 1, 1}, {2, 3, 2}, {3, 0, 3}});
    assert(priority.report(0, 3, 3).size() == 2);
    FractionalCascading<int> cascading({{1, 4, 9}, {2, 6}, {0, 7, 8}});
    assert((cascading.lower_bounds(5) == std::vector<int>{2, 1, 1}));

    const auto dominance = offline_3d_dominance({{0, 0, 0}, {1, 1, 1}, {0, 2, 1}});
    assert((dominance == std::vector<long long>{0, 1, 1}));
    const auto duplicate_dominance = offline_3d_dominance({{1, 1, 1}, {1, 1, 1}, {2, 2, 2}});
    assert((duplicate_dominance == std::vector<long long>{1, 1, 2}));
}
