#ifndef CPPLIB_SRC_STRUCTURE_BBST_MAP_AA_TREE_MONOID_MAP_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_BBST_MAP_AA_TREE_MONOID_MAP_HPP_INCLUDED

#include <algorithm>
#include <array>
#include "../../array/static_or_vector.hpp"
#include <cstddef>
#include <limits>
#include <functional>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>

template<
    class Key,
    auto Monoid,
    int MAX_SIZE = -1,
    class Compare = std::less<Key>
>
struct AATreeMonoidMap{
    static_assert(MAX_SIZE == -1 || MAX_SIZE >= 0);

    using monoid_type = std::remove_cvref_t<decltype(Monoid)>;
    using mapped_type = typename monoid_type::S;
    using value_type = std::pair<Key, mapped_type>;

private:
    struct Node{
        std::optional<value_type> value;
        std::optional<mapped_type> aggregate;
        int left = -1;
        int right = -1;
        int level = 1;
        int subtree_size = 1;
    };

    cpplib_detail::StaticOrVector<Node, (MAX_SIZE < 0 ? -1 : static_cast<std::ptrdiff_t>(static_cast<std::size_t>(MAX_SIZE)))> nodes{};
    cpplib_detail::StaticOrVector<int, (MAX_SIZE < 0 ? -1 : static_cast<std::ptrdiff_t>(static_cast<std::size_t>(MAX_SIZE)))> free_indices{};
    int root = -1;
    int allocated = 0;
    int free_count = 0;
    int element_count = 0;
    [[no_unique_address]] Compare compare;

    int level_of(int node) const noexcept{
        return node == -1 ? 0 : nodes[static_cast<std::size_t>(node)].level;
    }

    int size_of(int node) const noexcept{
        return node == -1 ? 0
            : nodes[static_cast<std::size_t>(node)].subtree_size;
    }

    mapped_type aggregate_of(int node) const{
        return node == -1 ? Monoid.e()
            : *nodes[static_cast<std::size_t>(node)].aggregate;
    }

    void pull(int node){
        if(node == -1) return;
        const auto current_node_index_1 = static_cast<std::size_t>(node);
        nodes[current_node_index_1].subtree_size =
            1 + size_of(nodes[current_node_index_1].left) + size_of(nodes[current_node_index_1].right);
        nodes[current_node_index_1].aggregate.emplace(Monoid.op(
            Monoid.op(
                aggregate_of(nodes[current_node_index_1].left),
                nodes[current_node_index_1].value->second
            ),
            aggregate_of(nodes[current_node_index_1].right)
        ));
    }

