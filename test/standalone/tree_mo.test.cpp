// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/algorithm/tree/decomposition/tree_mo.hpp"

namespace{

constexpr int MAX_TEST_SIZE = 1024;

std::vector<int> naive_distinct(
    int n,
    int root,
    const std::vector<int>& values,
    const std::vector<std::pair<int, int>>& edges,
    const std::vector<std::pair<int, int>>& queries
){
    std::vector<std::vector<int>> graph(static_cast<std::size_t>(n));
    for(const auto [left, right] : edges){
        graph[static_cast<std::size_t>(left)].push_back(right);
        graph[static_cast<std::size_t>(right)].push_back(left);
    }

    std::vector<int> parent(static_cast<std::size_t>(n), -2);
    std::vector<int> depth(static_cast<std::size_t>(n), 0);
    std::vector<int> order = {root};
    parent[static_cast<std::size_t>(root)] = -1;
    for(std::size_t index = 0; index < order.size(); ++index){
        const int vertex = order[index];
        for(const int to : graph[static_cast<std::size_t>(vertex)]){
            if(to == parent[static_cast<std::size_t>(vertex)]) continue;
            parent[static_cast<std::size_t>(to)] = vertex;
            depth[static_cast<std::size_t>(to)] =
                depth[static_cast<std::size_t>(vertex)] + 1;
            order.push_back(to);
        }
    }

    const int value_count = values.empty()
        ? 0
        : *std::max_element(values.begin(), values.end()) + 1;
    std::vector<int> answer;
    answer.reserve(queries.size());
    for(const auto [query_left, query_right] : queries){
        int left = query_left;
        int right = query_right;
        std::vector<unsigned char> used(
            static_cast<std::size_t>(value_count),
            0
        );
        int distinct = 0;
        auto take = [&](int vertex){
            const int value = values[static_cast<std::size_t>(vertex)];
            unsigned char& present = used[static_cast<std::size_t>(value)];
            if(!present){
                present = 1;
                ++distinct;
            }
        };
        while(left != right){
            if(depth[static_cast<std::size_t>(left)]
                >= depth[static_cast<std::size_t>(right)]){
                take(left);
                left = parent[static_cast<std::size_t>(left)];
            }else{
                take(right);
                right = parent[static_cast<std::size_t>(right)];
            }
        }
        take(left);
        answer.push_back(distinct);
    }
    return answer;
}

template<int MAX_SIZE>
std::vector<int> tree_mo_distinct(
    int n,
    int root,
    const std::vector<int>& values,
    const std::vector<std::pair<int, int>>& edges,
    const std::vector<std::pair<int, int>>& queries,
    int block_size
){
    TreeMo<MAX_SIZE> mo(n);
    for(const auto [left, right] : edges) mo.add_edge(left, right);
    mo.build(root);
    mo.reserve_queries(static_cast<int>(queries.size()));
    for(std::size_t index = 0; index < queries.size(); ++index){
        const auto [left, right] = queries[index];
        assert(mo.add_query(left, right) == static_cast<int>(index));
    }

    const int value_count = values.empty()
        ? 0
        : *std::max_element(values.begin(), values.end()) + 1;
    std::vector<int> count(static_cast<std::size_t>(value_count), 0);
    std::vector<unsigned char> active(static_cast<std::size_t>(n), 0);
    std::vector<int> answer(queries.size(), -1);
    int distinct = 0;
    int active_count = 0;

    auto add = [&](int vertex){
        unsigned char& present = active[static_cast<std::size_t>(vertex)];
        assert(!present);
        present = 1;
        ++active_count;
        const int value = values[static_cast<std::size_t>(vertex)];
        if(count[static_cast<std::size_t>(value)]++ == 0) ++distinct;
    };
    auto erase = [&](int vertex){
        unsigned char& present = active[static_cast<std::size_t>(vertex)];
        assert(present);
        present = 0;
        --active_count;
        const int value = values[static_cast<std::size_t>(vertex)];
        if(--count[static_cast<std::size_t>(value)] == 0) --distinct;
    };
    mo.solve(
        add,
        erase,
        [&](int query_index){
            answer[static_cast<std::size_t>(query_index)] = distinct;
        },
        block_size
    );

    assert(active_count == 0);
    assert(distinct == 0);
    for(const unsigned char present : active) assert(!present);
    for(const int frequency : count) assert(frequency == 0);
    return answer;
}

void test_fixed_metadata_and_cleanup(){
    const std::vector<int> values = {0, 1, 0, 2, 1, 2, 3};
    const std::vector<std::pair<int, int>> edges = {
        {0, 1}, {0, 2}, {1, 3}, {1, 4}, {2, 5}, {5, 6}
    };
    const std::vector<std::pair<int, int>> queries = {
        {3, 4}, {3, 6}, {2, 2}, {0, 6}, {6, 3}
    };
    const auto expected = naive_distinct(7, 0, values, edges, queries);

    TreeMo<16> mo(7);
    assert(mo.size() == 7);
    assert(mo.edge_count() == 0);
    assert(mo.query_count() == 0);
    for(const auto [left, right] : edges) mo.add_edge(left, right);
    assert(mo.edge_count() == 6);
    mo.build(0);
    assert(mo.root() == 0);
    assert(mo.parent(0) == -1);
    assert(mo.parent(6) == 5);
    assert(mo.depth(6) == 3);
    assert(mo.lca(3, 4) == 1);
    assert(mo.lca(3, 6) == 0);
    assert(mo.lca(2, 6) == 2);

    mo.reserve_queries(20);
    for(std::size_t index = 0; index < queries.size(); ++index){
        assert(mo.add_query(
            queries[index].first,
            queries[index].second
        ) == static_cast<int>(index));
    }
    assert(mo.query_count() == static_cast<int>(queries.size()));

    std::vector<int> count(4, 0);
    std::vector<unsigned char> active(7, 0);
    std::vector<int> answer(queries.size(), -1);
    int distinct = 0;
    auto add = [&](int vertex){
        assert(!active[static_cast<std::size_t>(vertex)]);
        active[static_cast<std::size_t>(vertex)] = 1;
        const int value = values[static_cast<std::size_t>(vertex)];
        if(count[static_cast<std::size_t>(value)]++ == 0) ++distinct;
    };
    auto erase = [&](int vertex){
        assert(active[static_cast<std::size_t>(vertex)]);
        active[static_cast<std::size_t>(vertex)] = 0;
        const int value = values[static_cast<std::size_t>(vertex)];
        if(--count[static_cast<std::size_t>(value)] == 0) --distinct;
    };
    auto collect = [&](int index){
        answer[static_cast<std::size_t>(index)] = distinct;
    };

    mo.solve(add, erase, collect, 0);
    assert(answer == expected);
    assert(distinct == 0);
    for(const unsigned char present : active) assert(!present);

    std::fill(answer.begin(), answer.end(), -1);
    mo.solve(add, erase, collect, -1);
    assert(answer == expected);
    assert(distinct == 0);
    for(const int frequency : count) assert(frequency == 0);

    mo.clear_queries();
    assert(mo.query_count() == 0);
    assert(mo.add_query(4, 4) == 0);
    answer.assign(1, -1);
    mo.solve(add, erase, collect, 1000);
    assert(answer[0] == 1);
    assert(distinct == 0);
}

void test_empty_and_exceptions(){
    TreeMo<0> empty;
    assert(empty.size() == 0);
    assert(empty.edge_count() == 0);
    empty.reserve_queries(0);
    empty.build();
    assert(empty.root() == -1);
    int calls = 0;
    empty.solve(
        [&](int){ ++calls; },
        [&](int){ ++calls; },
        [&](int){ ++calls; },
        0
    );
    assert(calls == 0);

    bool thrown = false;
    try{
        TreeMo<2> invalid(3);
        (void)invalid;
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    TreeMo<3> not_built(3);
    thrown = false;
    try{
        (void)not_built.root();
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        not_built.reserve_queries(-1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        not_built.add_edge(0, 3);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        not_built.build();
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    TreeMo<3> cycle(3);
    cycle.add_edge(0, 1);
    cycle.add_edge(0, 1);
    thrown = false;
    try{
        cycle.build();
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    TreeMo<2> built(2);
    built.add_edge(0, 1);
    built.build(1);
    thrown = false;
    try{
        built.add_edge(0, 1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
    built.add_query(0, 1);
    thrown = false;
    try{
        built.build(0);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

void test_random_against_naive(){
    std::mt19937 random(20260714);
    for(int n = 1; n <= 80; ++n){
        for(int iteration = 0; iteration < 10; ++iteration){
            std::vector<std::pair<int, int>> edges;
            for(int vertex = 1; vertex < n; ++vertex){
                edges.push_back({
                    static_cast<int>(
                        random() % static_cast<unsigned int>(vertex)
                    ),
                    vertex
                });
            }
            std::shuffle(edges.begin(), edges.end(), random);
            const int root = static_cast<int>(
                random() % static_cast<unsigned int>(n)
            );
            const int value_count = 1 + static_cast<int>(
                random() % static_cast<unsigned int>(n)
            );
            std::vector<int> values(static_cast<std::size_t>(n));
            for(int& value : values){
                value = static_cast<int>(
                    random() % static_cast<unsigned int>(value_count)
                );
            }
            std::vector<std::pair<int, int>> queries;
            for(int query = 0; query < 120; ++query){
                queries.push_back({
                    static_cast<int>(
                        random() % static_cast<unsigned int>(n)
                    ),
                    static_cast<int>(
                        random() % static_cast<unsigned int>(n)
                    )
                });
            }

            const auto expected =
                naive_distinct(n, root, values, edges, queries);
            for(const int block_size : {-1, 0, 1, 2 * n + 7}){
                assert((tree_mo_distinct<80>(
                    n,
                    root,
                    values,
                    edges,
                    queries,
                    block_size
                ) == expected));
            }
        }
    }
}

void test_deep_path(){
    constexpr int N = 100000;
    static TreeMo<N> path(N);
    for(int vertex = 1; vertex < N; ++vertex){
        path.add_edge(vertex - 1, vertex);
    }
    path.build(0);
    assert(path.depth(N - 1) == N - 1);
    assert(path.lca(N / 3, N - 1) == N / 3);

    path.add_query(0, N - 1);
    path.add_query(N / 2, N - 1);
    path.add_query(N / 3, N / 3);
    const int expected[] = {N, N - N / 2, 1};
    int active = 0;
    int answered = 0;
    path.solve(
        [&](int){ ++active; },
        [&](int){ --active; },
        [&](int index){
            assert(active == expected[index]);
            ++answered;
        },
        0
    );
    assert(answered == 3);
    assert(active == 0);
}

void self_test(){
    test_fixed_metadata_and_cleanup();
    test_empty_and_exceptions();
    test_random_against_naive();
    test_deep_path();
}

} // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int test_count;
    if(!(std::cin >> test_count)){
        self_test();
        return 0;
    }

    while(test_count-- != 0){
        int n, root, query_count;
        std::cin >> n >> root >> query_count;
        std::vector<int> values(static_cast<std::size_t>(n));
        for(int& value : values) std::cin >> value;
        std::vector<std::pair<int, int>> edges;
        edges.reserve(static_cast<std::size_t>(std::max(0, n - 1)));
        for(int edge = 1; edge < n; ++edge){
            int left, right;
            std::cin >> left >> right;
            edges.push_back({left, right});
        }
        std::vector<std::pair<int, int>> queries(
            static_cast<std::size_t>(query_count)
        );
        for(auto& [left, right] : queries) std::cin >> left >> right;

        const auto answer = tree_mo_distinct<MAX_TEST_SIZE>(
            n,
            root,
            values,
            edges,
            queries,
            0
        );
        for(const int value : answer) std::cout << value << '\n';
    }
}
