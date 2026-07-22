#ifndef CPPLIB_SRC_ALGORITHM_TREE_ISOMORPHISM_TREE_ISOMORPHISM_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_TREE_ISOMORPHISM_TREE_ISOMORPHISM_HPP_INCLUDED

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace tree_isomorphism_detail{

inline void validate_tree(const std::vector<std::vector<int>>& graph, const char* message){
    const int n = (int)graph.size();
    if(n == 0){
        return;
    }

    long long edge_twice = 0;
    for(int v = 0; v < n; v++){
        edge_twice += (int)graph[v].size();
        for(int to: graph[v]){
            if(to < 0 || n <= to)[[unlikely]]{
                throw std::runtime_error(message);
            }
        }
    }
    if(edge_twice != 2LL * (n - 1))[[unlikely]]{
        throw std::runtime_error(message);
    }

    std::vector<int> parent(n, -1);
    std::vector<int> stack = {0};
    parent[0] = -2;
    int count = 0;
    while(!stack.empty()){
        int v = stack.back();
        stack.pop_back();
        count++;
        for(int to: graph[v]){
            if(to == parent[v]){
                continue;
            }
            if(parent[to] != -1)[[unlikely]]{
                throw std::runtime_error(message);
            }
            parent[to] = v;
            stack.push_back(to);
        }
    }
    if(count != n)[[unlikely]]{
        throw std::runtime_error(message);
    }
}

struct RootedTreeInfo{
    std::vector<std::vector<int>> children;
    std::vector<int> parent;
    std::vector<int> height;
    std::vector<int> order;
    int max_height = 0;
};

inline RootedTreeInfo build_rooted_tree(
    const std::vector<std::vector<int>>& graph,
    int root,
    const char* message
){
    const int n = (int)graph.size();
    if(root < 0 || n <= root)[[unlikely]]{
        throw std::runtime_error(message);
    }
    validate_tree(graph, message);

    RootedTreeInfo info;
    info.children.assign(n, {});
    info.parent.assign(n, -1);
    info.height.assign(n, 0);
    info.order.reserve(n);

    std::vector<int> stack = {root};
    info.parent[root] = -2;
    while(!stack.empty()){
        int v = stack.back();
        stack.pop_back();
        info.order.push_back(v);
        for(int to: graph[v]){
            if(to == info.parent[v]){
                continue;
            }
            info.parent[to] = v;
            info.children[v].push_back(to);
            stack.push_back(to);
        }
    }
    info.parent[root] = -1;
    for(auto it = info.order.rbegin(); it != info.order.rend(); ++it){
        const int v = *it;
        for(int to: info.children[v]){
            info.height[v] = std::max(info.height[v], info.height[to] + 1);
        }
        info.max_height = std::max(info.max_height, info.height[v]);
    }
    return info;
}

// Compresses all rooted-subtree types in a rooted forest. A block contains
// vertices of one subtree height and is refined by the multiplicity of every
// already-finalized child type. Only vertices touched by the current child
// type are visited, so the total work is linear in the number of forest edges.
inline std::vector<int> rooted_labels_from_forest(
    const std::vector<const RootedTreeInfo*>& forest
){
    int n = 0;
    int max_height = 0;
    for(const RootedTreeInfo* info: forest){
        const std::size_t size = info->children.size();
        if(size > static_cast<std::size_t>(std::numeric_limits<int>::max() - n))[[unlikely]]{
            throw std::length_error(
                "library assertion fault: too many vertices "
                "(rooted tree isomorphism)."
            );
        }
        n += static_cast<int>(size);
        max_height = std::max(max_height, info->max_height);
    }
    if(n == 0){
        return {};
    }

    std::vector<int> parent(n, -1), height(n, 0);
    int offset = 0;
    for(const RootedTreeInfo* info: forest){
        const int size = static_cast<int>(info->children.size());
        for(int v = 0; v < size; ++v){
            height[offset + v] = info->height[v];
            if(info->parent[v] != -1){
                parent[offset + v] = offset + info->parent[v];
            }
        }
        offset += size;
    }

    std::vector<std::vector<int>> by_height(max_height + 1);
    std::vector<int> block(n), block_size(n, 0);
    for(int v = 0; v < n; ++v){
        by_height[height[v]].push_back(v);
        block[v] = height[v];
        ++block_size[block[v]];
    }
    int next_block = max_height + 1;

    std::vector<int> label(n, 0);
    int next_label = 1;

    std::vector<int> member_head(n, -1), member_next(n, -1);
    std::vector<int> touched_head(n, -1), touched_next(n, -1);
    std::vector<int> multiplicity(n, 0), count_next(n, -1);
    std::vector<int> count_head(n + 1, -1);
    std::vector<int> active_classes, touched_parents, active_blocks;

    for(int current_height = 0; current_height <= max_height; ++current_height){
        active_classes.clear();
        for(int v: by_height[current_height]){
            const int b = block[v];
            if(member_head[b] == -1){
                active_classes.push_back(b);
            }
            member_next[v] = member_head[b];
            member_head[b] = v;
        }

        for(int child_class: active_classes){
            touched_parents.clear();
            for(int v = member_head[child_class]; v != -1; v = member_next[v]){
                label[v] = next_label;
                const int p = parent[v];
                if(p == -1){
                    continue;
                }
                if(multiplicity[p] == 0){
                    touched_parents.push_back(p);
                }
                ++multiplicity[p];
            }
            ++next_label;

            active_blocks.clear();
            for(int p: touched_parents){
                const int b = block[p];
                if(touched_head[b] == -1){
                    active_blocks.push_back(b);
                }
                touched_next[p] = touched_head[b];
                touched_head[b] = p;
            }

            for(int b: active_blocks){
                int touched_size = 0;
                int max_count = 0;
                for(int p = touched_head[b]; p != -1; p = touched_next[p]){
                    ++touched_size;
                    max_count = std::max(max_count, multiplicity[p]);
                    count_next[p] = count_head[multiplicity[p]];
                    count_head[multiplicity[p]] = p;
                }

                const bool has_zero_group = block_size[b] != touched_size;
                int retained_count = 0;
                if(has_zero_group){
                    block_size[b] -= touched_size;
                }else{
                    retained_count = 1;
                    while(count_head[retained_count] == -1){
                        ++retained_count;
                    }
                    block_size[b] = 0;
                }

                for(int count = 1; count <= max_count; ++count){
                    if(count_head[count] == -1){
                        continue;
                    }
                    int size = 0;
                    for(int p = count_head[count]; p != -1; p = count_next[p]){
                        ++size;
                    }
                    if(count == retained_count){
                        block_size[b] = size;
                    }else{
                        if(next_block >= n)[[unlikely]]{
                            throw std::logic_error(
                                "library assertion fault: invalid partition "
                                "(rooted tree isomorphism)."
                            );
                        }
                        const int new_block = next_block++;
                        block_size[new_block] = size;
                        for(int p = count_head[count]; p != -1; p = count_next[p]){
                            block[p] = new_block;
                        }
                    }
                    count_head[count] = -1;
                }
                touched_head[b] = -1;
            }

            for(int p: touched_parents){
                multiplicity[p] = 0;
            }
            member_head[child_class] = -1;
        }
    }
    return label;
}

inline std::vector<int> rooted_labels_from_info(const RootedTreeInfo& info){
    return rooted_labels_from_forest({&info});
}

inline std::pair<std::vector<int>, std::vector<int>> rooted_labels_pair(
    const RootedTreeInfo& a,
    const RootedTreeInfo& b
){
    const std::vector<int> labels = rooted_labels_from_forest({&a, &b});
    const int size_a = static_cast<int>(a.children.size());
    return {
        std::vector<int>(labels.begin(), labels.begin() + size_a),
        std::vector<int>(labels.begin() + size_a, labels.end())
    };
}

}

