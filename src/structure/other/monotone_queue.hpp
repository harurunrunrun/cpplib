#pragma once

#include <array>
#include <functional>
#include <memory>
#include <stdexcept>
#include <vector>

template<class T, int MAX_SIZE, class Compare = std::less<T>>
struct MonotoneQueue{
    static_assert(MAX_SIZE > 0);

private:
    struct State{
        std::array<T, MAX_SIZE> values;
        std::array<long long, MAX_SIZE> indices{};
    };

    int head = 0;
    int count = 0;
    bool has_last_index = false;
    long long last_index = 0;
    Compare comp;
    std::unique_ptr<State> state;

    int position(int offset) const{
        int result = head + offset;
        if(result >= MAX_SIZE) result -= MAX_SIZE;
        return result;
    }

    [[noreturn]] static void throw_empty(const char* operation){
        throw std::runtime_error(operation);
    }

public:
    explicit MonotoneQueue(const Compare& compare = Compare()):
        comp(compare), state(std::make_unique<State>()){}

    MonotoneQueue(const MonotoneQueue&) = delete;
    MonotoneQueue& operator=(const MonotoneQueue&) = delete;
    MonotoneQueue(MonotoneQueue&&) = default;
    MonotoneQueue& operator=(MonotoneQueue&&) = default;

    int size() const{ return count; }
    bool empty() const{ return count == 0; }

    void clear(){
        head = 0;
        count = 0;
        has_last_index = false;
    }

    void push(long long index, const T& value){
        if(has_last_index && index <= last_index)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: indices must be strictly increasing (MonotoneQueue::push)."
            );
        }
        while(count > 0){
            int back = position(count - 1);
            if(comp(state->values[static_cast<std::size_t>(back)], value)) break;
            count--;
        }
        if(count == MAX_SIZE)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: capacity exceeded (MonotoneQueue::push)."
            );
        }
        int tail = position(count);
        state->values[static_cast<std::size_t>(tail)] = value;
        state->indices[static_cast<std::size_t>(tail)] = index;
        count++;
        has_last_index = true;
        last_index = index;
    }

    void pop_front(){
        if(count == 0)[[unlikely]]{
            throw_empty("library assertion fault: empty queue (MonotoneQueue::pop_front).");
        }
        head++;
        if(head == MAX_SIZE) head = 0;
        count--;
    }

    void expire(long long first_valid_index){
        while(count > 0 &&
              state->indices[static_cast<std::size_t>(head)] < first_valid_index){
            pop_front();
        }
    }

    const T& best() const{
        if(count == 0)[[unlikely]]{
            throw_empty("library assertion fault: empty queue (MonotoneQueue::best).");
        }
        return state->values[static_cast<std::size_t>(head)];
    }

    long long best_index() const{
        if(count == 0)[[unlikely]]{
            throw_empty("library assertion fault: empty queue (MonotoneQueue::best_index).");
        }
        return state->indices[static_cast<std::size_t>(head)];
    }
};

template<class T, int MAX_SIZE>
using MonotoneMinQueue = MonotoneQueue<T, MAX_SIZE, std::less<T>>;

template<class T, int MAX_SIZE>
using MonotoneMaxQueue = MonotoneQueue<T, MAX_SIZE, std::greater<T>>;

template<class T, int MAX_WINDOW, class Compare>
std::vector<T> sliding_window_extreme(
    const std::vector<T>& values,
    int window,
    const Compare& compare
){
    if(window <= 0 || MAX_WINDOW < window ||
       static_cast<int>(values.size()) < window)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation (sliding_window_extreme)."
        );
    }
    MonotoneQueue<T, MAX_WINDOW, Compare> queue(compare);
    std::vector<T> result;
    result.reserve(values.size() - static_cast<std::size_t>(window) + 1);
    for(int i = 0; i < static_cast<int>(values.size()); i++){
        queue.expire(static_cast<long long>(i) - window + 1);
        queue.push(i, values[static_cast<std::size_t>(i)]);
        if(i + 1 >= window) result.push_back(queue.best());
    }
    return result;
}

template<class T, int MAX_WINDOW>
std::vector<T> sliding_window_min(const std::vector<T>& values, int window){
    return sliding_window_extreme<T, MAX_WINDOW>(values, window, std::less<T>());
}
