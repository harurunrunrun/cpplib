#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <stdexcept>
#include <utility>

namespace top_tree_internal{

template<class Traits, int MAX_SIZE>
struct SelfAdjustingTopTree{
    static_assert(MAX_SIZE > 0);
    using S = typename Traits::S;
    using T = typename Traits::T;

    enum class Type : unsigned char{ Edge, Compress, Rake };

    struct Node;

    struct Vertex{
        S value = Traits::e();
        Node* handle = nullptr;
        int index = -1;
        bool represented_root = false;

        Vertex() = default;
        explicit Vertex(const S& x, int vertex_index = -1):
            value(x), handle(nullptr), index(vertex_index), represented_root(false){}

        Vertex& operator=(const Vertex& other){
            value = other.value;
            return *this;
        }
    };

    struct Cluster{
        S forward = Traits::e();
        S backward = Traits::e();
        int length = 0;
        int root_marker = -1;

        void toggle(){ std::swap(forward, backward); }
    };

    struct Node{
        std::array<Vertex*, 2> boundary{};
        Cluster data;
        Node* parent = nullptr;
        Node* rake_child = nullptr;
        std::array<Node*, 2> child{};
        T lazy = Traits::id();
        bool reversed = false;
        bool guarded = false;
        bool has_lazy = false;
        Type type = Type::Edge;
    };

    struct Statistics{
        int edge = 0;
        int compress = 0;
        int rake = 0;
        int depth = 0;

        int total() const{ return edge + compress + rake; }
    };

private:
    struct State{
        std::array<Vertex, 2 * MAX_SIZE> vertices;
        std::array<Node, 4 * MAX_SIZE> nodes;
        std::array<Node*, 4 * MAX_SIZE> stack_node{};
        std::array<int, 4 * MAX_SIZE> stack_depth{};
        std::array<bool, 4 * MAX_SIZE> seen{};
    };

    std::unique_ptr<State> state;
    int vertex_used = 0;
    int node_used = 0;
    Node* recycled = nullptr;

    int parent_direction(const Node* node) const{
        Node* parent = node->parent;
        if(parent == nullptr || parent->guarded) return -1;
        if(parent->child[0] == node) return 0;
        if(parent->child[1] == node) return 1;
        return -1;
    }

    int parent_direction_ignoring_guard(const Node* node) const{
        Node* parent = node->parent;
        if(parent == nullptr) return -1;
        if(parent->child[0] == node) return 0;
        if(parent->child[1] == node) return 1;
        return -1;
    }

    static int merge_marker(int left, int right){
        if(left == -1) return right;
        if(right == -1) return left;
        return std::min(left, right);
    }

    static Cluster rake_data(const Cluster& path, const Cluster& branch){
        Cluster result = path;
        result.root_marker = merge_marker(path.root_marker, branch.root_marker);
        return result;
    }

    static Cluster compress_data(
        const Cluster& left,
        const Vertex& middle,
        const Cluster& right
    ){
        Cluster result;
        result.forward = Traits::op(
            left.forward,
            left.length,
            Traits::op(middle.value, 1, right.forward, right.length),
            right.length + 1
        );
        result.backward = Traits::op(
            right.backward,
            right.length,
            Traits::op(middle.value, 1, left.backward, left.length),
            left.length + 1
        );
        result.length = left.length + 1 + right.length;
        result.root_marker = merge_marker(
            left.root_marker,
            merge_marker(
                middle.represented_root ? middle.index : -1,
                right.root_marker
            )
        );
        return result;
    }

    void apply_node(Node* node, const T& action){
        if(node == nullptr || node->data.length == 0) return;
        node->data.forward = Traits::mapping(action, node->data.forward, node->data.length);
        node->data.backward = Traits::mapping(action, node->data.backward, node->data.length);
        if(node->has_lazy) node->lazy = Traits::composition(action, node->lazy);
        else{
            node->lazy = action;
            node->has_lazy = true;
        }
    }

