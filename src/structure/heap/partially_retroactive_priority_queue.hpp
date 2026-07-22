#ifndef CPPLIB_SRC_STRUCTURE_HEAP_PARTIALLY_RETROACTIVE_PRIORITY_QUEUE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_HEAP_PARTIALLY_RETROACTIVE_PRIORITY_QUEUE_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

enum class RetroactivePriorityQueueOperationKind{
    push,
    pop
};

template<
    class Time,
    class T,
    std::size_t MAX_OPERATION,
    class Compare = std::less<T>,
    class TimeCompare = std::less<Time>
>
class PartiallyRetroactivePriorityQueue{
    static constexpr std::size_t NONE = MAX_OPERATION;
    using balance_type = std::ptrdiff_t;

    struct Node{
        Time time;
        RetroactivePriorityQueueOperationKind kind;
        std::optional<T> value;
        bool active;
        std::uint64_t serial;
        std::uint64_t priority;
        std::size_t left = NONE, right = NONE;
        std::size_t subtree_size = 1;
        balance_type bridge_sum = 0, bridge_minimum = 0;
        balance_type raw_sum = 0, raw_minimum = 0;
        std::size_t minimum_active = NONE, maximum_inactive = NONE;

        Node(
            const Time& time_,
            RetroactivePriorityQueueOperationKind kind_,
            std::optional<T> value_,
            bool active_,
            std::uint64_t serial_,
            std::uint64_t priority_
        )
            : time(time_), kind(kind_), value(std::move(value_)),
              active(active_), serial(serial_), priority(priority_){}
    };

    struct SequenceAggregate{
        bool empty = true;
        balance_type sum = 0;
        balance_type minimum = 0;
    };

    [[no_unique_address]] Compare compare;
    [[no_unique_address]] TimeCompare time_compare;
    std::array<std::optional<Node>, MAX_OPERATION> nodes{};
    std::array<std::size_t, MAX_OPERATION> free_slots{};
    std::size_t free_count = MAX_OPERATION;
    std::size_t root = NONE;
    std::uint64_t next_serial = 1;

    Node& node(std::size_t index){ return *nodes[index]; }
    const Node& node(std::size_t index) const{ return *nodes[index]; }

    static std::uint64_t mix(std::uint64_t value) noexcept{
        value += 0x9e3779b97f4a7c15ULL;
        value = (value ^ (value >> 30)) * 0xbf58476d1ce4e5b9ULL;
        value = (value ^ (value >> 27)) * 0x94d049bb133111ebULL;
        return value ^ (value >> 31);
    }

    bool equivalent_time(const Time& left, const Time& right) const{
        return !time_compare(left, right) && !time_compare(right, left);
    }

    balance_type bridge_delta(const Node& current) const noexcept{
        if(current.kind == RetroactivePriorityQueueOperationKind::pop) return -1;
        return current.active ? 0 : 1;
    }

    balance_type raw_delta(const Node& current) const noexcept{
        return current.kind == RetroactivePriorityQueueOperationKind::push ? 1 : -1;
    }

    std::size_t subtree_size(std::size_t current) const noexcept{
        return current == NONE ? 0 : node(current).subtree_size;
    }
    balance_type bridge_sum(std::size_t current) const noexcept{
        return current == NONE ? 0 : node(current).bridge_sum;
    }
    balance_type raw_sum(std::size_t current) const noexcept{
        return current == NONE ? 0 : node(current).raw_sum;
    }

    bool priority_precedes(std::size_t left, std::size_t right) const{
        const T& a = *node(left).value;
        const T& b = *node(right).value;
        if(compare(a, b)) return true;
        if(compare(b, a)) return false;
        return node(left).serial < node(right).serial;
    }

    std::size_t choose_minimum(std::size_t left, std::size_t right) const{
        if(left == NONE) return right;
        if(right == NONE) return left;
        return priority_precedes(left, right) ? left : right;
    }

    std::size_t choose_maximum(std::size_t left, std::size_t right) const{
        if(left == NONE) return right;
        if(right == NONE) return left;
        return priority_precedes(left, right) ? right : left;
    }

