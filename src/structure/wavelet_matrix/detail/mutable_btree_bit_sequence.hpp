#ifndef CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_DETAIL_MUTABLE_BTREE_BIT_SEQUENCE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_DETAIL_MUTABLE_BTREE_BIT_SEQUENCE_HPP_INCLUDED

#include <cstddef>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace wavelet_matrix_detail{

struct MutableBTreeNoPayload{};

struct MutableBTreeNoSum{
    friend constexpr MutableBTreeNoSum operator+(
        MutableBTreeNoSum,
        MutableBTreeNoSum
    ){
        return {};
    }
    friend constexpr MutableBTreeNoSum operator-(
        MutableBTreeNoSum,
        MutableBTreeNoSum
    ){
        return {};
    }
};

template<class Payload, class Sum>
struct MutableBTreeDefaultLift{
    constexpr Sum operator()(const Payload& value) const{
        return static_cast<Sum>(value);
    }
};

struct MutableBTreeNoPayloadLift{
    constexpr MutableBTreeNoSum operator()(MutableBTreeNoPayload) const{
        return {};
    }
};

template<class Payload, bool StoresPayload>
struct MutableBTreePayloadStorage;

template<class Payload>
struct MutableBTreePayloadStorage<Payload, true>{
    std::vector<Payload> values;
};

template<class Payload>
struct MutableBTreePayloadStorage<Payload, false>{};

template<
    class Payload,
    class Sum,
    class Lift = MutableBTreeDefaultLift<Payload, Sum>,
    int LEAF_CAPACITY = 128,
    int INTERNAL_CAPACITY = 16
>
class MutableBTreeBitSequence{
    static_assert(LEAF_CAPACITY >= 4);
    static_assert(INTERNAL_CAPACITY >= 4);
    static_assert(std::is_default_constructible_v<Payload>);
    static_assert(std::is_copy_constructible_v<Payload>);
    static_assert(std::is_copy_assignable_v<Payload>);
    static_assert(std::is_default_constructible_v<Sum>);
    static_assert(std::is_copy_constructible_v<Sum>);
    static_assert(std::is_copy_assignable_v<Sum>);

public:
    static constexpr bool stores_payload =
        !std::is_same_v<Payload, MutableBTreeNoPayload>;

    struct Entry{
        bool bit = false;
        [[no_unique_address]] Payload payload{};
    };

    struct Stats{
        int size = 0;
        int ones = 0;
        Sum sum{};
        Sum zero_sum{};
        Sum one_sum{};
    };

private:
    static constexpr int leaf_minimum = LEAF_CAPACITY / 2;
    static constexpr int internal_minimum = INTERNAL_CAPACITY / 2;

    struct Node{
        bool is_leaf;
        Stats stats{};
        explicit Node(bool leaf): is_leaf(leaf){}
        virtual ~Node() = default;
    };

    struct Leaf final : Node{
        std::vector<bool> bits;
        [[no_unique_address]]
        MutableBTreePayloadStorage<Payload, stores_payload> payload;

        Leaf(): Node(true){
            bits.reserve(static_cast<std::size_t>(LEAF_CAPACITY + 1));
            if constexpr(stores_payload){
                payload.values.reserve(
                    static_cast<std::size_t>(LEAF_CAPACITY + 1)
                );
            }
        }
    };

    struct Internal final : Node{
        std::vector<std::unique_ptr<Node>> children;
        Internal(): Node(false){
            children.reserve(static_cast<std::size_t>(INTERNAL_CAPACITY + 1));
        }
    };

    std::unique_ptr<Node> root = std::make_unique<Leaf>();
    [[no_unique_address]] Lift lift{};

    static Leaf* as_leaf(Node* node){
        return static_cast<Leaf*>(node);
    }
    static const Leaf* as_leaf(const Node* node){
        return static_cast<const Leaf*>(node);
    }
    static Internal* as_internal(Node* node){
        return static_cast<Internal*>(node);
    }
    static const Internal* as_internal(const Node* node){
        return static_cast<const Internal*>(node);
    }

    static int leaf_size(const Leaf* leaf){
        return static_cast<int>(leaf->bits.size());
    }