    Node* pull(Node* node){
        if(node->type == Type::Compress){
            Node* left = node->child[0];
            Node* right = node->child[1];
            node->boundary[0] = left->boundary[0];
            node->boundary[1] = right->boundary[1];
            Cluster left_data = left->data;
            if(node->rake_child != nullptr){
                left_data = rake_data(left_data, node->rake_child->data);
            }
            node->data = compress_data(left_data, *right->boundary[0], right->data);
            left->boundary[1]->handle = node;
        }else if(node->type == Type::Rake){
            propagate(node);
            Node* left = node->child[0];
            Node* right = node->child[1];
            node->boundary[0] = left->boundary[0];
            node->boundary[1] = left->boundary[1];
            node->data = rake_data(left->data, right->data);
        }

        if(node->type != Type::Rake){
            if(node->parent == nullptr){
                node->boundary[0]->handle = node;
                node->boundary[1]->handle = node;
            }else if(node->parent->type == Type::Compress){
                if(parent_direction(node) == -1) node->boundary[0]->handle = node;
            }else if(node->parent->type == Type::Rake){
                node->boundary[0]->handle = node;
            }
        }
        return node;
    }

    void toggle(Node* node){
        if(node->type == Type::Edge){
            std::swap(node->boundary[0], node->boundary[1]);
            node->data.toggle();
        }else if(node->type == Type::Compress){
            std::swap(node->boundary[0], node->boundary[1]);
            node->data.toggle();
            node->reversed = !node->reversed;
        }
    }

    void propagate(Node* node){
        if(node->type == Type::Compress && node->reversed){
            std::swap(node->child[0], node->child[1]);
            toggle(node->child[0]);
            toggle(node->child[1]);
            node->reversed = false;
        }
        if(!node->has_lazy) return;
        if(node->type == Type::Compress){
            apply_node(node->child[0], node->lazy);
            Vertex* middle = node->child[1]->boundary[0];
            middle->value = Traits::mapping(node->lazy, middle->value, 1);
            apply_node(node->child[1], node->lazy);
        }else if(node->type == Type::Rake){
            apply_node(node->child[0], node->lazy);
        }
        node->lazy = Traits::id();
        node->has_lazy = false;
    }

    void toggle_and_propagate(Node* node){
        toggle(node);
        propagate(node);
    }

    void push_to(Node* node){
        int count = 0;
        for(Node* current = node; current != nullptr; current = current->parent){
            if(count >= 4 * MAX_SIZE)[[unlikely]]{
                throw std::runtime_error("library assertion fault: invalid top tree parent chain.");
            }
            state->stack_node[count++] = current;
        }
        while(count > 0) propagate(state->stack_node[--count]);
    }

    void rotate(Node* node, Node* parent, int direction){
        Node* grandparent = parent->parent;
        int parent_dir = parent_direction_ignoring_guard(parent);
        propagate(node->child[direction]);
        parent->child[direction ^ 1] = node->child[direction];
        node->child[direction]->parent = parent;
        node->child[direction] = parent;
        parent->parent = node;
        node->parent = grandparent;
        if(parent_dir != -1) grandparent->child[parent_dir] = node;
        else if(grandparent != nullptr && grandparent->type == Type::Compress){
            grandparent->rake_child = node;
        }
        pull(parent);
        pull(node);
        if(grandparent != nullptr && !grandparent->guarded) pull(grandparent);
    }

