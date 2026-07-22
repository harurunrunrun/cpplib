#ifndef CPPLIB_SRC_ALGORITHM_STRING_SUFFIX_AUTOMATON_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_SUFFIX_AUTOMATON_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cstddef>
#include <limits>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

template<int ALPHABET, int MAX_STATES, char OFFSET = 'a'>
struct SuffixAutomaton{
    static_assert(ALPHABET > 0);
    static_assert(MAX_STATES > 0);

    struct State{
        std::array<int, ALPHABET> next;
        int link = -1;
        int length = 0;
        long long occurrence = 0;

        State(){
            next.fill(-1);
        }
    };

    struct Match{
        int position;
        int length;
    };

private:
    int used = 1;
    int last_state = 0;
    bool occurrence_built = false;
    std::array<State, MAX_STATES> states{};
    std::array<bool, MAX_STATES> terminal_contribution{};

    int char_id(char c) const{
        int id = static_cast<int>(c - OFFSET);
        if(id < 0 || ALPHABET <= id)[[unlikely]]{
            throw std::runtime_error("library assertion fault: character is out of alphabet.");
        }
        return id;
    }
    int new_state(){
        if(used == MAX_STATES)[[unlikely]]{
            throw std::runtime_error("library assertion fault: capacity exceeded (extend).");
        }
        states[used] = State();
        terminal_contribution[used] = false;
        return used++;
    }

public:
    SuffixAutomaton() = default;

    explicit SuffixAutomaton(std::string_view s){
        for(char c: s) extend(c);
    }

    int size() const{ return used; }
    int last() const{ return last_state; }
    const State& operator[](int state) const{
        if(state < 0 || used <= state)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (operator[]).");
        }
        return states[state];
    }

    int extend(char c){
        occurrence_built = false;
        int x = char_id(c);
        int cur = new_state();
        states[cur].length = states[last_state].length + 1;
        states[cur].occurrence = 1;
        terminal_contribution[cur] = true;

        int p = last_state;
        while(p != -1 && states[p].next[x] == -1){
            states[p].next[x] = cur;
            p = states[p].link;
        }
        if(p == -1){
            states[cur].link = 0;
        }else{
            int q = states[p].next[x];
            if(states[p].length + 1 == states[q].length){
                states[cur].link = q;
            }else{
                int clone = new_state();
                states[clone] = states[q];
                states[clone].length = states[p].length + 1;
                states[clone].occurrence = 0;
                while(p != -1 && states[p].next[x] == q){
                    states[p].next[x] = clone;
                    p = states[p].link;
                }
                states[q].link = states[cur].link = clone;
            }
        }
        last_state = cur;
        return cur;
    }

    bool contains(std::string_view s) const{
        int v = 0;
        for(char c: s){
            int id = char_id(c);
            v = states[v].next[id];
            if(v == -1) return false;
        }
        return true;
    }

    long long count_distinct_substrings() const{
        long long result = 0;
        for(int v = 1; v < used; v++){
            result += states[v].length - states[states[v].link].length;
        }
        return result;
    }

    void build_occurrences(){
        if(occurrence_built) return;
        int max_len = 0;
        for(int v = 0; v < used; v++){
            states[v].occurrence = terminal_contribution[v] ? 1 : 0;
            max_len = std::max(max_len, states[v].length);
        }
        std::vector<int> count(static_cast<std::size_t>(max_len) + 1), order(static_cast<std::size_t>(used));
        for(int v = 0; v < used; v++) count[static_cast<std::size_t>(states[v].length)]++;
        for(int i = 1; i <= max_len; i++) count[static_cast<std::size_t>(i)] += count[static_cast<std::size_t>(i - 1)];
        for(int v = used - 1; v >= 0; v--){
            order[static_cast<std::size_t>(--count[static_cast<std::size_t>(states[v].length)])] = v;
        }
        for(int i = used - 1; i > 0; i--){
            int v = order[static_cast<std::size_t>(i)];
            if(states[v].link != -1) states[states[v].link].occurrence += states[v].occurrence;
        }
        occurrence_built = true;
    }

    long long occurrence_count(std::string_view s){
        build_occurrences();
        int v = 0;
        for(char c: s){
            int id = char_id(c);
            v = states[v].next[id];
            if(v == -1) return 0;
        }
        return states[v].occurrence;
    }

    std::vector<long long> maximum_occurrences_by_length(){
        build_occurrences();
        const int text_length = states[last_state].length;
        std::vector<long long> result(static_cast<std::size_t>(text_length));
        for(int v = 1; v < used; v++){
            const int length = states[v].length;
            result[static_cast<std::size_t>(length - 1)] = std::max(
                result[static_cast<std::size_t>(length - 1)],
                states[v].occurrence
            );
        }
        for(int length = text_length - 1; length > 0; length--){
            result[static_cast<std::size_t>(length - 1)] = std::max(
                result[static_cast<std::size_t>(length - 1)],
                result[static_cast<std::size_t>(length)]
            );
        }
        return result;
    }

    Match longest_common_substring(std::string_view s) const{
        int v = 0;
        int len = 0;
        Match best{0, 0};
        for(int i = 0; i < static_cast<int>(s.size()); i++){
            int id = char_id(s[static_cast<std::size_t>(i)]);
            while(v != 0 && states[v].next[id] == -1){
                v = states[v].link;
                len = states[v].length;
            }
            if(states[v].next[id] != -1){
                v = states[v].next[id];
                len++;
            }else{
                v = 0;
                len = 0;
            }
            if(best.length < len){
                best.length = len;
                best.position = i - len + 1;
            }
        }
        return best;
    }

    int longest_common_substring_length(
        std::span<const std::string_view> additional_strings
    ) const{
        const int text_length = states[last_state].length;
        std::vector<int> count(text_length + 1), order(used);
        for(int v = 0; v < used; v++) count[states[v].length]++;
        for(int length = 1; length <= text_length; length++){
            count[length] += count[length - 1];
        }
        for(int v = used - 1; v >= 0; v--){
            order[--count[states[v].length]] = v;
        }

        std::vector<int> common(used);
        for(int v = 0; v < used; v++) common[v] = states[v].length;

        for(std::string_view text: additional_strings){
            std::vector<int> best(used);
            int state = 0;
            int matched = 0;
            for(char c: text){
                const int id = char_id(c);
                while(state != 0 && states[state].next[id] == -1){
                    state = states[state].link;
                    matched = std::min(matched, states[state].length);
                }
                if(states[state].next[id] == -1){
                    state = 0;
                    matched = 0;
                }else{
                    state = states[state].next[id];
                    matched++;
                    best[state] = std::max(best[state], matched);
                }
            }

            for(int index = used - 1; index > 0; index--){
                const int v = order[index];
                const int parent = states[v].link;
                best[parent] = std::max(
                    best[parent],
                    std::min(best[v], states[parent].length)
                );
            }
            for(int v = 0; v < used; v++){
                common[v] = std::min(common[v], best[v]);
            }
        }

        return *std::max_element(common.begin(), common.end());
    }
};