    static Entry leaf_entry(const Leaf* leaf, int position){
        Entry result;
        result.bit = leaf->bits[static_cast<std::size_t>(position)];
        if constexpr(stores_payload){
            result.payload = leaf->payload.values[
                static_cast<std::size_t>(position)
            ];
        }
        return result;
    }

    static void leaf_insert(Leaf* leaf, int position, Entry entry){
        leaf->bits.insert(
            leaf->bits.begin() + static_cast<std::ptrdiff_t>(position),
            entry.bit
        );
        if constexpr(stores_payload){
            leaf->payload.values.insert(
                leaf->payload.values.begin() +
                    static_cast<std::ptrdiff_t>(position),
                std::move(entry.payload)
            );
        }
    }

    static Entry leaf_erase(Leaf* leaf, int position){
        Entry result = leaf_entry(leaf, position);
        leaf->bits.erase(
            leaf->bits.begin() + static_cast<std::ptrdiff_t>(position)
        );
        if constexpr(stores_payload){
            leaf->payload.values.erase(
                leaf->payload.values.begin() +
                    static_cast<std::ptrdiff_t>(position)
            );
        }
        return result;
    }

    static void leaf_append(Leaf* left, Leaf* right){
        left->bits.insert(
            left->bits.end(),
            right->bits.begin(),
            right->bits.end()
        );
        right->bits.clear();
        if constexpr(stores_payload){
            left->payload.values.insert(
                left->payload.values.end(),
                std::make_move_iterator(right->payload.values.begin()),
                std::make_move_iterator(right->payload.values.end())
            );
            right->payload.values.clear();
        }
    }

    void recalculate(Leaf* leaf) const{
        leaf->stats = {};
        leaf->stats.size = leaf_size(leaf);
        for(int position = 0; position < leaf_size(leaf); position++){
            const bool bit = leaf->bits[static_cast<std::size_t>(position)];
            leaf->stats.ones += bit ? 1 : 0;
            if constexpr(stores_payload){
                const Sum value = lift(leaf->payload.values[
                    static_cast<std::size_t>(position)
                ]);
                leaf->stats.sum = leaf->stats.sum + value;
                if(bit){
                    leaf->stats.one_sum = leaf->stats.one_sum + value;
                }else{
                    leaf->stats.zero_sum = leaf->stats.zero_sum + value;
                }
            }
        }
    }

    static void recalculate(Internal* internal){
        internal->stats = {};
        for(const auto& child: internal->children){
            internal->stats.size += child->stats.size;
            internal->stats.ones += child->stats.ones;
            internal->stats.sum =
                internal->stats.sum + child->stats.sum;
            internal->stats.zero_sum =
                internal->stats.zero_sum + child->stats.zero_sum;
            internal->stats.one_sum =
                internal->stats.one_sum + child->stats.one_sum;
        }
    }

    static std::vector<int> balanced_group_sizes(int total, int capacity){
        const int group_count = (total + capacity - 1) / capacity;
        std::vector<int> result(
            static_cast<std::size_t>(group_count),
            total / group_count
        );
        for(int index = 0; index < total % group_count; index++){
            result[static_cast<std::size_t>(index)]++;
        }
        return result;
    }

    std::unique_ptr<Node> make_leaf(
        const std::vector<unsigned char>& bits,
        const std::vector<Payload>& payloads,
        int first,
        int length
    ) const{
        auto leaf = std::make_unique<Leaf>();
        for(int offset = 0; offset < length; offset++){
            Entry entry;
            entry.bit = bits[static_cast<std::size_t>(first + offset)] != 0;
            if constexpr(stores_payload){
                entry.payload =
                    payloads[static_cast<std::size_t>(first + offset)];
            }
            leaf_insert(leaf.get(), leaf_size(leaf.get()), std::move(entry));
        }
        recalculate(leaf.get());
        return leaf;
    }

