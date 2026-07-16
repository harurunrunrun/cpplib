// competitive-verifier: STANDALONE

#include <cassert>
#include <cstddef>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/structure/other/bitset_xor_shift.hpp"

template<class Function>
bool throws_runtime_error(Function&& function){
    try{
        function();
    }catch(const std::runtime_error&){
        return true;
    }
    return false;
}

void naive_left(std::vector<unsigned char>& bits, std::size_t shift){
    if(shift >= bits.size()) return;
    const auto old = bits;
    for(std::size_t index = 0; index < bits.size(); ++index){
        if(index >= shift) bits[index] ^= old[index - shift];
    }
}

void naive_right(std::vector<unsigned char>& bits, std::size_t shift){
    if(shift >= bits.size()) return;
    const auto old = bits;
    for(std::size_t index = 0; index < bits.size(); ++index){
        if(index + shift < bits.size()) bits[index] ^= old[index + shift];
    }
}

template<std::size_t MAX_BITS>
void check_equal(
    const BitsetXorShift<MAX_BITS>& actual,
    const std::vector<unsigned char>& expected
){
    assert(actual.size() == expected.size());
    std::size_t count = 0;
    for(std::size_t index = 0; index < expected.size(); ++index){
        assert(actual.test(index) == (expected[index] != 0));
        count += expected[index] != 0;
    }
    assert(actual.count() == count);
    assert(actual.any() == (count != 0));
    assert(actual.none() == (count == 0));
    assert(actual.all() == (count == expected.size()));
}

void self_test(){
    BitsetXorShift<0> zero;
    assert(zero.empty());
    assert(zero.none() && zero.all());
    zero.xor_left_shift(0).xor_right_shift(100).set().flip().reset();
    assert(throws_runtime_error([&]{ (void)zero.test(0); }));

    assert(throws_runtime_error([]{ (void)BitsetXorShift<10>(11); }));
    BitsetXorShift<10> ten(10);
    assert(throws_runtime_error([&]{ ten.set(10); }));
    assert(throws_runtime_error([&]{ ten.flip(10); }));
    ten.set().reset(3).flip(3);
    assert(ten.all());
    ten.flip();
    assert(ten.none());

    BitsetXorShift<130> boundary(130);
    std::vector<unsigned char> boundary_naive(130, 0);
    for(const std::size_t position: {0U, 1U, 62U, 63U, 64U, 65U, 127U, 128U, 129U}){
        boundary.set(position);
        boundary_naive[position] = 1;
    }
    for(const std::size_t shift: {1U, 63U, 64U, 65U, 127U, 129U, 130U, 1000U, 0U}){
        boundary.xor_left_shift(shift);
        naive_left(boundary_naive, shift);
        check_equal(boundary, boundary_naive);
        boundary.xor_right_shift(shift);
        naive_right(boundary_naive, shift);
        check_equal(boundary, boundary_naive);
    }

    BitsetXorShift<20> xor_left(20);
    BitsetXorShift<20> xor_right(20);
    xor_left.set(1).set(7);
    xor_right.set(7).set(19);
    const auto xor_result = xor_left ^ xor_right;
    assert(xor_result.count() == 2 && xor_result.test(1) && xor_result.test(19));
    assert(throws_runtime_error([&]{
        BitsetXorShift<20> short_bits(19);
        xor_left ^= short_bits;
    }));

    std::mt19937_64 random(2026071611);
    for(int iteration = 0; iteration < 2000; ++iteration){
        const std::size_t size = random() % 321;
        BitsetXorShift<320> bits(size);
        std::vector<unsigned char> naive(size, 0);
        for(int operation = 0; operation < 500; ++operation){
            const int kind = static_cast<int>(random() % (size == 0 ? 5 : 8));
            if(kind == 0){
                const std::size_t shift = random() % 500;
                bits.xor_left_shift(shift);
                naive_left(naive, shift);
            }else if(kind == 1){
                const std::size_t shift = random() % 500;
                bits.xor_right_shift(shift);
                naive_right(naive, shift);
            }else if(kind == 2){
                bits.set();
                for(auto& value: naive) value = 1;
            }else if(kind == 3){
                bits.reset();
                for(auto& value: naive) value = 0;
            }else if(kind == 4){
                bits.flip();
                for(auto& value: naive) value ^= 1;
            }else{
                const std::size_t position = random() % size;
                if(kind == 5){
                    const bool value = (random() & 1U) != 0;
                    bits.set(position, value);
                    naive[position] = static_cast<unsigned char>(value);
                }else if(kind == 6){
                    bits.reset(position);
                    naive[position] = 0;
                }else{
                    bits.flip(position);
                    naive[position] ^= 1;
                }
            }
            check_equal(bits, naive);
        }
    }
}

template<std::size_t MAX_BITS>
void print_state(const BitsetXorShift<MAX_BITS>& bits){
    std::cout << bits.count() << ' ';
    for(std::size_t index = 0; index < bits.size(); ++index){
        std::cout << static_cast<int>(bits.test(index));
    }
    std::cout << '\n';
}

int main(){
    std::size_t size;
    std::string initial;
    if(!(std::cin >> size >> initial)){
        self_test();
        return 0;
    }
    BitsetXorShift<4096> bits(size);
    for(std::size_t index = 0; index < size; ++index){
        if(initial[index] == '1') bits.set(index);
    }

    int query_count;
    std::cin >> query_count;
    for(int query = 0; query < query_count; ++query){
        char type;
        std::cin >> type;
        if(type == 'L' || type == 'R'){
            std::size_t shift;
            std::cin >> shift;
            if(type == 'L') bits.xor_left_shift(shift);
            else bits.xor_right_shift(shift);
        }else if(type == 'S'){
            std::size_t position;
            int value;
            std::cin >> position >> value;
            bits.set(position, value != 0);
        }else if(type == 'C'){
            std::size_t position;
            std::cin >> position;
            bits.reset(position);
        }else if(type == 'F'){
            std::size_t position;
            std::cin >> position;
            bits.flip(position);
        }else if(type == 'A'){
            bits.set();
        }else if(type == 'Z'){
            bits.reset();
        }else{
            bits.flip();
        }
        print_state(bits);
    }
}
