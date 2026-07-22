#ifndef CPPLIB_SRC_STRUCTURE_GRAPH_ONLINE_DYNAMIC_CONNECTIVITY_INTERNAL_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_GRAPH_ONLINE_DYNAMIC_CONNECTIVITY_INTERNAL_HPP_INCLUDED

#include <algorithm>
#include <map>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

namespace online_dynamic_connectivity_internal{

template<int MAX_SIZE>
struct EulerTourForest{
    static_assert(MAX_SIZE > 0);

private:
    struct Node{
        int left = -1;
        int right = -1;
        int parent = -1;
        int from = -1;
        int to = -1;
        int vertex_count = 0;
        bool exact_tree = false;
        bool subtree_exact_tree = false;
        bool own_non_tree = false;
        bool subtree_non_tree = false;
    };

    int _n = 0;
    std::vector<Node> nodes;
    std::vector<int> free_nodes;
    std::map<int, int> vertex_node;
    std::map<std::pair<int, int>, std::pair<int, int>> tree_edge;
    std::map<int, std::set<int>> non_tree_adjacent;

    static constexpr std::size_t NODE_CAPACITY =
        static_cast<std::size_t>(MAX_SIZE) * 3;

    static std::pair<int, int> normalize(int left, int right){
        if(right < left) std::swap(left, right);
        return {left, right};
    }

    int count_of(int node) const{
        return node == -1 ? 0 : nodes[static_cast<std::size_t>(node)].vertex_count;
    }

    bool exact_of(int node) const{
        return node != -1 && nodes[static_cast<std::size_t>(node)].subtree_exact_tree;
    }

    bool non_tree_of(int node) const{
        return node != -1 && nodes[static_cast<std::size_t>(node)].subtree_non_tree;
    }

    void pull(int node){
        Node& current = nodes[static_cast<std::size_t>(node)];
        current.vertex_count = (current.from == current.to ? 1 : 0) +
            count_of(current.left) + count_of(current.right);
        current.subtree_exact_tree = current.exact_tree ||
            exact_of(current.left) || exact_of(current.right);
        current.subtree_non_tree = current.own_non_tree ||
            non_tree_of(current.left) || non_tree_of(current.right);
    }

    void rotate(int node){
        int parent = nodes[static_cast<std::size_t>(node)].parent;
        int grandparent = nodes[static_cast<std::size_t>(parent)].parent;
        bool right_rotation = nodes[static_cast<std::size_t>(parent)].left == node;
        int middle = right_rotation ?
            nodes[static_cast<std::size_t>(node)].right :
            nodes[static_cast<std::size_t>(node)].left;

        if(right_rotation){
            nodes[static_cast<std::size_t>(parent)].left = middle;
            nodes[static_cast<std::size_t>(node)].right = parent;
        }else{
            nodes[static_cast<std::size_t>(parent)].right = middle;
            nodes[static_cast<std::size_t>(node)].left = parent;
        }
        if(middle != -1) nodes[static_cast<std::size_t>(middle)].parent = parent;
        nodes[static_cast<std::size_t>(parent)].parent = node;
        nodes[static_cast<std::size_t>(node)].parent = grandparent;
        if(grandparent != -1){
            if(nodes[static_cast<std::size_t>(grandparent)].left == parent){
                nodes[static_cast<std::size_t>(grandparent)].left = node;
            }else{
                nodes[static_cast<std::size_t>(grandparent)].right = node;
            }
        }
        pull(parent);
        pull(node);
        if(grandparent != -1) pull(grandparent);
    }

    int splay(int node){
        while(nodes[static_cast<std::size_t>(node)].parent != -1){
            int parent = nodes[static_cast<std::size_t>(node)].parent;
            int grandparent = nodes[static_cast<std::size_t>(parent)].parent;
            if(grandparent == -1){
                rotate(node);
            }else{
                bool node_is_left = nodes[static_cast<std::size_t>(parent)].left == node;
                bool parent_is_left = nodes[static_cast<std::size_t>(grandparent)].left == parent;
                if(node_is_left == parent_is_left){
                    rotate(parent);
                    rotate(node);
                }else{
                    rotate(node);
                    rotate(node);
                }
            }
        }
        return node;
    }

