#ifndef CPPLIB_SRC_ALGORITHM_TREE_QUERY_LEVEL_ANCESTOR_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_TREE_QUERY_LEVEL_ANCESTOR_HPP_INCLUDED

#include <algorithm>
#include <bit>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

class LevelAncestor{
    static constexpr std::size_t jump_multiplier_ = 3;
    static constexpr int direct_distance_limit_ = 4;

    int n_ = 0;
    std::vector<int> parent_;
    std::vector<int> depth_;
    std::vector<int> height_;
    std::vector<int> heavy_child_;
    std::vector<int> ladder_id_;
    std::vector<std::size_t> ladder_position_;
    std::vector<std::vector<int>> ladders_;
    std::vector<std::size_t> occurrence_;
    std::vector<int> jump_;

    void validate_and_root(
        const std::vector<std::vector<int>>& graph,
        int root,
        std::vector<int>& order
    ){
        if(n_ == 0){
            if(root != 0){
                throw std::out_of_range(
                    "LevelAncestor: invalid root for an empty tree"
                );
            }
            return;
        }
        if(root < 0 || root >= n_){
            throw std::out_of_range("LevelAncestor: invalid root");
        }

        const std::size_t expected_entries =
            2 * (static_cast<std::size_t>(n_) - 1);
        std::size_t entry_count = 0;
        for(int vertex = 0; vertex < n_; ++vertex){
            for(const int to: graph[static_cast<std::size_t>(vertex)]){
                if(to < 0 || to >= n_){
                    throw std::out_of_range(
                        "LevelAncestor: invalid edge endpoint"
                    );
                }
                if(to == vertex){
                    throw std::invalid_argument(
                        "LevelAncestor: a tree cannot have a self-loop"
                    );
                }
                ++entry_count;
            }
        }
        if(entry_count != expected_entries){
            throw std::invalid_argument(
                "LevelAncestor: the undirected tree must have n-1 edges"
            );
        }

        parent_.assign(static_cast<std::size_t>(n_), -1);
        depth_.assign(static_cast<std::size_t>(n_), 0);
        order.reserve(static_cast<std::size_t>(n_));
        order.push_back(root);
        parent_[static_cast<std::size_t>(root)] = root;
        for(std::size_t order_index = 0;
            order_index < order.size();
            ++order_index){
            const int vertex = order[order_index];
            int parent_occurrences = 0;
            for(const int to: graph[static_cast<std::size_t>(vertex)]){
                if(vertex != root
                    && to == parent_[static_cast<std::size_t>(vertex)]){
                    ++parent_occurrences;
                    if(parent_occurrences > 1){
                        throw std::invalid_argument(
                            "LevelAncestor: duplicate undirected edge"
                        );
                    }
                    continue;
                }
                if(parent_[static_cast<std::size_t>(to)] != -1){
                    throw std::invalid_argument(
                        "LevelAncestor: the graph is not a tree"
                    );
                }
                parent_[static_cast<std::size_t>(to)] = vertex;
                depth_[static_cast<std::size_t>(to)] =
                    depth_[static_cast<std::size_t>(vertex)] + 1;
                order.push_back(to);
            }
            if(vertex != root && parent_occurrences != 1){
                throw std::invalid_argument(
                    "LevelAncestor: every edge must occur in both lists"
                );
            }
        }
        if(static_cast<int>(order.size()) != n_){
            throw std::invalid_argument(
                "LevelAncestor: the graph is disconnected"
            );
        }
    }

    void build_long_paths(const std::vector<int>& order){
        height_.assign(static_cast<std::size_t>(n_), 1);
        heavy_child_.assign(static_cast<std::size_t>(n_), -1);
        for(std::size_t index = order.size(); index-- > 1;){
            const int vertex = order[index];
            const int parent = parent_[static_cast<std::size_t>(vertex)];
            const int candidate_height =
                height_[static_cast<std::size_t>(vertex)] + 1;
            if(height_[static_cast<std::size_t>(parent)]
                < candidate_height){
                height_[static_cast<std::size_t>(parent)] =
                    candidate_height;
                heavy_child_[static_cast<std::size_t>(parent)] = vertex;
            }
        }

        ladder_id_.assign(static_cast<std::size_t>(n_), -1);
        ladder_position_.assign(static_cast<std::size_t>(n_), 0);
        ladders_.clear();
        ladders_.reserve(static_cast<std::size_t>(n_));
        for(const int start: order){
            const int parent = parent_[static_cast<std::size_t>(start)];
            if(start != parent
                && heavy_child_[static_cast<std::size_t>(parent)] == start){
                continue;
            }

            std::vector<int> path;
            for(int vertex = start;
                vertex != -1;
                vertex =
                    heavy_child_[static_cast<std::size_t>(vertex)]){
                path.push_back(vertex);
            }

            std::vector<int> extension;
            extension.reserve(2 * path.size());
            int vertex = start;
            for(std::size_t count = 0;
                count < 2 * path.size()
                    && parent_[static_cast<std::size_t>(vertex)] != vertex;
                ++count){
                vertex = parent_[static_cast<std::size_t>(vertex)];
                extension.push_back(vertex);
            }
            std::reverse(extension.begin(), extension.end());

            const int ladder = static_cast<int>(ladders_.size());
            ladders_.emplace_back();
            std::vector<int>& vertices = ladders_.back();
            vertices.reserve(extension.size() + path.size());
            vertices.insert(
                vertices.end(), extension.begin(), extension.end()
            );
            const std::size_t path_offset = vertices.size();
            vertices.insert(vertices.end(), path.begin(), path.end());
            for(std::size_t index = 0; index < path.size(); ++index){
                const int path_vertex = path[index];
                ladder_id_[static_cast<std::size_t>(path_vertex)] =
                    ladder;
                ladder_position_[static_cast<std::size_t>(path_vertex)] =
                    path_offset + index;
            }
        }
    }