    int allocate(const Key& key, const mapped_type& mapped){
        if constexpr(MAX_SIZE < 0){nodes.ensure(cpplib_detail::checked_int_size(static_cast<std::size_t>(allocated) + 1)); free_indices.ensure(cpplib_detail::checked_int_size(static_cast<std::size_t>(allocated) + 1));}
        if((MAX_SIZE >= 0 && element_count >= MAX_SIZE))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: capacity violation "
                "(AATreeMonoidMap)."
            );
        }
        const bool reuse = free_count != 0;
        const int index = reuse
            ? free_indices[static_cast<std::size_t>(free_count - 1)]
            : allocated;
        const auto node_node_index_2 = static_cast<std::size_t>(index);
        nodes[node_node_index_2].value.emplace(key, mapped);
        nodes[node_node_index_2].aggregate.emplace(mapped);
        if(reuse) --free_count;
        else ++allocated;
        nodes[node_node_index_2].left = -1;
        nodes[node_node_index_2].right = -1;
        nodes[node_node_index_2].level = 1;
        nodes[node_node_index_2].subtree_size = 1;
        ++element_count;
        return index;
    }

    void release(int node) noexcept{
        const auto current_node_index_3 = static_cast<std::size_t>(node);
        nodes[current_node_index_3].value.reset();
        nodes[current_node_index_3].aggregate.reset();
        free_indices[static_cast<std::size_t>(free_count++)] = node;
        --element_count;
    }

    int skew(int node){
        if(node == -1) return node;
        const auto current_node_index_4 = static_cast<std::size_t>(node);
        if(nodes[current_node_index_4].left == -1 || level_of(nodes[current_node_index_4].left) != nodes[current_node_index_4].level){
            pull(node);
            return node;
        }
        const int top = nodes[current_node_index_4].left;
        nodes[current_node_index_4].left = nodes[static_cast<std::size_t>(top)].right;
        nodes[static_cast<std::size_t>(top)].right = node;
        pull(node);
        pull(top);
        return top;
    }

    int split(int node){
        if(node == -1) return node;
        const auto current_node_index_5 = static_cast<std::size_t>(node);
        if(nodes[current_node_index_5].right == -1 ||
           nodes[static_cast<std::size_t>(nodes[current_node_index_5].right)].right == -1 ||
           level_of(nodes[static_cast<std::size_t>(nodes[current_node_index_5].right)].right)
               != nodes[current_node_index_5].level){
            pull(node);
            return node;
        }
        const int top = nodes[current_node_index_5].right;
        nodes[current_node_index_5].right = nodes[static_cast<std::size_t>(top)].left;
        nodes[static_cast<std::size_t>(top)].left = node;
        ++nodes[static_cast<std::size_t>(top)].level;
        pull(node);
        pull(top);
        return top;
    }

    int rebalance_after_erase(int node){
        if(node == -1) return node;
        const auto current_node_index_6 = static_cast<std::size_t>(node);
        const int expected_level =
            std::min(level_of(nodes[current_node_index_6].left), level_of(nodes[current_node_index_6].right)) + 1;
        if(expected_level < nodes[current_node_index_6].level){
            nodes[current_node_index_6].level = expected_level;
            if(level_of(nodes[current_node_index_6].right) > expected_level){
                nodes[static_cast<std::size_t>(nodes[current_node_index_6].right)].level =
                    expected_level;
            }
        }
        node = skew(node);
        const auto after_first_skew_node_index_7 = static_cast<std::size_t>(node);
        nodes[after_first_skew_node_index_7].right = skew(nodes[after_first_skew_node_index_7].right);
        if(nodes[after_first_skew_node_index_7].right != -1){
            const auto right_node_index_8 = static_cast<std::size_t>(nodes[after_first_skew_node_index_7].right);
            nodes[right_node_index_8].right = skew(nodes[right_node_index_8].right);
            pull(nodes[after_first_skew_node_index_7].right);
        }
        node = split(node);
        const auto after_split_node_index_9 = static_cast<std::size_t>(node);
        nodes[after_split_node_index_9].right = split(nodes[after_split_node_index_9].right);
        if(nodes[after_split_node_index_9].right != -1) pull(nodes[after_split_node_index_9].right);
        pull(node);
        return node;
    }

    int insert_node(
        int node,
        const Key& key,
        const mapped_type& mapped,
        bool assign_existing,
        bool& inserted
    ){
        if(node == -1){
            inserted = true;
            return allocate(key, mapped);
        }
        const auto current_node_index_10 = static_cast<std::size_t>(node);
        if(compare(key, nodes[current_node_index_10].value->first)){
            nodes[current_node_index_10].left = insert_node(
                nodes[current_node_index_10].left, key, mapped, assign_existing, inserted
            );
        }else if(compare(nodes[current_node_index_10].value->first, key)){
            nodes[current_node_index_10].right = insert_node(
                nodes[current_node_index_10].right, key, mapped, assign_existing, inserted
            );
        }else{
            if(assign_existing){
                nodes[current_node_index_10].value->second = mapped;
                pull(node);
            }
            inserted = false;
            return node;
        }
        node = skew(node);
        node = split(node);
        pull(node);
        return node;
    }

    int erase_node(int node, const Key& key, bool& erased){
        if(node == -1) return node;
        const auto current_node_index_11 = static_cast<std::size_t>(node);
        if(compare(key, nodes[current_node_index_11].value->first)){
            nodes[current_node_index_11].left = erase_node(nodes[current_node_index_11].left, key, erased);
        }else if(compare(nodes[current_node_index_11].value->first, key)){
            nodes[current_node_index_11].right = erase_node(nodes[current_node_index_11].right, key, erased);
        }else{
            erased = true;
            if(nodes[current_node_index_11].left == -1 || nodes[current_node_index_11].right == -1){
                const int replacement =
                    nodes[current_node_index_11].left == -1 ? nodes[current_node_index_11].right : nodes[current_node_index_11].left;
                release(node);
                return replacement;
            }
            int successor = nodes[current_node_index_11].right;
            while(nodes[static_cast<std::size_t>(successor)].left != -1){
                successor = nodes[static_cast<std::size_t>(successor)].left;
            }
            nodes[current_node_index_11].value = nodes[static_cast<std::size_t>(successor)].value;
            bool successor_erased = false;
            nodes[current_node_index_11].right = erase_node(
                nodes[current_node_index_11].right, nodes[current_node_index_11].value->first, successor_erased
            );
        }
        return rebalance_after_erase(node);
    }

    int find_node(const Key& key) const{
        int node = root;
        while(node != -1){
            const value_type& value =
                *nodes[static_cast<std::size_t>(node)].value;
            if(compare(key, value.first)){
                node = nodes[static_cast<std::size_t>(node)].left;
            }else if(compare(value.first, key)){
                node = nodes[static_cast<std::size_t>(node)].right;
            }else{
                return node;
            }
        }
        return -1;
    }

    bool set_node(int node, const Key& key, const mapped_type& mapped){
        if(node == -1) return false;
        const auto current_node_index_12 = static_cast<std::size_t>(node);
        bool found = false;
        if(compare(key, nodes[current_node_index_12].value->first)){
            found = set_node(nodes[current_node_index_12].left, key, mapped);
        }else if(compare(nodes[current_node_index_12].value->first, key)){
            found = set_node(nodes[current_node_index_12].right, key, mapped);
        }else{
            nodes[current_node_index_12].value->second = mapped;
            found = true;
        }
        if(found) pull(node);
        return found;
    }

    mapped_type product_from(int node, const Key& lower) const{
        mapped_type result = Monoid.e();
        while(node != -1){
            const auto current_node_index_13 = static_cast<std::size_t>(node);
            if(compare(nodes[current_node_index_13].value->first, lower)){
                node = nodes[current_node_index_13].right;
            }else{
                result = Monoid.op(
                    Monoid.op(
                        nodes[current_node_index_13].value->second,
                        aggregate_of(nodes[current_node_index_13].right)
                    ),
                    result
                );
                node = nodes[current_node_index_13].left;
            }
        }
        return result;
    }

    mapped_type product_until(int node, const Key& upper) const{
        mapped_type result = Monoid.e();
        while(node != -1){
            const auto current_node_index_14 = static_cast<std::size_t>(node);
            if(!compare(nodes[current_node_index_14].value->first, upper)){
                node = nodes[current_node_index_14].left;
            }else{
                result = Monoid.op(
                    result,
                    Monoid.op(
                        aggregate_of(nodes[current_node_index_14].left),
                        nodes[current_node_index_14].value->second
                    )
                );
                node = nodes[current_node_index_14].right;
            }
        }
        return result;
    }

    mapped_type product_range(
        int node,
        const Key& lower,
        const Key& upper
    ) const{
        while(node != -1){
            const auto current_node_index_15 = static_cast<std::size_t>(node);
            if(compare(nodes[current_node_index_15].value->first, lower)){
                node = nodes[current_node_index_15].right;
            }else if(!compare(nodes[current_node_index_15].value->first, upper)){
                node = nodes[current_node_index_15].left;
            }else{
                return Monoid.op(
                    product_from(nodes[current_node_index_15].left, lower),
                    Monoid.op(
                        nodes[current_node_index_15].value->second,
                        product_until(nodes[current_node_index_15].right, upper)
                    )
                );
            }
        }
        return Monoid.e();
    }

