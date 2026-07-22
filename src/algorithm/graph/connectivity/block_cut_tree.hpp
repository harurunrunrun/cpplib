#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTIVITY_BLOCK_CUT_TREE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTIVITY_BLOCK_CUT_TREE_HPP_INCLUDED

#include <stdexcept>
#include <utility>
#include <vector>

#include "two_vertex_connected_components.hpp"

struct BlockCutTreeResult{
    int original_vertex_count = 0;
    int block_count = 0;
    std::vector<int> articulation;
    std::vector<std::vector<int>> block_vertices;
    std::vector<std::vector<int>> block_edges;
    std::vector<std::vector<int>> tree;
    std::vector<std::vector<int>> vertex_blocks;

    int node_count() const{
        return static_cast<int>(tree.size());
    }

    bool is_original_vertex(int node) const{
        return 0 <= node && node < original_vertex_count;
    }

    bool is_block_node(int node) const{
        return original_vertex_count <= node && node < node_count();
    }

    int block_node(int block) const{
        if(block < 0 || block_count <= block)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (BlockCutTreeResult::block_node)."
            );
        }
        return original_vertex_count + block;
    }

    int block_id(int node) const{
        if(!is_block_node(node))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (BlockCutTreeResult::block_id)."
            );
        }
        return node - original_vertex_count;
    }
};

inline BlockCutTreeResult block_cut_tree(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges
){
    if(vertex_count < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation (block_cut_tree)."
        );
    }
    for(auto [u, v]: edges){
        if(u < 0 || vertex_count <= u || v < 0 || vertex_count <= v)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (block_cut_tree)."
            );
        }
        if(u == v)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: self loop is not supported (block_cut_tree)."
            );
        }
    }

    auto components = two_vertex_connected_components(vertex_count, edges);
    BlockCutTreeResult result;
    result.original_vertex_count = vertex_count;
    result.block_count = components.count;
    result.articulation = std::move(components.articulation);
    result.block_vertices = std::move(components.vertex_groups);
    result.block_edges = std::move(components.edge_groups);
    result.tree.resize(static_cast<std::size_t>(vertex_count + result.block_count));
    result.vertex_blocks.resize(static_cast<std::size_t>(vertex_count));

    for(int block = 0; block < result.block_count; ++block){
        const int block_node = vertex_count + block;
        for(int vertex: result.block_vertices[static_cast<std::size_t>(block)]){
            result.tree[static_cast<std::size_t>(vertex)].push_back(block_node);
            result.tree[static_cast<std::size_t>(block_node)].push_back(vertex);
            result.vertex_blocks[static_cast<std::size_t>(vertex)].push_back(block);
        }
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_CONNECTIVITY_BLOCK_CUT_TREE_HPP_INCLUDED
