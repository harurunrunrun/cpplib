#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_FLOW_CUT_GOMORY_HU_TREE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_FLOW_CUT_GOMORY_HU_TREE_HPP_INCLUDED

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../matching/flow/dinic.hpp"

struct GomoryHuEdge{
    int from;
    int to;
    long long capacity;
};

struct GomoryHuTree{
    std::vector<int> parent;
    std::vector<long long> cut_value;

private:
    std::vector<int> depth_;
    std::vector<std::vector<int>> ancestor_;
    std::vector<std::vector<long long>> minimum_;

public:
    GomoryHuTree() = default;

    GomoryHuTree(
        std::vector<int> parent_,
        std::vector<long long> cut_value_
    ) : parent(std::move(parent_)), cut_value(std::move(cut_value_)){
        const int vertex_count = static_cast<int>(parent.size());
        if(cut_value.size() != parent.size())[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: size mismatch (GomoryHuTree)."
            );
        }
        if(vertex_count == 0) return;
        std::vector<std::vector<std::pair<int, long long>>> tree(
            static_cast<std::size_t>(vertex_count)
        );
        for(int vertex = 1; vertex < vertex_count; ++vertex){
            int to = parent[static_cast<std::size_t>(vertex)];
            if(to < 0 || vertex_count <= to)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: invalid parent (GomoryHuTree)."
                );
            }
            long long value = cut_value[static_cast<std::size_t>(vertex)];
            tree[static_cast<std::size_t>(vertex)].emplace_back(to, value);
            tree[static_cast<std::size_t>(to)].emplace_back(vertex, value);
        }
        int level_count = 1;
        while((std::size_t{1} << static_cast<unsigned>(level_count))
            <= static_cast<std::size_t>(vertex_count)){
            ++level_count;
        }
        depth_.assign(static_cast<std::size_t>(vertex_count), 0);
        ancestor_.assign(
            static_cast<std::size_t>(level_count),
            std::vector<int>(static_cast<std::size_t>(vertex_count), 0)
        );
        minimum_.assign(
            static_cast<std::size_t>(level_count),
            std::vector<long long>(
                static_cast<std::size_t>(vertex_count),
                std::numeric_limits<long long>::max()
            )
        );
        std::vector<int> stack{0};
        std::vector<int> traversal;
        traversal.reserve(static_cast<std::size_t>(vertex_count));
        ancestor_[0][0] = 0;
        while(!stack.empty()){
            int vertex = stack.back();
            stack.pop_back();
            traversal.push_back(vertex);
            for(auto [to, value]: tree[static_cast<std::size_t>(vertex)]){
                if(to == ancestor_[0][static_cast<std::size_t>(vertex)]
                    && vertex != 0){
                    continue;
                }
                ancestor_[0][static_cast<std::size_t>(to)] = vertex;
                minimum_[0][static_cast<std::size_t>(to)] = value;
                depth_[static_cast<std::size_t>(to)] =
                    depth_[static_cast<std::size_t>(vertex)] + 1;
                stack.push_back(to);
            }
        }
        for(int level = 1; level < level_count; ++level){
            for(int vertex = 0; vertex < vertex_count; ++vertex){
                int middle = ancestor_[static_cast<std::size_t>(level - 1)]
                    [static_cast<std::size_t>(vertex)];
                ancestor_[static_cast<std::size_t>(level)]
                    [static_cast<std::size_t>(vertex)] =
                    ancestor_[static_cast<std::size_t>(level - 1)]
                        [static_cast<std::size_t>(middle)];
                minimum_[static_cast<std::size_t>(level)]
                    [static_cast<std::size_t>(vertex)] = std::min(
                        minimum_[static_cast<std::size_t>(level - 1)]
                            [static_cast<std::size_t>(vertex)],
                        minimum_[static_cast<std::size_t>(level - 1)]
                            [static_cast<std::size_t>(middle)]
                    );
            }
        }
    }

    [[nodiscard]] int size() const{
        return static_cast<int>(parent.size());
    }

    [[nodiscard]] std::vector<GomoryHuEdge> edges() const{
        std::vector<GomoryHuEdge> result;
        result.reserve(parent.empty() ? 0 : parent.size() - 1);
        for(int vertex = 1; vertex < size(); ++vertex){
            result.push_back({
                vertex,
                parent[static_cast<std::size_t>(vertex)],
                cut_value[static_cast<std::size_t>(vertex)]
            });
        }
        return result;
    }

    [[nodiscard]] long long min_cut_value(int left, int right) const{
        const int vertex_count = size();
        if(left < 0 || vertex_count <= left
            || right < 0 || vertex_count <= right)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation "
                "(GomoryHuTree::min_cut_value)."
            );
        }
        if(left == right) return std::numeric_limits<long long>::max();
        long long result = std::numeric_limits<long long>::max();
        if(depth_[static_cast<std::size_t>(left)]
            < depth_[static_cast<std::size_t>(right)]){
            std::swap(left, right);
        }
        int difference = depth_[static_cast<std::size_t>(left)]
            - depth_[static_cast<std::size_t>(right)];
        for(std::size_t level = 0; level < ancestor_.size(); ++level){
            if(((static_cast<unsigned>(difference) >> level) & 1U) != 0){
                result = std::min(
                    result,
                    minimum_[level][static_cast<std::size_t>(left)]
                );
                left = ancestor_[level][static_cast<std::size_t>(left)];
            }
        }
        if(left == right) return result;
        for(std::size_t level = ancestor_.size(); level-- > 0; ){
            if(ancestor_[level][static_cast<std::size_t>(left)]
                == ancestor_[level][static_cast<std::size_t>(right)]){
                continue;
            }
            result = std::min({
                result,
                minimum_[level][static_cast<std::size_t>(left)],
                minimum_[level][static_cast<std::size_t>(right)]
            });
            left = ancestor_[level][static_cast<std::size_t>(left)];
            right = ancestor_[level][static_cast<std::size_t>(right)];
        }
        return std::min({
            result,
            minimum_[0][static_cast<std::size_t>(left)],
            minimum_[0][static_cast<std::size_t>(right)]
        });
    }
};

