// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/structure/convex_hull_trick/slope_trick.hpp"

namespace{

constexpr long long INF = (1LL << 60);

struct NaiveConvexFunction{
    static constexpr int BOUND = 4096;
    std::vector<long long> value = std::vector<long long>(2 * BOUND + 1, 0);

    static int index(int x){ return x + BOUND; }

    void add_const(long long c){
        for(auto& y: value) y += c;
    }

    void add_a_minus_x(int a){
        for(int x = -BOUND; x <= BOUND; x++){
            value[static_cast<std::size_t>(index(x))] += std::max(a - x, 0);
        }
    }

    void add_x_minus_a(int a){
        for(int x = -BOUND; x <= BOUND; x++){
            value[static_cast<std::size_t>(index(x))] += std::max(x - a, 0);
        }
    }

    void add_abs(int a){
        for(int x = -BOUND; x <= BOUND; x++){
            value[static_cast<std::size_t>(index(x))] += std::abs(x - a);
        }
    }

    void prefix_min(){
        for(std::size_t i = 1; i < value.size(); i++){
            value[i] = std::min(value[i], value[i - 1]);
        }
    }

    void suffix_min(){
        for(std::size_t i = value.size() - 1; i > 0; i--){
            value[i - 1] = std::min(value[i - 1], value[i]);
        }
    }

    void shift(int left, int right){
        std::vector<long long> next(value.size(), INF);
        for(int x = -BOUND; x <= BOUND; x++){
            long long best = INF;
            for(int y = x - right; y <= x - left; y++){
                if(-BOUND <= y && y <= BOUND){
                    best = std::min(
                        best,
                        value[static_cast<std::size_t>(index(y))]
                    );
                }
            }
            next[static_cast<std::size_t>(index(x))] = best;
        }
        value.swap(next);
    }

    long long eval(int x) const{
        return value[static_cast<std::size_t>(index(x))];
    }

    long long min_f() const{
        return *std::min_element(value.begin(), value.end());
    }
};

void test_basic(){
    SlopeTrick<long long, 16> slope;
    assert(slope.empty());
    assert(slope.size() == 0);
    assert(slope.left_size() == 0);
    assert(slope.right_size() == 0);
    assert(slope.min_f() == 0);
    assert(slope.get_min() == std::tuple(
        0LL, std::numeric_limits<long long>::lowest(),
        std::numeric_limits<long long>::max()));
    assert(slope.argmin() == std::pair(
        std::numeric_limits<long long>::lowest(),
        std::numeric_limits<long long>::max()
    ));

    slope.add_abs(3);
    assert(slope.left_size() == 1);
    assert(slope.right_size() == 1);
    assert(slope.min_f() == 0);
    assert(slope.argmin() == std::pair(3LL, 3LL));
    assert(slope.eval(0) == 3);

    slope.add_abs(-2);
    assert(slope.min_f() == 5);
    assert(slope.argmin() == std::pair(-2LL, 3LL));
    assert(slope.eval(0) == 5);

    slope.add_const(7);
    assert(slope.min_f() == 12);
    slope.shift(4);
    assert(slope.argmin() == std::pair(2LL, 7LL));
    slope.shift(-1, 2);
    assert(slope.argmin() == std::pair(1LL, 9LL));

    slope.prefix_min();
    assert(slope.right_size() == 0);
    assert(slope.argmin() == std::pair(
        1LL, std::numeric_limits<long long>::max()
    ));
    assert(slope.eval(20) == 12);
    slope.suffix_min();
    assert(slope.left_size() == 0);
    assert(slope.argmin() == std::pair(
        std::numeric_limits<long long>::lowest(),
        std::numeric_limits<long long>::max()
    ));
    assert(slope.eval(-100) == 12);

    slope.clear();
    assert(slope.empty());
    assert(slope.min_f() == 0);
    assert(slope.eval(100) == 0);
}

void test_random(){
    SlopeTrick<long long, 1024> slope;
    NaiveConvexFunction naive;
    std::mt19937 rng(20260713);

    for(int step = 0; step < 600; step++){
        int type = static_cast<int>(rng() % 9);
        int a = static_cast<int>(rng() % 101) - 50;
        if(type == 0){
            int c = static_cast<int>(rng() % 21) - 10;
            slope.add_const(c);
            naive.add_const(c);
        }else if(type == 1){
            slope.add_a_minus_x(a);
            naive.add_a_minus_x(a);
        }else if(type == 2){
            slope.add_x_minus_a(a);
            naive.add_x_minus_a(a);
        }else if(type == 3){
            slope.add_abs(a);
            naive.add_abs(a);
        }else if(type == 4){
            int amount = static_cast<int>(rng() % 7) - 3;
            slope.shift(amount);
            naive.shift(amount, amount);
        }else if(type == 5){
            int left = static_cast<int>(rng() % 7) - 3;
            int right = left + static_cast<int>(rng() % 5);
            slope.shift(left, right);
            naive.shift(left, right);
        }else if(type == 6 || type == 7){
            slope.prefix_min();
            naive.prefix_min();
        }else{
            slope.suffix_min();
            naive.suffix_min();
        }

        for(int repeat = 0; repeat < 12; repeat++){
            int x = static_cast<int>(rng() % 257) - 128;
            assert(slope.eval(x) == naive.eval(x));
        }
        assert(slope.min_f() == naive.min_f());
    }
}

void test_errors(){
    SlopeTrick<long long, 1> slope;
    slope.add_abs(0);
    bool capacity = false;
    try{
        slope.add_abs(1);
    }catch(const std::runtime_error&){
        capacity = true;
    }
    assert(capacity);
    assert(slope.eval(5) == 5);

    bool invalid_interval = false;
    try{
        slope.shift(2, 1);
    }catch(const std::runtime_error&){
        invalid_interval = true;
    }
    assert(invalid_interval);

    SlopeTrick<long long, 0> constant(13);
    assert(constant.min_f() == 13);
    bool zero_capacity = false;
    try{
        constant.add_x_minus_a(0);
    }catch(const std::runtime_error&){
        zero_capacity = true;
    }
    assert(zero_capacity);
}

}  // namespace

