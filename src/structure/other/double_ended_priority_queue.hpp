#pragma once

#include <algorithm>
#include <array>
#include <bit>
#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>

template<class T, int MAX_SIZE, class Compare = std::less<T>>
struct DoubleEndedPriorityQueue{
    static_assert(MAX_SIZE > 0);

private:
    int count = 0;
    Compare comp;
    std::array<T, MAX_SIZE> heap;

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
            std::swap(heap[static_cast<std::size_t>(index)], heap[static_cast<std::size_t>(grandparent)]);
            index = grandparent;
        }
    }

    void bubble_up_max(int index){
        while(index >= 3){
            int grandparent = (index - 3) >> 2;
            if(!less_value(grandparent, index)) break;
            std::swap(heap[static_cast<std::size_t>(index)], heap[static_cast<std::size_t>(grandparent)]);
            index = grandparent;
        }
    }

    void bubble_up(int index){
        if(index == 0) return;
        int parent = (index - 1) >> 1;
        if(min_level(index)){
            if(less_value(parent, index)){
                std::swap(heap[static_cast<std::size_t>(parent)], heap[static_cast<std::size_t>(index)]);
                bubble_up_max(parent);
            }else{
                bubble_up_min(index);
            }
        }else{
            if(less_value(index, parent)){
                std::swap(heap[static_cast<std::size_t>(parent)], heap[static_cast<std::size_t>(index)]);
                bubble_up_min(parent);
            }else{
                bubble_up_max(index);
            }
        }
    }

    int best_descendant(int index, bool take_min) const{
        int best = -1;
        int child = index * 2 + 1;
        for(int candidate = child; candidate <= child + 1 && candidate < count; candidate++){
            if(best == -1 ||
               (take_min ? less_value(candidate, best) : less_value(best, candidate))){
                best = candidate;
            }
        }
        int grandchild = index * 4 + 3;
        for(int candidate = grandchild; candidate <= grandchild + 3 && candidate < count; candidate++){
            if(best == -1 ||
               (take_min ? less_value(candidate, best) : less_value(best, candidate))){
                best = candidate;
            }
        }
        return best;
    }

    void trickle_down_min(int index){
        while(true){
            int next = best_descendant(index, true);
            if(next == -1) return;
            int first_grandchild = index * 4 + 3;
            if(next >= first_grandchild){
                if(!less_value(next, index)) return;
                std::swap(heap[static_cast<std::size_t>(next)], heap[static_cast<std::size_t>(index)]);
                int parent = (next - 1) >> 1;
                if(less_value(parent, next)){
                    std::swap(heap[static_cast<std::size_t>(parent)], heap[static_cast<std::size_t>(next)]);
                }
                index = next;
            }else{
                if(less_value(next, index)){
                    std::swap(heap[static_cast<std::size_t>(next)], heap[static_cast<std::size_t>(index)]);
                }
                return;
            }
        }
    }

    void trickle_down_max(int index){
        while(true){
            int next = best_descendant(index, false);
            if(next == -1) return;
            int first_grandchild = index * 4 + 3;
            if(next >= first_grandchild){
                if(!less_value(index, next)) return;
                std::swap(heap[static_cast<std::size_t>(next)], heap[static_cast<std::size_t>(index)]);
                int parent = (next - 1) >> 1;
                if(less_value(next, parent)){
                    std::swap(heap[static_cast<std::size_t>(parent)], heap[static_cast<std::size_t>(next)]);
                }
                index = next;
            }else{
                if(less_value(index, next)){
                    std::swap(heap[static_cast<std::size_t>(next)], heap[static_cast<std::size_t>(index)]);
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
    ): count(static_cast<int>(values.size())), comp(compare){
        if(count > MAX_SIZE)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: capacity exceeded (DoubleEndedPriorityQueue)."
            );
        }
        for(int i = 0; i < count; i++) heap[static_cast<std::size_t>(i)] = values[static_cast<std::size_t>(i)];
        for(int i = count / 2 - 1; i >= 0; i--) trickle_down(i);
    }

    int size() const{ return count; }
    bool empty() const{ return count == 0; }
    void clear(){ count = 0; }

    const T& min() const{
        if(count == 0)[[unlikely]]{
            throw_empty("library assertion fault: empty queue (DoubleEndedPriorityQueue::min).");
        }
        return heap[0];
    }

    const T& max() const{
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
        heap[static_cast<std::size_t>(count)] = value;
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
