#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <numeric>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

namespace approximate::randomized {

struct UndirectedEdge {
    std::size_t first;
    std::size_t second;
};

struct RandomizedMinCutResult {
    std::size_t cut_size = 0;
    std::vector<std::uint8_t> side;
};

namespace detail {

class ContractionDsu {
public:
    explicit ContractionDsu(std::size_t size)
        : parent_(size), size_(size, 1), components_(size) {
        std::iota(parent_.begin(), parent_.end(), std::size_t{0});
    }

    std::size_t leader(std::size_t vertex) {
        std::size_t root = vertex;
        while(parent_[root] != root) root = parent_[root];
        while(parent_[vertex] != vertex){
            const std::size_t next = parent_[vertex];
            parent_[vertex] = root;
            vertex = next;
        }
        return root;
    }

    std::size_t leader(std::size_t vertex) const {
        while(parent_[vertex] != vertex) vertex = parent_[vertex];
        return vertex;
    }

    bool unite(std::size_t first, std::size_t second) {
        first = leader(first);
        second = leader(second);
        if(first == second) return false;
        if(size_[first] < size_[second]) std::swap(first, second);
        parent_[second] = first;
        size_[first] += size_[second];
        --components_;
        return true;
    }

    std::size_t components() const noexcept { return components_; }

private:
    std::vector<std::size_t> parent_;
    std::vector<std::size_t> size_;
    std::size_t components_;
};

inline void validate_graph(std::size_t vertex_count, const std::vector<UndirectedEdge>& edges) {
    for(const auto& edge: edges){
        if(edge.first >= vertex_count || edge.second >= vertex_count){
            throw std::out_of_range("edge endpoint is out of range");
        }
    }
}

inline RandomizedMinCutResult disconnected_cut(
    std::size_t vertex_count,
    const std::vector<UndirectedEdge>& edges
) {
    RandomizedMinCutResult result;
    result.side.assign(vertex_count, std::uint8_t{0});
    if(vertex_count <= 1) return result;
    ContractionDsu connected(vertex_count);
    for(const auto& edge: edges){
        if(edge.first != edge.second) connected.unite(edge.first, edge.second);
    }
    if(connected.components() == 1){
        result.cut_size = std::numeric_limits<std::size_t>::max();
        return result;
    }
    const std::size_t first_component = connected.leader(0);
    for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
        result.side[vertex] = connected.leader(vertex) == first_component
            ? std::uint8_t{0} : std::uint8_t{1};
    }
    return result;
}

inline RandomizedMinCutResult cut_from_dsu(
    std::size_t vertex_count,
    const std::vector<UndirectedEdge>& edges,
    const ContractionDsu& dsu
) {
    RandomizedMinCutResult result;
    result.side.resize(vertex_count);
    const std::size_t first_component = dsu.leader(0);
    for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
        result.side[vertex] = dsu.leader(vertex) == first_component
            ? std::uint8_t{0} : std::uint8_t{1};
    }
    for(const auto& edge: edges){
        if(result.side[edge.first] != result.side[edge.second]) ++result.cut_size;
    }
    return result;
}

template<class RandomEngine>
bool contract_to(
    ContractionDsu& dsu,
    std::size_t target_components,
    const std::vector<UndirectedEdge>& edges,
    RandomEngine& random
) {
    std::vector<std::size_t> active;
    active.reserve(edges.size());
    while(dsu.components() > target_components){
        active.clear();
        for(std::size_t index = 0; index < edges.size(); ++index){
            const auto& edge = edges[index];
            if(dsu.leader(edge.first) != dsu.leader(edge.second)) active.push_back(index);
        }
        if(active.empty()) return false;
        std::uniform_int_distribution<std::size_t> distribution(0, active.size() - 1);
        const auto& selected = edges[active[distribution(random)]];
        dsu.unite(selected.first, selected.second);
    }
    return true;
}

