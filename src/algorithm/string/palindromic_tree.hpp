#pragma once

#include <array>
#include <stdexcept>
#include <string>
#include <string_view>

template<int ALPHABET, int MAX_NODES, char OFFSET = 'a'>
struct PalindromicTree{
    static_assert(ALPHABET > 0);
    static_assert(MAX_NODES >= 2);

    struct Node{
        std::array<int, ALPHABET> next;
        int link = 0;
        int length = 0;
        int occurrence = 0;
        int first_position = -1;

        Node(){
            next.fill(-1);
        }
    };

private:
    int used = 2;
    int last_node = 1;
    bool occurrence_built = false;
    std::string text;
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
            throw std::runtime_error("library assertion fault: capacity exceeded (add).");
        }
        nodes[used] = Node();
        return used++;
    }
    int suffix_candidate(int v, int pos, char c) const{
        while(true){
            int l = nodes[v].length;
            if(pos - 1 - l >= 0 && text[static_cast<std::size_t>(pos - 1 - l)] == c) return v;
            v = nodes[v].link;
        }
    }

public:
    PalindromicTree(){
        nodes[0] = Node();
        nodes[1] = Node();
        nodes[0].length = -1;
        nodes[0].link = 0;
        nodes[1].length = 0;
        nodes[1].link = 0;
    }

    explicit PalindromicTree(std::string_view s): PalindromicTree(){
        for(char c: s) add(c);
    }

    int size() const{ return static_cast<int>(text.size()); }
    int node_count() const{ return used; }
    int distinct_palindromes() const{ return used - 2; }
    int last() const{ return last_node; }
    const Node& operator[](int v) const{
        if(v < 0 || used <= v)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (operator[]).");
        }
        return nodes[v];
    }

    int add(char c){
        occurrence_built = false;
        int id = char_id(c);
        text.push_back(c);
        int pos = static_cast<int>(text.size()) - 1;
        int cur = suffix_candidate(last_node, pos, c);
        if(nodes[cur].next[id] != -1){
            last_node = nodes[cur].next[id];
            nodes[last_node].occurrence++;
            return last_node;
        }

        int created = new_node();
        nodes[created].length = nodes[cur].length + 2;
        nodes[created].occurrence = 1;
        nodes[created].first_position = pos;
        nodes[cur].next[id] = created;

        if(nodes[created].length == 1){
            nodes[created].link = 1;
        }else{
            int link_candidate = suffix_candidate(nodes[cur].link, pos, c);
            nodes[created].link = nodes[link_candidate].next[id];
        }
        last_node = created;
        return created;
    }

    void build_occurrences(){
        if(occurrence_built) return;
        for(int v = used - 1; v >= 2; v--){
            nodes[nodes[v].link].occurrence += nodes[v].occurrence;
        }
        occurrence_built = true;
    }

    int count_palindromic_substrings(){
        build_occurrences();
        int result = 0;
        for(int v = 2; v < used; v++) result += nodes[v].occurrence;
        return result;
    }

    int longest_suffix_palindrome_length() const{
        return nodes[last_node].length;
    }

    bool contains(std::string_view s) const{
        for(int v = 2; v < used; v++){
            if(nodes[v].length != static_cast<int>(s.size())) continue;
            int l = nodes[v].first_position - nodes[v].length + 1;
            bool ok = true;
            for(int i = 0; i < nodes[v].length; i++){
                if(text[static_cast<std::size_t>(l + i)] != s[static_cast<std::size_t>(i)]){
                    ok = false;
                    break;
                }
            }
            if(ok) return true;
        }
        return false;
    }

    int occurrence_count(std::string_view s){
        build_occurrences();
        for(int v = 2; v < used; v++){
            if(nodes[v].length != static_cast<int>(s.size())) continue;
            int l = nodes[v].first_position - nodes[v].length + 1;
            bool ok = true;
            for(int i = 0; i < nodes[v].length; i++){
                if(text[static_cast<std::size_t>(l + i)] != s[static_cast<std::size_t>(i)]){
                    ok = false;
                    break;
                }
            }
            if(ok) return nodes[v].occurrence;
        }
        return 0;
    }
};