    std::unique_ptr<Node> clone_node(const Node* node) const{
        if(node->is_leaf){
            auto result = std::make_unique<Leaf>();
            result->bits = as_leaf(node)->bits;
            if constexpr(stores_payload){
                result->payload.values = as_leaf(node)->payload.values;
            }
            recalculate(result.get());
            return result;
        }
        auto result = std::make_unique<Internal>();
        for(const auto& child: as_internal(node)->children){
            result->children.push_back(clone_node(child.get()));
        }
        recalculate(result.get());
        return result;
    }

    std::unique_ptr<Node> split_full_leaf(
        Leaf* leaf,
        int position,
        Entry entry
    ){
        leaf_insert(leaf, position, std::move(entry));
        const int left_size = leaf_size(leaf) / 2;
        auto right = std::make_unique<Leaf>();
        right->bits.insert(
            right->bits.end(),
            leaf->bits.begin() + left_size,
            leaf->bits.end()
        );
        leaf->bits.erase(leaf->bits.begin() + left_size, leaf->bits.end());
        if constexpr(stores_payload){
            right->payload.values.insert(
                right->payload.values.end(),
                std::make_move_iterator(
                    leaf->payload.values.begin() + left_size
                ),
                std::make_move_iterator(leaf->payload.values.end())
            );
            leaf->payload.values.erase(
                leaf->payload.values.begin() + left_size,
                leaf->payload.values.end()
            );
        }
        recalculate(leaf);
        recalculate(right.get());
        return right;
    }

    std::unique_ptr<Node> insert_recursive(
        std::unique_ptr<Node>& node,
        int position,
        Entry entry
    ){
        if(node->is_leaf){
            Leaf* leaf = as_leaf(node.get());
            if(leaf_size(leaf) < LEAF_CAPACITY){
                leaf_insert(leaf, position, std::move(entry));
                recalculate(leaf);
                return nullptr;
            }
            return split_full_leaf(leaf, position, std::move(entry));
        }

        Internal* internal = as_internal(node.get());
        std::size_t child_index = 0;
        int local_position = position;
        while(child_index + 1 < internal->children.size()){
            const int child_size =
                internal->children[child_index]->stats.size;
            if(local_position <= child_size) break;
            local_position -= child_size;
            child_index++;
        }
        auto sibling = insert_recursive(
            internal->children[child_index],
            local_position,
            std::move(entry)
        );
        if(sibling){
            internal->children.insert(
                internal->children.begin() +
                    static_cast<std::ptrdiff_t>(child_index + 1),
                std::move(sibling)
            );
        }
        recalculate(internal);
        if(static_cast<int>(internal->children.size()) <= INTERNAL_CAPACITY){
            return nullptr;
        }

        auto right = std::make_unique<Internal>();
        const int left_count =
            static_cast<int>(internal->children.size()) / 2;
        right->children.insert(
            right->children.end(),
            std::make_move_iterator(
                internal->children.begin() + left_count
            ),
            std::make_move_iterator(internal->children.end())
        );
        internal->children.erase(
            internal->children.begin() + left_count,
            internal->children.end()
        );
        recalculate(internal);
        recalculate(right.get());
        return right;
    }

    static bool is_underfull(const Node* node){
        if(node->is_leaf){
            return leaf_size(as_leaf(node)) <
                leaf_minimum;
        }
        return static_cast<int>(as_internal(node)->children.size()) <
            internal_minimum;
    }

    static bool can_lend(const Node* node){
        if(node->is_leaf){
            return leaf_size(as_leaf(node)) >
                leaf_minimum;
        }
        return static_cast<int>(as_internal(node)->children.size()) >
            internal_minimum;
    }

    void borrow_from_left(Node* left_node, Node* current_node){
        if(current_node->is_leaf){
            Leaf* left = as_leaf(left_node);
            Leaf* current = as_leaf(current_node);
            Entry entry = leaf_erase(left, leaf_size(left) - 1);
            leaf_insert(current, 0, std::move(entry));
            recalculate(left);
            recalculate(current);
            return;
        }
        Internal* left = as_internal(left_node);
        Internal* current = as_internal(current_node);
        current->children.insert(
            current->children.begin(),
            std::move(left->children.back())
        );
        left->children.pop_back();
        recalculate(left);
        recalculate(current);
    }

