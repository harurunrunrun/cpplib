#ifndef CPPLIB_SRC_STRUCTURE_HEAP_PRIORITY_QUEUE_DOUBLE_ENDED_PRIORITY_QUEUE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_HEAP_PRIORITY_QUEUE_DOUBLE_ENDED_PRIORITY_QUEUE_HPP_INCLUDED

#include <algorithm>
#include <array>
#include "../../array/static_or_vector.hpp"
#include <bit>
#include <functional>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

template<class T, int MAX_SIZE = -1, class Compare = std::less<T>>
struct DoubleEndedPriorityQueue{
    static_assert(MAX_SIZE == -1 || MAX_SIZE > 0);

private:
    template<class Left, class Right>
    static void swap_values(Left&& left, Right&& right){
        using std::swap;
        swap(std::forward<Left>(left), std::forward<Right>(right));
    }

    int count = 0;
    Compare comp;
    cpplib_detail::StaticOrVector<T, MAX_SIZE> heap;

    bool min_level(int index) const{
        int depth = static_cast<int>(std::bit_width(static_cast<unsigned>(index + 1))) - 1;
        return (depth & 1) == 0;
    }

    bool less_value(int lhs, int rhs) const{
        return comp(heap[static_cast<std::size_t>(lhs)], heap[static_cast<std::size_t>(rhs)]);
    }

    void bubble_up_min(int index){
        while(index >= 3){
            int grandparent = (index - 3) >> 2;
            if(!less_value(index, grandparent)) break;
            swap_values(heap[static_cast<std::size_t>(index)], heap[static_cast<std::size_t>(grandparent)]);
            index = grandparent;
        }
    }

    void bubble_up_max(int index){
        while(index >= 3){
            int grandparent = (index - 3) >> 2;
            if(!less_value(grandparent, index)) break;
            swap_values(heap[static_cast<std::size_t>(index)], heap[static_cast<std::size_t>(grandparent)]);
            index = grandparent;
        }
    }

    void bubble_up(int index){
        if(index == 0) return;
        int parent = (index - 1) >> 1;
        if(min_level(index)){
            if(less_value(parent, index)){
                swap_values(heap[static_cast<std::size_t>(parent)], heap[static_cast<std::size_t>(index)]);
                bubble_up_max(parent);
            }else{
                bubble_up_min(index);
            }
        }else{
            if(less_value(index, parent)){
                swap_values(heap[static_cast<std::size_t>(parent)], heap[static_cast<std::size_t>(index)]);
                bubble_up_min(parent);
            }else{
                bubble_up_max(index);
            }
        }
    }

    int best_descendant(int index, bool take_min) const{
        int best = -1;
        const long long child = static_cast<long long>(index) * 2 + 1;
        for(long long candidate = child; candidate <= child + 1 && candidate < count; candidate++){
            if(best == -1 ||
               (take_min ? less_value(static_cast<int>(candidate), best) : less_value(best, static_cast<int>(candidate)))){
                best = static_cast<int>(candidate);
            }
        }
        const long long grandchild = static_cast<long long>(index) * 4 + 3;
        for(long long candidate = grandchild; candidate <= grandchild + 3 && candidate < count; candidate++){
            if(best == -1 ||
               (take_min ? less_value(static_cast<int>(candidate), best) : less_value(best, static_cast<int>(candidate)))){
                best = static_cast<int>(candidate);
            }
        }
        return best;
    }

    void trickle_down_min(int index){
        while(true){
            int next = best_descendant(index, true);
            if(next == -1) return;
            const long long first_grandchild = static_cast<long long>(index) * 4 + 3;
            if(next >= first_grandchild){
                if(!less_value(next, index)) return;
                swap_values(heap[static_cast<std::size_t>(next)], heap[static_cast<std::size_t>(index)]);
                int parent = (next - 1) >> 1;
                if(less_value(parent, next)){
                    swap_values(heap[static_cast<std::size_t>(parent)], heap[static_cast<std::size_t>(next)]);
                }
                index = next;
            }else{
                if(less_value(next, index)){
                    swap_values(heap[static_cast<std::size_t>(next)], heap[static_cast<std::size_t>(index)]);
                }
                return;
            }
        }
    }

