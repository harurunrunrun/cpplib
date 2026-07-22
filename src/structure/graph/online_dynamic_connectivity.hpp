#ifndef CPPLIB_SRC_STRUCTURE_GRAPH_ONLINE_DYNAMIC_CONNECTIVITY_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_GRAPH_ONLINE_DYNAMIC_CONNECTIVITY_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <limits>
#include <map>
#include <memory>
#include <stdexcept>
#include <utility>
#include "online_dynamic_connectivity_internal.hpp"

template<int MAX_SIZE>
struct OnlineDynamicConnectivity{
    static_assert(MAX_SIZE > 0);

private:
    using Forest = online_dynamic_connectivity_internal::EulerTourForest<MAX_SIZE>;

    static constexpr int level_count(){
        int result = 1;
        std::size_t bound = 1;
        while(bound < static_cast<std::size_t>(MAX_SIZE)){
            bound *= 2;
            ++result;
        }
        return result;
    }

    static constexpr int LEVEL_COUNT = level_count();

    struct EdgeInfo{
        int multiplicity = 0;
        int level = 0;
        bool tree = false;
    };

    struct State{
        std::array<std::unique_ptr<Forest>, LEVEL_COUNT> forest;
        std::map<std::pair<int, int>, EdgeInfo> edge;
        int group_count = 0;
    };

    int _n = 0;
    mutable std::unique_ptr<State> state;

    void check_vertex(int vertex, const char* message) const{
        if(vertex < 0 || _n <= vertex)[[unlikely]] throw std::runtime_error(message);
    }

    std::pair<int, int> normalize(int left, int right) const{
        check_vertex(left, "library assertion fault: range violation (edge).");
        check_vertex(right, "library assertion fault: range violation (edge).");
        if(right < left) std::swap(left, right);
        return {left, right};
    }

    Forest& ensure_level(int level){
        if(level < 0 || LEVEL_COUNT <= level)[[unlikely]]{
            throw std::runtime_error("library assertion fault: HDLT level capacity exceeded.");
        }
        if(state->forest[static_cast<std::size_t>(level)] == nullptr){
            state->forest[static_cast<std::size_t>(level)] = std::make_unique<Forest>(_n);
        }
        return *state->forest[static_cast<std::size_t>(level)];
    }

