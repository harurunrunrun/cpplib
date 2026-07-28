// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <random>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/range/offline_query/offline_3d_dominance.hpp"
#include "../../src/algorithm/tree/construction/prufer_code.hpp"
#include "../../src/algorithm/tree/query/level_ancestor.hpp"
#include "../../src/structure/range_query/extrema/fischer_heun_rmq.hpp"
#include "../../src/structure/range_query/extrema/plus_minus_one_rmq.hpp"
#include "../../src/structure/range_query/value_query/fractional_cascading.hpp"
#include "../../src/structure/range_query/value_query/priority_search_tree.hpp"
#include "../../src/structure/range_query/value_query/range_tree.hpp"
#include "../../src/structure/wavelet_matrix/static/wavelet_tree.hpp"

int main(){
    std::mt19937 random(20260728);
    for(int repetition = 0; repetition < 100; ++repetition){
        const int n = 1 + static_cast<int>(random() % 30);
        std::vector<std::vector<int>> graph(n);
        std::vector<int> parents(n);
        std::set<std::pair<int, int>> expected_edges;
        for(int v = 1; v < n; ++v){
            const int parent = static_cast<int>(random() % v);
            graph[v].push_back(parent);
            parents[v] = parent;
            graph[parent].push_back(v);
            expected_edges.emplace(parent, v);
        }
        if(n >= 2){
            std::set<std::pair<int, int>> actual_edges;
            for(auto [a, b]: prufer_decode(prufer_encode(graph))){
                if(a > b) std::swap(a, b);
                actual_edges.emplace(a, b);
            }
            assert(actual_edges == expected_edges);
        }
        LevelAncestor ancestor(graph);
        for(int v = 0; v < n; ++v){
            int current = v;
            for(int distance = 0; distance <= ancestor.depth(v); ++distance){
                assert(ancestor.kth_ancestor(v, distance) == current);
                current = parents[current];
            }
        }

        std::vector<int> values(n);
        for(int& value: values) value = static_cast<int>(random() % 41) - 20;
        FischerHeunRMQ<int> rmq(values);
        WaveletTree<int> wavelet(values);
        for(int query = 0; query < 100; ++query){
            int left = static_cast<int>(random() % n), right = static_cast<int>(random() % n);
            if(left > right) std::swap(left, right);
            ++right;
            const int minimum = *std::min_element(values.begin() + left, values.begin() + right);
            assert(rmq.minimum(left, right) == minimum);
            std::vector<int> sorted(values.begin() + left, values.begin() + right);
            std::sort(sorted.begin(), sorted.end());
            const int rank = static_cast<int>(random() % sorted.size());
            assert(wavelet.kth_smallest(left, right, rank) == sorted[rank]);
            const int bound = static_cast<int>(random() % 51) - 25;
            assert(wavelet.count_less(left, right, bound)
                   == static_cast<int>(std::lower_bound(sorted.begin(), sorted.end(), bound) - sorted.begin()));
        }

        std::vector<int> walk(n);
        for(int i = 1; i < n; ++i) walk[i] = walk[i - 1] + ((random() & 1U) ? 1 : -1);
        PlusMinusOneRMQ walk_rmq(walk);
        for(int left = 0; left < n; ++left) for(int right = left + 1; right <= n; ++right)
            assert(walk_rmq.minimum(left, right) == *std::min_element(walk.begin() + left, walk.begin() + right));

        std::vector<DominancePoint3D> points(n);
        for(auto& point: points) point = {static_cast<int>(random() % 7), static_cast<int>(random() % 7),
                                          static_cast<int>(random() % 7)};
        const auto dominance = offline_3d_dominance(points);
        for(int i = 0; i < n; ++i){
            long long brute = 0;
            for(int j = 0; j < n; ++j) if(i != j && points[j].x <= points[i].x &&
                points[j].y <= points[i].y && points[j].z <= points[i].z) ++brute;
            assert(dominance[i] == brute);
        }

        using Range = RangeTree<int, int>;
        using Priority = PrioritySearchTree<int, int>;
        std::vector<Range::Point> range_points;
        std::vector<Priority::Point> priority_points;
        for(int i = 0; i < n; ++i){
            range_points.push_back({static_cast<int>(points[i].x), static_cast<int>(points[i].y), i});
            priority_points.push_back({static_cast<int>(points[i].x), static_cast<int>(points[i].y), i});
        }
        Range range_tree(range_points);
        Priority priority_tree(priority_points);
        for(int query = 0; query < 30; ++query){
            int x1 = static_cast<int>(random() % 8), x2 = static_cast<int>(random() % 8);
            int y1 = static_cast<int>(random() % 8), y2 = static_cast<int>(random() % 8);
            if(x1 > x2) std::swap(x1, x2);
            if(y1 > y2) std::swap(y1, y2);
            auto report = range_tree.report(x1, x2, y1, y2);
            std::sort(report.begin(), report.end());
            std::vector<int> brute;
            for(int i = 0; i < n; ++i) if(x1 <= points[i].x && points[i].x < x2 &&
                y1 <= points[i].y && points[i].y < y2) brute.push_back(i);
            assert(report == brute);
            auto three_sided = priority_tree.report(x1, x2, y2);
            std::sort(three_sided.begin(), three_sided.end());
            brute.clear();
            for(int i = 0; i < n; ++i) if(x1 <= points[i].x && points[i].x < x2 && points[i].y < y2)
                brute.push_back(i);
            assert(three_sided == brute);
        }
    }

    {
        for(int repetition = 0; repetition < 140; ++repetition){
            const int vertex_count =
                1 + static_cast<int>(random() % 180);
            std::vector<std::vector<int>> graph(
                static_cast<std::size_t>(vertex_count)
            );
            for(int vertex = 1; vertex < vertex_count; ++vertex){
                const int parent =
                    static_cast<int>(random()
                        % static_cast<unsigned int>(vertex));
                graph[static_cast<std::size_t>(vertex)]
                    .push_back(parent);
                graph[static_cast<std::size_t>(parent)]
                    .push_back(vertex);
            }
            for(auto& adjacent: graph){
                std::shuffle(adjacent.begin(), adjacent.end(), random);
            }
            const int root =
                static_cast<int>(random()
                    % static_cast<unsigned int>(vertex_count));
            std::vector<int> brute_parent(
                static_cast<std::size_t>(vertex_count), -1
            );
            std::vector<int> brute_depth(
                static_cast<std::size_t>(vertex_count), 0
            );
            std::vector<int> order{root};
            brute_parent[static_cast<std::size_t>(root)] = root;
            for(std::size_t index = 0; index < order.size(); ++index){
                const int vertex = order[index];
                for(const int to:
                    graph[static_cast<std::size_t>(vertex)]){
                    if(brute_parent[static_cast<std::size_t>(to)]
                        != -1) continue;
                    brute_parent[static_cast<std::size_t>(to)] =
                        vertex;
                    brute_depth[static_cast<std::size_t>(to)] =
                        brute_depth[static_cast<std::size_t>(vertex)]
                        + 1;
                    order.push_back(to);
                }
            }

            const LevelAncestor ancestor(graph, root);
            assert(ancestor.size() == vertex_count);
            for(int vertex = 0; vertex < vertex_count; ++vertex){
                assert(ancestor.depth(vertex)
                    == brute_depth[static_cast<std::size_t>(vertex)]);
                int current = vertex;
                for(int distance = 0;
                    distance
                        <= brute_depth[
                            static_cast<std::size_t>(vertex)
                        ];
                    ++distance){
                    assert(ancestor.kth_ancestor(vertex, distance)
                        == current);
                    assert(ancestor.ancestor_at_depth(
                        vertex,
                        brute_depth[
                            static_cast<std::size_t>(vertex)
                        ] - distance
                    ) == current);
                    current =
                        brute_parent[static_cast<std::size_t>(current)];
                }
                assert(ancestor.kth_ancestor(vertex, -1) == -1);
                assert(ancestor.kth_ancestor(
                    vertex,
                    brute_depth[static_cast<std::size_t>(vertex)] + 1
                ) == -1);
                assert(ancestor.ancestor_at_depth(vertex, -1) == -1);
                assert(ancestor.ancestor_at_depth(
                    vertex,
                    brute_depth[static_cast<std::size_t>(vertex)] + 1
                ) == -1);
            }
        }

        constexpr int path_size = 20'000;
        std::vector<std::vector<int>> path(
            static_cast<std::size_t>(path_size)
        );
        for(int vertex = 1; vertex < path_size; ++vertex){
            path[static_cast<std::size_t>(vertex - 1)]
                .push_back(vertex);
            path[static_cast<std::size_t>(vertex)]
                .push_back(vertex - 1);
        }
        const LevelAncestor from_left(path, 0);
        const LevelAncestor from_right(path, path_size - 1);
        for(int distance = 0; distance < path_size; ++distance){
            assert(from_left.kth_ancestor(path_size - 1, distance)
                == path_size - 1 - distance);
            assert(from_right.kth_ancestor(0, distance) == distance);
        }

        const auto rejects_invalid_tree =
            [](const std::vector<std::vector<int>>& invalid_graph){
                try{
                    const LevelAncestor structure(invalid_graph);
                    (void)structure;
                }catch(const std::invalid_argument&){
                    return true;
                }
                return false;
            };
        assert(rejects_invalid_tree({{0}}));
        assert(rejects_invalid_tree(
            {{1, 1}, {0}, {0}}
        ));
        assert(rejects_invalid_tree(
            {{1, 2}, {2}, {3}, {0, 1}}
        ));
        assert(rejects_invalid_tree(
            {{1}, {0}, {3, 4}, {2, 4}, {2, 3}}
        ));

        bool range_error = false;
        try{
            const LevelAncestor invalid_edge({{1}, {0, 2}});
            (void)invalid_edge;
        }catch(const std::out_of_range&){
            range_error = true;
        }
        assert(range_error);
        range_error = false;
        try{
            const LevelAncestor invalid_root({{}}, 1);
            (void)invalid_root;
        }catch(const std::out_of_range&){
            range_error = true;
        }
        assert(range_error);
        range_error = false;
        try{
            const LevelAncestor empty({}, 1);
            (void)empty;
        }catch(const std::out_of_range&){
            range_error = true;
        }
        assert(range_error);

        const LevelAncestor singleton({{}});
        range_error = false;
        try{
            (void)singleton.depth(1);
        }catch(const std::out_of_range&){
            range_error = true;
        }
        assert(range_error);
    }

    {
        using Priority = PrioritySearchTree<int, int>;
        const Priority empty;
        assert(empty.size() == 0);
        assert(empty.report(-1, 1, 1).empty());

        bool invalid_range_thrown = false;
        try{
            (void)empty.report(1, 0, 1);
        }catch(const std::invalid_argument&){
            invalid_range_thrown = true;
        }
        assert(invalid_range_thrown);

        for(int repetition = 0; repetition < 160; ++repetition){
            const int point_count =
                static_cast<int>(random() % 181);
            std::vector<Priority::Point> points;
            points.reserve(static_cast<std::size_t>(point_count));
            for(int index = 0; index < point_count; ++index){
                points.push_back({
                    static_cast<int>(random() % 41) - 20,
                    static_cast<int>(random() % 41) - 20,
                    index
                });
            }
            const Priority tree(points);
            assert(tree.size() == point_count);
            for(int query = 0; query < 180; ++query){
                int lower_x =
                    static_cast<int>(random() % 51) - 25;
                int upper_x =
                    static_cast<int>(random() % 51) - 25;
                if(lower_x > upper_x){
                    std::swap(lower_x, upper_x);
                }
                const int upper_y =
                    static_cast<int>(random() % 51) - 25;
                auto actual =
                    tree.report(lower_x, upper_x, upper_y);
                std::sort(actual.begin(), actual.end());
                std::vector<int> expected;
                for(const auto& point: points){
                    if(lower_x <= point.x && point.x < upper_x
                        && point.y < upper_y){
                        expected.push_back(point.payload);
                    }
                }
                std::sort(expected.begin(), expected.end());
                assert(actual == expected);
            }
        }

        std::vector<Priority::Point> duplicates;
        for(int index = 0; index < 256; ++index){
            duplicates.push_back({7, -3, index});
        }
        const Priority duplicate_tree(duplicates);
        assert(duplicate_tree.report(7, 8, -2).size() == 256);
        assert(duplicate_tree.report(7, 8, -3).empty());
        assert(duplicate_tree.report(6, 7, 0).empty());
        assert(duplicate_tree.report(8, 9, 0).empty());
    }

    FractionalCascading<int> cascading({{1, 4, 9}, {2, 6}, {}, {0, 7, 8}});
    for(int value = -2; value <= 11; ++value){
        const auto answers = cascading.lower_bounds(value);
        const std::vector<std::vector<int>> catalogs{{1, 4, 9}, {2, 6}, {}, {0, 7, 8}};
        for(int i = 0; i < 4; ++i)
            assert(answers[i] == static_cast<int>(std::lower_bound(catalogs[i].begin(), catalogs[i].end(), value)
                                                  - catalogs[i].begin()));
    }
}