    void build_euler_jumps(
        const std::vector<std::vector<int>>& graph,
        int root
    ){
        occurrence_.assign(static_cast<std::size_t>(n_), 0);
        jump_.clear();
        jump_.reserve(2 * static_cast<std::size_t>(n_));
        jump_.push_back(-1);

        std::vector<int> path{root};
        std::vector<std::pair<int, std::size_t>> stack{{root, 0}};
        const auto append_occurrence = [&](int vertex){
            const std::size_t euler_index = jump_.size();
            if(occurrence_[static_cast<std::size_t>(vertex)] == 0){
                occurrence_[static_cast<std::size_t>(vertex)] =
                    euler_index;
            }
            const std::size_t low_bit =
                euler_index & (~euler_index + 1);
            const std::size_t distance =
                jump_multiplier_ * low_bit;
            if(distance
                <= static_cast<std::size_t>(
                    depth_[static_cast<std::size_t>(vertex)]
                )){
                jump_.push_back(path[
                    static_cast<std::size_t>(
                        depth_[static_cast<std::size_t>(vertex)]
                    ) - distance
                ]);
            }else{
                jump_.push_back(-1);
            }
        };

        append_occurrence(root);
        while(!stack.empty()){
            const int vertex = stack.back().first;
            std::size_t& edge_index = stack.back().second;
            if(edge_index
                == graph[static_cast<std::size_t>(vertex)].size()){
                stack.pop_back();
                path.pop_back();
                if(!stack.empty()){
                    append_occurrence(stack.back().first);
                }
                continue;
            }

            const int to =
                graph[static_cast<std::size_t>(vertex)][edge_index];
            ++edge_index;
            if(parent_[static_cast<std::size_t>(to)] != vertex){
                continue;
            }
            stack.emplace_back(to, 0);
            path.push_back(to);
            append_occurrence(to);
        }
    }

    void check_vertex(int vertex) const{
        if(vertex < 0 || vertex >= n_){
            throw std::out_of_range("LevelAncestor: invalid vertex");
        }
    }

public:
    LevelAncestor() = default;

    explicit LevelAncestor(
        const std::vector<std::vector<int>>& graph,
        int root = 0
    ){
        if(graph.size()
            > static_cast<std::size_t>(std::numeric_limits<int>::max())){
            throw std::length_error(
                "LevelAncestor: too many vertices"
            );
        }
        n_ = static_cast<int>(graph.size());
        std::vector<int> order;
        validate_and_root(graph, root, order);
        if(n_ == 0) return;
        build_long_paths(order);
        build_euler_jumps(graph, root);
    }

    int size() const noexcept{
        return n_;
    }

    int depth(int vertex) const{
        check_vertex(vertex);
        return depth_[static_cast<std::size_t>(vertex)];
    }

    int kth_ancestor(int vertex, int distance) const{
        check_vertex(vertex);
        if(distance < 0
            || distance > depth_[static_cast<std::size_t>(vertex)]){
            return -1;
        }
        if(distance < direct_distance_limit_){
            for(int count = 0; count < distance; ++count){
                vertex = parent_[static_cast<std::size_t>(vertex)];
            }
            return vertex;
        }

        const std::size_t step = std::bit_floor(
            static_cast<std::size_t>(
                distance / direct_distance_limit_
            )
        );
        const std::size_t occurrence =
            occurrence_[static_cast<std::size_t>(vertex)];
        const std::size_t nearby =
            (occurrence & ~(step - 1)) | step;
        const int jumped = jump_[nearby];
        const int target_depth =
            depth_[static_cast<std::size_t>(vertex)] - distance;
        const std::size_t remaining =
            static_cast<std::size_t>(
                depth_[static_cast<std::size_t>(jumped)]
                - target_depth
            );
        const int ladder =
            ladder_id_[static_cast<std::size_t>(jumped)];
        return ladders_[static_cast<std::size_t>(ladder)][
            ladder_position_[static_cast<std::size_t>(jumped)]
            - remaining
        ];
    }

    int ancestor_at_depth(int vertex, int target_depth) const{
        check_vertex(vertex);
        if(target_depth < 0
            || target_depth
                > depth_[static_cast<std::size_t>(vertex)]){
            return -1;
        }
        return kth_ancestor(
            vertex,
            depth_[static_cast<std::size_t>(vertex)] - target_depth
        );
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_TREE_QUERY_LEVEL_ANCESTOR_HPP_INCLUDED