    bool replace_after_cut(int left, int right, int start_level){
        for(int level = start_level; level >= 0; --level){
            Forest& current = *state->forest[static_cast<std::size_t>(level)];
            int smaller = current.component_size(left) <= current.component_size(right) ?
                left : right;

            while(true){
                auto edge = current.pop_exact_tree(smaller);
                if(edge.first == -1) break;
                if(level + 1 >= LEVEL_COUNT)[[unlikely]]{
                    throw std::runtime_error("library assertion fault: HDLT tree level overflow.");
                }
                auto iterator = state->edge.find(edge);
                if(iterator == state->edge.end() || !iterator->second.tree)[[unlikely]]{
                    throw std::runtime_error("library assertion fault: missing promoted tree edge.");
                }
                iterator->second.level = level + 1;
                ensure_level(level + 1).link(edge.first, edge.second, true);
            }

            while(true){
                auto edge = current.any_non_tree(smaller);
                if(edge.first == -1) break;
                auto iterator = state->edge.find(edge);
                if(iterator == state->edge.end() || iterator->second.tree ||
                   iterator->second.level != level)[[unlikely]]{
                    throw std::runtime_error("library assertion fault: invalid non-tree edge level.");
                }
                if(!current.same(edge.first, edge.second)){
                    current.erase_non_tree(edge.first, edge.second);
                    iterator->second.tree = true;
                    iterator->second.level = level;
                    current.link(edge.first, edge.second, true);
                    for(int lower = 0; lower < level; ++lower){
                        state->forest[static_cast<std::size_t>(lower)]->link(
                            edge.first,
                            edge.second,
                            false
                        );
                    }
                    return true;
                }

                if(level + 1 >= LEVEL_COUNT)[[unlikely]]{
                    throw std::runtime_error("library assertion fault: HDLT non-tree level overflow.");
                }
                current.erase_non_tree(edge.first, edge.second);
                iterator->second.level = level + 1;
                ensure_level(level + 1).add_non_tree(edge.first, edge.second);
            }
        }
        return false;
    }

public:
    explicit OnlineDynamicConnectivity(int n = MAX_SIZE): _n(n), state(nullptr){
        if(n < 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        state = std::make_unique<State>();
        state->group_count = _n;
        state->forest[0] = std::make_unique<Forest>(_n);
    }

    OnlineDynamicConnectivity(const OnlineDynamicConnectivity&) = delete;
    OnlineDynamicConnectivity& operator=(const OnlineDynamicConnectivity&) = delete;
    OnlineDynamicConnectivity(OnlineDynamicConnectivity&&) = default;
    OnlineDynamicConnectivity& operator=(OnlineDynamicConnectivity&&) = default;

    int size() const{ return _n; }
    int groups() const{ return state->group_count; }
    int active_levels() const{
        int result = 0;
        while(result < LEVEL_COUNT &&
              state->forest[static_cast<std::size_t>(result)] != nullptr){
            ++result;
        }
        return result;
    }

    bool same(int left, int right) const{
        check_vertex(left, "library assertion fault: range violation (same).");
        check_vertex(right, "library assertion fault: range violation (same).");
        return state->forest[0]->same(left, right);
    }

    int component_size(int vertex) const{
        check_vertex(vertex, "library assertion fault: range violation (component_size).");
        return state->forest[0]->component_size(vertex);
    }

    int edge_multiplicity(int left, int right) const{
        auto edge = normalize(left, right);
        auto iterator = state->edge.find(edge);
        return iterator == state->edge.end() ? 0 : iterator->second.multiplicity;
    }

    bool add_edge(int left, int right){
        auto edge = normalize(left, right);
        auto iterator = state->edge.find(edge);
        if(iterator != state->edge.end()){
            if(iterator->second.multiplicity == std::numeric_limits<int>::max())[[unlikely]]{
                throw std::runtime_error("library assertion fault: edge multiplicity overflow.");
            }
            ++iterator->second.multiplicity;
            return false;
        }

        EdgeInfo info;
        info.multiplicity = 1;
        auto [inserted, success] = state->edge.emplace(edge, info);
        (void)success;
        if(edge.first == edge.second) return true;

        Forest& base = *state->forest[0];
        if(base.same(edge.first, edge.second)){
            base.add_non_tree(edge.first, edge.second);
        }else{
            inserted->second.tree = true;
            base.link(edge.first, edge.second, true);
            --state->group_count;
        }
        return true;
    }

    bool erase_edge(int left, int right){
        auto edge = normalize(left, right);
        auto iterator = state->edge.find(edge);
        if(iterator == state->edge.end()) return false;
        --iterator->second.multiplicity;
        if(iterator->second.multiplicity > 0) return true;
        if(edge.first == edge.second){
            state->edge.erase(iterator);
            return true;
        }

        int level = iterator->second.level;
        if(!iterator->second.tree){
            state->forest[static_cast<std::size_t>(level)]->erase_non_tree(
                edge.first,
                edge.second
            );
            state->edge.erase(iterator);
            return true;
        }

        for(int current = 0; current <= level; ++current){
            state->forest[static_cast<std::size_t>(current)]->cut(edge.first, edge.second);
        }
        state->edge.erase(iterator);
        if(!replace_after_cut(edge.first, edge.second, level)) ++state->group_count;
        return true;
    }

    bool link(int left, int right){ return add_edge(left, right); }
    bool cut(int left, int right){ return erase_edge(left, right); }
};

#endif  // CPPLIB_SRC_STRUCTURE_GRAPH_ONLINE_DYNAMIC_CONNECTIVITY_HPP_INCLUDED