namespace suffix_automaton_internal{

class ByteSuffixAutomaton{
public:
    struct State{
        std::vector<std::pair<unsigned char, int>> next;
        int link = -1;
        int length = 0;
        int first_end = -1;
    };

private:
    std::vector<State> states_{{}};
    int last_ = 0;

    int transition(int state, unsigned char byte) const{
        for(const auto& [label, target]:
            states_[static_cast<std::size_t>(state)].next){
            if(label == byte) return target;
        }
        return -1;
    }

    void set_transition(int state, unsigned char byte, int target){
        auto& transitions = states_[static_cast<std::size_t>(state)].next;
        for(auto& [label, destination]: transitions){
            if(label == byte){
                destination = target;
                return;
            }
        }
        transitions.push_back({byte, target});
    }

public:
    ByteSuffixAutomaton() = default;

    explicit ByteSuffixAutomaton(std::string_view text){
        if(text.size() >
           static_cast<std::size_t>(std::numeric_limits<int>::max())){
            throw std::length_error(
                "library assertion fault: text is too long."
            );
        }
        states_.reserve(2 * text.size() + 1);
        int position = 0;
        for(const unsigned char byte: text) extend(byte, position++);
    }

    void extend(unsigned char byte, int position){
        const int current = static_cast<int>(states_.size());
        states_.push_back({});
        states_[static_cast<std::size_t>(current)].length =
            states_[static_cast<std::size_t>(last_)].length + 1;
        states_[static_cast<std::size_t>(current)].first_end = position;

        int suffix = last_;
        while(suffix != -1 && transition(suffix, byte) == -1){
            set_transition(suffix, byte, current);
            suffix = states_[static_cast<std::size_t>(suffix)].link;
        }
        if(suffix == -1){
            states_[static_cast<std::size_t>(current)].link = 0;
        }else{
            const int target = transition(suffix, byte);
            if(states_[static_cast<std::size_t>(suffix)].length + 1 ==
               states_[static_cast<std::size_t>(target)].length){
                states_[static_cast<std::size_t>(current)].link = target;
            }else{
                const int clone = static_cast<int>(states_.size());
                states_.push_back(states_[static_cast<std::size_t>(target)]);
                states_[static_cast<std::size_t>(clone)].length =
                    states_[static_cast<std::size_t>(suffix)].length + 1;
                while(suffix != -1 && transition(suffix, byte) == target){
                    set_transition(suffix, byte, clone);
                    suffix = states_[static_cast<std::size_t>(suffix)].link;
                }
                states_[static_cast<std::size_t>(target)].link = clone;
                states_[static_cast<std::size_t>(current)].link = clone;
            }
        }
        last_ = current;
    }

    int next(int state, unsigned char byte) const{
        return transition(state, byte);
    }

    const State& operator[](int state) const{
        return states_[static_cast<std::size_t>(state)];
    }

    int size() const{
        return static_cast<int>(states_.size());
    }

    std::vector<int> length_order() const{
        const int maximum = states_[static_cast<std::size_t>(last_)].length;
        std::vector<int> count(static_cast<std::size_t>(maximum + 1));
        std::vector<int> order(states_.size());
        for(const State& state: states_){
            ++count[static_cast<std::size_t>(state.length)];
        }
        for(int length = 1; length <= maximum; ++length){
            count[static_cast<std::size_t>(length)] +=
                count[static_cast<std::size_t>(length - 1)];
        }
        for(int state = size() - 1; state >= 0; --state){
            order[static_cast<std::size_t>(
                --count[static_cast<std::size_t>(
                    states_[static_cast<std::size_t>(state)].length
                )]
            )] = state;
        }
        return order;
    }
};

} // namespace suffix_automaton_internal

#endif  // CPPLIB_SRC_ALGORITHM_STRING_SUFFIX_AUTOMATON_HPP_INCLUDED