    int join(int left, int right){
        if(left == -1){
            if(right != -1) nodes[static_cast<std::size_t>(right)].parent = -1;
            return right;
        }
        if(right == -1){
            nodes[static_cast<std::size_t>(left)].parent = -1;
            return left;
        }
        int last = left;
        while(nodes[static_cast<std::size_t>(last)].right != -1){
            last = nodes[static_cast<std::size_t>(last)].right;
        }
        left = splay(last);
        nodes[static_cast<std::size_t>(left)].right = right;
        nodes[static_cast<std::size_t>(right)].parent = left;
        pull(left);
        return left;
    }

    int reroot(int vertex){
        int root = splay(vertex);
        int prefix = nodes[static_cast<std::size_t>(root)].left;
        if(prefix == -1) return root;
        nodes[static_cast<std::size_t>(prefix)].parent = -1;
        nodes[static_cast<std::size_t>(root)].left = -1;
        pull(root);
        return join(root, prefix);
    }

    int allocate_node(){
        if(!free_nodes.empty()){
            int index = free_nodes.back();
            free_nodes.pop_back();
            return index;
        }
        if(nodes.size() >= NODE_CAPACITY)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: Euler Tour Tree node capacity exceeded."
            );
        }
        int index = static_cast<int>(nodes.size());
        nodes.emplace_back();
        return index;
    }

    int find_vertex_node(int vertex) const{
        auto iterator = vertex_node.find(vertex);
        return iterator == vertex_node.end() ? -1 : iterator->second;
    }

    int materialize_vertex(int vertex){
        auto iterator = vertex_node.find(vertex);
        if(iterator != vertex_node.end()) return iterator->second;

        int index = allocate_node();
        Node& node = nodes[static_cast<std::size_t>(index)];
        node = Node{};
        node.from = vertex;
        node.to = vertex;
        pull(index);
        try{
            vertex_node.emplace(vertex, index);
        }catch(...){
            node = Node{};
            free_nodes.push_back(index);
            throw;
        }
        return index;
    }

    int allocate_edge_node(int from, int to, bool exact){
        int index = allocate_node();
        Node& node = nodes[static_cast<std::size_t>(index)];
        node = Node{};
        node.from = from;
        node.to = to;
        node.exact_tree = exact;
        pull(index);
        return index;
    }

    void release_edge_node(int node){
        nodes[static_cast<std::size_t>(node)] = Node{};
        free_nodes.push_back(node);
    }

    void refresh_non_tree_flag(int vertex){
        int node = materialize_vertex(vertex);
        splay(node);
        auto iterator = non_tree_adjacent.find(vertex);
        nodes[static_cast<std::size_t>(node)].own_non_tree =
            iterator != non_tree_adjacent.end() && !iterator->second.empty();
        pull(node);
    }

