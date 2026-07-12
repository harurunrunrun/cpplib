#pragma once

#include <array>
#include <optional>
#include <stdexcept>
#include <type_traits>

template<class T, int BIT, int MAX_NODES>
struct BinaryTrie{
    static_assert(std::is_integral_v<T>);
    static_assert(BIT > 0);
    static_assert(MAX_NODES > 0);
    using U = std::make_unsigned_t<T>;

private:
    struct Node{
        std::array<int, 2> next{};
        int count = 0;
    };

    int used = 1;
    std::array<Node, MAX_NODES> nodes{};

    int new_node(){
        if(used == MAX_NODES)[[unlikely]]{
            throw std::runtime_error("library assertion fault: capacity exceeded (insert).");
        }
        int v = used++;
        nodes[v] = Node();
        return v;
    }
    int count_or_zero(int v) const{
        return v == 0 ? 0 : nodes[v].count;
    }

public:
    BinaryTrie(){
        nodes[0] = Node();
    }

    int size() const{ return nodes[0].count; }
    int node_count() const{ return used; }
    bool empty() const{ return size() == 0; }

    void insert(T value){
        U x = static_cast<U>(value);
        int v = 0;
        nodes[v].count++;
        for(int b = BIT - 1; b >= 0; b--){
            int bit = static_cast<int>((x >> b) & 1U);
            if(nodes[v].next[bit] == 0) nodes[v].next[bit] = new_node();
            v = nodes[v].next[bit];
            nodes[v].count++;
        }
    }
    int count(T value) const{
        U x = static_cast<U>(value);
        int v = 0;
        for(int b = BIT - 1; b >= 0; b--){
            int bit = static_cast<int>((x >> b) & 1U);
            v = nodes[v].next[bit];
            if(v == 0) return 0;
        }
        return nodes[v].count;
    }
    bool contains(T value) const{
        return count(value) > 0;
    }
    bool erase(T value){
        if(count(value) == 0) return false;
        U x = static_cast<U>(value);
        int v = 0;
        nodes[v].count--;
        for(int b = BIT - 1; b >= 0; b--){
            int bit = static_cast<int>((x >> b) & 1U);
            v = nodes[v].next[bit];
            nodes[v].count--;
        }
        return true;
    }

    std::optional<T> kth(int k, T xor_value = 0) const{
        if(k < 0 || size() <= k) return std::nullopt;
        U x = static_cast<U>(xor_value);
        U result = 0;
        int v = 0;
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
    std::optional<T> min_element(T xor_value = 0) const{
        return kth(0, xor_value);
    }
    std::optional<T> max_element(T xor_value = 0) const{
        return kth(size() - 1, xor_value);
    }
    int count_less(T value, T xor_value = 0) const{
        U x = static_cast<U>(value);
        U y = static_cast<U>(xor_value);
        int v = 0;
        int result = 0;
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
