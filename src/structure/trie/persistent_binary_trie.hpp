#ifndef CPPLIB_SRC_STRUCTURE_TRIE_PERSISTENT_BINARY_TRIE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_TRIE_PERSISTENT_BINARY_TRIE_HPP_INCLUDED

#include <array>
#include <limits>
#include <optional>
#include <stdexcept>
#include <type_traits>

template<class T, int BIT, int MAX_NODES, int MAX_VERSIONS>
struct PersistentBinaryTrie{
    static_assert(std::is_integral_v<T>);
    static_assert(BIT > 0);
    static_assert(BIT <= std::numeric_limits<std::make_unsigned_t<T>>::digits);
    static_assert(MAX_NODES > 0);
    static_assert(MAX_VERSIONS > 0);
    using U = std::make_unsigned_t<T>;

private:
    struct Node{
        std::array<int, 2> next{};
        int count = 0;
    };

    int used = 1;
    int version_count = 1;
    std::array<Node, MAX_NODES> nodes{};
    std::array<int, MAX_VERSIONS> roots{};

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
    int count_or_zero(int v) const{
        return v == 0 ? 0 : nodes[v].count;
    }

public:
    PersistentBinaryTrie() = default;

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
    int size(int version) const{
        check_version(version);
        return count_or_zero(roots[version]);
    }

    int count(int version, T value) const{
        check_version(version);
        U x = static_cast<U>(value);
        int v = roots[version];
        if(v == 0) return 0;
        for(int b = BIT - 1; b >= 0; b--){
            int bit = static_cast<int>((x >> b) & 1U);
            v = nodes[v].next[bit];
            if(v == 0) return 0;
        }
        return nodes[v].count;
    }
    bool contains(int version, T value) const{
        return count(version, value) > 0;
    }
    int insert(int version, T value){
        check_version(version);
        check_version_capacity();
        int snapshot = used;
        try{
            U x = static_cast<U>(value);
            int old_cur = roots[version];
            int new_root = clone_node(old_cur);
            int new_cur = new_root;
            nodes[new_cur].count++;
            for(int b = BIT - 1; b >= 0; b--){
                int bit = static_cast<int>((x >> b) & 1U);
                int old_next = old_cur == 0 ? 0 : nodes[old_cur].next[bit];
                int new_next = clone_node(old_next);
                nodes[new_cur].next[bit] = new_next;
                old_cur = old_next;
                new_cur = new_next;
                nodes[new_cur].count++;
            }
            return new_version(new_root);
        }catch(...){
            used = snapshot;
            throw;
        }
    }
    int erase(int version, T value){
        check_version(version);
        if(count(version, value) == 0) return fork(version);
        check_version_capacity();
        int snapshot = used;
        try{
            U x = static_cast<U>(value);
            int old_cur = roots[version];
            int new_root = clone_node(old_cur);
            int new_cur = new_root;
            nodes[new_cur].count--;
            for(int b = BIT - 1; b >= 0; b--){
                int bit = static_cast<int>((x >> b) & 1U);
                int old_next = nodes[old_cur].next[bit];
                int new_next = clone_node(old_next);
                nodes[new_cur].next[bit] = new_next;
                old_cur = old_next;
                new_cur = new_next;
                nodes[new_cur].count--;
            }
            return new_version(new_root);
        }catch(...){
            used = snapshot;
            throw;
        }
    }

    std::optional<T> kth(int version, int k, T xor_value = 0) const{
        check_version(version);
        if(k < 0 || size(version) <= k) return std::nullopt;
        U x = static_cast<U>(xor_value);
        U result = 0;
        int v = roots[version];
        for(int b = BIT - 1; b >= 0; b--){
            int xb = static_cast<int>((x >> b) & 1U);
            int left = nodes[v].next[xb];
            int left_count = count_or_zero(left);
            int bit;
            if(k < left_count){
                bit = xb;
                v = left;
            }else{
                k -= left_count;
                bit = xb ^ 1;
                v = nodes[v].next[bit];
            }
            if(bit) result |= U(1) << b;
        }
        return static_cast<T>(result);
    }
    std::optional<T> min_element(int version, T xor_value = 0) const{
        return kth(version, 0, xor_value);
    }
    std::optional<T> max_element(int version, T xor_value = 0) const{
        return kth(version, size(version) - 1, xor_value);
    }
    int count_less(int version, T value, T xor_value = 0) const{
        check_version(version);
        U x = static_cast<U>(value);
        U y = static_cast<U>(xor_value);
        int v = roots[version];
        int result = 0;
        if(v == 0) return 0;
        for(int b = BIT - 1; b >= 0; b--){
            int xb = static_cast<int>((x >> b) & 1U);
            int yb = static_cast<int>((y >> b) & 1U);
            if(xb == 1){
                result += count_or_zero(nodes[v].next[yb]);
                v = nodes[v].next[yb ^ 1];
            }else{
                v = nodes[v].next[yb];
            }
            if(v == 0) break;
        }
        return result;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_TRIE_PERSISTENT_BINARY_TRIE_HPP_INCLUDED