    void pull(std::size_t current){
        Node& target = node(current);
        const balance_type left_bridge = bridge_sum(target.left);
        const balance_type through_bridge =
            left_bridge + bridge_delta(target);
        target.subtree_size =
            subtree_size(target.left) + 1 + subtree_size(target.right);
        target.bridge_sum = through_bridge + bridge_sum(target.right);

        balance_type bridge_min =
            std::numeric_limits<balance_type>::max();
        if(target.left != NONE) bridge_min = node(target.left).bridge_minimum;
        bridge_min = std::min(bridge_min, through_bridge);
        if(target.right != NONE){
            bridge_min = std::min(
                bridge_min,
                through_bridge + node(target.right).bridge_minimum
            );
        }
        target.bridge_minimum = bridge_min;

        const balance_type left_raw = raw_sum(target.left);
        const balance_type through_raw = left_raw + raw_delta(target);
        target.raw_sum = through_raw + raw_sum(target.right);
        balance_type raw_min = std::numeric_limits<balance_type>::max();
        if(target.left != NONE) raw_min = node(target.left).raw_minimum;
        raw_min = std::min(raw_min, through_raw);
        if(target.right != NONE){
            raw_min = std::min(
                raw_min,
                through_raw + node(target.right).raw_minimum
            );
        }
        target.raw_minimum = raw_min;

        std::size_t minimum_active = NONE;
        std::size_t maximum_inactive = NONE;
        if(target.left != NONE){
            minimum_active = node(target.left).minimum_active;
            maximum_inactive = node(target.left).maximum_inactive;
        }
        if(target.kind == RetroactivePriorityQueueOperationKind::push){
            if(target.active){
                minimum_active = choose_minimum(minimum_active, current);
            }else{
                maximum_inactive = choose_maximum(maximum_inactive, current);
            }
        }
        if(target.right != NONE){
            minimum_active =
                choose_minimum(minimum_active, node(target.right).minimum_active);
            maximum_inactive =
                choose_maximum(maximum_inactive, node(target.right).maximum_inactive);
        }
        target.minimum_active = minimum_active;
        target.maximum_inactive = maximum_inactive;
    }

    bool treap_precedes(std::size_t left, std::size_t right) const noexcept{
        if(node(left).priority != node(right).priority){
            return node(left).priority > node(right).priority;
        }
        return node(left).serial > node(right).serial;
    }

    std::size_t rotate_left(std::size_t current){
        const std::size_t next = node(current).right;
        node(current).right = node(next).left;
        node(next).left = current;
        pull(current);
        pull(next);
        return next;
    }

    std::size_t rotate_right(std::size_t current){
        const std::size_t next = node(current).left;
        node(current).left = node(next).right;
        node(next).right = current;
        pull(current);
        pull(next);
        return next;
    }

    std::size_t insert_node(std::size_t current, std::size_t inserted){
        if(current == NONE){
            pull(inserted);
            return inserted;
        }
        if(time_compare(node(inserted).time, node(current).time)){
            node(current).left = insert_node(node(current).left, inserted);
            if(treap_precedes(node(current).left, current)){
                return rotate_right(current);
            }
        }else{
            node(current).right = insert_node(node(current).right, inserted);
            if(treap_precedes(node(current).right, current)){
                return rotate_left(current);
            }
        }
        pull(current);
        return current;
    }

    std::size_t merge_treaps(std::size_t left, std::size_t right){
        if(left == NONE) return right;
        if(right == NONE) return left;
        if(treap_precedes(left, right)){
            node(left).right = merge_treaps(node(left).right, right);
            pull(left);
            return left;
        }
        node(right).left = merge_treaps(left, node(right).left);
        pull(right);
        return right;
    }

    std::size_t erase_node(
        std::size_t current,
        const Time& time,
        std::size_t& erased
    ){
        if(time_compare(time, node(current).time)){
            node(current).left = erase_node(node(current).left, time, erased);
            pull(current);
            return current;
        }
        if(time_compare(node(current).time, time)){
            node(current).right = erase_node(node(current).right, time, erased);
            pull(current);
            return current;
        }
        erased = current;
        return merge_treaps(node(current).left, node(current).right);
    }

    std::size_t find_node(const Time& time) const{
        std::size_t current = root;
        while(current != NONE){
            if(time_compare(time, node(current).time)){
                current = node(current).left;
            }else if(time_compare(node(current).time, time)){
                current = node(current).right;
            }else{
                return current;
            }
        }
        return NONE;
    }

    void set_active(std::size_t current, const Time& time, bool active){
        if(time_compare(time, node(current).time)){
            set_active(node(current).left, time, active);
        }else if(time_compare(node(current).time, time)){
            set_active(node(current).right, time, active);
        }else{
            node(current).active = active;
        }
        pull(current);
    }

