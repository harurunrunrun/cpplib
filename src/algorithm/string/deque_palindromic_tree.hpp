#ifndef CPPLIB_SRC_ALGORITHM_STRING_DEQUE_PALINDROMIC_TREE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_STRING_DEQUE_PALINDROMIC_TREE_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <memory>
#include <stdexcept>

struct DequePalindromeStatistics{
    int distinct_palindromes;
    int longest_prefix;
    int longest_suffix;
};

template<int ALPHABET, int MAX_NODES, int MAX_LENGTH, char OFFSET = 'a'>
struct DequePalindromicTree{
    static_assert(ALPHABET > 0);
    static_assert(MAX_NODES >= 2);
    static_assert(MAX_LENGTH > 0);

private:
    static constexpr int ODD_ROOT = 0;
    static constexpr int EVEN_ROOT = 1;

    struct Node{
        std::array<int, ALPHABET> next;
        int parent = ODD_ROOT;
        int link = ODD_ROOT;
        int quick = ODD_ROOT;
        int length = 0;
        int surface_count = 0;
        int link_count = 0;

        Node(){
            next.fill(-1);
        }
    };

    struct DequeNode{
        int character = 0;
        int prefix_surface = EVEN_ROOT;
        int suffix_surface = EVEN_ROOT;
    };

    struct CircularDeque{
        std::unique_ptr<std::array<DequeNode, MAX_LENGTH>> data =
            std::make_unique<std::array<DequeNode, MAX_LENGTH>>();
        int begin = 0;
        int count = 0;

        int physical_index(int index) const{
            int result = begin + index;
            if(result >= MAX_LENGTH) result -= MAX_LENGTH;
            return result;
        }

        DequeNode& operator[](int index){
            return (*data)[static_cast<std::size_t>(physical_index(index))];
        }
        const DequeNode& operator[](int index) const{
            return (*data)[static_cast<std::size_t>(physical_index(index))];
        }
        DequeNode& front(){ return (*this)[0]; }
        const DequeNode& front() const{ return (*this)[0]; }
        DequeNode& back(){ return (*this)[count - 1]; }
        const DequeNode& back() const{ return (*this)[count - 1]; }
        bool empty() const{ return count == 0; }
        int size() const{ return count; }

        void push_back(const DequeNode& value){
            (*this)[count] = value;
            ++count;
        }
        void push_front(const DequeNode& value){
            begin = begin == 0 ? MAX_LENGTH - 1 : begin - 1;
            (*data)[static_cast<std::size_t>(begin)] = value;
            ++count;
        }
        void pop_back(){ --count; }
        void pop_front(){
            ++begin;
            if(begin == MAX_LENGTH) begin = 0;
            --count;
        }
    };

    std::unique_ptr<std::array<Node, MAX_NODES>> nodes =
        std::make_unique<std::array<Node, MAX_NODES>>();
    CircularDeque text;
    int used_nodes = 2;
    int active_nodes = 0;

    Node& node(int index){
        return (*nodes)[static_cast<std::size_t>(index)];
    }
    const Node& node(int index) const{
        return (*nodes)[static_cast<std::size_t>(index)];
    }

    int char_id(char c) const{
        int id = static_cast<int>(static_cast<unsigned char>(c)) -
                 static_cast<int>(static_cast<unsigned char>(OFFSET));
        if(id < 0 || ALPHABET <= id)[[unlikely]]{
            throw std::runtime_error("library assertion fault: character is out of alphabet.");
        }
        return id;
    }

    void check_push_capacity() const{
        if(text.size() == MAX_LENGTH)[[unlikely]]{
            throw std::runtime_error("library assertion fault: capacity exceeded (push).");
        }
    }

    int new_node(){
        if(used_nodes == MAX_NODES)[[unlikely]]{
            throw std::runtime_error("library assertion fault: capacity exceeded (node).");
        }
        node(used_nodes) = Node();
        return used_nodes++;
    }

