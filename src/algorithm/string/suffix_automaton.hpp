#pragma once

#include <algorithm>
#include <array>
#include <stdexcept>
#include <string>
#include <string_view>
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
};