int main(){
    int query_count;
    if(std::cin >> query_count){
        SlopeTrick<long long, 4096> slope;
        while(query_count--){
            std::string operation;
            std::cin >> operation;
            if(operation == "CONST"){
                long long value;
                std::cin >> value;
                slope.add_const(value);
            }else if(operation == "A_MINUS_X"){
                long long a;
                std::cin >> a;
                slope.add_a_minus_x(a);
            }else if(operation == "X_MINUS_A"){
                long long a;
                std::cin >> a;
                slope.add_x_minus_a(a);
            }else if(operation == "ABS"){
                long long a;
                std::cin >> a;
                slope.add_abs(a);
            }else if(operation == "CLEAR_LEFT"){
                slope.clear_left();
            }else if(operation == "CLEAR_RIGHT"){
                slope.clear_right();
            }else if(operation == "PREFIX_MIN"){
                slope.prefix_min();
            }else if(operation == "CUMULATIVE_MIN"){
                slope.cumulative_min();
            }else if(operation == "SUFFIX_MIN"){
                slope.suffix_min();
            }else if(operation == "SHIFT"){
                long long amount;
                std::cin >> amount;
                slope.shift(amount);
            }else if(operation == "SHIFT_RANGE"){
                long long left, right;
                std::cin >> left >> right;
                slope.shift(left, right);
            }else if(operation == "EVAL"){
                long long x;
                std::cin >> x;
                std::cout << slope.eval(x) << '\n';
            }else if(operation == "MIN"){
                std::cout << slope.min_f() << '\n';
            }else if(operation == "ARGMIN"){
                const auto [left, right] = slope.argmin();
                std::cout << left << ' ' << right << '\n';
            }else if(operation == "CLEAR"){
                slope.clear();
            }else{
                throw std::runtime_error("unknown operation");
            }
        }
        return 0;
    }

    test_basic();
    test_random();
    test_errors();
}