    void borrow_from_right(Node* current_node, Node* right_node){
        if(current_node->is_leaf){
            Leaf* current = as_leaf(current_node);
            Leaf* right = as_leaf(right_node);
            Entry entry = leaf_erase(right, 0);
            leaf_insert(current, leaf_size(current), std::move(entry));
            recalculate(current);
            recalculate(right);
            return;
        }
        Internal* current = as_internal(current_node);
        Internal* right = as_internal(right_node);
        current->children.push_back(std::move(right->children.front()));
        right->children.erase(right->children.begin());
        recalculate(current);
        recalculate(right);
    }

    void merge_nodes(Node* left_node, Node* right_node){
        if(left_node->is_leaf){
            Leaf* left = as_leaf(left_node);
            Leaf* right = as_leaf(right_node);
            leaf_append(left, right);
            recalculate(left);
            recalculate(right);
            return;
        }
        Internal* left = as_internal(left_node);
        Internal* right = as_internal(right_node);
        for(auto& child: right->children){
            left->children.push_back(std::move(child));
        }
        right->children.clear();
        recalculate(left);
        recalculate(right);
    }

    void rebalance_child(Internal* parent, std::size_t child_index){
        if(!is_underfull(parent->children[child_index].get())) return;
        if(child_index > 0 &&
           can_lend(parent->children[child_index - 1].get())){
            borrow_from_left(
                parent->children[child_index - 1].get(),
                parent->children[child_index].get()
            );
            recalculate(parent);
            return;
        }
        if(child_index + 1 < parent->children.size() &&
           can_lend(parent->children[child_index + 1].get())){
            borrow_from_right(
                parent->children[child_index].get(),
                parent->children[child_index + 1].get()
            );
            recalculate(parent);
            return;
        }
        if(child_index > 0){
            merge_nodes(
                parent->children[child_index - 1].get(),
                parent->children[child_index].get()
            );
            parent->children.erase(
                parent->children.begin() +
                    static_cast<std::ptrdiff_t>(child_index)
            );
        }else if(child_index + 1 < parent->children.size()){
            merge_nodes(
                parent->children[child_index].get(),
                parent->children[child_index + 1].get()
            );
            parent->children.erase(
                parent->children.begin() +
                    static_cast<std::ptrdiff_t>(child_index + 1)
            );
        }
        recalculate(parent);
    }

    Entry erase_recursive(std::unique_ptr<Node>& node, int position){
        if(node->is_leaf){
            Leaf* leaf = as_leaf(node.get());
            Entry result = leaf_erase(leaf, position);
            recalculate(leaf);
            return result;
        }
        Internal* internal = as_internal(node.get());
        std::size_t child_index = 0;
        int local_position = position;
        while(true){
            const int child_size =
                internal->children[child_index]->stats.size;
            if(local_position < child_size) break;
            local_position -= child_size;
            child_index++;
        }
        Entry result = erase_recursive(
            internal->children[child_index],
            local_position
        );
        rebalance_child(internal, child_index);
        recalculate(internal);
        return result;
    }

    Entry entry_recursive(const Node* node, int position) const{
        if(node->is_leaf){
            return leaf_entry(as_leaf(node), position);
        }
        for(const auto& child: as_internal(node)->children){
            if(position < child->stats.size){
                return entry_recursive(child.get(), position);
            }
            position -= child->stats.size;
        }
        throw std::logic_error("MutableBTreeBitSequence access failure.");
    }

    Entry set_entry_recursive(
        Node* node,
        int position,
        bool update_bit,
        bool bit,
        bool update_payload,
        const Payload& payload
    ){
        if(node->is_leaf){
            Leaf* leaf = as_leaf(node);
            Entry old = leaf_entry(leaf, position);
            if(update_bit){
                leaf->bits[static_cast<std::size_t>(position)] = bit;
            }
            if constexpr(stores_payload){
                if(update_payload){
                    leaf->payload.values[
                        static_cast<std::size_t>(position)
                    ] = payload;
                }
            }
            recalculate(leaf);
            return old;
        }
        Internal* internal = as_internal(node);
        for(auto& child: internal->children){
            if(position < child->stats.size){
                Entry result = set_entry_recursive(
                    child.get(),
                    position,
                    update_bit,
                    bit,
                    update_payload,
                    payload
                );
                recalculate(internal);
                return result;
            }
            position -= child->stats.size;
        }
        throw std::logic_error("MutableBTreeBitSequence update failure.");
    }

