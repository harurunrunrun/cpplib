#ifndef CPPLIB_SRC_STRUCTURE_CONVEX_HULL_TRICK_SLOPE_TRICK_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_CONVEX_HULL_TRICK_SLOPE_TRICK_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <functional>
#include <limits>
#include <stdexcept>
#include <tuple>
#include <utility>

template<class T, int MAX_SIZE>
struct SlopeTrick{
    static_assert(MAX_SIZE >= 0);

private:
    template<class Compare>
    struct FixedHeap{
        int count = 0;
        std::array<T, MAX_SIZE> data{};

        bool empty() const{ return count == 0; }
        int size() const{ return count; }
        const T& top() const{ return data[0]; }
        void clear(){ count = 0; }

        void push(const T& value){
            data[static_cast<std::size_t>(count)] = value;
            count++;
            std::push_heap(data.begin(), data.begin() + count, Compare{});
        }

        T pop(){
            std::pop_heap(data.begin(), data.begin() + count, Compare{});
            count--;
            return data[static_cast<std::size_t>(count)];
        }

        template<class F>
        void for_each(F&& function) const{
            for(int i = 0; i < count; i++){
                function(data[static_cast<std::size_t>(i)]);
            }
        }
    };

    T minimum_value{};
    T left_add{};
    T right_add{};
    FixedHeap<std::less<T>> left_heap;
    FixedHeap<std::greater<T>> right_heap;

    [[noreturn]] static void throw_capacity(const char* operation){
        throw std::runtime_error(operation);
    }

    T left_top() const{ return left_heap.top() + left_add; }
    T right_top() const{ return right_heap.top() + right_add; }

    void push_left(const T& value){ left_heap.push(value - left_add); }
    void push_right(const T& value){ right_heap.push(value - right_add); }

    T pop_left(){ return left_heap.pop() + left_add; }
    T pop_right(){ return right_heap.pop() + right_add; }

    void add_a_minus_x_unchecked(const T& a){
        if(right_heap.empty()){
            push_left(a);
            return;
        }
        const T right = right_top();
        if(right < a){
            minimum_value += a - right;
            pop_right();
            push_right(a);
            push_left(right);
        }else{
            push_left(a);
        }
    }

    void add_x_minus_a_unchecked(const T& a){
        if(left_heap.empty()){
            push_right(a);
            return;
        }
        const T left = left_top();
        if(a < left){
            minimum_value += left - a;
            pop_left();
            push_left(a);
            push_right(left);
        }else{
            push_right(a);
        }
    }

public:
    using value_type = T;

    SlopeTrick() = default;
    explicit SlopeTrick(const T& constant): minimum_value(constant){}

    T min_f() const{ return minimum_value; }
    int size() const{ return left_heap.size() + right_heap.size(); }
    bool empty() const{ return left_heap.empty() && right_heap.empty(); }
    int left_size() const{ return left_heap.size(); }
    int right_size() const{ return right_heap.size(); }

    void clear(){
        minimum_value = T{};
        left_add = T{};
        right_add = T{};
        left_heap.clear();
        right_heap.clear();
    }

    std::pair<T, T> argmin() const{
        const T left = left_heap.empty()
            ? std::numeric_limits<T>::lowest() : left_top();
        const T right = right_heap.empty()
            ? std::numeric_limits<T>::max() : right_top();
        return {left, right};
    }

    std::tuple<T, T, T> get_min() const{
        const auto [left, right] = argmin();
        return {minimum_value, left, right};
    }

    void add_const(const T& value){ minimum_value += value; }

    void add_a_minus_x(const T& a){
        if(left_heap.size() == MAX_SIZE)[[unlikely]]{
            throw_capacity(
                "library assertion fault: capacity exceeded (SlopeTrick::add_a_minus_x)."
            );
        }
        add_a_minus_x_unchecked(a);
    }

    void add_x_minus_a(const T& a){
        if(right_heap.size() == MAX_SIZE)[[unlikely]]{
            throw_capacity(
                "library assertion fault: capacity exceeded (SlopeTrick::add_x_minus_a)."
            );
        }
        add_x_minus_a_unchecked(a);
    }

    void add_abs(const T& a){
        if(left_heap.size() == MAX_SIZE || right_heap.size() == MAX_SIZE)[[unlikely]]{
            throw_capacity(
                "library assertion fault: capacity exceeded (SlopeTrick::add_abs)."
            );
        }
        add_a_minus_x_unchecked(a);
        add_x_minus_a_unchecked(a);
    }

    void clear_left(){ left_heap.clear(); }
    void clear_right(){ right_heap.clear(); }

    void prefix_min(){ clear_right(); }
    void cumulative_min(){ clear_right(); }
    void suffix_min(){ clear_left(); }

    void shift(const T& amount){
        left_add += amount;
        right_add += amount;
    }

    void shift(const T& left, const T& right){
        if(right < left)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: invalid interval (SlopeTrick::shift)."
            );
        }
        left_add += left;
        right_add += right;
    }

    T eval(const T& x) const{
        T result = minimum_value;
        left_heap.for_each([&](const T& stored){
            const T breakpoint = stored + left_add;
            if(x < breakpoint) result += breakpoint - x;
        });
        right_heap.for_each([&](const T& stored){
            const T breakpoint = stored + right_add;
            if(breakpoint < x) result += x - breakpoint;
        });
        return result;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_CONVEX_HULL_TRICK_SLOPE_TRICK_HPP_INCLUDED
