#pragma once

#include <array>
#include <cstddef>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include "../tree/link_cut_tree.hpp"

namespace incremental_component_affine_internal{

template<auto MonoidActLen>
struct ActionPathMonoid{
    using S = typename std::remove_cvref_t<decltype(MonoidActLen)>::T;

    constexpr S op(S older, S newer) const{
        return MonoidActLen.composition(newer, older);
    }

    constexpr S e() const{
        return MonoidActLen.id();
    }
};

} // namespace incremental_component_affine_internal

template<auto MonoidActLen, int MAX_SIZE>
struct IncrementalComponentAffine{
    static_assert(MAX_SIZE > 0);
    static_assert(MAX_SIZE <= 1'000'000'000);

    using act_type = std::remove_cvref_t<decltype(MonoidActLen)>;
    using S = typename act_type::S;
    using T = typename act_type::T;

private:
    static constexpr auto action_path_monoid =
        incremental_component_affine_internal::
            ActionPathMonoid<MonoidActLen>{};
    using HistoryTree = LinkCutTree<action_path_monoid, 2 * MAX_SIZE>;

    int _n = 0;
    int group_count = 0;
    int next_history_node = 0;
    std::array<int, MAX_SIZE> parent_or_size{};
    std::array<int, MAX_SIZE> history_root{};
    std::array<S, MAX_SIZE> initial_value{};
    std::array<S, MAX_SIZE> aggregate{};
    HistoryTree history;

    static int checked_size(const std::vector<S>& values){
        if(values.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        return static_cast<int>(values.size());
    }

    void check_vertex(int vertex, const char* message) const{
        if(vertex < 0 || _n <= vertex)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    void initialize_empty_values(){
        for(int vertex = 0; vertex < _n; ++vertex){
            parent_or_size[vertex] = -1;
            history_root[vertex] = vertex;
            initial_value[vertex] = MonoidActLen.e();
            aggregate[vertex] = MonoidActLen.e();
        }
    }

public:
    explicit IncrementalComponentAffine(int n = MAX_SIZE):
        _n(n),
        group_count(n),
        next_history_node(n),
        history(2 * MAX_SIZE){
        if(n < 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        initialize_empty_values();
    }

    explicit IncrementalComponentAffine(const std::vector<S>& values):
        _n(checked_size(values)),
        group_count(_n),
        next_history_node(_n),
        history(2 * MAX_SIZE){
        initialize_empty_values();
        for(int vertex = 0; vertex < _n; ++vertex){
            initial_value[vertex] = values[vertex];
            aggregate[vertex] = values[vertex];
        }
    }

    IncrementalComponentAffine(const IncrementalComponentAffine&) = delete;
    IncrementalComponentAffine& operator=(const IncrementalComponentAffine&) = delete;
    IncrementalComponentAffine(IncrementalComponentAffine&&) = default;
    IncrementalComponentAffine& operator=(IncrementalComponentAffine&&) = default;

    int size() const{ return _n; }
    int groups() const{ return group_count; }
    int history_node_count() const{ return next_history_node; }

    int leader(int vertex){
        check_vertex(
            vertex,
            "library assertion fault: range violation (leader)."
        );
        if(parent_or_size[vertex] < 0) return vertex;
        return parent_or_size[vertex] = leader(parent_or_size[vertex]);
    }

    bool same(int first, int second){
        check_vertex(
            first,
            "library assertion fault: range violation (same)."
        );
        check_vertex(
            second,
            "library assertion fault: range violation (same)."
        );
        return leader(first) == leader(second);
    }

    int component_size(int vertex){
        check_vertex(
            vertex,
            "library assertion fault: range violation (component_size)."
        );
        return -parent_or_size[leader(vertex)];
    }

    int size(int vertex){
        return component_size(vertex);
    }

    bool add_edge(int first, int second){
        check_vertex(
            first,
            "library assertion fault: range violation (add_edge)."
        );
        check_vertex(
            second,
            "library assertion fault: range violation (add_edge)."
        );
        first = leader(first);
        second = leader(second);
        if(first == second) return false;

        const int new_history_root = next_history_node++;
        if(!history.link(history_root[first], new_history_root) ||
           !history.link(history_root[second], new_history_root))[[unlikely]]{
            throw std::logic_error(
                "IncrementalComponentAffine: invalid history forest"
            );
        }

        if(-parent_or_size[first] < -parent_or_size[second]){
            std::swap(first, second);
        }
        const int first_size = -parent_or_size[first];
        const int second_size = -parent_or_size[second];
        aggregate[first] = MonoidActLen.op(
            aggregate[first],
            first_size,
            aggregate[second],
            second_size
        );
        parent_or_size[first] += parent_or_size[second];
        parent_or_size[second] = first;
        history_root[first] = new_history_root;
        --group_count;
        return true;
    }

    void apply_component(int vertex, const T& action){
        check_vertex(
            vertex,
            "library assertion fault: range violation (apply_component)."
        );
        const int root = leader(vertex);
        const int action_node = history_root[root];
        const T previous_action = history.get(action_node);
        history.set(
            action_node,
            MonoidActLen.composition(action, previous_action)
        );
        aggregate[root] = MonoidActLen.mapping(
            action,
            aggregate[root],
            -parent_or_size[root]
        );
    }

    S vertex_value(int vertex){
        check_vertex(
            vertex,
            "library assertion fault: range violation (vertex_value)."
        );
        const int root = leader(vertex);
        const T action = history.path_prod(vertex, history_root[root]);
        return MonoidActLen.mapping(action, initial_value[vertex], 1);
    }

    S component_sum(int vertex){
        check_vertex(
            vertex,
            "library assertion fault: range violation (component_sum)."
        );
        return aggregate[leader(vertex)];
    }
};