    balance_type raw_prefix_before(
        std::size_t current,
        const Time& time
    ) const{
        if(current == NONE) return 0;
        if(time_compare(node(current).time, time)){
            return raw_sum(node(current).left) + raw_delta(node(current)) +
                raw_prefix_before(node(current).right, time);
        }
        return raw_prefix_before(node(current).left, time);
    }

    balance_type raw_prefix_through(
        std::size_t current,
        const Time& time
    ) const{
        if(current == NONE) return 0;
        if(!time_compare(time, node(current).time)){
            return raw_sum(node(current).left) + raw_delta(node(current)) +
                raw_prefix_through(node(current).right, time);
        }
        return raw_prefix_through(node(current).left, time);
    }

    balance_type bridge_prefix_before(
        std::size_t current,
        const Time& time
    ) const{
        if(current == NONE) return 0;
        if(time_compare(node(current).time, time)){
            return bridge_sum(node(current).left) + bridge_delta(node(current)) +
                bridge_prefix_before(node(current).right, time);
        }
        return bridge_prefix_before(node(current).left, time);
    }

    balance_type bridge_prefix_through(
        std::size_t current,
        const Time& time
    ) const{
        if(current == NONE) return 0;
        if(!time_compare(time, node(current).time)){
            return bridge_sum(node(current).left) + bridge_delta(node(current)) +
                bridge_prefix_through(node(current).right, time);
        }
        return bridge_prefix_through(node(current).left, time);
    }

    static SequenceAggregate concatenate(
        SequenceAggregate left,
        SequenceAggregate right
    ) noexcept{
        if(left.empty) return right;
        if(right.empty) return left;
        return {
            false,
            left.sum + right.sum,
            std::min(left.minimum, left.sum + right.minimum)
        };
    }

    SequenceAggregate raw_aggregate(std::size_t current) const noexcept{
        if(current == NONE) return {};
        return {
            false,
            node(current).raw_sum,
            node(current).raw_minimum
        };
    }

    SequenceAggregate raw_after(
        std::size_t current,
        const Time& time
    ) const{
        if(current == NONE) return {};
        const Node& target = node(current);
        if(!time_compare(time, target.time)){
            return raw_after(target.right, time);
        }
        SequenceAggregate result = raw_after(target.left, time);
        result = concatenate(
            result,
            {false, raw_delta(target), raw_delta(target)}
        );
        return concatenate(result, raw_aggregate(target.right));
    }

    bool subtree_reaches_zero(
        std::size_t current,
        balance_type before
    ) const noexcept{
        return current != NONE &&
            before + node(current).bridge_minimum == 0;
    }

    std::size_t first_zero(
        std::size_t current,
        balance_type before
    ) const{
        const Node& target = node(current);
        if(subtree_reaches_zero(target.left, before)){
            return first_zero(target.left, before);
        }
        const balance_type through =
            before + bridge_sum(target.left) + bridge_delta(target);
        if(through == 0) return current;
        return first_zero(target.right, through);
    }

    std::size_t last_zero(
        std::size_t current,
        balance_type before
    ) const{
        const Node& target = node(current);
        const balance_type through =
            before + bridge_sum(target.left) + bridge_delta(target);
        if(subtree_reaches_zero(target.right, through)){
            return last_zero(target.right, through);
        }
        if(through == 0) return current;
        return last_zero(target.left, before);
    }

    std::size_t last_zero_before(
        std::size_t current,
        const Time& time,
        balance_type before
    ) const{
        if(current == NONE) return NONE;
        const Node& target = node(current);
        if(!time_compare(target.time, time)){
            return last_zero_before(target.left, time, before);
        }
        const balance_type through =
            before + bridge_sum(target.left) + bridge_delta(target);
        const std::size_t in_right =
            last_zero_before(target.right, time, through);
        if(in_right != NONE) return in_right;
        if(through == 0) return current;
        if(subtree_reaches_zero(target.left, before)){
            return last_zero(target.left, before);
        }
        return NONE;
    }

    std::size_t first_zero_after(
        std::size_t current,
        const Time& time,
        balance_type before
    ) const{
        if(current == NONE) return NONE;
        const Node& target = node(current);
        if(!time_compare(time, target.time)){
            const balance_type through =
                before + bridge_sum(target.left) + bridge_delta(target);
            return first_zero_after(target.right, time, through);
        }
        const std::size_t in_left =
            first_zero_after(target.left, time, before);
        if(in_left != NONE) return in_left;
        const balance_type through =
            before + bridge_sum(target.left) + bridge_delta(target);
        if(through == 0) return current;
        if(subtree_reaches_zero(target.right, through)){
            return first_zero(target.right, through);
        }
        return NONE;
    }