    void splay(Node* node){
        propagate(node);
        while(parent_direction(node) != -1){
            Node* parent = node->parent;
            if(parent->type != node->type) break;
            int node_dir = parent_direction(node);
            int parent_dir = parent_direction(parent);
            if(parent_dir != -1 && parent->parent != nullptr &&
               parent->parent->type == parent->type){
                Node* grandparent = parent->parent;
                if(grandparent->parent != nullptr) propagate(grandparent->parent);
                propagate(grandparent);
                propagate(parent);
                propagate(node);
                if(parent_dir == node_dir){
                    rotate(parent, grandparent, parent_dir ^ 1);
                    rotate(node, parent, node_dir ^ 1);
                }else{
                    rotate(node, parent, node_dir ^ 1);
                    rotate(node, grandparent, parent_dir ^ 1);
                }
            }else{
                if(parent->parent != nullptr) propagate(parent->parent);
                propagate(parent);
                propagate(node);
                rotate(node, parent, node_dir ^ 1);
            }
        }
    }

    Node* expose(Node* node){
        push_to(node);
        while(true){
            if(node->type == Type::Rake)[[unlikely]]{
                throw std::runtime_error("library assertion fault: invalid top tree expose.");
            }
            if(node->type == Type::Compress) splay(node);
            Node* next = nullptr;
            Node* parent = node->parent;
            if(parent == nullptr) break;
            if(parent->type == Type::Rake){
                propagate(parent);
                splay(parent);
                next = parent->parent;
            }else if(parent->type == Type::Compress){
                propagate(parent);
                if(parent->guarded && parent_direction_ignoring_guard(node) != -1) break;
                next = parent;
            }
            splay(next);
            int direction = parent_direction_ignoring_guard(next);
            if(direction == -1 || next->parent->type == Type::Rake) direction = 0;
            Node* displaced = next->child[direction];
            if(direction == 1){
                toggle_and_propagate(displaced);
                toggle_and_propagate(node);
            }
            int node_dir = parent_direction(node);
            if(node_dir != -1){
                Node* rake_parent = node->parent;
                propagate(displaced);
                propagate(rake_parent);
                rake_parent->child[node_dir] = displaced;
                displaced->parent = rake_parent;
                next->child[direction] = node;
                node->parent = next;
                pull(displaced);
                pull(rake_parent);
                pull(node);
                pull(next);
                splay(rake_parent);
            }else{
                propagate(displaced);
                next->rake_child = displaced;
                displaced->parent = next;
                next->child[direction] = node;
                node->parent = next;
                pull(displaced);
                pull(node);
                pull(next);
            }
            if(node->type == Type::Edge) node = next;
        }
        return node;
    }

    Node* make_edge(Vertex* left, Vertex* right){
        Node* node = allocate_node();
        node->boundary[0] = left;
        node->boundary[1] = right;
        node->type = Type::Edge;
        return pull(node);
    }

    Node* make_compress(Node* left, Node* right){
        Node* node = allocate_node();
        node->child[0] = left;
        node->child[1] = right;
        node->type = Type::Compress;
        return pull(node);
    }

    Node* make_rake(Node* left, Node* right){
        Node* node = allocate_node();
        node->child[0] = left;
        node->child[1] = right;
        node->type = Type::Rake;
        return pull(node);
    }

    Node* allocate_node(){
        Node* result;
        if(recycled != nullptr){
            result = recycled;
            recycled = recycled->parent;
        }else{
            if(node_used >= 4 * MAX_SIZE)[[unlikely]]{
                throw std::runtime_error("library assertion fault: top tree node capacity exceeded.");
            }
            result = &state->nodes[node_used++];
        }
        *result = Node{};
        return result;
    }

    void dispose_node(Node* node){
        *node = Node{};
        node->parent = recycled;
        recycled = node;
    }

    void soft_expose(Vertex* left, Vertex* right){
        push_to(left->handle);
        push_to(right->handle);
        Node* root = expose(left);
        if(left->handle == right->handle){
            if(root->boundary[1] == left || root->boundary[0] == right){
                toggle_and_propagate(root);
            }
            return;
        }
        root->guarded = true;
        Node* exposed = expose(right);
        root->guarded = false;
        pull(root);
        if(parent_direction(exposed) == 0) toggle_and_propagate(root);
    }

