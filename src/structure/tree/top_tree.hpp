#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <memory>
#include <map>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>
#include "top_tree_internal.hpp"

template<auto Monoid, int MAX_SIZE>
struct TopTree{
    static_assert(MAX_SIZE > 0);
    using S = typename std::remove_cvref_t<decltype(Monoid)>::S;

    struct ClusterStatistics{
        int edge = 0;
        int compress = 0;
        int rake = 0;
        int depth = 0;

        int total() const{ return edge + compress + rake; }
    };

private:
    struct Traits{
        using S = TopTree::S;
        using T = bool;

        static S e(){ return Monoid.e(); }
        static S op(const S& left, int, const S& right, int){
            return Monoid.op(left, right);
        }
        static S mapping(bool, const S& value, int){ return value; }
        static bool composition(bool, bool){ return false; }
        static bool id(){ return false; }
    };

    using Engine = top_tree_internal::SelfAdjustingTopTree<Traits, MAX_SIZE>;
    using Vertex = typename Engine::Vertex;

    struct State{
        Engine engine;
        std::array<Vertex*, MAX_SIZE> vertex{};
        std::array<int, MAX_SIZE> free_edge{};
        int free_edge_count = 0;
        std::map<std::pair<int, int>, int> edge_index;
    };

    int _n = 0;
    std::unique_ptr<State> state;

    void check_vertex(int vertex, const char* message) const{
        if(vertex < 0 || _n <= vertex)[[unlikely]] throw std::runtime_error(message);
    }


    void evert_unchecked(int vertex){
        int old_root = state->engine.represented_root(state->vertex[vertex]);
        if(old_root != vertex){
            state->engine.set_represented_root(state->vertex[old_root], false);
            state->engine.set_represented_root(state->vertex[vertex], true);
        }
    }

    void require_connected(int left, int right, const char* message){
        if(!state->engine.connected(state->vertex[left], state->vertex[right]))[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

public:
    explicit TopTree(int n = MAX_SIZE): _n(n), state(nullptr){
        if(n < 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        state = std::make_unique<State>();
        for(int index = 0; index < _n; ++index){
            state->free_edge[state->free_edge_count++] = index;
        }
        for(int vertex = 0; vertex < _n; ++vertex){
            state->vertex[vertex] = state->engine.create_vertex(Monoid.e(), vertex);
        }
    }

    explicit TopTree(const std::vector<S>& values):
        TopTree(static_cast<int>(values.size())){
        if(values.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        for(int vertex = 0; vertex < _n; ++vertex){
            state->engine.set_vertex(state->vertex[vertex], values[static_cast<std::size_t>(vertex)]);
        }
    }

    TopTree(const TopTree&) = delete;
    TopTree& operator=(const TopTree&) = delete;
    TopTree(TopTree&&) = default;
    TopTree& operator=(TopTree&&) = default;

    int size() const{ return _n; }

    void evert(int vertex){
        check_vertex(vertex, "library assertion fault: range violation (evert).");
        evert_unchecked(vertex);
    }

    int root(int vertex){
        check_vertex(vertex, "library assertion fault: range violation (root).");
        int result = state->engine.represented_root(state->vertex[vertex]);
        if(result == -1)[[unlikely]]{
            throw std::runtime_error("library assertion fault: represented root is missing.");
        }
        return result;
    }

    bool connected(int left, int right){
        check_vertex(left, "library assertion fault: range violation (connected).");
        check_vertex(right, "library assertion fault: range violation (connected).");
        return state->engine.connected(state->vertex[left], state->vertex[right]);
    }

    bool link(int left, int right){
        check_vertex(left, "library assertion fault: range violation (link).");
        check_vertex(right, "library assertion fault: range violation (link).");
        if(state->engine.connected(state->vertex[left], state->vertex[right])) return false;
        if(state->free_edge_count == 0)[[unlikely]]{
            throw std::runtime_error("library assertion fault: top tree edge capacity exceeded.");
        }
        int slot = state->free_edge[--state->free_edge_count];
        int old_root = state->engine.represented_root(state->vertex[left]);
        state->engine.set_represented_root(state->vertex[old_root], false);
        state->engine.link(state->vertex[left], state->vertex[right]);
        state->edge_index.emplace(std::minmax(left, right), slot);
        return true;
    }

    bool cut(int left, int right){
        check_vertex(left, "library assertion fault: range violation (cut).");
        check_vertex(right, "library assertion fault: range violation (cut).");
        auto iterator = state->edge_index.find(std::minmax(left, right));
        if(iterator == state->edge_index.end()) return false;
        int slot = iterator->second;
        int old_root = state->engine.represented_root(state->vertex[left]);
        state->engine.set_represented_root(state->vertex[old_root], false);
        state->engine.cut(state->vertex[left], state->vertex[right]);
        state->edge_index.erase(iterator);
        state->free_edge[state->free_edge_count++] = slot;
        state->engine.set_represented_root(state->vertex[left], true);
        state->engine.set_represented_root(state->vertex[right], true);
        return true;
    }

    void set(int vertex, const S& value){
        check_vertex(vertex, "library assertion fault: range violation (set).");
        state->engine.set_vertex(state->vertex[vertex], value);
    }

    S get(int vertex){
        check_vertex(vertex, "library assertion fault: range violation (get).");
        return state->engine.get_vertex(state->vertex[vertex]);
    }

    S path_prod(int left, int right){
        check_vertex(left, "library assertion fault: range violation (path_prod).");
        check_vertex(right, "library assertion fault: range violation (path_prod).");
        require_connected(left, right, "library assertion fault: disconnected path (path_prod).");
        return state->engine.path_product(state->vertex[left], state->vertex[right]);
    }

    ClusterStatistics cluster_statistics(int vertex){
        check_vertex(vertex, "library assertion fault: range violation (cluster_statistics).");
        auto source = state->engine.statistics(state->vertex[vertex]);
        return {source.edge, source.compress, source.rake, source.depth};
    }
};