    std::size_t maximum_inactive_from(
        std::size_t current,
        const Time& lower,
        bool inclusive
    ) const{
        if(current == NONE) return NONE;
        const Node& target = node(current);
        const bool below = time_compare(target.time, lower);
        const bool equal = equivalent_time(target.time, lower);
        if(below || (equal && !inclusive)){
            return maximum_inactive_from(target.right, lower, inclusive);
        }
        std::size_t result =
            maximum_inactive_from(target.left, lower, inclusive);
        if(target.kind == RetroactivePriorityQueueOperationKind::push &&
            !target.active){
            result = choose_maximum(result, current);
        }
        if(target.right != NONE){
            result =
                choose_maximum(result, node(target.right).maximum_inactive);
        }
        return result;
    }

    std::size_t minimum_active_until(
        std::size_t current,
        const Time& upper,
        bool inclusive
    ) const{
        if(current == NONE) return NONE;
        const Node& target = node(current);
        const bool above = time_compare(upper, target.time);
        const bool equal = equivalent_time(target.time, upper);
        if(above || (equal && !inclusive)){
            return minimum_active_until(target.left, upper, inclusive);
        }
        std::size_t result =
            target.left == NONE ? NONE : node(target.left).minimum_active;
        if(target.kind == RetroactivePriorityQueueOperationKind::push &&
            target.active){
            result = choose_minimum(result, current);
        }
        result = choose_minimum(
            result,
            minimum_active_until(target.right, upper, inclusive)
        );
        return result;
    }

    std::size_t acquire_node(
        const Time& time,
        RetroactivePriorityQueueOperationKind kind,
        std::optional<T> value,
        bool active
    ){
        const std::size_t slot = free_slots[--free_count];
        const std::uint64_t serial = next_serial++;
        try{
            nodes[slot].emplace(
                time, kind, std::move(value), active, serial, mix(serial)
            );
        }catch(...){
            ++free_count;
            throw;
        }
        return slot;
    }

    void release_node(std::size_t released) noexcept{
        nodes[released].reset();
        free_slots[free_count++] = released;
    }

    void remove_node(const Time& time){
        std::size_t erased = NONE;
        root = erase_node(root, time, erased);
        release_node(erased);
    }

    void check_insertable(const Time& time) const{
        if(free_count == 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: operation capacity violation "
                "(PartiallyRetroactivePriorityQueue)."
            );
        }
        if(find_node(time) != NONE)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: duplicate time "
                "(PartiallyRetroactivePriorityQueue)."
            );
        }
    }

    [[noreturn]] static void throw_invalid_history(){
        throw std::runtime_error(
            "library assertion fault: invalid history "
            "(PartiallyRetroactivePriorityQueue pop on empty queue)."
        );
    }

    void collect_active(std::size_t current, std::vector<T>& result) const{
        if(current == NONE) return;
        collect_active(node(current).left, result);
        if(node(current).kind == RetroactivePriorityQueueOperationKind::push &&
            node(current).active){
            result.push_back(*node(current).value);
        }
        collect_active(node(current).right, result);
    }

    void clear_subtree(std::size_t current) noexcept{
        if(current == NONE) return;
        const std::size_t left = node(current).left;
        const std::size_t right = node(current).right;
        clear_subtree(left);
        clear_subtree(right);
        release_node(current);
    }

