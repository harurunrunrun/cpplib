#ifndef CPPLIB_SRC_STRUCTURE_TRIE_PERSISTENT_TRIE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_TRIE_PERSISTENT_TRIE_HPP_INCLUDED

#include <array>
#include <stdexcept>
#include <string_view>

template<int ALPHABET, int MAX_NODES, int MAX_VERSIONS, char OFFSET = 'a'>
struct PersistentTrie{
    static_assert(ALPHABET > 0);
    static_assert(MAX_NODES > 0);
    static_assert(MAX_VERSIONS > 0);

private:
    struct Node{
        std::array<int, ALPHABET> next{};
        int pass_count = 0;
        int terminal_count = 0;
    };

    int used = 1;
    int version_count = 1;
    std::array<Node, MAX_NODES> nodes{};
    std::array<int, MAX_VERSIONS> roots{};

    int char_id(char c) const{
        int id = static_cast<int>(c - OFFSET);
        if(id < 0 || ALPHABET <= id)[[unlikely]]{
            throw std::runtime_error("library assertion fault: character is out of alphabet.");
        }
        return id;
    }
    void check_version(int version) const{
        if(version < 0 || version_count <= version)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (version).");
        }
    }
    int new_node(){
        if(used == MAX_NODES)[[unlikely]]{
            throw std::runtime_error("library assertion fault: capacity exceeded (node).");
        }
        int v = used++;
        nodes[v] = Node();
        return v;
    }
    int clone_node(int v){
        int u = new_node();
        if(v != 0) nodes[u] = nodes[v];
        return u;
    }
    int new_version(int root){
        if(version_count == MAX_VERSIONS)[[unlikely]]{
            throw std::runtime_error("library assertion fault: capacity exceeded (version).");
        }
        roots[version_count] = root;
        return version_count++;
    }
    void check_version_capacity() const{
        if(version_count == MAX_VERSIONS)[[unlikely]]{
            throw std::runtime_error("library assertion fault: capacity exceeded (version).");
        }
    }

public:
    PersistentTrie(){
        roots[0] = new_node();
    }

    int versions() const{ return version_count; }
    int node_count() const{ return used; }
    int root(int version) const{
        check_version(version);
        return roots[version];
    }

    int fork(int version){
        check_version(version);
        return new_version(roots[version]);
    }
    int node(int version, std::string_view s) const{
        check_version(version);
        int v = roots[version];
        for(char c: s){
            int id = char_id(c);
            if(v != 0) v = nodes[v].next[id];
        }
        return v == 0 ? -1 : v;
    }
    int count(int version, std::string_view s) const{
        int v = node(version, s);
        return v == -1 ? 0 : nodes[v].terminal_count;
    }
    bool contains(int version, std::string_view s) const{
        return count(version, s) > 0;
    }
    int prefix_count(int version, std::string_view s) const{
        int v = node(version, s);
        return v == -1 ? 0 : nodes[v].pass_count;
    }
    int size(int version) const{
        check_version(version);
        return nodes[roots[version]].pass_count;
    }

    int insert(int version, std::string_view s){
        check_version(version);
        check_version_capacity();
        int snapshot = used;
        try{
            int old_cur = roots[version];
            int new_root = clone_node(old_cur);
            int new_cur = new_root;
            nodes[new_cur].pass_count++;
            for(char c: s){
                int id = char_id(c);
                int old_next = old_cur == 0 ? 0 : nodes[old_cur].next[id];
                int new_next = clone_node(old_next);
                nodes[new_cur].next[id] = new_next;
                old_cur = old_next;
                new_cur = new_next;
                nodes[new_cur].pass_count++;
            }
            nodes[new_cur].terminal_count++;
            return new_version(new_root);
        }catch(...){
            used = snapshot;
            throw;
        }
    }
    int erase(int version, std::string_view s){
        check_version(version);
        if(count(version, s) == 0) return fork(version);
        check_version_capacity();
        int snapshot = used;
        try{
            int old_cur = roots[version];
            int new_root = clone_node(old_cur);
            int new_cur = new_root;
            nodes[new_cur].pass_count--;
            for(char c: s){
                int id = char_id(c);
                int old_next = nodes[old_cur].next[id];
                int new_next = clone_node(old_next);
                nodes[new_cur].next[id] = new_next;
                old_cur = old_next;
                new_cur = new_next;
                nodes[new_cur].pass_count--;
            }
            nodes[new_cur].terminal_count--;
            return new_version(new_root);
        }catch(...){
            used = snapshot;
            throw;
        }
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_TRIE_PERSISTENT_TRIE_HPP_INCLUDED
