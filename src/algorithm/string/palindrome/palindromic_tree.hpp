#ifndef CPPLIB_SRC_ALGORITHM_STRING_PALINDROME_PALINDROMIC_TREE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_PALINDROME_PALINDROMIC_TREE_HPP_INCLUDED

#include <array>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

template<int ALPHABET, int MAX_NODES, char OFFSET = 'a'>
struct PalindromicTree{
    static_assert(ALPHABET > 0);
    static_assert(MAX_NODES >= 2);

    struct Node{
        std::array<int, ALPHABET> next;
        int parent = 0;
        int link = 0;
        int length = 0;
        int diff = 0;
        int series_link = 0;
        int terminal_occurrence = 0;
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
        nodes[0].series_link = 0;
        nodes[1].length = 0;
        nodes[1].link = 0;
        nodes[1].series_link = 0;
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
    int length(int v) const{ return (*this)[v].length; }
    int parent(int v) const{ return (*this)[v].parent; }
    int link(int v) const{ return (*this)[v].link; }
    int diff(int v) const{ return (*this)[v].diff; }
    int series_link(int v) const{ return (*this)[v].series_link; }
    const std::string& str() const{ return text; }

    int add(char c){
        occurrence_built = false;
        int id = char_id(c);
        text.push_back(c);
        int pos = static_cast<int>(text.size()) - 1;
        int cur = suffix_candidate(last_node, pos, c);
        if(nodes[cur].next[id] != -1){
            last_node = nodes[cur].next[id];
            nodes[last_node].terminal_occurrence++;
            nodes[last_node].occurrence++;
            return last_node;
        }

        int created = new_node();
        nodes[created].parent = cur;
        nodes[created].length = nodes[cur].length + 2;
        nodes[created].terminal_occurrence = 1;
        nodes[created].occurrence = 1;
        nodes[created].first_position = pos;
        nodes[cur].next[id] = created;

        if(nodes[created].length == 1){
            nodes[created].link = 1;
        }else{
            int link_candidate = suffix_candidate(nodes[cur].link, pos, c);
            nodes[created].link = nodes[link_candidate].next[id];
        }
        nodes[created].diff = nodes[created].length - nodes[nodes[created].link].length;
        if(nodes[created].diff == nodes[nodes[created].link].diff){
            nodes[created].series_link = nodes[nodes[created].link].series_link;
        }else{
            nodes[created].series_link = nodes[created].link;
        }
        last_node = created;
        return created;
    }

    void build_occurrences(){
        if(occurrence_built) return;
        for(int v = 0; v < used; v++){
            nodes[v].occurrence = nodes[v].terminal_occurrence;
        }
        for(int v = used - 1; v >= 2; v--){
            nodes[nodes[v].link].occurrence += nodes[v].occurrence;
        }
        occurrence_built = true;
    }

    long long count_palindromic_substrings(){
        build_occurrences();
        long long result = 0;
        for(int v = 2; v < used; v++) result += nodes[v].occurrence;
        return result;
    }

    int longest_suffix_palindrome_length() const{
        return nodes[last_node].length;
    }

    int find_node(std::string_view s) const{
        if(s.empty()) return 1;
        int node = s.size() % 2 == 0 ? 1 : 0;
        for(int left = (static_cast<int>(s.size()) - 1) / 2; left >= 0; left--){
            int right = static_cast<int>(s.size()) - 1 - left;
            if(s[static_cast<std::size_t>(left)] != s[static_cast<std::size_t>(right)]){
                return -1;
            }
            int id = static_cast<int>(s[static_cast<std::size_t>(left)] - OFFSET);
            if(id < 0 || ALPHABET <= id) return -1;
            node = nodes[node].next[static_cast<std::size_t>(id)];
            if(node == -1) return -1;
        }
        return node;
    }

    bool contains(std::string_view s) const{
        return find_node(s) != -1;
    }

    int occurrence_count(std::string_view s){
        if(s.empty()) return static_cast<int>(text.size()) + 1;
        build_occurrences();
        int v = find_node(s);
        return v == -1 ? 0 : nodes[v].occurrence;
    }

    std::pair<int, int> first_occurrence_range(int v) const{
        const Node& node = (*this)[v];
        if(node.length < 0)[[unlikely]]{
            throw std::runtime_error("library assertion fault: root does not represent a string.");
        }
        if(node.length == 0) return {0, 0};
        int r = node.first_position + 1;
        return {r - node.length, r};
    }

    std::string palindrome(int v) const{
        const auto [l, r] = first_occurrence_range(v);
        return text.substr(static_cast<std::size_t>(l), static_cast<std::size_t>(r - l));
    }

    std::vector<int> palindrome_nodes() const{
        std::vector<int> result;
        result.reserve(static_cast<std::size_t>(distinct_palindromes()));
        for(int v = 2; v < used; v++) result.push_back(v);
        return result;
    }

    std::vector<std::string> palindromes() const{
        std::vector<std::string> result;
        result.reserve(static_cast<std::size_t>(distinct_palindromes()));
        for(int v = 2; v < used; v++) result.push_back(palindrome(v));
        return result;
    }

    std::vector<int> suffix_palindrome_nodes() const{
        std::vector<int> result;
        for(int v = last_node; v > 1; v = nodes[v].link) result.push_back(v);
        return result;
    }

    std::vector<int> suffix_palindrome_lengths() const{
        std::vector<int> result;
        for(int v = last_node; v > 1; v = nodes[v].link) result.push_back(nodes[v].length);
        return result;
    }

    int count_suffix_palindromes() const{
        int result = 0;
        for(int v = last_node; v > 1; v = nodes[v].link) result++;
        return result;
    }

    int longest_palindrome_node() const{
        int result = 1;
        for(int v = 2; v < used; v++){
            if(nodes[result].length < nodes[v].length) result = v;
        }
        return result;
    }

    int longest_palindrome_length() const{
        return nodes[longest_palindrome_node()].length;
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_STRING_PALINDROME_PALINDROMIC_TREE_HPP_INCLUDED
