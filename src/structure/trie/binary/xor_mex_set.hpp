#ifndef CPPLIB_SRC_STRUCTURE_TRIE_BINARY_XOR_MEX_SET_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_TRIE_BINARY_XOR_MEX_SET_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

template<unsigned BitWidth = 30>
class XorMexSet{
    static_assert(1 <= BitWidth && BitWidth <= 62);
    struct Node{
        std::array<int, 2> child{-1, -1};
        std::uint64_t distinct = 0;
        std::uint64_t multiplicity = 0;
    };

    static constexpr std::uint64_t limit_ = std::uint64_t{1} << BitWidth;
    std::vector<Node> nodes_{Node{}};
    std::uint64_t xor_mask_ = 0;

    void validate(std::uint64_t value) const{
        if(value >= limit_) throw std::out_of_range("XorMexSet: value is outside the bit universe");
    }

public:
    static constexpr std::uint64_t universe() noexcept{ return limit_; }
    std::uint64_t distinct_size() const noexcept{ return nodes_[0].distinct; }
    std::uint64_t xor_mask() const noexcept{ return xor_mask_; }

    bool insert(std::uint64_t logical_value){
        validate(logical_value);
        const std::uint64_t value = logical_value ^ xor_mask_;
        std::array<int, BitWidth + 1> path{};
        int node = 0;
        path[0] = node;
        for(unsigned depth = 0; depth < BitWidth; depth++){
            const unsigned bit_index = BitWidth - 1 - depth;
            const unsigned bit = static_cast<unsigned>((value >> bit_index) & 1U);
            if(nodes_[static_cast<std::size_t>(node)].child[bit] < 0){
                nodes_[static_cast<std::size_t>(node)].child[bit] = static_cast<int>(nodes_.size());
                nodes_.push_back(Node{});
            }
            node = nodes_[static_cast<std::size_t>(node)].child[bit];
            path[depth + 1] = node;
        }
        Node& leaf = nodes_[static_cast<std::size_t>(node)];
        const bool was_absent = leaf.multiplicity == 0;
        ++leaf.multiplicity;
        if(was_absent){
            for(const int index: path) ++nodes_[static_cast<std::size_t>(index)].distinct;
        }
        return was_absent;
    }

    bool erase(std::uint64_t logical_value){
        validate(logical_value);
        const std::uint64_t value = logical_value ^ xor_mask_;
        std::array<int, BitWidth + 1> path{};
        int node = 0;
        path[0] = node;
        for(unsigned depth = 0; depth < BitWidth; depth++){
            const unsigned bit_index = BitWidth - 1 - depth;
            const unsigned bit = static_cast<unsigned>((value >> bit_index) & 1U);
            node = nodes_[static_cast<std::size_t>(node)].child[bit];
            if(node < 0) return false;
            path[depth + 1] = node;
        }
        Node& leaf = nodes_[static_cast<std::size_t>(node)];
        if(leaf.multiplicity == 0) return false;
        --leaf.multiplicity;
        if(leaf.multiplicity == 0){
            for(const int index: path) --nodes_[static_cast<std::size_t>(index)].distinct;
        }
        return true;
    }

    void apply_xor(std::uint64_t value){
        validate(value);
        xor_mask_ ^= value;
    }

    std::uint64_t mex() const noexcept{
        if(nodes_[0].distinct == limit_) return limit_;
        int node = 0;
        std::uint64_t answer = 0;
        for(unsigned depth = 0; depth < BitWidth; depth++){
            const unsigned bit_index = BitWidth - 1 - depth;
            const unsigned mask_bit = static_cast<unsigned>((xor_mask_ >> bit_index) & 1U);
            const int zero_child = nodes_[static_cast<std::size_t>(node)].child[mask_bit];
            const std::uint64_t capacity = std::uint64_t{1} << bit_index;
            const std::uint64_t zero_count = zero_child < 0
                ? 0 : nodes_[static_cast<std::size_t>(zero_child)].distinct;
            if(zero_count < capacity){
                if(zero_child < 0) return answer;
                node = zero_child;
            }else{
                answer |= std::uint64_t{1} << bit_index;
                node = nodes_[static_cast<std::size_t>(node)].child[mask_bit ^ 1U];
                if(node < 0) return answer;
            }
        }
        return answer;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_TRIE_BINARY_XOR_MEX_SET_HPP_INCLUDED
