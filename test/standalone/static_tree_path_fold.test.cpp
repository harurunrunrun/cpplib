// competitive-verifier: STANDALONE

#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "../../src/algorithm/tree/query/static_tree_path_fold.hpp"
#include "../../src/structure/types/monoid.hpp"

using Range = std::pair<long long, long long>;

constexpr Range range_op(Range left, Range right){
    return {
        left.first < right.first ? left.first : right.first,
        left.second < right.second ? right.second : left.second
    };
}

constexpr Range range_identity(){
    return {
        std::numeric_limits<long long>::max(),
        std::numeric_limits<long long>::lowest()
    };
}

constexpr Monoid<range_op, range_identity> range_monoid{};

std::string concatenate(std::string left, std::string right){
    return left + right;
}

std::string empty_string(){
    return {};
}

constexpr Monoid<concatenate, empty_string> concatenate_monoid{};

template<class Function>
void expect_runtime_error(Function&& function){
    bool thrown = false;
    try{
        function();
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

void test_non_commutative_product(){
    StaticTreePathFold<concatenate_monoid, 8> tree(5);
    tree.add_edge(0, 1, "a");
    tree.add_edge(1, 2, "b");
    tree.add_edge(1, 3, "c");
    tree.add_edge(3, 4, "d");
    tree.build(0);

    assert(tree.size() == 5);
    assert(tree.root() == 0);
    assert(tree.parent(4) == 3);
    assert(tree.depth(4) == 3);
    assert(tree.jump(4, 2) == 1);
    assert(tree.lca(2, 4) == 1);
    assert(tree.dist(2, 4) == 3);
    assert(tree.prod(2, 4) == "bcd");
    assert(tree.prod(4, 2) == "dcb");
    assert(tree.prod(4, 0) == "dca");
    assert(tree.prod(0, 4) == "acd");
    assert(tree.prod(2, 2).empty());

    tree.build(3);
    assert(tree.root() == 3);
    assert(tree.lca(0, 2) == 1);
    assert(tree.prod(2, 4) == "bcd");
    assert(tree.prod(4, 2) == "dcb");

    auto copied = tree;
    assert(copied.prod(0, 4) == "acd");
    auto moved = std::move(copied);
    assert(moved.prod(4, 0) == "dca");
}

void test_random_non_commutative_products(){
    std::uint64_t state = 0x9e3779b97f4a7c15ULL;
    const auto random = [&state](){
        state ^= state << 7;
        state ^= state >> 9;
        return state;
    };
    for(int iteration = 0; iteration < 80; ++iteration){
        const int n = 1 + static_cast<int>(random() % 24ULL);
        StaticTreePathFold<concatenate_monoid, 24> tree(n);
        std::vector<std::vector<std::pair<int, std::string>>> graph(
            static_cast<std::size_t>(n)
        );
        for(int vertex = 1; vertex < n; ++vertex){
            const int parent = static_cast<int>(
                random() % static_cast<std::uint64_t>(vertex)
            );
            const std::string label = "[" + std::to_string(vertex) + "]";
            tree.add_edge(vertex, parent, label);
            graph[static_cast<std::size_t>(vertex)].push_back({parent, label});
            graph[static_cast<std::size_t>(parent)].push_back({vertex, label});
        }
        tree.build(static_cast<int>(random() % static_cast<std::uint64_t>(n)));
        for(int source = 0; source < n; ++source){
            std::vector<int> parent(static_cast<std::size_t>(n), -1);
            std::vector<std::string> parent_label(static_cast<std::size_t>(n));
            std::vector<int> order = {source};
            parent[static_cast<std::size_t>(source)] = source;
            for(std::size_t index = 0; index < order.size(); ++index){
                const int vertex = order[index];
                for(const auto& [to, label]: graph[static_cast<std::size_t>(vertex)]){
                    if(parent[static_cast<std::size_t>(to)] != -1) continue;
                    parent[static_cast<std::size_t>(to)] = vertex;
                    parent_label[static_cast<std::size_t>(to)] = label;
                    order.push_back(to);
                }
            }
            for(int target = 0; target < n; ++target){
                std::vector<std::string> reverse_labels;
                for(int vertex = target; vertex != source;
                    vertex = parent[static_cast<std::size_t>(vertex)]){
                    reverse_labels.push_back(
                        parent_label[static_cast<std::size_t>(vertex)]
                    );
                }
                std::string expected;
                for(auto iterator = reverse_labels.rbegin();
                    iterator != reverse_labels.rend(); ++iterator){
                    expected += *iterator;
                }
                assert(tree.prod(source, target) == expected);
            }
        }
    }
}

void test_empty_and_exceptions(){
    StaticTreePathFold<range_monoid, 1> empty(0);
    empty.build();
    assert(empty.size() == 0);
    assert(empty.root() == -1);

    expect_runtime_error([]{
        StaticTreePathFold<range_monoid, 2> invalid(-1);
        (void)invalid;
    });
    expect_runtime_error([]{
        StaticTreePathFold<range_monoid, 2> invalid(3);
        (void)invalid;
    });
    expect_runtime_error([]{
        StaticTreePathFold<range_monoid, 2> invalid(std::numeric_limits<int>::max());
        (void)invalid;
    });
    expect_runtime_error([]{
        StaticTreePathFold<range_monoid, 3> tree(3);
        (void)tree.root();
    });
    expect_runtime_error([]{
        StaticTreePathFold<range_monoid, 3> tree(3);
        tree.add_edge(0, 3, {1, 1});
    });
    expect_runtime_error([]{
        StaticTreePathFold<range_monoid, 3> tree(3);
        tree.add_edge(0, 1, {1, 1});
        tree.build();
    });
    expect_runtime_error([]{
        StaticTreePathFold<range_monoid, 4> tree(4);
        tree.add_edge(0, 1, {1, 1});
        tree.add_edge(1, 2, {2, 2});
        tree.add_edge(2, 0, {3, 3});
        tree.build();
    });

    StaticTreePathFold<range_monoid, 3> tree(3);
    tree.add_edge(0, 1, {1, 1});
    tree.add_edge(1, 2, {2, 2});
    tree.build();
    expect_runtime_error([&]{ tree.add_edge(0, 2, {3, 3}); });
    expect_runtime_error([&]{ (void)tree.jump(2, 3); });
    expect_runtime_error([&]{ (void)tree.prod(-1, 2); });

    expect_runtime_error([&]{ tree.build(3); });
    expect_runtime_error([&]{ (void)tree.root(); });
    expect_runtime_error([&]{ (void)tree.prod(0, 2); });
    expect_runtime_error([&]{ tree.add_edge(0, 2, {3, 3}); });
    tree.build(1);
    assert(tree.root() == 1);
    assert(tree.prod(0, 2) == (Range{1, 2}));
}

int main(){
    test_non_commutative_product();
    test_random_non_commutative_products();
    test_empty_and_exceptions();

    int test_count;
    if(!(std::cin >> test_count)) return 0;
    while(test_count-- != 0){
        int vertex_count, query_count, root;
        std::cin >> vertex_count >> query_count >> root;
        StaticTreePathFold<range_monoid, 100000> tree(vertex_count);
        for(int edge = 1; edge < vertex_count; ++edge){
            int left, right;
            long long weight;
            std::cin >> left >> right >> weight;
            tree.add_edge(left, right, {weight, weight});
        }
        tree.build(root);
        while(query_count-- != 0){
            int left, right;
            std::cin >> left >> right;
            const auto [minimum, maximum] = tree.prod(left, right);
            std::cout << minimum << ' ' << maximum << '\n';
        }
    }
}
