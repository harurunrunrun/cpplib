// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <random>
#include <utility>
#include <vector>

#include "../../src/algorithm/other/combinatorial_optimization/matroid/matroid_intersection.hpp"
#include "../../src/algorithm/other/combinatorial_optimization/matroid/matroid_partition.hpp"
#include "../../src/algorithm/other/combinatorial_optimization/matroid/matroid_union.hpp"
#include "../../src/algorithm/other/combinatorial_optimization/matroid/weighted_matroid_intersection.hpp"
#include "../../src/algorithm/other/combinatorial_optimization/submodular/submodular_function_minimization.hpp"

namespace {

struct Dsu {
    std::vector<int> parent;
    explicit Dsu(int size) : parent(static_cast<std::size_t>(size), -1) {}
    int root(int vertex) {
        if(parent[static_cast<std::size_t>(vertex)] < 0) return vertex;
        return parent[static_cast<std::size_t>(vertex)] =
            root(parent[static_cast<std::size_t>(vertex)]);
    }
    bool unite(int first, int second) {
        first = root(first);
        second = root(second);
        if(first == second) return false;
        if(parent[static_cast<std::size_t>(first)]
           > parent[static_cast<std::size_t>(second)]){
            std::swap(first, second);
        }
        parent[static_cast<std::size_t>(first)] +=
            parent[static_cast<std::size_t>(second)];
        parent[static_cast<std::size_t>(second)] = first;
        return true;
    }
};

}  // namespace