    Stats prefix_stats_recursive(const Node* node, int end) const{
        Stats result;
        result.size = end;
        if(node->is_leaf){
            const Leaf* leaf = as_leaf(node);
            for(int index = 0; index < end; index++){
                const bool bit =
                    leaf->bits[static_cast<std::size_t>(index)];
                result.ones += bit ? 1 : 0;
                if constexpr(stores_payload){
                    const Sum value = lift(leaf->payload.values[
                        static_cast<std::size_t>(index)
                    ]);
                    result.sum = result.sum + value;
                    if(bit){
                        result.one_sum = result.one_sum + value;
                    }else{
                        result.zero_sum = result.zero_sum + value;
                    }
                }
            }
            return result;
        }
        int remaining = end;
        for(const auto& child: as_internal(node)->children){
            if(remaining >= child->stats.size){
                result.ones += child->stats.ones;
                result.sum = result.sum + child->stats.sum;
                result.zero_sum =
                    result.zero_sum + child->stats.zero_sum;
                result.one_sum =
                    result.one_sum + child->stats.one_sum;
                remaining -= child->stats.size;
            }else{
                Stats suffix = prefix_stats_recursive(
                    child.get(),
                    remaining
                );
                result.ones += suffix.ones;
                result.sum = result.sum + suffix.sum;
                result.zero_sum =
                    result.zero_sum + suffix.zero_sum;
                result.one_sum = result.one_sum + suffix.one_sum;
                break;
            }
        }
        return result;
    }

    Sum sum_first_recursive(
        const Node* node,
        bool bit,
        int count
    ) const requires(stores_payload){
        if(count == 0) return {};
        if(node->is_leaf){
            Sum result{};
            const Leaf* leaf = as_leaf(node);
            for(int position = 0; position < leaf_size(leaf); position++){
                if(leaf->bits[static_cast<std::size_t>(position)] == bit){
                    result = result + lift(leaf->payload.values[
                        static_cast<std::size_t>(position)
                    ]);
                    if(--count == 0) break;
                }
            }
            return result;
        }
        Sum result{};
        for(const auto& child: as_internal(node)->children){
            const int child_count = bit
                ? child->stats.ones
                : child->stats.size - child->stats.ones;
            if(count >= child_count){
                result = result + (
                    bit ? child->stats.one_sum : child->stats.zero_sum
                );
                count -= child_count;
            }else{
                result = result +
                    sum_first_recursive(child.get(), bit, count);
                break;
            }
        }
        return result;
    }

    int select_recursive(const Node* node, bool bit, int kth) const{
        if(node->is_leaf){
            const Leaf* leaf = as_leaf(node);
            for(int index = 0; index < leaf_size(leaf); index++){
                if(leaf->bits[static_cast<std::size_t>(index)] == bit &&
                   kth-- == 0){
                    return index;
                }
            }
            throw std::logic_error("MutableBTreeBitSequence select failure.");
        }
        int offset = 0;
        for(const auto& child: as_internal(node)->children){
            const int count = bit
                ? child->stats.ones
                : child->stats.size - child->stats.ones;
            if(kth < count){
                return offset + select_recursive(child.get(), bit, kth);
            }
            kth -= count;
            offset += child->stats.size;
        }
        throw std::logic_error("MutableBTreeBitSequence select failure.");
    }

public:
    MutableBTreeBitSequence() = default;

    MutableBTreeBitSequence(const MutableBTreeBitSequence& other):
        root(other.clone_node(other.root.get())),
        lift(other.lift){}

    MutableBTreeBitSequence& operator=(
        const MutableBTreeBitSequence& other
    ){
        if(this == &other) return *this;
        auto copied = other.clone_node(other.root.get());
        lift = other.lift;
        root = std::move(copied);
        return *this;
    }

    MutableBTreeBitSequence(MutableBTreeBitSequence&&) noexcept = default;
    MutableBTreeBitSequence& operator=(
        MutableBTreeBitSequence&&
    ) noexcept = default;

