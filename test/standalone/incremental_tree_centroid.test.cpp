// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/structure/tree/incremental_tree_centroid.hpp"

using Tree = IncrementalTreeCentroid<200000>;

static std::vector<int> naive_subtree_sizes(const std::vector<int>& parent){
    std::vector<int> sizes(parent.size(), 1);
    for(std::size_t vertex = parent.size(); vertex-- > 1;){
        sizes[static_cast<std::size_t>(parent[vertex])] += sizes[vertex];
    }
    return sizes;
}

static bool is_centroid(int candidate, const std::vector<int>& parent,
                        const std::vector<int>& subtree){
    const int size = static_cast<int>(parent.size());
    int maximum = size - subtree[static_cast<std::size_t>(candidate)];
    for(std::size_t vertex = 1; vertex < parent.size(); ++vertex){
        if(parent[vertex] == candidate){
            maximum = std::max(maximum, subtree[vertex]);
        }
    }
    return maximum * 2 <= size;
}

static void test_random(){
    std::mt19937_64 random(2026071631ULL);
    for(int iteration = 0; iteration < 80; ++iteration){
        Tree tree;
        std::vector<int> parent;
        assert(tree.centroid() == -1);
        assert(tree.add_root() == 0);
        parent.push_back(-1);
        for(int step = 1; step <= 700; ++step){
            const int selected_parent = static_cast<int>(random() % parent.size());
            assert(tree.add_leaf(static_cast<std::size_t>(selected_parent)) == step);
            parent.push_back(selected_parent);
            const auto subtree = naive_subtree_sizes(parent);
            assert(is_centroid(tree.centroid(), parent, subtree));
            for(int probe = 0; probe < 5; ++probe){
                const std::size_t vertex = random() % parent.size();
                assert(tree.subtree_size(vertex)
                    == static_cast<std::size_t>(subtree[vertex]));
                assert(tree.parent(vertex) == parent[vertex]);
                assert(tree.depth(vertex) >= 0);
                assert(tree.is_ancestor(vertex, vertex));
            }
        }
    }
}

static void test_shapes_and_errors(){
    IncrementalTreeCentroid<20> chain;
    chain.add_root();
    for(int vertex = 1; vertex < 20; ++vertex){
        chain.add_leaf(static_cast<std::size_t>(vertex - 1));
        assert(chain.centroid() == vertex / 2);
        assert(chain.subtree_size(0) == static_cast<std::size_t>(vertex + 1));
    }
    assert(chain.kth_ancestor(19, 19) == 0);
    assert(chain.kth_ancestor(19, 20) == -1);
    assert(chain.is_ancestor(3, 19));
    assert(!chain.is_ancestor(19, 3));

    bool thrown = false;
    try{ chain.add_leaf(0); }catch(const std::length_error&){ thrown = true; }
    assert(thrown);
    thrown = false;
    try{ chain.add_root(); }catch(const std::logic_error&){ thrown = true; }
    assert(thrown);

    IncrementalTreeCentroid<0> empty;
    thrown = false;
    try{ empty.add_root(); }catch(const std::length_error&){ thrown = true; }
    assert(thrown);
}

static void test_large(){
    Tree tree;
    tree.add_root();
    for(std::size_t vertex = 1; vertex < Tree::capacity(); ++vertex){
        tree.add_leaf((vertex - 1) / 2);
    }
    assert(tree.size() == Tree::capacity());
    assert(tree.subtree_size(0) == Tree::capacity());
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::size_t query_count;
    if(!(std::cin >> query_count)){
        test_random();
        test_shapes_and_errors();
        test_large();
        return 0;
    }

    Tree tree;
    while(query_count-- > 0){
        char operation;
        std::cin >> operation;
        if(operation == 'R'){
            std::cout << tree.add_root() << ' ' << tree.centroid() << '\n';
        }else if(operation == 'A'){
            std::size_t parent;
            std::cin >> parent;
            const int vertex = tree.add_leaf(parent);
            std::cout << vertex << ' ' << tree.centroid() << '\n';
        }else if(operation == 'Q'){
            std::size_t vertex;
            std::cin >> vertex;
            std::cout << tree.subtree_size(vertex) << '\n';
        }else{
            throw std::runtime_error("unknown operation");
        }
    }
}