    void bring(Node* root){
        Node* rake = root->rake_child;
        if(rake == nullptr){
            Node* left = root->child[0];
            dispose_node(left->parent);
            left->parent = nullptr;
            pull(left);
        }else if(rake->type == Type::Compress || rake->type == Type::Edge){
            Node* next_root = rake;
            toggle_and_propagate(next_root);
            root->child[1] = next_root;
            next_root->parent = root;
            root->rake_child = nullptr;
            pull(next_root);
            pull(root);
        }else{
            propagate(rake);
            while(rake->child[1]->type == Type::Rake){
                propagate(rake->child[1]);
                rake = rake->child[1];
            }
            push_to(rake);
            root->guarded = true;
            splay(rake);
            root->guarded = false;
            Node* left = rake->child[0];
            Node* right = rake->child[1];
            propagate(left);
            toggle_and_propagate(right);
            root->child[1] = right;
            right->parent = root;
            root->rake_child = left;
            left->parent = root;
            dispose_node(rake);
            pull(left);
            pull(right);
            pull(root);
        }
    }

public:
    SelfAdjustingTopTree(): state(std::make_unique<State>()){}

    SelfAdjustingTopTree(const SelfAdjustingTopTree&) = delete;
    SelfAdjustingTopTree& operator=(const SelfAdjustingTopTree&) = delete;
    SelfAdjustingTopTree(SelfAdjustingTopTree&&) = default;
    SelfAdjustingTopTree& operator=(SelfAdjustingTopTree&&) = default;

    Vertex* create_vertex(const S& value, int index){
        if(vertex_used + 2 > 2 * MAX_SIZE)[[unlikely]]{
            throw std::runtime_error("library assertion fault: top tree vertex capacity exceeded.");
        }
        Vertex* vertex = &state->vertices[vertex_used++];
        Vertex* dummy = &state->vertices[vertex_used++];
        vertex->value = value;
        vertex->handle = nullptr;
        vertex->index = index;
        vertex->represented_root = true;
        dummy->value = Traits::e();
        dummy->handle = nullptr;
        dummy->index = -1;
        dummy->represented_root = false;
        Node* edge = make_edge(vertex, dummy);
        vertex->handle = edge;
        dummy->handle = edge;
        return vertex;
    }

    Node* expose(Vertex* vertex){ return expose(vertex->handle); }

    bool connected(Vertex* left, Vertex* right) const{
        Node* a = left->handle;
        Node* b = right->handle;
        while(a->parent != nullptr) a = a->parent;
        while(b->parent != nullptr) b = b->parent;
        return a == b;
    }

    void link(Vertex* left, Vertex* right){
        Node* left_handle = left->handle;
        Node* right_handle = right->handle;
        Node* edge = make_edge(left, right);
        Node* combined = nullptr;

        Node* right_root = expose(right_handle);
        propagate(right_root);
        if(right_root->boundary[1] == right) toggle_and_propagate(right_root);
        if(right_root->boundary[0] == right){
            Node* compressed = make_compress(edge, right_root);
            edge->parent = compressed;
            pull(edge);
            right_root->parent = compressed;
            pull(right_root);
            pull(compressed);
            combined = compressed;
        }else{
            Node* child = right_root->child[0];
            propagate(child);
            right_root->child[0] = edge;
            edge->parent = right_root;
            pull(edge);
            Node* branch = right_root->rake_child;
            Node* rake = child;
            if(branch != nullptr){
                propagate(branch);
                rake = make_rake(branch, child);
                branch->parent = rake;
                child->parent = rake;
                pull(branch);
                pull(child);
            }
            right_root->rake_child = rake;
            rake->parent = right_root;
            pull(rake);
            pull(right_root);
            combined = right_root;
        }

        Node* left_root = expose(left_handle);
        propagate(left_root);
        if(left_root->boundary[0] == left) toggle_and_propagate(left_root);
        if(left_root->boundary[1] == left){
            Node* root = make_compress(left_root, combined);
            left_root->parent = root;
            combined->parent = root;
            pull(left_root);
            pull(combined);
            pull(root);
        }else{
            Node* child = left_root->child[1];
            toggle(child);
            propagate(child);
            left_root->child[1] = combined;
            combined->parent = left_root;
            pull(combined);
            Node* existing = left_root->rake_child;
            Node* rake = child;
            if(existing != nullptr){
                propagate(existing);
                rake = make_rake(existing, child);
                existing->parent = rake;
                child->parent = rake;
                pull(existing);
                pull(child);
            }
            left_root->rake_child = rake;
            rake->parent = left_root;
            pull(rake);
            pull(left_root);
        }
    }

