#ifndef CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_DETAIL_PERSISTENT_BTREE_BIT_SEQUENCE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_DETAIL_PERSISTENT_BTREE_BIT_SEQUENCE_HPP_INCLUDED

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace wavelet_matrix_detail{

struct PersistentBTreeNoPayload{};

struct PersistentBTreeNoSum{
    friend constexpr PersistentBTreeNoSum operator+(
        PersistentBTreeNoSum,
        PersistentBTreeNoSum
    ){
        return {};
    }

    friend constexpr PersistentBTreeNoSum operator-(
        PersistentBTreeNoSum,
        PersistentBTreeNoSum
    ){
        return {};
    }
};

template<class Payload, class Sum>
struct PersistentBTreeDefaultLift{
    constexpr Sum operator()(const Payload& value) const{
        return static_cast<Sum>(value);
    }
};

struct PersistentBTreeNoPayloadLift{
    constexpr PersistentBTreeNoSum operator()(
        PersistentBTreeNoPayload
    ) const{
        return {};
    }
};

template<class Payload, std::size_t Capacity, bool StoresPayload>
struct PersistentBTreePayloadStorage;

template<class Payload, std::size_t Capacity>
struct PersistentBTreePayloadStorage<Payload, Capacity, true>{
    std::array<Payload, Capacity> value{};
};

template<class Payload, std::size_t Capacity>
struct PersistentBTreePayloadStorage<Payload, Capacity, false>{};

template<
    class Payload,
    class Sum,
    class Lift = PersistentBTreeDefaultLift<Payload, Sum>,
    int LEAF_WORDS = 2,
    int INTERNAL_CAPACITY = 16
>
class PersistentBTreeBitSequence{
    static_assert(LEAF_WORDS > 0);
    static_assert(INTERNAL_CAPACITY >= 3);
    static_assert(std::is_default_constructible_v<Payload>);
    static_assert(std::is_copy_constructible_v<Payload>);
    static_assert(std::is_copy_assignable_v<Payload>);
    static_assert(std::is_default_constructible_v<Sum>);
    static_assert(std::is_copy_constructible_v<Sum>);
    static_assert(std::is_copy_assignable_v<Sum>);

public:
    static constexpr int leaf_words = LEAF_WORDS;
    static constexpr int leaf_capacity = leaf_words * 64;
    static constexpr int internal_capacity = INTERNAL_CAPACITY;
    static constexpr bool stores_payload =
        !std::is_same_v<Payload, PersistentBTreeNoPayload>;

    struct Root{
        bool is_leaf = true;
        int index = -1;

        friend constexpr bool operator==(Root, Root) = default;
    };

    struct Snapshot{
        std::size_t leaf_count = 0;
        std::size_t internal_count = 0;
    };

    struct Entry{
        bool bit = false;
        Payload payload{};
    };

    struct RankPair{
        int ones_l = 0;
        int ones_r = 0;
    };

    struct RangeStats{
        int ones_l = 0;
        int ones_r = 0;
        Sum zero_sum{};
        Sum one_sum{};
    };

    struct EraseResult{
        Root root{};
        bool bit = false;
        Payload payload{};
    };

    struct PayloadUpdateResult{
        Root root{};
        bool bit = false;
        Payload old_payload{};
    };

private:
    static constexpr int leaf_minimum = leaf_capacity / 2;
    static constexpr int internal_minimum = internal_capacity / 2;

    using PayloadStorage = PersistentBTreePayloadStorage<
        Payload,
        static_cast<std::size_t>(leaf_capacity),
        stores_payload
    >;

    struct Summary{
        int size = 0;
        int ones = 0;
        Sum zero_sum{};
        Sum one_sum{};
    };

    struct Leaf{
        int length = 0;
        std::array<std::uint64_t, static_cast<std::size_t>(leaf_words)> words{};
        [[no_unique_address]] PayloadStorage payload{};
        Summary summary{};
    };

    struct Internal{
        int child_count = 0;
        std::array<Root, static_cast<std::size_t>(internal_capacity)> child{};
        Summary summary{};
    };

    struct InsertSplit{
        Root left{};
        bool has_right = false;
        Root right{};
    };

    struct PrefixStats{
        int ones = 0;
        Sum zero_sum{};
        Sum one_sum{};
    };

    std::deque<Leaf> leaves;
    std::deque<Internal> internals;
    [[no_unique_address]] Lift lift{};

    static bool leaf_bit(const Leaf& leaf, int position){
        return static_cast<bool>(
            (leaf.words[static_cast<std::size_t>(position >> 6)]
                >> (position & 63)) & std::uint64_t{1}
        );
    }

    static void set_leaf_bit(Leaf& leaf, int position, bool value){
        const std::uint64_t mask = std::uint64_t{1} << (position & 63);
        auto& word = leaf.words[static_cast<std::size_t>(position >> 6)];
        if(value) word |= mask;
        else word &= ~mask;
    }

    static Payload leaf_payload(const Leaf& leaf, int position){
        if constexpr(stores_payload){
            return leaf.payload.value[static_cast<std::size_t>(position)];
        }else{
            (void)leaf;
            (void)position;
            return {};
        }
    }

    static void set_leaf_payload(
        Leaf& leaf,
        int position,
        const Payload& value
    ){
        if constexpr(stores_payload){
            leaf.payload.value[static_cast<std::size_t>(position)] = value;
        }else{
            (void)leaf;
            (void)position;
            (void)value;
        }
    }

    static void clear_leaf_payload(Leaf& leaf, int position){
        if constexpr(stores_payload){
            leaf.payload.value[static_cast<std::size_t>(position)] = Payload{};
        }else{
            (void)leaf;
            (void)position;
        }
    }

    const Leaf& get_leaf(Root root) const{
        return leaves[static_cast<std::size_t>(root.index)];
    }

    const Internal& get_internal(Root root) const{
        return internals[static_cast<std::size_t>(root.index)];
    }

    const Summary& get_summary(Root root) const{
        return root.is_leaf
            ? get_leaf(root).summary
            : get_internal(root).summary;
    }

    void check_root(Root root) const{
        const bool valid = root.index >= 0 && (
            root.is_leaf
                ? static_cast<std::size_t>(root.index) < leaves.size()
                : static_cast<std::size_t>(root.index) < internals.size()
        );
        if(!valid)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: invalid persistent B-tree root."
            );
        }
    }

    void check_position(Root root, int position, bool allow_end) const{
        check_root(root);
        const int length = get_summary(root).size;
        if(position < 0 || length < position || (!allow_end && position == length)){
            throw std::runtime_error(
                "library assertion fault: persistent B-tree position violation."
            );
        }
    }

    Root append_leaf(Leaf leaf){
        if(leaves.size() >= static_cast<std::size_t>(std::numeric_limits<int>::max())){
            throw std::runtime_error(
                "library assertion fault: persistent B-tree leaf capacity violation."
            );
        }
        leaves.push_back(std::move(leaf));
        return {
            true,
            static_cast<int>(leaves.size() - 1)
        };
    }

    Root append_internal(Internal internal){
        if(internals.size() >= static_cast<std::size_t>(std::numeric_limits<int>::max())){
            throw std::runtime_error(
                "library assertion fault: persistent B-tree internal capacity violation."
            );
        }
        internals.push_back(std::move(internal));
        return {
            false,
            static_cast<int>(internals.size() - 1)
        };
    }

    void recalculate(Leaf& leaf) const{
        leaf.summary = {};
        leaf.summary.size = leaf.length;
        for(int position = 0; position < leaf.length; position++){
            const bool bit = leaf_bit(leaf, position);
            leaf.summary.ones += bit ? 1 : 0;
            if constexpr(stores_payload){
                const Sum value = lift(
                    leaf.payload.value[static_cast<std::size_t>(position)]
                );
                if(bit){
                    leaf.summary.one_sum = leaf.summary.one_sum + value;
                }else{
                    leaf.summary.zero_sum = leaf.summary.zero_sum + value;
                }
            }
        }
    }

    void recalculate(Internal& internal) const{
        internal.summary = {};
        for(int index = 0; index < internal.child_count; index++){
            const Summary& child_summary = get_summary(
                internal.child[static_cast<std::size_t>(index)]
            );
            internal.summary.size += child_summary.size;
            internal.summary.ones += child_summary.ones;
            internal.summary.zero_sum =
                internal.summary.zero_sum + child_summary.zero_sum;
            internal.summary.one_sum =
                internal.summary.one_sum + child_summary.one_sum;
        }
    }

    void insert_into_leaf(
        Leaf& leaf,
        int position,
        bool bit,
        const Payload& payload
    ) const{
        for(int index = leaf.length; index > position; index--){
            set_leaf_bit(leaf, index, leaf_bit(leaf, index - 1));
            if constexpr(stores_payload){
                leaf.payload.value[static_cast<std::size_t>(index)] =
                    leaf.payload.value[static_cast<std::size_t>(index - 1)];
            }
        }
        set_leaf_bit(leaf, position, bit);
        set_leaf_payload(leaf, position, payload);
        leaf.length++;
        recalculate(leaf);
    }

    void erase_from_leaf(Leaf& leaf, int position) const{
        for(int index = position; index + 1 < leaf.length; index++){
            set_leaf_bit(leaf, index, leaf_bit(leaf, index + 1));
            if constexpr(stores_payload){
                leaf.payload.value[static_cast<std::size_t>(index)] =
                    leaf.payload.value[static_cast<std::size_t>(index + 1)];
            }
        }
        leaf.length--;
        set_leaf_bit(leaf, leaf.length, false);
        clear_leaf_payload(leaf, leaf.length);
        recalculate(leaf);
    }

    Internal make_internal(
        const Root* child,
        int child_count
    ) const{
        Internal result;
        result.child_count = child_count;
        for(int index = 0; index < child_count; index++){
            result.child[static_cast<std::size_t>(index)] =
                child[static_cast<std::size_t>(index)];
        }
        recalculate(result);
        return result;
    }

    InsertSplit insert_recursive(
        Root root,
        int position,
        bool bit,
        const Payload& payload
    ){
        if(root.is_leaf){
            const Leaf& source = get_leaf(root);
            if(source.length < leaf_capacity){
                Leaf result = source;
                insert_into_leaf(result, position, bit, payload);
                return {append_leaf(std::move(result)), false, {}};
            }

            constexpr int total = leaf_capacity + 1;
            constexpr int left_length = total / 2;
            Leaf left;
            Leaf right;
            left.length = left_length;
            right.length = total - left_length;
            for(int index = 0; index < total; index++){
                const bool current_bit = index < position
                    ? leaf_bit(source, index)
                    : index == position
                        ? bit
                        : leaf_bit(source, index - 1);
                Payload current_payload{};
                if constexpr(stores_payload){
                    current_payload = index < position
                        ? leaf_payload(source, index)
                        : index == position
                            ? payload
                            : leaf_payload(source, index - 1);
                }
                if(index < left_length){
                    set_leaf_bit(left, index, current_bit);
                    set_leaf_payload(left, index, current_payload);
                }else{
                    const int local = index - left_length;
                    set_leaf_bit(right, local, current_bit);
                    set_leaf_payload(right, local, current_payload);
                }
            }
            recalculate(left);
            recalculate(right);
            Root left_root = append_leaf(std::move(left));
            Root right_root = append_leaf(std::move(right));
            return {left_root, true, right_root};
        }

        const Internal& source = get_internal(root);
        int child_index = 0;
        int local_position = position;
        while(child_index + 1 < source.child_count){
            const int child_size = get_summary(
                source.child[static_cast<std::size_t>(child_index)]
            ).size;
            if(local_position <= child_size) break;
            local_position -= child_size;
            child_index++;
        }

        InsertSplit child_result = insert_recursive(
            source.child[static_cast<std::size_t>(child_index)],
            local_position,
            bit,
            payload
        );

        std::array<Root, static_cast<std::size_t>(internal_capacity + 1)> child{};
        int child_count = source.child_count;
        for(int index = 0; index < child_count; index++){
            child[static_cast<std::size_t>(index)] =
                source.child[static_cast<std::size_t>(index)];
        }
        child[static_cast<std::size_t>(child_index)] = child_result.left;
        if(child_result.has_right){
            for(int index = child_count; index > child_index + 1; index--){
                child[static_cast<std::size_t>(index)] =
                    child[static_cast<std::size_t>(index - 1)];
            }
            child[static_cast<std::size_t>(child_index + 1)] =
                child_result.right;
            child_count++;
        }

        if(child_count <= internal_capacity){
            return {
                append_internal(make_internal(child.data(), child_count)),
                false,
                {}
            };
        }

        const int left_count = child_count / 2;
        Root left_root = append_internal(
            make_internal(child.data(), left_count)
        );
        Root right_root = append_internal(
            make_internal(child.data() + left_count, child_count - left_count)
        );
        return {left_root, true, right_root};
    }

    static bool is_underfull(
        Root root,
        const PersistentBTreeBitSequence& sequence
    ){
        if(root.is_leaf){
            return sequence.get_leaf(root).length < leaf_minimum;
        }
        return sequence.get_internal(root).child_count < internal_minimum;
    }

    static bool can_lend(
        Root root,
        const PersistentBTreeBitSequence& sequence
    ){
        if(root.is_leaf){
            return sequence.get_leaf(root).length > leaf_minimum;
        }
        return sequence.get_internal(root).child_count > internal_minimum;
    }

    std::pair<Root, Root> borrow_from_left(
        Root left_root,
        Root child_root
    ){
        if(left_root.is_leaf){
            Leaf left = get_leaf(left_root);
            Leaf child = get_leaf(child_root);
            const int source_position = left.length - 1;
            const bool bit = leaf_bit(left, source_position);
            const Payload payload = leaf_payload(left, source_position);
            erase_from_leaf(left, source_position);
            insert_into_leaf(child, 0, bit, payload);
            return {
                append_leaf(std::move(left)),
                append_leaf(std::move(child))
            };
        }

        Internal left = get_internal(left_root);
        Internal child = get_internal(child_root);
        for(int index = child.child_count; index > 0; index--){
            child.child[static_cast<std::size_t>(index)] =
                child.child[static_cast<std::size_t>(index - 1)];
        }
        child.child[0] =
            left.child[static_cast<std::size_t>(left.child_count - 1)];
        child.child_count++;
        left.child_count--;
        recalculate(left);
        recalculate(child);
        return {
            append_internal(std::move(left)),
            append_internal(std::move(child))
        };
    }

    std::pair<Root, Root> borrow_from_right(
        Root child_root,
        Root right_root
    ){
        if(child_root.is_leaf){
            Leaf child = get_leaf(child_root);
            Leaf right = get_leaf(right_root);
            const bool bit = leaf_bit(right, 0);
            const Payload payload = leaf_payload(right, 0);
            erase_from_leaf(right, 0);
            insert_into_leaf(child, child.length, bit, payload);
            return {
                append_leaf(std::move(child)),
                append_leaf(std::move(right))
            };
        }

        Internal child = get_internal(child_root);
        Internal right = get_internal(right_root);
        child.child[static_cast<std::size_t>(child.child_count)] = right.child[0];
        child.child_count++;
        for(int index = 0; index + 1 < right.child_count; index++){
            right.child[static_cast<std::size_t>(index)] =
                right.child[static_cast<std::size_t>(index + 1)];
        }
        right.child_count--;
        recalculate(child);
        recalculate(right);
        return {
            append_internal(std::move(child)),
            append_internal(std::move(right))
        };
    }

    Root merge_nodes(Root left_root, Root right_root){
        if(left_root.is_leaf){
            const Leaf& left_source = get_leaf(left_root);
            const Leaf& right_source = get_leaf(right_root);
            Leaf result = left_source;
            const int offset = result.length;
            result.length += right_source.length;
            for(int index = 0; index < right_source.length; index++){
                set_leaf_bit(
                    result,
                    offset + index,
                    leaf_bit(right_source, index)
                );
                set_leaf_payload(
                    result,
                    offset + index,
                    leaf_payload(right_source, index)
                );
            }
            recalculate(result);
            return append_leaf(std::move(result));
        }

        const Internal& left_source = get_internal(left_root);
        const Internal& right_source = get_internal(right_root);
        Internal result = left_source;
        for(int index = 0; index < right_source.child_count; index++){
            result.child[static_cast<std::size_t>(result.child_count + index)] =
                right_source.child[static_cast<std::size_t>(index)];
        }
        result.child_count += right_source.child_count;
        recalculate(result);
        return append_internal(std::move(result));
    }

    void rebalance_child(
        std::array<Root, static_cast<std::size_t>(internal_capacity)>& child,
        int& child_count,
        int child_index
    ){
        if(child_count <= 1 || !is_underfull(child[
            static_cast<std::size_t>(child_index)
        ], *this)){
            return;
        }

        if(child_index > 0 && can_lend(
            child[static_cast<std::size_t>(child_index - 1)],
            *this
        )){
            auto [left, current] = borrow_from_left(
                child[static_cast<std::size_t>(child_index - 1)],
                child[static_cast<std::size_t>(child_index)]
            );
            child[static_cast<std::size_t>(child_index - 1)] = left;
            child[static_cast<std::size_t>(child_index)] = current;
            return;
        }

        if(child_index + 1 < child_count && can_lend(
            child[static_cast<std::size_t>(child_index + 1)],
            *this
        )){
            auto [current, right] = borrow_from_right(
                child[static_cast<std::size_t>(child_index)],
                child[static_cast<std::size_t>(child_index + 1)]
            );
            child[static_cast<std::size_t>(child_index)] = current;
            child[static_cast<std::size_t>(child_index + 1)] = right;
            return;
        }

        int erased_index;
        if(child_index > 0){
            child[static_cast<std::size_t>(child_index - 1)] = merge_nodes(
                child[static_cast<std::size_t>(child_index - 1)],
                child[static_cast<std::size_t>(child_index)]
            );
            erased_index = child_index;
        }else{
            child[0] = merge_nodes(child[0], child[1]);
            erased_index = 1;
        }
        for(int index = erased_index; index + 1 < child_count; index++){
            child[static_cast<std::size_t>(index)] =
                child[static_cast<std::size_t>(index + 1)];
        }
        child_count--;
    }

    Root erase_recursive(Root root, int position){
        if(root.is_leaf){
            Leaf result = get_leaf(root);
            erase_from_leaf(result, position);
            return append_leaf(std::move(result));
        }

        const Internal& source = get_internal(root);
        int child_index = 0;
        int local_position = position;
        while(true){
            const int child_size = get_summary(
                source.child[static_cast<std::size_t>(child_index)]
            ).size;
            if(local_position < child_size) break;
            local_position -= child_size;
            child_index++;
        }

        std::array<Root, static_cast<std::size_t>(internal_capacity)> child =
            source.child;
        int child_count = source.child_count;
        child[static_cast<std::size_t>(child_index)] = erase_recursive(
            child[static_cast<std::size_t>(child_index)],
            local_position
        );
        rebalance_child(child, child_count, child_index);
        return append_internal(make_internal(child.data(), child_count));
    }

    Root set_payload_recursive(
        Root root,
        int position,
        const Payload& payload
    ) requires(stores_payload){
        if(root.is_leaf){
            Leaf result = get_leaf(root);
            set_leaf_payload(result, position, payload);
            recalculate(result);
            return append_leaf(std::move(result));
        }

        const Internal& source = get_internal(root);
        Internal result = source;
        int child_index = 0;
        int local_position = position;
        while(true){
            const int child_size = get_summary(
                source.child[static_cast<std::size_t>(child_index)]
            ).size;
            if(local_position < child_size) break;
            local_position -= child_size;
            child_index++;
        }
        result.child[static_cast<std::size_t>(child_index)] =
            set_payload_recursive(
                source.child[static_cast<std::size_t>(child_index)],
                local_position,
                payload
            );
        recalculate(result);
        return append_internal(std::move(result));
    }

    int rank1_unchecked(Root root, int end) const{
        if(root.is_leaf){
            const Leaf& leaf = get_leaf(root);
            int result = 0;
            const int full_words = end >> 6;
            for(int index = 0; index < full_words; index++){
                result += std::popcount(
                    leaf.words[static_cast<std::size_t>(index)]
                );
            }
            const int remaining = end & 63;
            if(remaining != 0){
                const std::uint64_t mask =
                    (std::uint64_t{1} << remaining) - 1;
                result += std::popcount(
                    leaf.words[static_cast<std::size_t>(full_words)] & mask
                );
            }
            return result;
        }

        const Internal& internal = get_internal(root);
        int result = 0;
        int remaining = end;
        for(int index = 0; index < internal.child_count; index++){
            Root current = internal.child[static_cast<std::size_t>(index)];
            const Summary& child_summary = get_summary(current);
            if(remaining >= child_summary.size){
                result += child_summary.ones;
                remaining -= child_summary.size;
            }else{
                result += rank1_unchecked(current, remaining);
                break;
            }
        }
        return result;
    }

    PrefixStats prefix_stats_unchecked(Root root, int end) const{
        if(root.is_leaf){
            const Leaf& leaf = get_leaf(root);
            PrefixStats result;
            for(int position = 0; position < end; position++){
                const bool bit = leaf_bit(leaf, position);
                result.ones += bit ? 1 : 0;
                if constexpr(stores_payload){
                    const Sum value = lift(
                        leaf.payload.value[static_cast<std::size_t>(position)]
                    );
                    if(bit) result.one_sum = result.one_sum + value;
                    else result.zero_sum = result.zero_sum + value;
                }
            }
            return result;
        }

        const Internal& internal = get_internal(root);
        PrefixStats result;
        int remaining = end;
        for(int index = 0; index < internal.child_count; index++){
            Root current = internal.child[static_cast<std::size_t>(index)];
            const Summary& child_summary = get_summary(current);
            if(remaining >= child_summary.size){
                result.ones += child_summary.ones;
                result.zero_sum = result.zero_sum + child_summary.zero_sum;
                result.one_sum = result.one_sum + child_summary.one_sum;
                remaining -= child_summary.size;
            }else{
                PrefixStats partial = prefix_stats_unchecked(current, remaining);
                result.ones += partial.ones;
                result.zero_sum = result.zero_sum + partial.zero_sum;
                result.one_sum = result.one_sum + partial.one_sum;
                break;
            }
        }
        return result;
    }

    int select_unchecked(Root root, bool bit, int occurrence) const{
        if(root.is_leaf){
            const Leaf& leaf = get_leaf(root);
            for(int position = 0; position < leaf.length; position++){
                if(leaf_bit(leaf, position) == bit && occurrence-- == 0){
                    return position;
                }
            }
            throw std::logic_error(
                "PersistentBTreeBitSequence select inconsistency."
            );
        }

        const Internal& internal = get_internal(root);
        int offset = 0;
        for(int index = 0; index < internal.child_count; index++){
            Root current = internal.child[static_cast<std::size_t>(index)];
            const Summary& child_summary = get_summary(current);
            const int count = bit
                ? child_summary.ones
                : child_summary.size - child_summary.ones;
            if(occurrence < count){
                return offset + select_unchecked(current, bit, occurrence);
            }
            occurrence -= count;
            offset += child_summary.size;
        }
        throw std::logic_error(
            "PersistentBTreeBitSequence select inconsistency."
        );
    }

    Sum weight_of_first_unchecked(
        Root root,
        bool bit,
        int count
    ) const requires(stores_payload){
        if(count == 0) return Sum{};
        if(root.is_leaf){
            const Leaf& leaf = get_leaf(root);
            Sum result{};
            for(int position = 0; position < leaf.length && count > 0; position++){
                if(leaf_bit(leaf, position) == bit){
                    result = result + lift(
                        leaf.payload.value[static_cast<std::size_t>(position)]
                    );
                    count--;
                }
            }
            return result;
        }

        const Internal& internal = get_internal(root);
        Sum result{};
        for(int index = 0; index < internal.child_count && count > 0; index++){
            Root current = internal.child[static_cast<std::size_t>(index)];
            const Summary& child_summary = get_summary(current);
            const int child_count = bit
                ? child_summary.ones
                : child_summary.size - child_summary.ones;
            if(count >= child_count){
                result = result + (
                    bit ? child_summary.one_sum : child_summary.zero_sum
                );
                count -= child_count;
            }else{
                result = result + weight_of_first_unchecked(
                    current,
                    bit,
                    count
                );
                count = 0;
            }
        }
        return result;
    }

    template<class PayloadGetter>
    Root build_impl(
        const std::vector<unsigned char>& bits,
        PayloadGetter payload_getter
    ){
        if(bits.empty()){
            Leaf leaf;
            recalculate(leaf);
            return append_leaf(std::move(leaf));
        }

        const auto balanced_group_sizes = [](int total, int capacity){
            const int group_count = (total + capacity - 1) / capacity;
            std::vector<int> size(
                static_cast<std::size_t>(group_count),
                total / group_count
            );
            for(int index = 0; index < total % group_count; index++){
                size[static_cast<std::size_t>(index)]++;
            }
            return size;
        };

        std::vector<Root> current;
        const std::vector<int> leaf_size = balanced_group_sizes(
            static_cast<int>(bits.size()),
            leaf_capacity
        );
        current.reserve(leaf_size.size());
        int first = 0;
        for(int length : leaf_size){
            Leaf leaf;
            leaf.length = length;
            for(int index = 0; index < length; index++){
                set_leaf_bit(
                    leaf,
                    index,
                    bits[static_cast<std::size_t>(first + index)] != 0
                );
                if constexpr(stores_payload){
                    set_leaf_payload(
                        leaf,
                        index,
                        payload_getter(first + index)
                    );
                }
            }
            recalculate(leaf);
            current.push_back(append_leaf(std::move(leaf)));
            first += length;
        }

        while(current.size() > 1){
            const std::vector<int> group_size = balanced_group_sizes(
                static_cast<int>(current.size()),
                internal_capacity
            );
            std::vector<Root> next;
            next.reserve(group_size.size());
            std::size_t index = 0;
            for(int length : group_size){
                next.push_back(append_internal(
                    make_internal(current.data() + index, length)
                ));
                index += static_cast<std::size_t>(length);
            }
            current = std::move(next);
        }
        return current[0];
    }