inline GomoryHuTree gomory_hu_tree(
    int vertex_count,
    const std::vector<GomoryHuEdge>& edges
){
    if(vertex_count < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation (gomory_hu_tree)."
        );
    }
    __int128 total = 0;
    for(const auto& edge: edges){
        if(edge.from < 0 || vertex_count <= edge.from
            || edge.to < 0 || vertex_count <= edge.to
            || edge.capacity < 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (gomory_hu_tree)."
            );
        }
        if(edge.from != edge.to){
            total += static_cast<__int128>(edge.capacity);
        }
    }
    if(total > std::numeric_limits<long long>::max())[[unlikely]]{
        throw std::overflow_error(
            "gomory_hu_tree capacity sum overflows long long"
        );
    }
    std::vector<int> parent(static_cast<std::size_t>(vertex_count), 0);
    std::vector<long long> cut_value(
        static_cast<std::size_t>(vertex_count), 0
    );
    for(int source = 1; source < vertex_count; ++source){
        int sink = parent[static_cast<std::size_t>(source)];
        Dinic<long long> flow(vertex_count);
        for(const auto& edge: edges){
            if(edge.from == edge.to) continue;
            flow.add_edge(edge.from, edge.to, edge.capacity);
            flow.add_edge(edge.to, edge.from, edge.capacity);
        }
        long long value = flow.max_flow(
            source, sink, static_cast<long long>(total)
        );
        std::vector<char> side = flow.min_cut(source);
        for(int vertex = source + 1; vertex < vertex_count; ++vertex){
            if(parent[static_cast<std::size_t>(vertex)] == sink
                && side[static_cast<std::size_t>(vertex)] != 0){
                parent[static_cast<std::size_t>(vertex)] = source;
            }
        }
        int sink_parent = parent[static_cast<std::size_t>(sink)];
        if(sink != 0 && side[static_cast<std::size_t>(sink_parent)] != 0){
            parent[static_cast<std::size_t>(source)] = sink_parent;
            parent[static_cast<std::size_t>(sink)] = source;
            cut_value[static_cast<std::size_t>(source)] =
                cut_value[static_cast<std::size_t>(sink)];
            cut_value[static_cast<std::size_t>(sink)] = value;
        }else{
            cut_value[static_cast<std::size_t>(source)] = value;
        }
    }
    if(vertex_count != 0) parent[0] = -1;
    return GomoryHuTree(std::move(parent), std::move(cut_value));
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_FLOW_CUT_GOMORY_HU_TREE_HPP_INCLUDED