int main() {
    {
        constexpr std::size_t large_size = 80;
        std::size_t oracle_calls = 0;
        const auto modular_oracle =
            [&](const std::vector<unsigned char>& set){
                ++oracle_calls;
                long long value = 0;
                for(std::size_t element = 0;
                    element < large_size;
                    ++element){
                    if(set[element]){
                        value += (element % 3 == 0 ? -3LL : 2LL);
                    }
                }
                return value;
            };
        const auto result =
            combinatorial_optimization::submodular_function_minimization(
                large_size, modular_oracle
            );
        assert(result.value == -81);
        assert(result.exact_certificate_used);
        assert(oracle_calls < 1000);
        std::vector<unsigned char> selected(large_size, 0);
        for(const std::size_t element : result.elements){
            selected[element] = 1;
        }
        for(std::size_t element = 0; element < large_size; ++element){
            assert(selected[element] == (element % 3 == 0));
        }
    }
    std::uint64_t seed;
    int rounds;
    if(!(std::cin >> seed >> rounds)) return 0;
    std::mt19937_64 random(seed);
    for(int round = 0; round < rounds; ++round){
        const int vertex_count = 3 + static_cast<int>(random() % 5);
        const std::size_t edge_count = 3 + random() % 9;
        std::vector<std::pair<int, int>> edges(edge_count);
        std::vector<int> color(edge_count);
        std::vector<long long> weight(edge_count);
        for(std::size_t edge = 0; edge < edge_count; ++edge){
            int first = static_cast<int>(random() % vertex_count);
            int second = static_cast<int>(random() % vertex_count);
            if(first == second) second = (second + 1) % vertex_count;
            edges[edge] = {first, second};
            color[edge] = static_cast<int>(random() % 4);
            weight[edge] = static_cast<long long>(random() % 21) - 5;
        }
        const auto graphic = [&](const std::vector<std::size_t>& set){
            Dsu dsu(vertex_count);
            for(const std::size_t edge : set){
                if(!dsu.unite(edges[edge].first, edges[edge].second)){
                    return false;
                }
            }
            return true;
        };
        const auto partition = [&](const std::vector<std::size_t>& set){
            std::vector<unsigned char> used(4, 0);
            for(const std::size_t edge : set){
                if(used[static_cast<std::size_t>(color[edge])]) return false;
                used[static_cast<std::size_t>(color[edge])] = 1;
            }
            return true;
        };

        std::size_t best_size = 0;
        long long best_weight = 0;
        for(std::size_t mask = 0;
            mask < (std::size_t{1} << edge_count);
            ++mask){
            std::vector<std::size_t> set;
            long long sum = 0;
            for(std::size_t edge = 0; edge < edge_count; ++edge){
                if((mask >> edge) & 1U){
                    set.push_back(edge);
                    sum += weight[edge];
                }
            }
            if(!graphic(set) || !partition(set)) continue;
            best_size = std::max(best_size, set.size());
            best_weight = std::max(best_weight, sum);
        }
        const auto unweighted =
            combinatorial_optimization::matroid_intersection(
                edge_count, graphic, partition
            );
        assert(unweighted.elements.size() == best_size);
        assert(graphic(unweighted.elements));
        assert(partition(unweighted.elements));
        const auto weighted =
            combinatorial_optimization::weighted_matroid_intersection(
                weight, graphic, partition
            );
        assert(weighted.weight == best_weight);
        assert(graphic(weighted.elements));
        assert(partition(weighted.elements));

        const std::size_t n = 4 + random() % 5;
        const auto rank_two = [](const std::vector<std::size_t>& set){
            return set.size() <= 2;
        };
        const auto union_result =
            combinatorial_optimization::matroid_union(n, 2, rank_two);
        std::size_t covered = 0;
        for(const auto& group : union_result.groups){
            assert(group.size() <= 2);
            covered += group.size();
        }
        assert(covered == std::min<std::size_t>(n, 4));
        assert(covered + union_result.uncovered.size() == n);

        const auto zero_union =
            combinatorial_optimization::matroid_union(n, 0, rank_two);
        assert(zero_union.groups.empty());
        assert(zero_union.uncovered.size() == n);

        const std::size_t color_copy_count = 1 + random() % 3;
        const auto color_union =
            combinatorial_optimization::matroid_union(
                edge_count, color_copy_count, partition
            );
        std::vector<unsigned char> color_seen(edge_count, 0);
        std::size_t color_covered = 0;
        for(const auto& group: color_union.groups){
            assert(partition(group));
            for(std::size_t element: group){
                assert(color_seen[element] == 0);
                color_seen[element] = 1;
                ++color_covered;
            }
        }
        for(std::size_t element: color_union.uncovered){
            assert(color_seen[element] == 0);
            color_seen[element] = 1;
        }
        assert(std::all_of(
            color_seen.begin(), color_seen.end(),
            [](unsigned char value){ return value != 0; }
        ));
        std::vector<std::size_t> color_frequency(4, 0);
        for(int value: color){
            ++color_frequency[static_cast<std::size_t>(value)];
        }
        std::size_t expected_color_covered = 0;
        for(std::size_t count: color_frequency){
            expected_color_covered += std::min(count, color_copy_count);
        }
        assert(color_covered == expected_color_covered);

        if(edge_count <= 9){
            std::size_t expected_graphic_covered = 0;
            const std::size_t subset_count =
                std::size_t{1} << edge_count;
            for(std::size_t mask = 0; mask < subset_count; ++mask){
                bool feasible = false;
                std::size_t left_mask = mask;
                while(true){
                    std::vector<std::size_t> left;
                    std::vector<std::size_t> right;
                    for(std::size_t edge = 0; edge < edge_count; ++edge){
                        if(((mask >> edge) & 1U) == 0U) continue;
                        if(((left_mask >> edge) & 1U) != 0U){
                            left.push_back(edge);
                        }else{
                            right.push_back(edge);
                        }
                    }
                    if(graphic(left) && graphic(right)){
                        feasible = true;
                        break;
                    }
                    if(left_mask == 0) break;
                    left_mask = (left_mask - 1) & mask;
                }
                if(feasible){
                    expected_graphic_covered = std::max(
                        expected_graphic_covered,
                        static_cast<std::size_t>(
                            __builtin_popcountll(mask)
                        )
                    );
                }
            }
            const auto graphic_union =
                combinatorial_optimization::matroid_union(
                    edge_count, 2, graphic
                );
            std::vector<unsigned char> seen(edge_count, 0);
            std::size_t graphic_covered = 0;
            for(const auto& group: graphic_union.groups){
                assert(graphic(group));
                for(std::size_t element: group){
                    assert(seen[element] == 0);
                    seen[element] = 1;
                    ++graphic_covered;
                }
            }
            for(std::size_t element: graphic_union.uncovered){
                assert(seen[element] == 0);
                seen[element] = 1;
            }
            assert(graphic_covered == expected_graphic_covered);
        }

        const std::size_t submodular_size = 4 + random() % 4;
        std::vector<long long> unary(submodular_size);
        for(long long& value : unary){
            value = static_cast<long long>(random() % 9) - 4;
        }
        std::vector<std::vector<long long>> cut(
            submodular_size,
            std::vector<long long>(submodular_size)
        );
        for(std::size_t first = 0; first < submodular_size; ++first){
            for(std::size_t second = first + 1;
                second < submodular_size;
                ++second){
                cut[first][second] = cut[second][first] =
                    static_cast<long long>(random() % 6);
            }
        }
        const auto oracle = [&](const std::vector<unsigned char>& set){
            long long value = 0;
            for(std::size_t element = 0;
                element < submodular_size;
                ++element){
                if(set[element]) value += unary[element];
            }
            for(std::size_t first = 0; first < submodular_size; ++first){
                for(std::size_t second = first + 1;
                    second < submodular_size;
                    ++second){
                    if(set[first] != set[second]){
                        value += cut[first][second];
                    }
                }
            }
            return value;
        };
        long long brute = oracle(
            std::vector<unsigned char>(submodular_size)
        );
        for(std::size_t mask = 0;
            mask < (std::size_t{1} << submodular_size);
            ++mask){
            std::vector<unsigned char> set(submodular_size);
            for(std::size_t element = 0;
                element < submodular_size;
                ++element){
                set[element] = (mask >> element) & 1U;
            }
            brute = std::min(brute, oracle(set));
        }
        const auto minimized =
            combinatorial_optimization::submodular_function_minimization(
                submodular_size, oracle, 2000, 1e-13L
            );
        assert(minimized.value == brute);
        const auto one_iteration =
            combinatorial_optimization::submodular_function_minimization(
                submodular_size, oracle, 1, 1e-30L
            );
        assert(one_iteration.value == brute);
    }
    std::cout << "OK\n";
}
