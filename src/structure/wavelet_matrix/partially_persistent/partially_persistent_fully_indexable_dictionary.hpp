#ifndef CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_PARTIALLY_PERSISTENT_PARTIALLY_PERSISTENT_FULLY_INDEXABLE_DICTIONARY_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_PARTIALLY_PERSISTENT_PARTIALLY_PERSISTENT_FULLY_INDEXABLE_DICTIONARY_HPP_INCLUDED

#include <array>
#include "../../array/static_or_vector.hpp"
#include "../../array/static_or_vector.hpp"
#include <bit>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

template<int MAX_SIZE = -1, int MAX_VERSION = -1>
struct PartiallyPersistentFullyIndexableDictionary{
    static_assert(MAX_SIZE == -1 || MAX_SIZE >= 0);
    static_assert(MAX_VERSION == -1 || MAX_VERSION >= 0);

private:
    static constexpr int fixed_word_count = MAX_SIZE < 0 ? 0 : MAX_SIZE / 64 + (MAX_SIZE % 64 != 0);
    int word_count = 0;

    static constexpr int tree_height(){
        int h = 0;
        int n = fixed_word_count;
        while(n > 1){
            n = (n + 1) >> 1;
            h++;
        }
        return h;
    }

    static constexpr int max_nodes = fixed_word_count == 0
        ? 1
        : 2 * fixed_word_count + (MAX_VERSION + 1) * (tree_height() + 2) + 5;

    struct Node{
        int left = 0;
        int right = 0;
        int sum = 0;
        std::uint64_t word = 0;
    };

    int _n = 0;
    int _versions = 0;
    int node_count = 0;
    cpplib_detail::StaticOrVector<int, (MAX_VERSION < 0 ? -1 : static_cast<std::ptrdiff_t>(MAX_VERSION + 1))> root{};
    cpplib_detail::StaticOrVector<Node, (MAX_SIZE < 0 || MAX_VERSION < 0 ? -1 : max_nodes)> nodes{};

    void check_version(int version, const char* message) const{
        if(version < 0 || _versions <= version)[[unlikely]] throw std::runtime_error(message);
    }

    void check_index(int k, const char* message) const{
        if(k < 0 || _n <= k)[[unlikely]] throw std::runtime_error(message);
    }

    void check_position(int k, const char* message) const{
        if(k < 0 || _n < k)[[unlikely]] throw std::runtime_error(message);
    }

    void check_capacity(){
        if constexpr(MAX_VERSION < 0) root.ensure(cpplib_detail::checked_int_size(static_cast<std::size_t>(_versions) + 1));
        if((MAX_VERSION >= 0 && _versions > MAX_VERSION))[[unlikely]]{
            throw std::runtime_error("library assertion fault: capacity violation (version).");
        }
    }

    int new_node(){
        if(MAX_SIZE >= 0 && MAX_VERSION >= 0 && node_count >= max_nodes)[[unlikely]]{
            throw std::runtime_error("library assertion fault: capacity violation (node).");
        }
        nodes.ensure(static_cast<std::size_t>(node_count) + 1);
        return node_count++;
    }

    int build(const std::vector<std::uint64_t>& words, int l, int r){
        int id = new_node();
        if(r - l == 1){
            nodes[static_cast<std::size_t>(id)].word = words[static_cast<std::size_t>(l)];
            nodes[static_cast<std::size_t>(id)].sum =
                std::popcount(words[static_cast<std::size_t>(l)]);
            return id;
        }
        int m = (l + r) >> 1;
        int left = build(words, l, m);
        int right = build(words, m, r);
        nodes[static_cast<std::size_t>(id)].left = left;
        nodes[static_cast<std::size_t>(id)].right = right;
        nodes[static_cast<std::size_t>(id)].sum =
            nodes[static_cast<std::size_t>(left)].sum + nodes[static_cast<std::size_t>(right)].sum;
        return id;
    }

    int set_word(int id, int l, int r, int pos, std::uint64_t word){
        int next = new_node();
        nodes[static_cast<std::size_t>(next)] = nodes[static_cast<std::size_t>(id)];
        if(r - l == 1){
            nodes[static_cast<std::size_t>(next)].word = word;
            nodes[static_cast<std::size_t>(next)].sum = std::popcount(word);
            return next;
        }
        int m = (l + r) >> 1;
        if(pos < m){
            nodes[static_cast<std::size_t>(next)].left =
                set_word(nodes[static_cast<std::size_t>(id)].left, l, m, pos, word);
        }else{
            nodes[static_cast<std::size_t>(next)].right =
                set_word(nodes[static_cast<std::size_t>(id)].right, m, r, pos, word);
        }
        int left = nodes[static_cast<std::size_t>(next)].left;
        int right = nodes[static_cast<std::size_t>(next)].right;
        nodes[static_cast<std::size_t>(next)].sum =
            nodes[static_cast<std::size_t>(left)].sum + nodes[static_cast<std::size_t>(right)].sum;
        return next;
    }