    void cut(Vertex* left, Vertex* right){
        soft_expose(left, right);
        Node* root = left->handle;
        propagate(root);
        Node* right_root = root->child[1];
        right_root->parent = nullptr;
        toggle_and_propagate(right_root);
        dispose_node(right_root->child[1]);
        bring(right_root);
        bring(root);
    }

    Node* path(Vertex* left, Vertex* right){
        soft_expose(left, right);
        Node* root = left->handle;
        propagate(root);
        propagate(root->child[1]);
        return root->child[1]->child[0];
    }

    void set_vertex(Vertex* vertex, const S& value){
        Node* root = expose(vertex);
        vertex->value = value;
        pull(root);
    }

    S get_vertex(Vertex* vertex){
        expose(vertex);
        return vertex->value;
    }

    void set_represented_root(Vertex* vertex, bool value){
        Node* root = expose(vertex);
        vertex->represented_root = value;
        pull(root);
    }

    int represented_root(Vertex* vertex){
        Node* root = expose(vertex);
        int result = root->data.root_marker;
        for(Vertex* boundary: root->boundary){
            if(boundary->represented_root){
                result = merge_marker(result, boundary->index);
            }
        }
        return result;
    }

    S path_product(Vertex* left, Vertex* right){
        if(left == right) return get_vertex(left);
        Node* middle = path(left, right);
        return Traits::op(
            left->value,
            1,
            Traits::op(middle->data.forward, middle->data.length, right->value, 1),
            middle->data.length + 1
        );
    }

    void path_apply(Vertex* left, Vertex* right, const T& action){
        if(left == right){
            set_vertex(left, Traits::mapping(action, get_vertex(left), 1));
            return;
        }
        Node* middle = path(left, right);
        left->value = Traits::mapping(action, left->value, 1);
        right->value = Traits::mapping(action, right->value, 1);
        apply_node(middle, action);
        for(Node* node = middle; node != nullptr; node = node->parent){
            if(node == middle) propagate(node);
            pull(node);
        }
    }

    Statistics statistics(Vertex* vertex){
        Node* root = expose(vertex);
        state->seen.fill(false);
        Statistics result;
        int count = 0;
        state->stack_node[count] = root;
        state->stack_depth[count++] = 1;
        while(count > 0){
            --count;
            Node* node = state->stack_node[count];
            int depth = state->stack_depth[count];
            int index = static_cast<int>(node - state->nodes.data());
            if(index < 0 || node_used <= index || state->seen[index]) continue;
            state->seen[index] = true;
            result.depth = std::max(result.depth, depth);
            if(node->type == Type::Edge) ++result.edge;
            else if(node->type == Type::Compress) ++result.compress;
            else ++result.rake;
            if(node->type != Type::Edge){
                state->stack_node[count] = node->child[0];
                state->stack_depth[count++] = depth + 1;
                state->stack_node[count] = node->child[1];
                state->stack_depth[count++] = depth + 1;
                if(node->rake_child != nullptr){
                    state->stack_node[count] = node->rake_child;
                    state->stack_depth[count++] = depth + 1;
                }
            }
        }
        return result;
    }
};

} // namespace top_tree_internal
