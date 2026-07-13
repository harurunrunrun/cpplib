#pragma once

#include <array>
#include <limits>
#include <memory>
#include <optional>
#include <set>
#include <stdexcept>
#include <type_traits>

#include "centroid_distance_index.hpp"

template<class Distance, int MAX_SIZE>
struct DynamicNearestMarkedVertex{
    static_assert(MAX_SIZE > 0);
    static_assert(!std::is_same_v<std::remove_cv_t<Distance>, bool>);

private:
    struct State{
        CentroidDistanceIndex<Distance, MAX_SIZE> index;
        std::array<std::multiset<Distance>, MAX_SIZE> centroid_distances;
        std::array<bool, MAX_SIZE> marked{};
        bool built = false;
        int marked_count = 0;

        explicit State(int n): index(n){}
    };

    int _n;
    std::unique_ptr<State> state;

    void check_vertex(int vertex, const char* message) const{
        if(vertex < 0 || _n <= vertex)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    void check_built(const char* message) const{
        if(!state->built)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    static Distance checked_add(const Distance& left, const Distance& right){
        if constexpr(std::is_integral_v<Distance>){
            constexpr Distance maximum = std::numeric_limits<Distance>::max();
            if constexpr(std::is_unsigned_v<Distance>){
                if(left > maximum - right)[[unlikely]]{
                    throw std::overflow_error(
                        "library assertion fault: distance overflow (nearest_distance)."
                    );
                }
            }else{
                constexpr Distance minimum = std::numeric_limits<Distance>::min();
                if((right > 0 && left > maximum - right)
                    || (right < 0 && left < minimum - right))[[unlikely]]{
                    throw std::overflow_error(
                        "library assertion fault: distance overflow (nearest_distance)."
                    );
                }
            }
        }
        return left + right;
    }

    void set_marked_unchecked(int vertex, bool value){
        if(state->marked[vertex] == value){
            return;
        }
        for(const auto& entry: state->index.entries(vertex)){
            auto& distances = state->centroid_distances[entry.centroid];
            if(value){
                distances.insert(entry.distance);
            }else{
                auto iterator = distances.find(entry.distance);
                if(iterator == distances.end())[[unlikely]]{
                    throw std::runtime_error(
                        "library assertion fault: inconsistent marked set (set_marked)."
                    );
                }
                distances.erase(iterator);
            }
        }
        state->marked[vertex] = value;
        state->marked_count += value ? 1 : -1;
    }

public:
    explicit DynamicNearestMarkedVertex(int n = MAX_SIZE): _n(n), state(nullptr){
        if(n < 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (constructor)."
            );
        }
        state = std::make_unique<State>(n);
    }

    int size() const{
        return _n;
    }

    void add_edge(int left, int right, const Distance& weight){
        if(weight < Distance{})[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: negative edge weight (add_edge)."
            );
        }
        state->index.add_edge(left, right, weight);
    }

    void build(bool initially_marked = false){
        state->index.build();
        for(int vertex = 0; vertex < _n; ++vertex){
            state->centroid_distances[vertex].clear();
            state->marked[vertex] = false;
        }
        state->marked_count = 0;
        state->built = true;
        if(initially_marked){
            for(int vertex = 0; vertex < _n; ++vertex){
                set_marked_unchecked(vertex, true);
            }
        }
    }

    int marked_count() const{
        check_built("library assertion fault: build is required (marked_count).");
        return state->marked_count;
    }

    bool is_marked(int vertex) const{
        check_built("library assertion fault: build is required (is_marked).");
        check_vertex(vertex, "library assertion fault: range violation (is_marked).");
        return state->marked[vertex];
    }

    void set_marked(int vertex, bool value){
        check_built("library assertion fault: build is required (set_marked).");
        check_vertex(vertex, "library assertion fault: range violation (set_marked).");
        set_marked_unchecked(vertex, value);
    }

    void mark(int vertex){
        set_marked(vertex, true);
    }

    void unmark(int vertex){
        set_marked(vertex, false);
    }

    void toggle(int vertex){
        check_built("library assertion fault: build is required (toggle).");
        check_vertex(vertex, "library assertion fault: range violation (toggle).");
        set_marked_unchecked(vertex, !state->marked[vertex]);
    }

    std::optional<Distance> nearest_distance(int vertex) const{
        check_built("library assertion fault: build is required (nearest_distance).");
        check_vertex(
            vertex,
            "library assertion fault: range violation (nearest_distance)."
        );
        if(state->marked_count == 0){
            return std::nullopt;
        }

        std::optional<Distance> answer;
        for(const auto& entry: state->index.entries(vertex)){
            const auto& distances = state->centroid_distances[entry.centroid];
            if(distances.empty()){
                continue;
            }
            Distance candidate = checked_add(entry.distance, *distances.begin());
            if(!answer || candidate < *answer){
                answer = candidate;
            }
        }
        return answer;
    }
};