    int sum_words(int id, int l, int r, int ql, int qr) const{
        if(qr <= l || r <= ql) return 0;
        if(ql <= l && r <= qr) return nodes[static_cast<std::size_t>(id)].sum;
        int m = (l + r) >> 1;
        return sum_words(nodes[static_cast<std::size_t>(id)].left, l, m, ql, qr) +
            sum_words(nodes[static_cast<std::size_t>(id)].right, m, r, ql, qr);
    }

    std::uint64_t get_word(int id, int l, int r, int pos) const{
        if(r - l == 1) return nodes[static_cast<std::size_t>(id)].word;
        int m = (l + r) >> 1;
        if(pos < m) return get_word(nodes[static_cast<std::size_t>(id)].left, l, m, pos);
        return get_word(nodes[static_cast<std::size_t>(id)].right, m, r, pos);
    }

public:
    explicit PartiallyPersistentFullyIndexableDictionary(int n = 0): _n(n), _versions(1){
        if(n < 0 || (MAX_SIZE >= 0 && MAX_SIZE < n))[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        word_count = MAX_SIZE < 0 ? _n / 64 + (_n % 64 != 0) : fixed_word_count;
        root.ensure(1);
        if(word_count > 0){
            std::vector<std::uint64_t> words(static_cast<std::size_t>(word_count));
            root[0] = build(words, 0, word_count);
        }
    }

    explicit PartiallyPersistentFullyIndexableDictionary(const std::vector<bool>& values):
        _n(cpplib_detail::checked_int_size(values.size())), _versions(1){
        if(values.size() > static_cast<std::size_t>(MAX_SIZE))[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        word_count = MAX_SIZE < 0 ? _n / 64 + (_n % 64 != 0) : fixed_word_count;
        root.ensure(1);
        if(word_count > 0){
            std::vector<std::uint64_t> words(static_cast<std::size_t>(word_count));
            for(int i = 0; i < _n; i++){
                if(values[static_cast<std::size_t>(i)]){
                    words[static_cast<std::size_t>(i >> 6)] |= std::uint64_t{1} << (i & 63);
                }
            }
            root[0] = build(words, 0, word_count);
        }
    }

    int size() const{ return _n; }
    int versions() const{ return _versions; }
    int latest_version() const{ return _versions - 1; }

    bool get(int version, int k) const{
        check_version(version, "library assertion fault: range violation (get).");
        check_index(k, "library assertion fault: range violation (get).");
        if(word_count == 0) return false;
        std::uint64_t word = get_word(root[static_cast<std::size_t>(version)], 0, word_count, k >> 6);
        return (word >> (k & 63)) & 1;
    }

    int set(int k, bool value){
        check_index(k, "library assertion fault: range violation (set).");
        check_capacity();
        _versions++;
        if(word_count == 0){
            root[static_cast<std::size_t>(_versions - 1)] = 0;
            return _versions - 1;
        }else{
            int word_index = k >> 6;
            std::uint64_t word = get_word(
                root[static_cast<std::size_t>(_versions - 2)], 0, word_count, word_index);
            std::uint64_t mask = std::uint64_t{1} << (k & 63);
            if(value) word |= mask;
            else word &= ~mask;
            root[static_cast<std::size_t>(_versions - 1)] = set_word(
                root[static_cast<std::size_t>(_versions - 2)], 0, word_count, word_index, word);
            return _versions - 1;
        }
    }

    int flip(int k){ return set(k, !get(_versions - 1, k)); }

    int rank(int version, bool value, int r) const{
        check_version(version, "library assertion fault: range violation (rank).");
        check_position(r, "library assertion fault: range violation (rank).");
        if(word_count == 0) return 0;
        int word_index = r >> 6;
        int ones = sum_words(root[static_cast<std::size_t>(version)], 0, word_count, 0, word_index);
        if((r & 63) != 0 && word_index < word_count){
            std::uint64_t word = get_word(
                root[static_cast<std::size_t>(version)], 0, word_count, word_index);
            ones += std::popcount(word & ((std::uint64_t{1} << (r & 63)) - 1));
        }
        return value ? ones : r - ones;
    }

    int rank(int version, bool value, int l, int r) const{
        if(l < 0 || r < l || _n < r)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (rank).");
        }
        return rank(version, value, r) - rank(version, value, l);
    }

    int select(int version, bool value, int k) const{
        check_version(version, "library assertion fault: range violation (select).");
        if(k < 0)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (select).");
        }
        if(rank(version, value, _n) <= k) return _n;
        int low = 0, high = _n;
        while(low < high){
            int mid = low + (high - low) / 2;
            if(rank(version, value, mid + 1) <= k) low = mid + 1;
            else high = mid;
        }
        return low;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_PARTIALLY_PERSISTENT_PARTIALLY_PERSISTENT_FULLY_INDEXABLE_DICTIONARY_HPP_INCLUDED
