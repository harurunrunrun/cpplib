// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <functional>
#include <iostream>
#include <numeric>
#include <random>
#include <stdexcept>
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

    PersistentFenwickTree<long long> branching(17);
    std::vector<std::vector<long long>> versions(
        1, std::vector<long long>(17)
    );
    std::mt19937 random(917'431U);
    for(int operation = 0; operation < 300; ++operation){
        const int base = static_cast<int>(
            random() % static_cast<unsigned int>(versions.size())
        );
        const int position = static_cast<int>(random() % 17U);
        const long long delta =
            static_cast<long long>(random() % 101U) - 50;
        std::vector<long long> next =
            versions[static_cast<std::size_t>(base)];
        next[static_cast<std::size_t>(position)] += delta;
        const int version = branching.add(base, position, delta);
        versions.push_back(std::move(next));
        assert(version == static_cast<int>(versions.size()) - 1);
        for(int query = 0; query < 20; ++query){
            const int checked = static_cast<int>(
                random() % static_cast<unsigned int>(versions.size())
            );
            int left = static_cast<int>(random() % 18U);
            int right = static_cast<int>(random() % 18U);
            if(right < left) std::swap(left, right);
            const long long expected = std::accumulate(
                versions[static_cast<std::size_t>(checked)].begin() + left,
                versions[static_cast<std::size_t>(checked)].begin() + right,
                0LL
            );
            assert(branching.sum(checked, left, right) == expected);
        }
    }

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
    OrthogonalRangeReporting<int> orthogonal(
        {{0, 2}, {1, 1}, {2, 3}, {1, 1}}
    );
    auto orthogonal_answer = orthogonal.report(0, 2, 0, 3);
    std::sort(orthogonal_answer.begin(), orthogonal_answer.end());
    assert((orthogonal_answer == std::vector<int>{0, 1, 3}));
    assert(orthogonal.report(1, 1, -10, 10).empty());
    bool invalid_rectangle = false;
    try{
        (void)orthogonal.report(2, 1, 0, 1);
    }catch(const std::invalid_argument&){
        invalid_rectangle = true;
    }
    assert(invalid_rectangle);
    PrioritySearchTree<int, int> priority({{0, 2, 0}, {1, 1, 1}, {2, 3, 2}, {3, 0, 3}});
    assert(priority.report(0, 3, 3).size() == 2);
    FractionalCascading<int> cascading({{1, 4, 9}, {2, 6}, {0, 7, 8}});
    assert((cascading.lower_bounds(5) == std::vector<int>{2, 1, 1}));

    const auto dominance = offline_3d_dominance({{0, 0, 0}, {1, 1, 1}, {0, 2, 1}});
    assert((dominance == std::vector<long long>{0, 1, 1}));
    const auto duplicate_dominance = offline_3d_dominance({{1, 1, 1}, {1, 1, 1}, {2, 2, 2}});
    assert((duplicate_dominance == std::vector<long long>{1, 1, 2}));

    int point_count;
    int query_count;
    assert(std::cin >> point_count >> query_count);
    std::vector<std::pair<int, int>> random_points(
        static_cast<std::size_t>(point_count)
    );
    for(auto& [x, y]: random_points) std::cin >> x >> y;
    OrthogonalRangeReporting<int> random_index(random_points);
    assert(random_index.size() == point_count);
    while(query_count-- > 0){
        int lower_x;
        int upper_x;
        int lower_y;
        int upper_y;
        std::cin >> lower_x >> upper_x >> lower_y >> upper_y;
        std::vector<int> expected;
        for(int index = 0; index < point_count; ++index){
            const auto [x, y] =
                random_points[static_cast<std::size_t>(index)];
            if(lower_x <= x && x < upper_x
                && lower_y <= y && y < upper_y){
                expected.push_back(index);
            }
        }
        auto actual = random_index.report(
            lower_x, upper_x, lower_y, upper_y
        );
        std::sort(actual.begin(), actual.end());
        assert(actual == expected);
    }
}
