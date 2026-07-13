#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <memory>
#include <optional>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

#include "centroid_distance_index.hpp"

template<class Distance, int MAX_SIZE>
struct DynamicMarkedTreeDiameter{
    static_assert(MAX_SIZE > 0);
    using Index = CentroidDistanceIndex<Distance, MAX_SIZE>;

private:
    struct State{
        std::array<unsigned char, MAX_SIZE> marked{};
        std::vector<std::multiset<std::pair<Distance, int>>> branch_values;
        std::vector<std::set<std::pair<Distance, int>>> centroid_maxima;
        std::set<std::pair<Distance, int>> centroid_candidates;
    };

    Index distance_index;
    std::unique_ptr<State> state;
    int active_count = 0;
    bool built = false;

    void check_vertex(int vertex, const char* message) const{
        if(vertex < 0 || size() <= vertex)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    void require_built(const char* message) const{
        if(!built)[[unlikely]] throw std::runtime_error(message);
    }

    std::optional<Distance> centroid_candidate(int centroid) const{
        const auto& maxima = state->centroid_maxima[static_cast<std::size_t>(centroid)];
        if(maxima.size() < 2) return std::nullopt;
        auto iterator = maxima.end();
        const Distance first = (--iterator)->first;
        const Distance second = (--iterator)->first;
        return centroid_distance_index_internal::checked_add(first, second);
    }

    void erase_candidate(int centroid){
        const auto candidate = centroid_candidate(centroid);
        if(candidate){
            const auto erased = state->centroid_candidates.erase({*candidate, centroid});
            if(erased != 1)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: invalid diameter state."
                );
            }
        }
    }

    void insert_candidate(int centroid){
        const auto candidate = centroid_candidate(centroid);
        if(candidate) state->centroid_candidates.insert({*candidate, centroid});
    }

    void insert_entry(int vertex, const typename Index::Entry& entry){
        const std::size_t branch = static_cast<std::size_t>(entry.branch);
        const std::size_t centroid = static_cast<std::size_t>(entry.centroid);
        auto& values = state->branch_values[branch];
        auto& maxima = state->centroid_maxima[centroid];
        erase_candidate(entry.centroid);
        if(!values.empty()) maxima.erase({values.rbegin()->first, entry.branch});
        values.insert({entry.distance, vertex});
        maxima.insert({values.rbegin()->first, entry.branch});
        insert_candidate(entry.centroid);
    }

    void erase_entry(int vertex, const typename Index::Entry& entry){
        const std::size_t branch = static_cast<std::size_t>(entry.branch);
        const std::size_t centroid = static_cast<std::size_t>(entry.centroid);
        auto& values = state->branch_values[branch];
        auto& maxima = state->centroid_maxima[centroid];
        erase_candidate(entry.centroid);
        if(values.empty())[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: invalid diameter state."
            );
        }
        maxima.erase({values.rbegin()->first, entry.branch});
        const auto iterator = values.find({entry.distance, vertex});
        if(iterator == values.end())[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: invalid diameter state."
            );
        }
        values.erase(iterator);
        if(!values.empty()) maxima.insert({values.rbegin()->first, entry.branch});
        insert_candidate(entry.centroid);
    }

public:
    explicit DynamicMarkedTreeDiameter(int n = MAX_SIZE):
        distance_index(n), state(std::make_unique<State>()){}

    DynamicMarkedTreeDiameter(const DynamicMarkedTreeDiameter&) = delete;
    DynamicMarkedTreeDiameter& operator=(const DynamicMarkedTreeDiameter&) = delete;
    DynamicMarkedTreeDiameter(DynamicMarkedTreeDiameter&&) = default;
    DynamicMarkedTreeDiameter& operator=(DynamicMarkedTreeDiameter&&) = default;

    int size() const{ return distance_index.size(); }

    void add_edge(int left, int right, Distance weight){
        if(built)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: edge added after build (add_edge)."
            );
        }
        distance_index.add_edge(left, right, weight);
    }

    void build(bool initially_marked = false){
        built = false;
        distance_index.build();
        state->marked.fill(0);
        state->branch_values.clear();
        state->branch_values.resize(
            static_cast<std::size_t>(distance_index.branch_count())
        );
        state->centroid_maxima.clear();
        state->centroid_maxima.resize(static_cast<std::size_t>(size()));
        state->centroid_candidates.clear();
        active_count = 0;

        if(initially_marked){
            for(int vertex = 0; vertex < size(); ++vertex){
                state->marked[static_cast<std::size_t>(vertex)] = 1;
                for(const auto& entry: distance_index.entries(vertex)){
                    state->branch_values[static_cast<std::size_t>(entry.branch)].insert({
                        entry.distance, vertex
                    });
                }
            }
            active_count = size();
            for(int branch = 0; branch < distance_index.branch_count(); ++branch){
                const auto& values =
                    state->branch_values[static_cast<std::size_t>(branch)];
                if(values.empty()) continue;
                const int centroid = distance_index.branch_centroid(branch);
                state->centroid_maxima[static_cast<std::size_t>(centroid)].insert({
                    values.rbegin()->first, branch
                });
            }
            for(int centroid = 0; centroid < size(); ++centroid){
                insert_candidate(centroid);
            }
        }
        built = true;
    }

    const Index& index() const{
        require_built("library assertion fault: tree is not built (index).");
        return distance_index;
    }

    bool is_marked(int vertex) const{
        require_built("library assertion fault: tree is not built (is_marked).");
        check_vertex(vertex, "library assertion fault: range violation (is_marked).");
        return state->marked[static_cast<std::size_t>(vertex)] != 0;
    }

    int marked_count() const{
        require_built("library assertion fault: tree is not built (marked_count).");
        return active_count;
    }

    bool mark(int vertex){
        require_built("library assertion fault: tree is not built (mark).");
        check_vertex(vertex, "library assertion fault: range violation (mark).");
        if(state->marked[static_cast<std::size_t>(vertex)] != 0) return false;
        for(const auto& entry: distance_index.entries(vertex)){
            insert_entry(vertex, entry);
        }
        state->marked[static_cast<std::size_t>(vertex)] = 1;
        ++active_count;
        return true;
    }

    bool unmark(int vertex){
        require_built("library assertion fault: tree is not built (unmark).");
        check_vertex(vertex, "library assertion fault: range violation (unmark).");
        if(state->marked[static_cast<std::size_t>(vertex)] == 0) return false;
        for(const auto& entry: distance_index.entries(vertex)){
            erase_entry(vertex, entry);
        }
        state->marked[static_cast<std::size_t>(vertex)] = 0;
        --active_count;
        return true;
    }

    bool toggle(int vertex){
        require_built("library assertion fault: tree is not built (toggle).");
        check_vertex(vertex, "library assertion fault: range violation (toggle).");
        if(state->marked[static_cast<std::size_t>(vertex)] != 0){
            unmark(vertex);
            return false;
        }
        mark(vertex);
        return true;
    }

    std::optional<Distance> diameter() const{
        require_built("library assertion fault: tree is not built (diameter).");
        if(active_count == 0) return std::nullopt;
        Distance result{};
        if(!state->centroid_candidates.empty()){
            result = std::max(result, state->centroid_candidates.rbegin()->first);
        }
        return result;
    }
};
