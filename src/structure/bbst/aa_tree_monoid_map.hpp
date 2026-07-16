#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <functional>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>

template<
    class Key,
    auto Monoid,
    int MAX_SIZE,
    class Compare = std::less<Key>
>
struct AATreeMonoidMap{
    static_assert(MAX_SIZE >= 0);

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

    std::array<Node, static_cast<std::size_t>(MAX_SIZE)> nodes{};
    std::array<int, static_cast<std::size_t>(MAX_SIZE)> free_indices{};
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
        Node& current = nodes[static_cast<std::size_t>(node)];
        current.subtree_size =
            1 + size_of(current.left) + size_of(current.right);
        current.aggregate.emplace(Monoid.op(
            Monoid.op(
                aggregate_of(current.left),
                current.value->second
            ),
            aggregate_of(current.right)
        ));
    }

    int allocate(const Key& key, const mapped_type& mapped){
        if(element_count >= MAX_SIZE)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: capacity violation "
                "(AATreeMonoidMap)."
            );
        }
        const bool reuse = free_count != 0;
        const int index = reuse
            ? free_indices[static_cast<std::size_t>(free_count - 1)]
            : allocated;
        Node& node = nodes[static_cast<std::size_t>(index)];
        node.value.emplace(key, mapped);
        node.aggregate.emplace(mapped);
        if(reuse) --free_count;
        else ++allocated;
        node.left = -1;
        node.right = -1;
        node.level = 1;
        node.subtree_size = 1;
        ++element_count;
        return index;
    }

    void release(int node) noexcept{
        Node& current = nodes[static_cast<std::size_t>(node)];
        current.value.reset();
        current.aggregate.reset();
        free_indices[static_cast<std::size_t>(free_count++)] = node;
        --element_count;
    }

    int skew(int node){
        if(node == -1) return node;
        Node& current = nodes[static_cast<std::size_t>(node)];
        if(current.left == -1 || level_of(current.left) != current.level){
            pull(node);
            return node;
        }
        const int top = current.left;
        current.left = nodes[static_cast<std::size_t>(top)].right;
        nodes[static_cast<std::size_t>(top)].right = node;
        pull(node);
        pull(top);
        return top;
    }

    int split(int node){
        if(node == -1) return node;
        Node& current = nodes[static_cast<std::size_t>(node)];
        if(current.right == -1 ||
           nodes[static_cast<std::size_t>(current.right)].right == -1 ||
           level_of(nodes[static_cast<std::size_t>(current.right)].right)
               != current.level){
            pull(node);
            return node;
        }
        const int top = current.right;
        current.right = nodes[static_cast<std::size_t>(top)].left;
        nodes[static_cast<std::size_t>(top)].left = node;
        ++nodes[static_cast<std::size_t>(top)].level;
        pull(node);
        pull(top);
        return top;
    }

    int rebalance_after_erase(int node){
        if(node == -1) return node;
        Node& current = nodes[static_cast<std::size_t>(node)];
        const int expected_level =
            std::min(level_of(current.left), level_of(current.right)) + 1;
        if(expected_level < current.level){
            current.level = expected_level;
            if(level_of(current.right) > expected_level){
                nodes[static_cast<std::size_t>(current.right)].level =
                    expected_level;
            }
        }
        node = skew(node);
        Node& after_first_skew = nodes[static_cast<std::size_t>(node)];
        after_first_skew.right = skew(after_first_skew.right);
        if(after_first_skew.right != -1){
            Node& right =
                nodes[static_cast<std::size_t>(after_first_skew.right)];
            right.right = skew(right.right);
            pull(after_first_skew.right);
        }
        node = split(node);
        Node& after_split = nodes[static_cast<std::size_t>(node)];
        after_split.right = split(after_split.right);
        if(after_split.right != -1) pull(after_split.right);
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
        Node& current = nodes[static_cast<std::size_t>(node)];
        if(compare(key, current.value->first)){
            current.left = insert_node(
                current.left, key, mapped, assign_existing, inserted
            );
        }else if(compare(current.value->first, key)){
            current.right = insert_node(
                current.right, key, mapped, assign_existing, inserted
            );
        }else{
            if(assign_existing){
                current.value->second = mapped;
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
        Node& current = nodes[static_cast<std::size_t>(node)];
        if(compare(key, current.value->first)){
            current.left = erase_node(current.left, key, erased);
        }else if(compare(current.value->first, key)){
            current.right = erase_node(current.right, key, erased);
        }else{
            erased = true;
            if(current.left == -1 || current.right == -1){
                const int replacement =
                    current.left == -1 ? current.right : current.left;
                release(node);
                return replacement;
            }
            int successor = current.right;
            while(nodes[static_cast<std::size_t>(successor)].left != -1){
                successor = nodes[static_cast<std::size_t>(successor)].left;
            }
            current.value = nodes[static_cast<std::size_t>(successor)].value;
            bool successor_erased = false;
            current.right = erase_node(
                current.right, current.value->first, successor_erased
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
        Node& current = nodes[static_cast<std::size_t>(node)];
        bool found = false;
        if(compare(key, current.value->first)){
            found = set_node(current.left, key, mapped);
        }else if(compare(current.value->first, key)){
            found = set_node(current.right, key, mapped);
        }else{
            current.value->second = mapped;
            found = true;
        }
        if(found) pull(node);
        return found;
    }

    mapped_type product_from(int node, const Key& lower) const{
        mapped_type result = Monoid.e();
        while(node != -1){
            const Node& current = nodes[static_cast<std::size_t>(node)];
            if(compare(current.value->first, lower)){
                node = current.right;
            }else{
                result = Monoid.op(
                    Monoid.op(
                        current.value->second,
                        aggregate_of(current.right)
                    ),
                    result
                );
                node = current.left;
            }
        }
        return result;
    }

    mapped_type product_until(int node, const Key& upper) const{
        mapped_type result = Monoid.e();
        while(node != -1){
            const Node& current = nodes[static_cast<std::size_t>(node)];
            if(!compare(current.value->first, upper)){
                node = current.left;
            }else{
                result = Monoid.op(
                    result,
                    Monoid.op(
                        aggregate_of(current.left),
                        current.value->second
                    )
                );
                node = current.right;
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
            const Node& current = nodes[static_cast<std::size_t>(node)];
            if(compare(current.value->first, lower)){
                node = current.right;
            }else if(!compare(current.value->first, upper)){
                node = current.left;
            }else{
                return Monoid.op(
                    product_from(current.left, lower),
                    Monoid.op(
                        current.value->second,
                        product_until(current.right, upper)
                    )
                );
            }
        }
        return Monoid.e();
    }

public:
    explicit AATreeMonoidMap(const Compare& compare_ = Compare()):
        compare(compare_){}

    static constexpr int capacity() noexcept{ return MAX_SIZE; }
    int size() const noexcept{ return element_count; }
    bool empty() const noexcept{ return element_count == 0; }

    void clear() noexcept{
        for(int index = 0; index < allocated; ++index){
            Node& node = nodes[static_cast<std::size_t>(index)];
            node.value.reset();
            node.aggregate.reset();
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
            const Node& current = nodes[static_cast<std::size_t>(node)];
            if(!compare(current.value->first, key)){
                candidate = node;
                node = current.left;
            }else{
                node = current.right;
            }
        }
        return candidate == -1 ? nullptr
            : &*nodes[static_cast<std::size_t>(candidate)].value;
    }

    const value_type* upper_bound(const Key& key) const{
        int node = root;
        int candidate = -1;
        while(node != -1){
            const Node& current = nodes[static_cast<std::size_t>(node)];
            if(compare(key, current.value->first)){
                candidate = node;
                node = current.left;
            }else{
                node = current.right;
            }
        }
        return candidate == -1 ? nullptr
            : &*nodes[static_cast<std::size_t>(candidate)].value;
    }

    int order_of_key(const Key& key) const{
        int result = 0;
        int node = root;
        while(node != -1){
            const Node& current = nodes[static_cast<std::size_t>(node)];
            if(compare(current.value->first, key)){
                result += size_of(current.left) + 1;
                node = current.right;
            }else{
                node = current.left;
            }
        }
        return result;
    }

    int order_of_key_inclusive(const Key& key) const{
        int result = 0;
        int node = root;
        while(node != -1){
            const Node& current = nodes[static_cast<std::size_t>(node)];
            if(compare(key, current.value->first)){
                node = current.left;
            }else{
                result += size_of(current.left) + 1;
                node = current.right;
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
            const Node& current = nodes[static_cast<std::size_t>(node)];
            const int left_size = size_of(current.left);
            if(index < left_size){
                node = current.left;
            }else if(index == left_size){
                return *current.value;
            }else{
                index -= left_size + 1;
                node = current.right;
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
