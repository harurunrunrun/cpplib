#ifndef CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_PROFILE_GRID_PLUG_DP_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_PROFILE_GRID_PLUG_DP_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <functional>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

namespace dynamic_programming {

class PlugDpState {
    std::vector<unsigned int> plugs_;

public:
    PlugDpState() = default;

    explicit PlugDpState(std::size_t frontier_size)
        : plugs_(frontier_size) {}

    explicit PlugDpState(std::vector<unsigned int> plugs)
        : plugs_(std::move(plugs)) {
        canonicalize();
    }

    [[nodiscard]] std::size_t size() const noexcept {
        return plugs_.size();
    }

    [[nodiscard]] unsigned int operator[](std::size_t index) const {
        return plugs_.at(index);
    }

    void set(std::size_t index, unsigned int label) {
        plugs_.at(index) = label;
    }

    void erase(std::size_t index) {
        plugs_.at(index) = 0;
    }

    void connect(unsigned int first, unsigned int second) {
        if(first == 0 || second == 0){
            throw std::invalid_argument("plug label zero is empty");
        }
        if(first == second) return;
        for(unsigned int& label : plugs_){
            if(label == second) label = first;
        }
        canonicalize();
    }

    void canonicalize() {
        std::vector<unsigned int> replacement(plugs_.size() + 1);
        unsigned int next = 1;
        for(unsigned int& label : plugs_){
            if(label == 0) continue;
            if(label >= replacement.size()){
                replacement.resize(
                    static_cast<std::size_t>(label) + 1
                );
            }
            if(replacement[label] == 0) replacement[label] = next++;
            label = replacement[label];
        }
    }

    [[nodiscard]] const std::vector<unsigned int>& plugs() const noexcept {
        return plugs_;
    }

    bool operator==(const PlugDpState&) const = default;
};

struct PlugDpStateHash {
    std::size_t operator()(const PlugDpState& state) const noexcept {
        std::size_t result = 0xcbf29ce484222325ULL;
        for(const unsigned int label : state.plugs()){
            result ^= static_cast<std::size_t>(label)
                + 0x9e3779b97f4a7c15ULL;
            result *= 0x100000001b3ULL;
        }
        return result;
    }
};

template<class Count, class Transition, class Accept>
Count plug_dp_grid(
    std::size_t row_count,
    std::size_t column_count,
    PlugDpState initial_state,
    Transition transition,
    Accept accept
) {
    using Map = std::unordered_map<PlugDpState, Count, PlugDpStateHash>;
    Map current;
    initial_state.canonicalize();
    current.emplace(std::move(initial_state), Count{1});
    for(std::size_t row = 0; row < row_count; ++row){
        for(std::size_t column = 0; column < column_count; ++column){
            Map next;
            for(const auto& [state, ways] : current){
                const auto emit = [&](PlugDpState output, const Count& multiplier){
                    output.canonicalize();
                    next[std::move(output)] += ways * multiplier;
                };
                transition(row, column, state, emit);
            }
            current = std::move(next);
        }
    }
    Count result{};
    for(const auto& [state, ways] : current){
        if(static_cast<bool>(std::invoke(accept, state))) result += ways;
    }
    return result;
}

}  // namespace dynamic_programming

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_PROFILE_GRID_PLUG_DP_HPP_INCLUDED
