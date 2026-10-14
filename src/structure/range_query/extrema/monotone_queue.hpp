#ifndef CPPLIB_SRC_STRUCTURE_RANGE_QUERY_EXTREMA_MONOTONE_QUEUE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_RANGE_QUERY_EXTREMA_MONOTONE_QUEUE_HPP_INCLUDED

#include <array>
#include <limits>
#include <optional>
#include "../../array/static_or_vector.hpp"
#include <algorithm>
#include <functional>
#include <memory>
#include <stdexcept>
#include <vector>

template<class T, int MAX_SIZE = -1, class Compare = std::less<T>>
struct MonotoneQueue{
    static_assert(MAX_SIZE == -1 || MAX_SIZE > 0);

private:
    struct State{
        cpplib_detail::StaticOrVector<T, MAX_SIZE> values;
        cpplib_detail::StaticOrVector<long long, MAX_SIZE> indices{};
    };

    int head = 0;
    int count = 0;
    bool has_last_index = false;
    long long last_index = 0;
    Compare comp;
    std::unique_ptr<State> state;

    int position(int offset) const{
        const std::size_t capacity = state->values.size();
        const std::size_t result = static_cast<std::size_t>(head) + offset;
        return static_cast<int>(result >= capacity ? result - capacity : result);
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
        std::optional<T> copied;
        if constexpr(MAX_SIZE < 0){
            copied.emplace(value);
            if(static_cast<std::size_t>(count) == state->values.size()){
                if(count == std::numeric_limits<int>::max()) throw std::length_error("MonotoneQueue size overflow");
                const std::size_t next = std::min(static_cast<std::size_t>(std::numeric_limits<int>::max()), std::max(std::size_t(1), state->values.size() * 2));
                cpplib_detail::StaticOrVector<T, -1> values(next);
                cpplib_detail::StaticOrVector<long long, -1> indices(next);
                for(int i = 0; i < count; ++i){
                    const auto old = position(i);
                    values[i] = std::move(state->values[old]);
                    indices[i] = state->indices[old];
                }
                state->values.swap(values);
                state->indices.swap(indices);
                head = 0;
            }
        }
        int tail = position(count);
        state->values[static_cast<std::size_t>(tail)] = MAX_SIZE < 0 ? *copied : value;
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
        if(static_cast<std::size_t>(head) == state->values.size()) head = 0;
        count--;
    }

    void expire(long long first_valid_index){
        while(count > 0 &&
              state->indices[static_cast<std::size_t>(head)] < first_valid_index){
            pop_front();
        }
    }

    typename cpplib_detail::StaticOrVector<T, MAX_SIZE>::const_reference best() const{
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

template<class T, int MAX_SIZE = -1>
using MonotoneMinQueue = MonotoneQueue<T, MAX_SIZE, std::less<T>>;

template<class T, int MAX_SIZE = -1>
using MonotoneMaxQueue = MonotoneQueue<T, MAX_SIZE, std::greater<T>>;

template<class T, int MAX_WINDOW = -1, class Compare = std::less<T>>
std::vector<T> sliding_window_extreme(
    const std::vector<T>& values,
    int window,
    const Compare& compare
){
    if(window <= 0 || (MAX_WINDOW >= 0 && MAX_WINDOW < window) ||
       cpplib_detail::checked_int_size(values.size()) < window)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation (sliding_window_extreme)."
        );
    }
    MonotoneQueue<T, MAX_WINDOW, Compare> queue(compare);
    std::vector<T> result;
    result.reserve(values.size() - static_cast<std::size_t>(window) + 1);
    for(int i = 0; i < cpplib_detail::checked_int_size(values.size()); i++){
        queue.expire(static_cast<long long>(i) - window + 1);
        queue.push(i, values[static_cast<std::size_t>(i)]);
        if(i + 1 >= window) result.push_back(queue.best());
    }
    return result;
}

template<class T, int MAX_WINDOW = -1>
std::vector<T> sliding_window_min(const std::vector<T>& values, int window){
    return sliding_window_extreme<T, MAX_WINDOW>(values, window, std::less<T>());
}

#endif  // CPPLIB_SRC_STRUCTURE_RANGE_QUERY_EXTREMA_MONOTONE_QUEUE_HPP_INCLUDED