public:
    PersistentBTreeBitSequence() = default;

    explicit PersistentBTreeBitSequence(const Lift& lift_function):
        lift(lift_function){}

    Snapshot snapshot() const noexcept{
        return {leaves.size(), internals.size()};
    }

    std::size_t nodes_used() const noexcept{
        return leaves.size() + internals.size();
    }

    void rollback(Snapshot target){
        if(target.leaf_count > leaves.size()
            || target.internal_count > internals.size())[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: persistent B-tree rollback violation."
            );
        }
        while(leaves.size() > target.leaf_count) leaves.pop_back();
        while(internals.size() > target.internal_count) internals.pop_back();
    }

    Root build(
        const std::vector<unsigned char>& bits,
        const std::vector<Payload>& payload
    ) requires(stores_payload){
        if(bits.size() != payload.size())[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: persistent B-tree build size violation."
            );
        }
        const Snapshot before = snapshot();
        try{
            return build_impl(bits, [&](int position) -> const Payload&{
                return payload[static_cast<std::size_t>(position)];
            });
        }catch(...){
            rollback(before);
            throw;
        }
    }

    Root build(
        const std::vector<unsigned char>& bits
    ) requires(!stores_payload){
        const Snapshot before = snapshot();
        try{
            return build_impl(bits, [](int){ return Payload{}; });
        }catch(...){
            rollback(before);
            throw;
        }
    }

    int size(Root root) const{
        check_root(root);
        return get_summary(root).size;
    }

    int ones(Root root) const{
        check_root(root);
        return get_summary(root).ones;
    }

    Entry access(Root root, int position) const{
        check_position(root, position, false);
        Root current = root;
        int local_position = position;
        while(!current.is_leaf){
            const Internal& internal = get_internal(current);
            int child_index = 0;
            while(true){
                Root child = internal.child[static_cast<std::size_t>(child_index)];
                const int child_size = get_summary(child).size;
                if(local_position < child_size){
                    current = child;
                    break;
                }
                local_position -= child_size;
                child_index++;
            }
        }
        const Leaf& leaf = get_leaf(current);
        return {
            leaf_bit(leaf, local_position),
            leaf_payload(leaf, local_position)
        };
    }

    int rank(Root root, bool bit, int end) const{
        check_position(root, end, true);
        const int one_count = rank1_unchecked(root, end);
        return bit ? one_count : end - one_count;
    }

    int rank(Root root, bool bit, int left, int right) const{
        check_position(root, left, true);
        check_position(root, right, true);
        if(right < left)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: persistent B-tree range violation."
            );
        }
        return rank(root, bit, right) - rank(root, bit, left);
    }

    RankPair rank_pair(Root root, int left, int right) const{
        check_position(root, left, true);
        check_position(root, right, true);
        if(right < left)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: persistent B-tree range violation."
            );
        }
        return {
            rank1_unchecked(root, left),
            rank1_unchecked(root, right)
        };
    }

    int select(Root root, bool bit, int occurrence) const{
        check_root(root);
        if(occurrence < 0) return size(root);
        const int total = bit
            ? get_summary(root).ones
            : get_summary(root).size - get_summary(root).ones;
        if(total <= occurrence) return size(root);
        return select_unchecked(root, bit, occurrence);
    }

    RangeStats range_stats(Root root, int left, int right) const{
        check_position(root, left, true);
        check_position(root, right, true);
        if(right < left)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: persistent B-tree range violation."
            );
        }
        const PrefixStats first = prefix_stats_unchecked(root, left);
        const PrefixStats last = prefix_stats_unchecked(root, right);
        return {
            first.ones,
            last.ones,
            last.zero_sum - first.zero_sum,
            last.one_sum - first.one_sum
        };
    }

    Sum sum(
        Root root,
        bool bit,
        int left,
        int right
    ) const requires(stores_payload){
        const RangeStats stats = range_stats(root, left, right);
        return bit ? stats.one_sum : stats.zero_sum;
    }

    Sum sum_all(
        Root root,
        int left,
        int right
    ) const requires(stores_payload){
        const RangeStats stats = range_stats(root, left, right);
        return stats.zero_sum + stats.one_sum;
    }

    Sum weight_of_first(
        Root root,
        bool bit,
        int count
    ) const requires(stores_payload){
        check_root(root);
        const int total = bit
            ? get_summary(root).ones
            : get_summary(root).size - get_summary(root).ones;
        if(count < 0 || total < count)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: persistent B-tree count violation."
            );
        }
        return weight_of_first_unchecked(root, bit, count);
    }

    Sum weight_of_first(
        Root root,
        bool bit,
        int left,
        int count
    ) const requires(stores_payload){
        check_position(root, left, true);
        const int skipped = rank(root, bit, left);
        const int total = bit
            ? get_summary(root).ones
            : get_summary(root).size - get_summary(root).ones;
        if(count < 0 || total - skipped < count)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: persistent B-tree count violation."
            );
        }
        return weight_of_first_unchecked(root, bit, skipped + count)
            - weight_of_first_unchecked(root, bit, skipped);
    }

    Root insert(
        Root root,
        int position,
        bool bit,
        const Payload& payload = Payload{}
    ){
        check_position(root, position, true);
        const Snapshot before = snapshot();
        try{
            InsertSplit result = insert_recursive(root, position, bit, payload);
            if(!result.has_right) return result.left;
            const std::array<Root, 2> child = {
                result.left,
                result.right
            };
            return append_internal(make_internal(child.data(), 2));
        }catch(...){
            rollback(before);
            throw;
        }
    }

    EraseResult erase(Root root, int position){
        check_position(root, position, false);
        const Entry erased = access(root, position);
        const Snapshot before = snapshot();
        try{
            Root result = erase_recursive(root, position);
            while(!result.is_leaf){
                const Internal& internal = get_internal(result);
                if(internal.child_count != 1) break;
                result = internal.child[0];
            }
            return {
                result,
                erased.bit,
                erased.payload
            };
        }catch(...){
            rollback(before);
            throw;
        }
    }

    PayloadUpdateResult set_payload(
        Root root,
        int position,
        const Payload& payload
    ) requires(stores_payload){
        check_position(root, position, false);
        const Entry old = access(root, position);
        const Snapshot before = snapshot();
        try{
            return {
                set_payload_recursive(root, position, payload),
                old.bit,
                old.payload
            };
        }catch(...){
            rollback(before);
            throw;
        }
    }
};

template<int LEAF_WORDS = 2, int INTERNAL_CAPACITY = 16>
using PersistentBTreeBitVector = PersistentBTreeBitSequence<
    PersistentBTreeNoPayload,
    PersistentBTreeNoSum,
    PersistentBTreeNoPayloadLift,
    LEAF_WORDS,
    INTERNAL_CAPACITY
>;

} // namespace wavelet_matrix_detail

#endif  // CPPLIB_SRC_STRUCTURE_WAVELET_MATRIX_DETAIL_PERSISTENT_BTREE_BIT_SEQUENCE_HPP_INCLUDED
