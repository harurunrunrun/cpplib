#ifndef CPPLIB_SRC_APPROXIMATE_SEARCH_ALT_LANDMARK_SEARCH_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_SEARCH_ALT_LANDMARK_SEARCH_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <functional>
#include <iterator>
#include <queue>
#include <stdexcept>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "heuristic_search.hpp"

namespace alt_landmark_search_internal{

template<class State, class Cost, class Neighbors, class Hash, class Equal>
std::unordered_map<State, Cost, Hash, Equal> dijkstra(
    const State& source,
    Neighbors& neighbors,
    Hash hash,
    Equal equal
){
    struct Entry{
        Cost distance;
        std::size_t order;
        State state;
    };
    struct Compare{
        bool operator()(const Entry& lhs, const Entry& rhs) const{
            if(rhs.distance < lhs.distance) return true;
            if(lhs.distance < rhs.distance) return false;
            return rhs.order < lhs.order;
        }
    };

    std::unordered_map<State, Cost, Hash, Equal> distance(
        0, std::move(hash), std::move(equal)
    );
    std::priority_queue<Entry, std::vector<Entry>, Compare> queue;
    std::size_t order = 0;
    distance.emplace(source, Cost{});
    queue.push(Entry{Cost{}, order++, source});
    while(!queue.empty()){
        Entry entry = queue.top();
        queue.pop();
        const auto current = distance.find(entry.state);
        if(current == distance.end() || current->second != entry.distance) continue;

        auto adjacent = std::invoke(neighbors, entry.state);
        for(const auto& edge : adjacent){
            const Cost edge_cost = static_cast<Cost>(edge.second);
            if(edge_cost < Cost{})[[unlikely]]{
                throw std::invalid_argument(
                    "ALT landmark preprocessing does not accept negative edges"
                );
            }
            const Cost candidate = entry.distance + edge_cost;
            const State& next_state = edge.first;
            auto iterator = distance.find(next_state);
            if(iterator != distance.end() && !(candidate < iterator->second)){
                continue;
            }
            if(iterator == distance.end()){
                distance.emplace(next_state, candidate);
            }else{
                iterator->second = candidate;
            }
            queue.push(Entry{candidate, order++, next_state});
        }
    }
    return distance;
}

} // namespace alt_landmark_search_internal

// Directed ALT heuristic.  `from_landmark[i][v]` stores d(L_i, v), while
// `to_landmark[i][v]` stores d(v, L_i).
template<class State, class Cost,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
class AltLandmarkHeuristic{
public:
    using state_type = State;
    using cost_type = Cost;
    using distance_map_type = std::unordered_map<State, Cost, Hash, Equal>;

    AltLandmarkHeuristic(
        std::vector<State> landmarks,
        std::vector<distance_map_type> from_landmark,
        std::vector<distance_map_type> to_landmark,
        Hash hash = {},
        Equal equal = {}
    )
        : landmarks_(std::move(landmarks)),
          from_landmark_(std::move(from_landmark)),
          to_landmark_(std::move(to_landmark)),
          hash_(std::move(hash)),
          equal_(std::move(equal)){
        if(from_landmark_.size() != landmarks_.size()
           || to_landmark_.size() != landmarks_.size()){
            throw std::invalid_argument(
                "AltLandmarkHeuristic requires two distance maps per landmark"
            );
        }
    }

    [[nodiscard]] std::size_t landmark_count() const noexcept{
        return landmarks_.size();
    }

    [[nodiscard]] const std::vector<State>& landmarks() const noexcept{
        return landmarks_;
    }

    [[nodiscard]] const distance_map_type& distances_from(
        std::size_t landmark_index
    ) const{
        return from_landmark_.at(landmark_index);
    }

    [[nodiscard]] const distance_map_type& distances_to(
        std::size_t landmark_index
    ) const{
        return to_landmark_.at(landmark_index);
    }

    [[nodiscard]] Hash hash_function() const{
        return hash_;
    }