public:
    explicit EulerTourForest(int n): _n(n){
        if(n < 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (Euler Tour Tree).");
        }
    }

    EulerTourForest(const EulerTourForest&) = delete;
    EulerTourForest& operator=(const EulerTourForest&) = delete;
    EulerTourForest(EulerTourForest&&) = default;
    EulerTourForest& operator=(EulerTourForest&&) = default;

    bool same(int left, int right){
        if(left == right) return true;
        int left_node = find_vertex_node(left);
        int right_node = find_vertex_node(right);
        if(left_node == -1 || right_node == -1) return false;
        splay(left_node);
        splay(right_node);
        return nodes[static_cast<std::size_t>(left_node)].parent != -1;
    }

    int component_size(int vertex){
        int node = find_vertex_node(vertex);
        if(node == -1) return 1;
        splay(node);
        return nodes[static_cast<std::size_t>(node)].vertex_count;
    }

    void link(int left, int right, bool exact){
        auto edge = normalize(left, right);
        if(tree_edge.find(edge) != tree_edge.end())[[unlikely]]{
            throw std::runtime_error("library assertion fault: duplicate Euler Tour Tree edge.");
        }
        int left_node = materialize_vertex(left);
        int right_node = materialize_vertex(right);
        int forward = allocate_edge_node(left, right, exact && left < right);
        int backward = allocate_edge_node(right, left, exact && right < left);
        tree_edge.emplace(edge, std::pair<int, int>{
            left < right ? forward : backward,
            left < right ? backward : forward
        });
        int left_root = reroot(left_node);
        int right_root = reroot(right_node);
        join(join(left_root, forward), join(right_root, backward));
    }

    void cut(int left, int right){
        auto edge = normalize(left, right);
        auto iterator = tree_edge.find(edge);
        if(iterator == tree_edge.end())[[unlikely]]{
            throw std::runtime_error("library assertion fault: missing Euler Tour Tree edge.");
        }
        int normalized_forward = iterator->second.first;
        int normalized_backward = iterator->second.second;
        int forward = left < right ? normalized_forward : normalized_backward;
        int backward = left < right ? normalized_backward : normalized_forward;
        int left_node = find_vertex_node(left);
        if(left_node == -1)[[unlikely]]{
            throw std::runtime_error("library assertion fault: missing Euler Tour Tree vertex.");
        }

        reroot(left_node);
        splay(forward);
        int first = nodes[static_cast<std::size_t>(forward)].left;
        int remaining = nodes[static_cast<std::size_t>(forward)].right;
        if(first != -1) nodes[static_cast<std::size_t>(first)].parent = -1;
        if(remaining != -1) nodes[static_cast<std::size_t>(remaining)].parent = -1;
        nodes[static_cast<std::size_t>(forward)].left = -1;
        nodes[static_cast<std::size_t>(forward)].right = -1;
        pull(forward);

        splay(backward);
        int middle = nodes[static_cast<std::size_t>(backward)].left;
        int last = nodes[static_cast<std::size_t>(backward)].right;
        if(middle != -1) nodes[static_cast<std::size_t>(middle)].parent = -1;
        if(last != -1) nodes[static_cast<std::size_t>(last)].parent = -1;
        nodes[static_cast<std::size_t>(backward)].left = -1;
        nodes[static_cast<std::size_t>(backward)].right = -1;
        pull(backward);

        join(first, last);
        tree_edge.erase(iterator);
        release_edge_node(forward);
        release_edge_node(backward);
    }

    void add_non_tree(int left, int right){
        non_tree_adjacent[left].insert(right);
        non_tree_adjacent[right].insert(left);
        refresh_non_tree_flag(left);
        refresh_non_tree_flag(right);
    }

    void erase_non_tree(int left, int right){
        auto left_iterator = non_tree_adjacent.find(left);
        auto right_iterator = non_tree_adjacent.find(right);
        if(left_iterator == non_tree_adjacent.end() ||
           right_iterator == non_tree_adjacent.end())[[unlikely]]{
            throw std::runtime_error("library assertion fault: missing non-tree edge.");
        }
        left_iterator->second.erase(right);
        right_iterator->second.erase(left);
        if(left_iterator->second.empty()) non_tree_adjacent.erase(left_iterator);
        if(right_iterator->second.empty()) non_tree_adjacent.erase(right_iterator);
        refresh_non_tree_flag(left);
        refresh_non_tree_flag(right);
    }

    std::pair<int, int> pop_exact_tree(int component_vertex){
        int node = find_vertex_node(component_vertex);
        if(node == -1) return {-1, -1};
        int root = splay(node);
        if(!nodes[static_cast<std::size_t>(root)].subtree_exact_tree) return {-1, -1};
        int current = root;
        while(true){
            int left = nodes[static_cast<std::size_t>(current)].left;
            if(exact_of(left)){
                current = left;
                continue;
            }
            if(nodes[static_cast<std::size_t>(current)].exact_tree) break;
            current = nodes[static_cast<std::size_t>(current)].right;
        }
        splay(current);
        nodes[static_cast<std::size_t>(current)].exact_tree = false;
        pull(current);
        return normalize(
            nodes[static_cast<std::size_t>(current)].from,
            nodes[static_cast<std::size_t>(current)].to
        );
    }

    std::pair<int, int> any_non_tree(int component_vertex){
        int node = find_vertex_node(component_vertex);
        if(node == -1) return {-1, -1};
        int root = splay(node);
        if(!nodes[static_cast<std::size_t>(root)].subtree_non_tree) return {-1, -1};
        int current = root;
        while(true){
            int left = nodes[static_cast<std::size_t>(current)].left;
            if(non_tree_of(left)){
                current = left;
                continue;
            }
            if(nodes[static_cast<std::size_t>(current)].own_non_tree) break;
            current = nodes[static_cast<std::size_t>(current)].right;
        }
        int vertex = nodes[static_cast<std::size_t>(current)].from;
        auto iterator = non_tree_adjacent.find(vertex);
        if(iterator == non_tree_adjacent.end() || iterator->second.empty())[[unlikely]]{
            throw std::runtime_error("library assertion fault: invalid non-tree aggregate.");
        }
        return normalize(vertex, *iterator->second.begin());
    }
};

} // namespace online_dynamic_connectivity_internal

#endif  // CPPLIB_SRC_STRUCTURE_GRAPH_ONLINE_DYNAMIC_CONNECTIVITY_INTERNAL_HPP_INCLUDED
