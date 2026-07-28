// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <functional>
#include <iostream>
#include <limits>
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

    std::mt19937 tree_random(83'711U);
    for(int tree_size = 1; tree_size <= 8; ++tree_size){
        for(int sample = 0; sample < 24; ++sample){
            std::vector<std::vector<int>> random_tree(
                static_cast<std::size_t>(tree_size)
            );
            std::vector<int> expected_parent(
                static_cast<std::size_t>(tree_size), 0
            );
            std::vector<int> expected_depth(
                static_cast<std::size_t>(tree_size)
            );
            for(int vertex = 1; vertex < tree_size; ++vertex){
                const int parent = static_cast<int>(
                    tree_random() % static_cast<unsigned int>(vertex)
                );
                expected_parent[static_cast<std::size_t>(vertex)] = parent;
                expected_depth[static_cast<std::size_t>(vertex)] =
                    expected_depth[static_cast<std::size_t>(parent)] + 1;
                random_tree[static_cast<std::size_t>(vertex)]
                    .push_back(parent);
                random_tree[static_cast<std::size_t>(parent)]
                    .push_back(vertex);
            }

            const LadderDecomposition decomposition(random_tree);
            for(int vertex = 0; vertex < tree_size; ++vertex){
                assert(decomposition.parent(vertex)
                    == expected_parent[static_cast<std::size_t>(vertex)]);
                assert(decomposition.depth(vertex)
                    == expected_depth[static_cast<std::size_t>(vertex)]);
                std::vector<int> actual_path;
                for(const auto [ladder, count]:
                    decomposition.path_to_root(vertex)){
                    const auto& chain =
                        decomposition.ladder_vertices(ladder);
                    assert(0 < count
                        && count <= static_cast<int>(chain.size()));
                    for(int index = count; index-- > 0;){
                        actual_path.push_back(
                            chain[static_cast<std::size_t>(index)]
                        );
                    }
                }
                std::vector<int> expected_path;
                for(int current = vertex;;
                    current = expected_parent[
                        static_cast<std::size_t>(current)
                    ]){
                    expected_path.push_back(current);
                    if(current == 0) break;
                }
                assert(actual_path == expected_path);
            }

            const auto contraction =
                rake_compress_tree_contraction(random_tree);
            std::vector<unsigned char> contracted(
                static_cast<std::size_t>(tree_size), 0
            );
            int contraction_count = 0;
            for(const auto& round: contraction){
                assert(!round.empty());
                for(const TreeContractionStep& step: round){
                    assert(0 <= step.vertex && step.vertex < tree_size);
                    assert(!contracted[
                        static_cast<std::size_t>(step.vertex)
                    ]);
                    contracted[static_cast<std::size_t>(step.vertex)] = 1;
                    ++contraction_count;
                    if(step.kind == TreeContractionStep::Kind::compress){
                        assert(0 <= step.left && step.left < tree_size);
                        assert(0 <= step.right && step.right < tree_size);
                        assert(step.left != step.right);
                    }
                }
            }
            assert(contraction_count == tree_size - 1);

            if(sample < 4 && tree_size <= 7){
                std::vector<int> permutation(
                    static_cast<std::size_t>(tree_size)
                );
                std::iota(permutation.begin(), permutation.end(), 0);
                long long unrooted_count = 0;
                long long rooted_count = 0;
                do{
                    bool valid = true;
                    for(int first = 0; first < tree_size && valid; ++first){
                        for(int second = first + 1;
                            second < tree_size;
                            ++second){
                            const bool original = std::find(
                                random_tree[
                                    static_cast<std::size_t>(first)
                                ].begin(),
                                random_tree[
                                    static_cast<std::size_t>(first)
                                ].end(), second
                            ) != random_tree[
                                static_cast<std::size_t>(first)
                            ].end();
                            const int mapped_first = permutation[
                                static_cast<std::size_t>(first)
                            ];
                            const int mapped_second = permutation[
                                static_cast<std::size_t>(second)
                            ];
                            const bool mapped = std::find(
                                random_tree[
                                    static_cast<std::size_t>(mapped_first)
                                ].begin(),
                                random_tree[
                                    static_cast<std::size_t>(mapped_first)
                                ].end(), mapped_second
                            ) != random_tree[
                                static_cast<std::size_t>(mapped_first)
                            ].end();
                            if(original != mapped) valid = false;
                        }
                    }
                    if(valid){
                        ++unrooted_count;
                        if(permutation[0] == 0) ++rooted_count;
                    }
                }while(std::next_permutation(
                    permutation.begin(), permutation.end()
                ));
                assert(tree_automorphism_count<long long>(random_tree)
                    == unrooted_count);
                assert(rooted_tree_automorphism_count<long long>(
                    random_tree, 0
                ) == rooted_count);
            }

            std::vector<std::uint64_t> path_values(
                static_cast<std::size_t>(tree_size)
            );
            for(auto& value: path_values) value = tree_random() & 255U;
            const TreePathXorBasis<8> random_basis(
                random_tree, path_values
            );
            for(int first = 0; first < tree_size; ++first){
                for(int second = 0; second < tree_size; ++second){
                    std::vector<int> first_path;
                    for(int vertex = first;;
                        vertex = expected_parent[
                            static_cast<std::size_t>(vertex)
                        ]){
                        first_path.push_back(vertex);
                        if(vertex == 0) break;
                    }
                    std::vector<int> second_path;
                    for(int vertex = second;;
                        vertex = expected_parent[
                            static_cast<std::size_t>(vertex)
                        ]){
                        second_path.push_back(vertex);
                        if(vertex == 0) break;
                    }
                    int left = static_cast<int>(first_path.size()) - 1;
                    int right = static_cast<int>(second_path.size()) - 1;
                    while(0 < left && 0 < right
                        && first_path[static_cast<std::size_t>(left - 1)]
                            == second_path[
                                static_cast<std::size_t>(right - 1)
                            ]){
                        --left;
                        --right;
                    }
                    std::vector<std::uint64_t> values;
                    for(int index = 0; index <= left; ++index){
                        values.push_back(path_values[static_cast<std::size_t>(
                            first_path[static_cast<std::size_t>(index)]
                        )]);
                    }
                    for(int index = right; index-- > 0;){
                        values.push_back(path_values[static_cast<std::size_t>(
                            second_path[static_cast<std::size_t>(index)]
                        )]);
                    }
                    const std::uint64_t seed = tree_random() & 255U;
                    std::uint64_t expected = seed;
                    for(std::size_t mask = 0;
                        mask < (std::size_t{1} << values.size());
                        ++mask){
                        std::uint64_t candidate = seed;
                        for(std::size_t index = 0;
                            index < values.size();
                            ++index){
                            if((mask >> index) & 1U){
                                candidate ^= values[index];
                            }
                        }
                        expected = std::max(expected, candidate);
                    }
                    assert(random_basis.maximum_xor(first, second, seed)
                        == expected);
                }
            }
        }
    }

    int merges = 0;
    cdq_divide_and_conquer(0, 8, [&](int, int, int){ ++merges; });
    assert(merges == 7);
    assert(hilbert_order(1, 2) != hilbert_order(2, 1));

    PersistentFenwickTree<int> persistent(4);
    const int v1 = persistent.add(1, 5);
    const int v2 = persistent.add(2, 7);
    assert(persistent.sum(v1, 0, 4) == 5);
    assert(persistent.sum(v2, 0, 4) == 12);
    bool fenwick_size_limit = false;
    try{
        PersistentFenwickTree<int> too_large(
            std::numeric_limits<int>::max()
        );
    }catch(const std::length_error&){
        fenwick_size_limit = true;
    }
    assert(fenwick_size_limit);

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