public:
    PartiallyRetroactivePriorityQueue(
        Compare compare_ = Compare{},
        TimeCompare time_compare_ = TimeCompare{}
    ) : compare(std::move(compare_)), time_compare(std::move(time_compare_)){
        for(std::size_t index = 0; index < MAX_OPERATION; ++index){
            free_slots[MAX_OPERATION - index - 1] = index;
        }
    }

    static constexpr std::size_t capacity() noexcept{ return MAX_OPERATION; }
    std::size_t operation_count() const noexcept{ return subtree_size(root); }
    std::size_t size() const noexcept{
        return static_cast<std::size_t>(raw_sum(root));
    }
    bool empty() const noexcept{ return size() == 0; }

    const T& top() const{
        if(root == NONE || node(root).minimum_active == NONE)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: empty queue "
                "(PartiallyRetroactivePriorityQueue::top)."
            );
        }
        return *node(node(root).minimum_active).value;
    }

    bool contains_time(const Time& time) const{
        return find_node(time) != NONE;
    }

    RetroactivePriorityQueueOperationKind operation_kind(
        const Time& time
    ) const{
        const std::size_t found = find_node(time);
        if(found == NONE)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: missing time "
                "(PartiallyRetroactivePriorityQueue::operation_kind)."
            );
        }
        return node(found).kind;
    }

    const T& pushed_value(const Time& time) const{
        const std::size_t found = find_node(time);
        if(found == NONE ||
            node(found).kind != RetroactivePriorityQueueOperationKind::push)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: missing push "
                "(PartiallyRetroactivePriorityQueue::pushed_value)."
            );
        }
        return *node(found).value;
    }

    void insert_push(const Time& time, const T& value){
        check_insertable(time);
        const balance_type before = bridge_prefix_before(root, time);
        const std::size_t bridge =
            before == 0 ? NONE : last_zero_before(root, time, 0);
        const std::size_t inserted = acquire_node(
            time,
            RetroactivePriorityQueueOperationKind::push,
            value,
            false
        );
        root = insert_node(root, inserted);

        std::size_t activated = NONE;
        if(before == 0){
            activated = maximum_inactive_from(root, time, true);
        }else if(bridge == NONE){
            activated = node(root).maximum_inactive;
        }else{
            activated =
                maximum_inactive_from(root, node(bridge).time, false);
        }
        set_active(root, node(activated).time, true);
    }

    void insert_pop(const Time& time){
        check_insertable(time);
        const balance_type raw_before = raw_prefix_before(root, time);
        const SequenceAggregate suffix = raw_after(root, time);
        if(raw_before < 1 ||
            (!suffix.empty && raw_before + suffix.minimum < 1))[[unlikely]]{
            throw_invalid_history();
        }

        const balance_type before = bridge_prefix_before(root, time);
        std::size_t deactivated = NONE;
        if(before == 0){
            deactivated = minimum_active_until(root, time, false);
        }else{
            const std::size_t bridge =
                first_zero_after(root, time, 0);
            deactivated =
                bridge == NONE ?
                node(root).minimum_active :
                minimum_active_until(root, node(bridge).time, true);
        }
        if(deactivated == NONE)[[unlikely]] throw_invalid_history();
        const std::size_t inserted = acquire_node(
            time,
            RetroactivePriorityQueueOperationKind::pop,
            std::nullopt,
            false
        );
        root = insert_node(root, inserted);
        set_active(root, node(deactivated).time, false);
    }

    void erase(const Time& time){
        const std::size_t erased = find_node(time);
        if(erased == NONE)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: missing time "
                "(PartiallyRetroactivePriorityQueue::erase)."
            );
        }

        if(node(erased).kind == RetroactivePriorityQueueOperationKind::pop){
            const balance_type before = bridge_prefix_before(root, time);
            const std::size_t bridge =
                before == 0 ? NONE : last_zero_before(root, time, 0);
            std::size_t activated = NONE;
            if(before == 0){
                activated = maximum_inactive_from(root, time, true);
            }else if(bridge == NONE){
                activated = node(root).maximum_inactive;
            }else{
                activated =
                    maximum_inactive_from(root, node(bridge).time, false);
            }
            remove_node(time);
            set_active(root, node(activated).time, true);
            return;
        }

        const balance_type raw_through = raw_prefix_through(root, time);
        const SequenceAggregate suffix = raw_after(root, time);
        if(!suffix.empty &&
            raw_through + suffix.minimum < 1)[[unlikely]]{
            throw_invalid_history();
        }

        if(node(erased).active){
            remove_node(time);
            return;
        }

        const balance_type through = bridge_prefix_through(root, time);
        std::size_t deactivated = NONE;
        if(through == 0){
            deactivated = minimum_active_until(root, time, true);
        }else{
            const std::size_t bridge =
                first_zero_after(root, time, 0);
            deactivated =
                bridge == NONE ?
                node(root).minimum_active :
                minimum_active_until(root, node(bridge).time, true);
        }
        if(deactivated == NONE)[[unlikely]] throw_invalid_history();
        remove_node(time);
        set_active(root, node(deactivated).time, false);
    }

    void clear() noexcept{
        clear_subtree(root);
        root = NONE;
    }

    std::vector<T> values() const{
        std::vector<T> result;
        result.reserve(size());
        collect_active(root, result);
        std::sort(result.begin(), result.end(), compare);
        return result;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_HEAP_PARTIALLY_RETROACTIVE_PRIORITY_QUEUE_HPP_INCLUDED