    int size() const{ return root->stats.size; }
    int ones() const{ return root->stats.ones; }

    void assign(
        const std::vector<unsigned char>& bits,
        const std::vector<Payload>& payloads = {}
    ){
        if constexpr(stores_payload){
            if(bits.size() != payloads.size())[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: mutable B-tree input size mismatch."
                );
            }
        }
        if(bits.empty()){
            root = std::make_unique<Leaf>();
            return;
        }
        std::vector<std::unique_ptr<Node>> current;
        const auto leaf_sizes = balanced_group_sizes(
            static_cast<int>(bits.size()),
            LEAF_CAPACITY
        );
        int first = 0;
        for(int length: leaf_sizes){
            current.push_back(
                make_leaf(bits, payloads, first, length)
            );
            first += length;
        }
        while(current.size() > 1){
            const auto group_sizes = balanced_group_sizes(
                static_cast<int>(current.size()),
                INTERNAL_CAPACITY
            );
            std::vector<std::unique_ptr<Node>> next;
            std::size_t index = 0;
            for(int group_size: group_sizes){
                auto internal = std::make_unique<Internal>();
                for(int count = 0; count < group_size; count++){
                    internal->children.push_back(
                        std::move(current[index++])
                    );
                }
                recalculate(internal.get());
                next.push_back(std::move(internal));
            }
            current = std::move(next);
        }
        root = std::move(current.front());
    }

    Entry access(int position) const{
        return entry_recursive(root.get(), position);
    }

    Stats prefix_stats(int end) const{
        return prefix_stats_recursive(root.get(), end);
    }

    Stats range_stats(int left, int right) const{
        Stats lhs = prefix_stats(left);
        Stats rhs = prefix_stats(right);
        rhs.size -= lhs.size;
        rhs.ones -= lhs.ones;
        rhs.sum = rhs.sum - lhs.sum;
        rhs.zero_sum = rhs.zero_sum - lhs.zero_sum;
        rhs.one_sum = rhs.one_sum - lhs.one_sum;
        return rhs;
    }

    int rank(bool bit, int end) const{
        const int one_count = prefix_stats(end).ones;
        return bit ? one_count : end - one_count;
    }

    int select(bool bit, int kth) const{
        if(kth < 0) return size();
        const int total = bit ? ones() : size() - ones();
        if(total <= kth) return size();
        return select_recursive(root.get(), bit, kth);
    }

    Sum sum_first(bool bit, int count) const requires(stores_payload){
        return sum_first_recursive(root.get(), bit, count);
    }

    Sum sum_first(
        bool bit,
        int left,
        int count
    ) const requires(stores_payload){
        const int skipped = rank(bit, left);
        return sum_first(bit, skipped + count) -
            sum_first(bit, skipped);
    }

    void insert(int position, bool bit, const Payload& payload = {}){
        auto sibling = insert_recursive(
            root,
            position,
            Entry{bit, payload}
        );
        if(sibling){
            auto new_root = std::make_unique<Internal>();
            new_root->children.push_back(std::move(root));
            new_root->children.push_back(std::move(sibling));
            recalculate(new_root.get());
            root = std::move(new_root);
        }
    }

    Entry erase(int position){
        Entry result = erase_recursive(root, position);
        while(!root->is_leaf){
            Internal* internal = as_internal(root.get());
            if(internal->children.size() != 1) break;
            root = std::move(internal->children.front());
        }
        return result;
    }

    bool set_bit(int position, bool bit){
        return set_entry_recursive(
            root.get(), position, true, bit, false, {}
        ).bit;
    }

    bool flip(int position){
        const bool old = access(position).bit;
        set_bit(position, !old);
        return old;
    }

    bool set_payload(
        int position,
        const Payload& payload
    ) requires(stores_payload){
        return set_entry_recursive(
            root.get(), position, false, false, true, payload
        ).bit;
    }
};

}  // namespace wavelet_matrix_detail

#endif  // CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_DETAIL_MUTABLE_BTREE_BIT_SEQUENCE_HPP_INCLUDED
