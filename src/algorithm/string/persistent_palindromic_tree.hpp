#pragma once

#include <array>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

template<int ALPHABET, int MAX_NODES, int MAX_LENGTH, char OFFSET = 'a'>
struct PersistentPalindromicTree{
    static_assert(ALPHABET > 0);
    static_assert(MAX_NODES >= 2);
    static_assert(MAX_LENGTH >= 0);

    struct Node{
        std::array<int, ALPHABET> next;
        int link = 0;
        int length = 0;
        int diff = 0;
        int series_link = 0;
        int suffix_count = 0;
        int first_position = -1;

        Node(){
            next.fill(-1);
        }
    };

private:
    int used = 2;
    int current_last = 1;
    std::string text;
    std::array<Node, MAX_NODES> nodes{};
    std::array<int, MAX_LENGTH + 1> last_at{};
    std::array<int, MAX_LENGTH + 1> distinct_at{};
    std::array<int, MAX_LENGTH + 1> longest_at{};
    std::array<long long, MAX_LENGTH + 1> pal_substrings_at{};

    int char_id(char c) const{
        int id = static_cast<int>(c - OFFSET);
        if(id < 0 || ALPHABET <= id)[[unlikely]]{
            throw std::runtime_error("library assertion fault: character is out of alphabet.");
        }
        return id;
    }

    int new_node(){
        if(used == MAX_NODES)[[unlikely]]{
            throw std::runtime_error("library assertion fault: capacity exceeded (append).");
        }
        nodes[used] = Node();
        return used++;
    }

    int suffix_candidate(int v, int pos, char c) const{
        while(true){
            int len = nodes[v].length;
            if(pos - 1 - len >= 0 && text[static_cast<std::size_t>(pos - 1 - len)] == c){
                return v;
            }
            v = nodes[v].link;
        }
    }

    void check_version(int version) const{
        if(version < 0 || static_cast<int>(text.size()) < version)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (version).");
        }
    }

public:
    PersistentPalindromicTree(){
        nodes[0] = Node();
        nodes[1] = Node();
        nodes[0].length = -1;
        nodes[0].link = 0;
        nodes[1].length = 0;
        nodes[1].link = 0;
        last_at.fill(1);
        distinct_at.fill(0);
        longest_at.fill(1);
        pal_substrings_at.fill(0);
    }

    explicit PersistentPalindromicTree(std::string_view s): PersistentPalindromicTree(){
        for(char c: s) append(c);
    }

    int size() const{ return static_cast<int>(text.size()); }
    int size(int version) const{
        check_version(version);
        return version;
    }
    int versions() const{ return static_cast<int>(text.size()) + 1; }
    int node_count() const{ return used; }
    const std::string& str() const{ return text; }

    const Node& operator[](int v) const{
        if(v < 0 || used <= v)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (operator[]).");
        }
        return nodes[v];
    }

    int length(int node) const{ return (*this)[node].length; }
    int link(int node) const{ return (*this)[node].link; }
    int diff(int node) const{ return (*this)[node].diff; }
    int series_link(int node) const{ return (*this)[node].series_link; }

    int append(char c){
        if(static_cast<int>(text.size()) == MAX_LENGTH)[[unlikely]]{
            throw std::runtime_error("library assertion fault: length capacity exceeded (append).");
        }
        int id = char_id(c);
        text.push_back(c);
        int pos = static_cast<int>(text.size()) - 1;
        int cur = suffix_candidate(current_last, pos, c);
        if(nodes[cur].next[id] != -1){
            current_last = nodes[cur].next[id];
        }else{
            int created = new_node();
            nodes[created].length = nodes[cur].length + 2;
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
            nodes[created].suffix_count = nodes[nodes[created].link].suffix_count + 1;
            current_last = created;
        }

        int version = static_cast<int>(text.size());
        last_at[static_cast<std::size_t>(version)] = current_last;
        distinct_at[static_cast<std::size_t>(version)] = used - 2;
        int previous_longest = longest_at[static_cast<std::size_t>(version - 1)];
        longest_at[static_cast<std::size_t>(version)] =
            nodes[previous_longest].length < nodes[current_last].length ? current_last : previous_longest;
        pal_substrings_at[static_cast<std::size_t>(version)] =
            pal_substrings_at[static_cast<std::size_t>(version - 1)] +
            nodes[current_last].suffix_count;
        return version;
    }

    int add(char c){
        return append(c);
    }

    int last(int version) const{
        check_version(version);
        return last_at[static_cast<std::size_t>(version)];
    }

    int distinct_palindromes(int version) const{
        check_version(version);
        return distinct_at[static_cast<std::size_t>(version)];
    }

    long long count_palindromic_substrings(int version) const{
        check_version(version);
        return pal_substrings_at[static_cast<std::size_t>(version)];
    }

    int longest_suffix_palindrome_length(int version) const{
        check_version(version);
        return nodes[last_at[static_cast<std::size_t>(version)]].length;
    }

    int longest_palindrome_node(int version) const{
        check_version(version);
        return longest_at[static_cast<std::size_t>(version)];
    }

    int longest_palindrome_length(int version) const{
        return nodes[longest_palindrome_node(version)].length;
    }

    std::pair<int, int> first_occurrence_range(int node) const{
        const Node& current = (*this)[node];
        if(current.length < 0)[[unlikely]]{
            throw std::runtime_error("library assertion fault: root does not represent a string.");
        }
        if(current.length == 0) return {0, 0};
        int r = current.first_position + 1;
        return {r - current.length, r};
    }

    std::string palindrome(int node) const{
        const auto [l, r] = first_occurrence_range(node);
        return text.substr(static_cast<std::size_t>(l), static_cast<std::size_t>(r - l));
    }

    int find_node(std::string_view s, int version) const{
        check_version(version);
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
        return nodes[node].first_position < version ? node : -1;
    }

    bool contains(std::string_view s, int version) const{
        return find_node(s, version) != -1;
    }

    std::vector<int> palindrome_nodes(int version) const{
        check_version(version);
        std::vector<int> result;
        result.reserve(static_cast<std::size_t>(distinct_palindromes(version)));
        for(int node = 2; node < used; node++){
            if(nodes[node].first_position < version) result.push_back(node);
        }
        return result;
    }

    std::vector<std::string> palindromes(int version) const{
        std::vector<std::string> result;
        result.reserve(static_cast<std::size_t>(distinct_palindromes(version)));
        for(int node: palindrome_nodes(version)) result.push_back(palindrome(node));
        return result;
    }

    std::vector<int> suffix_palindrome_nodes(int version) const{
        check_version(version);
        std::vector<int> result;
        for(int node = last_at[static_cast<std::size_t>(version)]; node > 1; node = nodes[node].link){
            result.push_back(node);
        }
        return result;
    }

    std::vector<int> suffix_palindrome_lengths(int version) const{
        check_version(version);
        std::vector<int> result;
        for(int node = last_at[static_cast<std::size_t>(version)]; node > 1; node = nodes[node].link){
            result.push_back(nodes[node].length);
        }
        return result;
    }
};