public:
    explicit AATreeMonoidMap(const Compare& compare_ = Compare()):
        compare(compare_){}

    static constexpr int capacity() noexcept{ return MAX_SIZE < 0 ? std::numeric_limits<int>::max() : MAX_SIZE; }
    int size() const noexcept{ return element_count; }
    bool empty() const noexcept{ return element_count == 0; }

    void clear() noexcept{
        for(int index = 0; index < allocated; ++index){
            const auto node_node_index_16 = static_cast<std::size_t>(index);
            nodes[node_node_index_16].value.reset();
            nodes[node_node_index_16].aggregate.reset();
        }
        root = -1;
        allocated = 0;
        free_count = 0;
        element_count = 0;
    }

    bool insert(const Key& key, const mapped_type& mapped){
        bool inserted = false;
        root = insert_node(root, key, mapped, false, inserted);
        return inserted;
    }

    bool insert_or_assign(const Key& key, const mapped_type& mapped){
        bool inserted = false;
        root = insert_node(root, key, mapped, true, inserted);
        return inserted;
    }

    bool set(const Key& key, const mapped_type& mapped){
        return set_node(root, key, mapped);
    }

    bool erase(const Key& key){
        bool erased = false;
        root = erase_node(root, key, erased);
        return erased;
    }

    bool contains(const Key& key) const{ return find_node(key) != -1; }

    const mapped_type* find(const Key& key) const{
        const int node = find_node(key);
        return node == -1 ? nullptr
            : &nodes[static_cast<std::size_t>(node)].value->second;
    }

    const mapped_type& at(const Key& key) const{
        const mapped_type* result = find(key);
        if(result == nullptr)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: missing key "
                "(AATreeMonoidMap::at)."
            );
        }
        return *result;
    }

    const value_type* lower_bound(const Key& key) const{
        int node = root;
        int candidate = -1;
        while(node != -1){
            const auto current_node_index_17 = static_cast<std::size_t>(node);
            if(!compare(nodes[current_node_index_17].value->first, key)){
                candidate = node;
                node = nodes[current_node_index_17].left;
            }else{
                node = nodes[current_node_index_17].right;
            }
        }
        return candidate == -1 ? nullptr
            : &*nodes[static_cast<std::size_t>(candidate)].value;
    }

    const value_type* upper_bound(const Key& key) const{
        int node = root;
        int candidate = -1;
        while(node != -1){
            const auto current_node_index_18 = static_cast<std::size_t>(node);
            if(compare(key, nodes[current_node_index_18].value->first)){
                candidate = node;
                node = nodes[current_node_index_18].left;
            }else{
                node = nodes[current_node_index_18].right;
            }
        }
        return candidate == -1 ? nullptr
            : &*nodes[static_cast<std::size_t>(candidate)].value;
    }

    int order_of_key(const Key& key) const{
        int result = 0;
        int node = root;
        while(node != -1){
            const auto current_node_index_19 = static_cast<std::size_t>(node);
            if(compare(nodes[current_node_index_19].value->first, key)){
                result += size_of(nodes[current_node_index_19].left) + 1;
                node = nodes[current_node_index_19].right;
            }else{
                node = nodes[current_node_index_19].left;
            }
        }
        return result;
    }

    int order_of_key_inclusive(const Key& key) const{
        int result = 0;
        int node = root;
        while(node != -1){
            const auto current_node_index_20 = static_cast<std::size_t>(node);
            if(compare(key, nodes[current_node_index_20].value->first)){
                node = nodes[current_node_index_20].left;
            }else{
                result += size_of(nodes[current_node_index_20].left) + 1;
                node = nodes[current_node_index_20].right;
            }
        }
        return result;
    }

    const value_type& kth(int index) const{
        if(index < 0 || index >= element_count)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: order out of range "
                "(AATreeMonoidMap::kth)."
            );
        }
        int node = root;
        while(true){
            const auto current_node_index_21 = static_cast<std::size_t>(node);
            const int left_size = size_of(nodes[current_node_index_21].left);
            if(index < left_size){
                node = nodes[current_node_index_21].left;
            }else if(index == left_size){
                return *nodes[current_node_index_21].value;
            }else{
                index -= left_size + 1;
                node = nodes[current_node_index_21].right;
            }
        }
    }

    mapped_type prod(const Key& lower, const Key& upper) const{
        if(!compare(lower, upper)) return Monoid.e();
        return product_range(root, lower, upper);
    }

    mapped_type prefix_prod(const Key& upper) const{
        return product_until(root, upper);
    }

    mapped_type all_prod() const{ return aggregate_of(root); }
};

#endif  // CPPLIB_SRC_STRUCTURE_BBST_MAP_AA_TREE_MONOID_MAP_HPP_INCLUDED