inline std::vector<int> tree_centers(const std::vector<std::vector<int>>& graph){
    const int n = (int)graph.size();
    if(n == 0){
        return {};
    }
    tree_isomorphism_detail::validate_tree(
        graph,
        "library assertion fault: graph is not a tree (tree_centers)."
    );
    if(n <= 2){
        std::vector<int> res(n);
        for(int i = 0; i < n; i++){
            res[i] = i;
        }
        return res;
    }

    std::vector<int> degree(n), leaves;
    leaves.reserve(n);
    for(int v = 0; v < n; v++){
        degree[v] = (int)graph[v].size();
        if(degree[v] <= 1){
            leaves.push_back(v);
        }
    }

    int remain = n;
    while(remain > 2){
        remain -= (int)leaves.size();
        std::vector<int> next;
        for(int v: leaves){
            for(int to: graph[v]){
                if(--degree[to] == 1){
                    next.push_back(to);
                }
            }
        }
        leaves.swap(next);
    }
    std::sort(leaves.begin(), leaves.end());
    return leaves;
}

inline std::vector<int> rooted_tree_isomorphism_labels(
    const std::vector<std::vector<int>>& graph,
    int root = 0
){
    auto info = tree_isomorphism_detail::build_rooted_tree(
        graph,
        root,
        "library assertion fault: graph is not a tree (rooted_tree_isomorphism_labels)."
    );
    return tree_isomorphism_detail::rooted_labels_from_info(info);
}

inline bool rooted_tree_isomorphic(
    const std::vector<std::vector<int>>& a,
    int root_a,
    const std::vector<std::vector<int>>& b,
    int root_b
){
    if(a.size() != b.size()){
        return false;
    }
    if(a.empty()){
        return true;
    }
    auto info_a = tree_isomorphism_detail::build_rooted_tree(
        a,
        root_a,
        "library assertion fault: graph is not a tree (rooted_tree_isomorphic)."
    );
    auto info_b = tree_isomorphism_detail::build_rooted_tree(
        b,
        root_b,
        "library assertion fault: graph is not a tree (rooted_tree_isomorphic)."
    );
    auto [label_a, label_b] = tree_isomorphism_detail::rooted_labels_pair(info_a, info_b);
    return label_a[root_a] == label_b[root_b];
}

inline bool tree_isomorphic(
    const std::vector<std::vector<int>>& a,
    const std::vector<std::vector<int>>& b
){
    if(a.size() != b.size()){
        return false;
    }
    if(a.empty()){
        return true;
    }

    const auto center_a = tree_centers(a);
    const auto center_b = tree_centers(b);
    if(center_a.size() != center_b.size()){
        return false;
    }
    if(center_a.size() == 1){
        return rooted_tree_isomorphic(a, center_a[0], b, center_b[0]);
    }
    return rooted_tree_isomorphic(a, center_a[0], b, center_b[0])
        || rooted_tree_isomorphic(a, center_a[0], b, center_b[1]);
}

#endif  // CPPLIB_SRC_ALGORITHM_TREE_ISOMORPHISM_TREE_ISOMORPHISM_HPP_INCLUDED
