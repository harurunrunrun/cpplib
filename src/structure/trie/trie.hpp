#pragma once

#include <array>
#include <stdexcept>
#include <string_view>

template<int ALPHABET, int MAX_NODES, char OFFSET = 'a'>
struct Trie{
    static_assert(ALPHABET > 0);
    static_assert(MAX_NODES > 0);

private:
    struct Node{
        std::array<int, ALPHABET> next{};
        int pass_count = 0;
        int terminal_count = 0;
    };

    int used = 1;
    int word_count = 0;
    std::array<Node, MAX_NODES> nodes{};

    int char_id(char c) const{
        int id = static_cast<int>(c - OFFSET);
        if(id < 0 || ALPHABET <= id)[[unlikely]]{
            throw std::runtime_error("library assertion fault: character is out of alphabet.");
        }
        return id;
    }
    int new_node(){
        if(used == MAX_NODES)[[unlikely]]{
            throw std::runtime_error("library assertion fault: capacity exceeded (insert).");
        }
        int v = used++;
        nodes[v] = Node();
        return v;
    }

public:
    Trie(){
        nodes[0] = Node();
    }

    int size() const{ return word_count; }
    int node_count() const{ return used; }
    bool empty() const{ return word_count == 0; }

    int node(std::string_view s) const{
        int v = 0;
        for(char c: s){
            int id = char_id(c);
            v = nodes[v].next[id];
            if(v == 0) return -1;
        }
        return v;
    }
    int count(std::string_view s) const{
        int v = node(s);
        return v == -1 ? 0 : nodes[v].terminal_count;
    }
    bool contains(std::string_view s) const{
        return count(s) > 0;
    }
    int prefix_count(std::string_view s) const{
        int v = node(s);
        return v == -1 ? 0 : nodes[v].pass_count;
    }

    void insert(std::string_view s){
        int v = 0;
        nodes[v].pass_count++;
        for(char c: s){
            int id = char_id(c);
            if(nodes[v].next[id] == 0) nodes[v].next[id] = new_node();
            v = nodes[v].next[id];
            nodes[v].pass_count++;
        }
        nodes[v].terminal_count++;
        word_count++;
    }
    bool erase(std::string_view s){
        if(count(s) == 0) return false;
        int v = 0;
        nodes[v].pass_count--;
        for(char c: s){
            v = nodes[v].next[char_id(c)];
            nodes[v].pass_count--;
        }
        nodes[v].terminal_count--;
        word_count--;
        return true;
    }
};