    void trickle_down_max(int index){
        while(true){
            int next = best_descendant(index, false);
            if(next == -1) return;
            const long long first_grandchild = static_cast<long long>(index) * 4 + 3;
            if(next >= first_grandchild){
                if(!less_value(index, next)) return;
                swap_values(heap[static_cast<std::size_t>(next)], heap[static_cast<std::size_t>(index)]);
                int parent = (next - 1) >> 1;
                if(less_value(next, parent)){
                    swap_values(heap[static_cast<std::size_t>(parent)], heap[static_cast<std::size_t>(next)]);
                }
                index = next;
            }else{
                if(less_value(index, next)){
                    swap_values(heap[static_cast<std::size_t>(next)], heap[static_cast<std::size_t>(index)]);
                }
                return;
            }
        }
    }

    void trickle_down(int index){
        if(min_level(index)) trickle_down_min(index);
        else trickle_down_max(index);
    }

    int max_index() const{
        if(count == 1) return 0;
        if(count == 2) return 1;
        return less_value(1, 2) ? 2 : 1;
    }

    [[noreturn]] static void throw_empty(const char* operation){
        throw std::runtime_error(operation);
    }

public:
    explicit DoubleEndedPriorityQueue(const Compare& compare = Compare()): comp(compare){}

    explicit DoubleEndedPriorityQueue(
        const std::vector<T>& values,
        const Compare& compare = Compare()
    ): count(cpplib_detail::checked_int_size(values.size())), comp(compare){
        if(MAX_SIZE >= 0 && count > MAX_SIZE)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: capacity exceeded (DoubleEndedPriorityQueue)."
            );
        }
        heap.resize(values.size());
        for(int i = 0; i < count; i++) heap[static_cast<std::size_t>(i)] = values[static_cast<std::size_t>(i)];
        for(int i = count / 2 - 1; i >= 0; i--) trickle_down(i);
    }

    int size() const{ return count; }
    bool empty() const{ return count == 0; }
    void clear(){ count = 0; }

    typename cpplib_detail::StaticOrVector<T, MAX_SIZE>::const_reference min() const{
        if(count == 0)[[unlikely]]{
            throw_empty("library assertion fault: empty queue (DoubleEndedPriorityQueue::min).");
        }
        return heap[0];
    }

    typename cpplib_detail::StaticOrVector<T, MAX_SIZE>::const_reference max() const{
        if(count == 0)[[unlikely]]{
            throw_empty("library assertion fault: empty queue (DoubleEndedPriorityQueue::max).");
        }
        return heap[static_cast<std::size_t>(max_index())];
    }

    void push(const T& value){
        if(count == MAX_SIZE)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: capacity exceeded (DoubleEndedPriorityQueue::push)."
            );
        }
        if constexpr(MAX_SIZE < 0){
            if(count == std::numeric_limits<int>::max()) throw std::length_error("DoubleEndedPriorityQueue size overflow");
            T copy = value;
            heap.ensure(static_cast<std::size_t>(count) + 1);
            heap[static_cast<std::size_t>(count)] = std::move(copy);
        }else{
            heap[static_cast<std::size_t>(count)] = value;
        }
        bubble_up(count++);
    }

    T pop_min(){
        if(count == 0)[[unlikely]]{
            throw_empty("library assertion fault: empty queue (DoubleEndedPriorityQueue::pop_min).");
        }
        T result = std::move(heap[0]);
        count--;
        if(count > 0){
            heap[0] = std::move(heap[static_cast<std::size_t>(count)]);
            trickle_down_min(0);
        }
        return result;
    }

    T pop_max(){
        if(count == 0)[[unlikely]]{
            throw_empty("library assertion fault: empty queue (DoubleEndedPriorityQueue::pop_max).");
        }
        int index = max_index();
        T result = std::move(heap[static_cast<std::size_t>(index)]);
        count--;
        if(index < count){
            heap[static_cast<std::size_t>(index)] = std::move(heap[static_cast<std::size_t>(count)]);
            trickle_down_max(index);
        }
        return result;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_HEAP_PRIORITY_QUEUE_DOUBLE_ENDED_PRIORITY_QUEUE_HPP_INCLUDED