    int back_appendable(int character, int palindrome) const{
        const int n = text.size();
        while(true){
            const Node& current = node(palindrome);
            if(current.length == -1 ||
               (current.length < n &&
                text[n - current.length - 1].character == character)){
                return palindrome;
            }
            const int suffix = current.link;
            const Node& suffix_node = node(suffix);
            if(suffix_node.length == -1 ||
               text[n - suffix_node.length - 1].character == character){
                return suffix;
            }
            palindrome = current.quick;
        }
    }

    int front_appendable(int character, int palindrome) const{
        const int n = text.size();
        while(true){
            const Node& current = node(palindrome);
            if(current.length == -1 ||
               (current.length < n && text[current.length].character == character)){
                return palindrome;
            }
            const int suffix = current.link;
            const Node& suffix_node = node(suffix);
            if(suffix_node.length == -1 ||
               text[suffix_node.length].character == character){
                return suffix;
            }
            palindrome = current.quick;
        }
    }

public:
    DequePalindromicTree(){
        node(ODD_ROOT) = Node();
        node(ODD_ROOT).length = -1;
        node(ODD_ROOT).parent = ODD_ROOT;
        node(ODD_ROOT).link = ODD_ROOT;
        node(ODD_ROOT).quick = ODD_ROOT;
        node(EVEN_ROOT) = node(ODD_ROOT);
        node(EVEN_ROOT).length = 0;
    }

    DequePalindromicTree(const DequePalindromicTree&) = delete;
    DequePalindromicTree& operator=(const DequePalindromicTree&) = delete;
    DequePalindromicTree(DequePalindromicTree&&) = default;
    DequePalindromicTree& operator=(DequePalindromicTree&&) = default;

    int size() const{ return text.size(); }
    bool empty() const{ return text.empty(); }

    void push_back(char c){
        const int character = char_id(c);
        check_push_capacity();
        const int parent = text.empty()
            ? ODD_ROOT
            : back_appendable(character, text.back().suffix_surface);
        int palindrome = node(parent).next[static_cast<std::size_t>(character)];
        int suffix = EVEN_ROOT;

        if(palindrome == -1){
            palindrome = new_node();
            node(palindrome).parent = parent;
            node(palindrome).length = node(parent).length + 2;
            if(parent != ODD_ROOT){
                const int candidate = back_appendable(character, node(parent).link);
                suffix = node(candidate).next[static_cast<std::size_t>(character)];
            }
            node(palindrome).link = suffix;
            ++node(suffix).link_count;

            text.push_back(DequeNode{character, EVEN_ROOT, EVEN_ROOT});
            const int n = text.size();
            if(node(suffix).link != ODD_ROOT &&
               text[n - node(suffix).length - 1].character ==
                   text[n - node(node(suffix).link).length - 1].character){
                node(palindrome).quick = node(suffix).quick;
            }else{
                node(palindrome).quick = node(suffix).link;
            }
            node(parent).next[static_cast<std::size_t>(character)] = palindrome;
            ++active_nodes;
        }else{
            text.push_back(DequeNode{character, EVEN_ROOT, EVEN_ROOT});
            suffix = node(palindrome).link;
        }

        const int n = text.size();
        text[n - 1].suffix_surface = palindrome;
        text[n - node(palindrome).length].prefix_surface = palindrome;
        if(node(suffix).length >= 1 &&
           text[n - node(palindrome).length + node(suffix).length - 1].suffix_surface == suffix){
            text[n - node(palindrome).length + node(suffix).length - 1].suffix_surface = EVEN_ROOT;
        }
        ++node(palindrome).surface_count;
    }