inline RandomizedMinCutResult exact_contracted_cut(
    std::size_t vertex_count,
    const std::vector<UndirectedEdge>& edges,
    const ContractionDsu& dsu
) {
    std::vector<std::size_t> roots;
    roots.reserve(dsu.components());
    std::vector<std::size_t> component(vertex_count);
    for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
        const std::size_t root = dsu.leader(vertex);
        auto iterator = std::find(roots.begin(), roots.end(), root);
        if(iterator == roots.end()){
            roots.push_back(root);
            component[vertex] = roots.size() - 1;
        }else{
            component[vertex] = static_cast<std::size_t>(iterator - roots.begin());
        }
    }
    const std::size_t component_count = roots.size();
    if(component_count <= 1) return RandomizedMinCutResult{0, std::vector<std::uint8_t>(vertex_count)};

    RandomizedMinCutResult best;
    best.cut_size = std::numeric_limits<std::size_t>::max();
    const std::uint64_t subset_count = std::uint64_t{1} << (component_count - 1);
    for(std::uint64_t mask = 1; mask < subset_count; ++mask){
        std::size_t cut = 0;
        for(const auto& edge: edges){
            const std::size_t first = component[edge.first];
            const std::size_t second = component[edge.second];
            const bool first_side = first != 0 && ((mask >> (first - 1)) & std::uint64_t{1}) != 0;
            const bool second_side = second != 0 && ((mask >> (second - 1)) & std::uint64_t{1}) != 0;
            if(first_side != second_side) ++cut;
        }
        if(cut >= best.cut_size) continue;
        best.cut_size = cut;
        best.side.resize(vertex_count);
        for(std::size_t vertex = 0; vertex < vertex_count; ++vertex){
            const std::size_t part = component[vertex];
            best.side[vertex] = part != 0 && ((mask >> (part - 1)) & std::uint64_t{1}) != 0
                ? std::uint8_t{1} : std::uint8_t{0};
        }
    }
    return best;
}

inline bool better_cut(const RandomizedMinCutResult& first, const RandomizedMinCutResult& second) {
    if(first.cut_size != second.cut_size) return first.cut_size < second.cut_size;
    return first.side < second.side;
}

template<class RandomEngine>
RandomizedMinCutResult karger_stein_recursive(
    std::size_t vertex_count,
    const std::vector<UndirectedEdge>& edges,
    const ContractionDsu& state,
    RandomEngine& random
) {
    const std::size_t components = state.components();
    if(components <= 6) return exact_contracted_cut(vertex_count, edges, state);
    const double scaled = std::ceil(static_cast<double>(components) / std::sqrt(2.0));
    const std::size_t target = std::min(components - 1, static_cast<std::size_t>(scaled));
    ContractionDsu first_state = state;
    ContractionDsu second_state = state;
    if(!contract_to(first_state, target, edges, random)
       || !contract_to(second_state, target, edges, random)){
        return exact_contracted_cut(vertex_count, edges, state);
    }
    auto first = karger_stein_recursive(vertex_count, edges, first_state, random);
    auto second = karger_stein_recursive(vertex_count, edges, second_state, random);
    return better_cut(first, second) ? std::move(first) : std::move(second);
}

}  // namespace detail

template<class RandomEngine>
RandomizedMinCutResult karger_min_cut(
    std::size_t vertex_count,
    const std::vector<UndirectedEdge>& edges,
    std::size_t trials,
    RandomEngine& random
) {
    detail::validate_graph(vertex_count, edges);
    auto disconnected = detail::disconnected_cut(vertex_count, edges);
    if(vertex_count <= 1 || disconnected.cut_size == 0) return disconnected;
    if(trials == 0) throw std::invalid_argument("trials must be positive");

    RandomizedMinCutResult best;
    best.cut_size = std::numeric_limits<std::size_t>::max();
    for(std::size_t trial = 0; trial < trials; ++trial){
        detail::ContractionDsu state(vertex_count);
        if(!detail::contract_to(state, 2, edges, random)) return disconnected;
        auto candidate = detail::cut_from_dsu(vertex_count, edges, state);
        if(detail::better_cut(candidate, best)) best = std::move(candidate);
    }
    return best;
}

template<class RandomEngine>
RandomizedMinCutResult karger_stein_min_cut(
    std::size_t vertex_count,
    const std::vector<UndirectedEdge>& edges,
    std::size_t repetitions,
    RandomEngine& random
) {
    detail::validate_graph(vertex_count, edges);
    auto disconnected = detail::disconnected_cut(vertex_count, edges);
    if(vertex_count <= 1 || disconnected.cut_size == 0) return disconnected;
    if(repetitions == 0) throw std::invalid_argument("repetitions must be positive");

    RandomizedMinCutResult best;
    best.cut_size = std::numeric_limits<std::size_t>::max();
    for(std::size_t repetition = 0; repetition < repetitions; ++repetition){
        detail::ContractionDsu state(vertex_count);
        auto candidate = detail::karger_stein_recursive(vertex_count, edges, state, random);
        if(detail::better_cut(candidate, best)) best = std::move(candidate);
    }
    return best;
}

}  // namespace approximate::randomized
