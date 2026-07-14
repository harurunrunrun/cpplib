#pragma once

#include <array>
#include <queue>
#include <stdexcept>
#include <string_view>

template<int ALPHABET, int MAX_NODES, char OFFSET = 'a'>
struct AhoCorasick{
    static_assert(ALPHABET > 0);
    static_assert(MAX_NODES > 0);

private:
    struct Node{
        std::array<int, ALPHABET> next{};
        int parent = 0;
        int fail = 0;
        int output_link_plus_one = 0;
        int terminal_count = 0;
        int output_count = 0;
    };

    int used = 1;
    bool built = false;
    std::array<Node, MAX_NODES> nodes{};

    int char_id(char c) const{
        int id = static_cast<int>(c - OFFSET);
        if(id < 0 || ALPHABET <= id)[[unlikely]]{
            throw std::runtime_error("library assertion fault: character is out of alphabet.");
        }
        return id;
    }
    int new_node(int parent){
        if(used == MAX_NODES)[[unlikely]]{
            throw std::runtime_error("library assertion fault: capacity exceeded (add).");
        }
        int v = used++;
        nodes[v] = Node();
        nodes[v].parent = parent;
        return v;
    }

public:
    AhoCorasick(){
        nodes[0] = Node();
    }

    int node_count() const{ return used; }
    bool is_built() const{ return built; }

    void clear(){
        for(int v = 0; v < used; v++) nodes[v] = Node();
        used = 1;
        built = false;
    }

    int add(std::string_view pattern){
        if(built)[[unlikely]]{
            throw std::runtime_error("library assertion fault: already built (add).");
        }
        int v = 0;
        for(char c: pattern){
            int id = char_id(c);
            if(nodes[v].next[id] == 0) nodes[v].next[id] = new_node(v);
            v = nodes[v].next[id];
        }
        nodes[v].terminal_count++;
        return v;
    }

    void build(){
        if(built) return;
        nodes[0].output_count = nodes[0].terminal_count;
        std::queue<int> que;
        for(int c = 0; c < ALPHABET; c++){
            int to = nodes[0].next[c];
            if(to != 0) que.push(to);
        }
        while(!que.empty()){
            int v = que.front();
            que.pop();
            const int failure = nodes[v].fail;
            nodes[v].output_link_plus_one = nodes[failure].terminal_count > 0
                ? failure + 1
                : nodes[failure].output_link_plus_one;
            nodes[v].output_count = nodes[v].terminal_count + nodes[nodes[v].fail].output_count;
            for(int c = 0; c < ALPHABET; c++){
                int to = nodes[v].next[c];
                if(to == 0){
                    nodes[v].next[c] = nodes[nodes[v].fail].next[c];
                }else{
                    nodes[to].fail = nodes[nodes[v].fail].next[c];
                    que.push(to);
                }
            }
        }
        built = true;
    }

    int next_state(int state, char c) const{
        if(!built)[[unlikely]]{
            throw std::runtime_error("library assertion fault: not built (next_state).");
        }
        if(state < 0 || used <= state)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (next_state).");
        }
        return nodes[state].next[char_id(c)];
    }

    int terminal_count(int state) const{
        if(state < 0 || used <= state)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (terminal_count).");
        }
        return nodes[state].terminal_count;
    }
    int parent(int state) const{
        if(state < 0 || used <= state)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (parent).");
        }
        return nodes[state].parent;
    }
    int failure_link(int state) const{
        if(!built)[[unlikely]]{
            throw std::runtime_error("library assertion fault: not built (failure_link).");
        }
        if(state < 0 || used <= state)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (failure_link).");
        }
        return nodes[state].fail;
    }
    int output_link(int state) const{
        if(!built)[[unlikely]]{
            throw std::runtime_error("library assertion fault: not built (output_link).");
        }
        if(state < 0 || used <= state)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (output_link).");
        }
        return nodes[state].output_link_plus_one - 1;
    }
    int output_count(int state) const{
        if(!built)[[unlikely]]{
            throw std::runtime_error("library assertion fault: not built (output_count).");
        }
        if(state < 0 || used <= state)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (output_count).");
        }
        return nodes[state].output_count;
    }

    long long count_matches(std::string_view text) const{
        if(!built)[[unlikely]]{
            throw std::runtime_error("library assertion fault: not built (count_matches).");
        }
        long long result = nodes[0].output_count;
        int state = 0;
        for(char c: text){
            state = nodes[state].next[char_id(c)];
            result += nodes[state].output_count;
        }
        return result;
    }
};