    void push_front(char c){
        const int character = char_id(c);
        check_push_capacity();
        const int parent = text.empty()
            ? ODD_ROOT
            : front_appendable(character, text.front().prefix_surface);
        int palindrome = node(parent).next[static_cast<std::size_t>(character)];
        int suffix = EVEN_ROOT;

        if(palindrome == -1){
            palindrome = new_node();
            node(palindrome).parent = parent;
            node(palindrome).length = node(parent).length + 2;
            if(parent != ODD_ROOT){
                const int candidate = front_appendable(character, node(parent).link);
                suffix = node(candidate).next[static_cast<std::size_t>(character)];
            }
            node(palindrome).link = suffix;
            ++node(suffix).link_count;

            text.push_front(DequeNode{character, EVEN_ROOT, EVEN_ROOT});
            if(node(suffix).link != ODD_ROOT &&
               text[node(suffix).length].character ==
                   text[node(node(suffix).link).length].character){
                node(palindrome).quick = node(suffix).quick;
            }else{
                node(palindrome).quick = node(suffix).link;
            }
            node(parent).next[static_cast<std::size_t>(character)] = palindrome;
            ++active_nodes;
        }else{
            text.push_front(DequeNode{character, EVEN_ROOT, EVEN_ROOT});
            suffix = node(palindrome).link;
        }

        text[0].prefix_surface = palindrome;
        text[node(palindrome).length - 1].suffix_surface = palindrome;
        if(node(suffix).length >= 1 &&
           text[node(palindrome).length - node(suffix).length].prefix_surface == suffix){
            text[node(palindrome).length - node(suffix).length].prefix_surface = EVEN_ROOT;
        }
        ++node(palindrome).surface_count;
    }

    void pop_back(){
        if(text.empty())[[unlikely]]{
            throw std::runtime_error("library assertion fault: pop from empty deque.");
        }
        const int palindrome = text.back().suffix_surface;
        const int character = text.back().character;
        const int suffix = node(palindrome).link;
        const int n = text.size();
        if(node(palindrome).length >= 2 &&
           node(text[n - node(palindrome).length + node(suffix).length - 1].suffix_surface).length <
               node(suffix).length){
            text[n - node(palindrome).length + node(suffix).length - 1].suffix_surface = suffix;
            text[n - node(palindrome).length].prefix_surface = suffix;
        }else{
            text[n - node(palindrome).length].prefix_surface = EVEN_ROOT;
        }
        --node(palindrome).surface_count;
        if(node(palindrome).link_count == 0 && node(palindrome).surface_count == 0){
            node(node(palindrome).parent).next[static_cast<std::size_t>(character)] = -1;
            --node(suffix).link_count;
            --active_nodes;
        }
        text.pop_back();
    }

    void pop_front(){
        if(text.empty())[[unlikely]]{
            throw std::runtime_error("library assertion fault: pop from empty deque.");
        }
        const int palindrome = text.front().prefix_surface;
        const int character = text.front().character;
        const int suffix = node(palindrome).link;
        if(node(palindrome).length >= 2 &&
           node(text[node(palindrome).length - node(suffix).length].prefix_surface).length <
               node(suffix).length){
            text[node(palindrome).length - node(suffix).length].prefix_surface = suffix;
            text[node(palindrome).length - 1].suffix_surface = suffix;
        }else{
            text[node(palindrome).length - 1].suffix_surface = EVEN_ROOT;
        }
        --node(palindrome).surface_count;
        if(node(palindrome).link_count == 0 && node(palindrome).surface_count == 0){
            node(node(palindrome).parent).next[static_cast<std::size_t>(character)] = -1;
            --node(suffix).link_count;
            --active_nodes;
        }
        text.pop_front();
    }

    int distinct_palindromes() const{ return active_nodes; }
    int longest_prefix_palindrome_length() const{
        return text.empty() ? 0 : node(text.front().prefix_surface).length;
    }
    int longest_suffix_palindrome_length() const{
        return text.empty() ? 0 : node(text.back().suffix_surface).length;
    }
    DequePalindromeStatistics statistics() const{
        return {
            distinct_palindromes(),
            longest_prefix_palindrome_length(),
            longest_suffix_palindrome_length(),
        };
    }
    DequePalindromeStatistics query() const{ return statistics(); }
};

#endif  // CPPLIB_SRC_ALGORITHM_STRING_DEQUE_PALINDROMIC_TREE_HPP_INCLUDED