    [[nodiscard]] Equal key_eq() const{
        return equal_;
    }

    [[nodiscard]] Cost operator()(
        const State& from,
        const State& target
    ) const{
        Cost lower_bound{};
        for(std::size_t i = 0; i < landmarks_.size(); ++i){
            const auto landmark_to_from = from_landmark_[i].find(from);
            const auto landmark_to_target = from_landmark_[i].find(target);
            if(landmark_to_from != from_landmark_[i].end()
               && landmark_to_target != from_landmark_[i].end()
               && landmark_to_from->second < landmark_to_target->second){
                lower_bound = std::max(
                    lower_bound,
                    landmark_to_target->second - landmark_to_from->second
                );
            }

            const auto from_to_landmark = to_landmark_[i].find(from);
            const auto target_to_landmark = to_landmark_[i].find(target);
            if(from_to_landmark != to_landmark_[i].end()
               && target_to_landmark != to_landmark_[i].end()
               && target_to_landmark->second < from_to_landmark->second){
                lower_bound = std::max(
                    lower_bound,
                    from_to_landmark->second - target_to_landmark->second
                );
            }
        }
        return lower_bound;
    }

private:
    std::vector<State> landmarks_;
    std::vector<distance_map_type> from_landmark_;
    std::vector<distance_map_type> to_landmark_;
    Hash hash_;
    Equal equal_;
};

template<class LandmarkRange, class ForwardNeighbors, class BackwardNeighbors,
         class State = std::decay_t<decltype(
             *std::begin(std::declval<const LandmarkRange&>())
         )>,
         class ForwardRange = std::invoke_result_t<
             ForwardNeighbors&, const State&
         >,
         class Edge = heuristic_search_internal::range_value_t<ForwardRange>,
         class Cost = std::decay_t<decltype(std::declval<Edge>().second)>,
         class Hash = std::hash<State>, class Equal = std::equal_to<State>>
auto make_alt_landmark_heuristic(
    const LandmarkRange& landmarks,
    ForwardNeighbors forward_neighbors,
    BackwardNeighbors backward_neighbors,
    Hash hash = {},
    Equal equal = {}
){
    using Heuristic = AltLandmarkHeuristic<State, Cost, Hash, Equal>;
    using DistanceMap = typename Heuristic::distance_map_type;

    std::vector<State> landmark_vector(
        std::begin(landmarks), std::end(landmarks)
    );
    std::vector<DistanceMap> from_landmark;
    std::vector<DistanceMap> to_landmark;
    from_landmark.reserve(landmark_vector.size());
    to_landmark.reserve(landmark_vector.size());
    for(const State& landmark : landmark_vector){
        from_landmark.push_back(
            alt_landmark_search_internal::dijkstra<State, Cost>(
                landmark, forward_neighbors, hash, equal
            )
        );
        to_landmark.push_back(
            alt_landmark_search_internal::dijkstra<State, Cost>(
                landmark, backward_neighbors, hash, equal
            )
        );
    }
    return Heuristic(
        std::move(landmark_vector),
        std::move(from_landmark),
        std::move(to_landmark),
        std::move(hash),
        std::move(equal)
    );
}

template<class State, class Neighbors, class Cost, class Hash, class Equal>
auto alt_a_star_search(
    const State& start,
    const State& target,
    Neighbors neighbors,
    const AltLandmarkHeuristic<State, Cost, Hash, Equal>& landmark_heuristic
){
    const Equal equal = landmark_heuristic.key_eq();
    return a_star_search(
        start,
        [target, equal](const State& state){
            return static_cast<bool>(equal(state, target));
        },
        std::move(neighbors),
        [&landmark_heuristic, &target](const State& state){
            return landmark_heuristic(state, target);
        },
        landmark_heuristic.hash_function(),
        equal
    );
}

#endif  // CPPLIB_SRC_APPROXIMATE_SEARCH_ALT_LANDMARK_SEARCH_HPP_INCLUDED
