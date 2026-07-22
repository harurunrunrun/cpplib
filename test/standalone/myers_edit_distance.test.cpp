// competitive-verifier: STANDALONE

#include "../../src/algorithm/string/sequence/myers_edit_distance.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

std::size_t naive_edit_distance(
    std::string_view first,
    std::string_view second
){
    if(first.size() > second.size()) std::swap(first, second);
    std::vector<std::size_t> distance(first.size() + 1);
    for(std::size_t index = 0; index <= first.size(); index++){
        distance[index] = index;
    }

    for(std::size_t second_index = 0;
        second_index < second.size();
        second_index++
    ){
        std::size_t diagonal = distance[0];
        distance[0] = second_index + 1;
        for(std::size_t first_index = 0;
            first_index < first.size();
            first_index++
        ){
            const std::size_t old = distance[first_index + 1];
            distance[first_index + 1] = std::min({
                old + 1,
                distance[first_index] + 1,
                diagonal +
                    (first[first_index] == second[second_index] ? 0 : 1),
            });
            diagonal = old;
        }
    }
    return distance.back();
}

void check_against_naive(
    const std::string& first,
    const std::string& second
){
    const std::size_t expected =
        naive_edit_distance(first, second);
    assert(
        myers_bit_parallel_edit_distance(first, second) == expected
    );
    assert(
        myers_bit_parallel_edit_distance(second, first) == expected
    );
}

void self_test(){
    check_against_naive("", "");
    check_against_naive("", "abc");
    check_against_naive("a", "");
    check_against_naive("a", "a");
    check_against_naive("a", "b");
    check_against_naive("kitten", "sitting");
    assert(
        myers_bit_parallel_edit_distance("kitten", "sitting") == 3
    );

    std::string all_bytes;
    for(int value = 0; value < 256; value++){
        all_bytes.push_back(
            static_cast<char>(static_cast<unsigned char>(value))
        );
    }
    std::string reversed_bytes = all_bytes;
    std::reverse(reversed_bytes.begin(), reversed_bytes.end());
    check_against_naive(all_bytes, reversed_bytes);
    check_against_naive(
        std::string("\0\xff\0\x80", 4),
        std::string("\xff\0\x81", 3)
    );

    for(std::size_t length: {
        std::size_t(1),
        std::size_t(2),
        std::size_t(62),
        std::size_t(63),
        std::size_t(64),
        std::size_t(65),
        std::size_t(66),
        std::size_t(127),
        std::size_t(128),
        std::size_t(129),
    }){
        std::string pattern(length, 'a');
        for(std::size_t index = 0; index < length; index++){
            pattern[index] = static_cast<char>(
                'a' + static_cast<int>(index % 5)
            );
        }
        check_against_naive(pattern, pattern);

        std::string changed = pattern;
        changed[length / 2] =
            changed[length / 2] == 'z' ? 'y' : 'z';
        check_against_naive(pattern, changed);

        std::string inserted = pattern;
        inserted.insert(inserted.begin() +
            static_cast<std::ptrdiff_t>(length / 2), '#');
        check_against_naive(pattern, inserted);

        std::string erased = pattern;
        erased.erase(erased.begin() +
            static_cast<std::ptrdiff_t>(length / 2));
        check_against_naive(pattern, erased);
    }

    std::mt19937 rng(20260803);
    for(int iteration = 0; iteration < 2400; iteration++){
        const std::size_t first_size = rng() % 151U;
        const std::size_t second_size = rng() % 151U;
        std::string first(first_size, '\0');
        std::string second(second_size, '\0');
        const unsigned alphabet =
            iteration % 5 == 0 ? 256U : 1U + rng() % 8U;
        for(char& value: first){
            value = static_cast<char>(
                static_cast<unsigned char>(rng() % alphabet)
            );
        }
        for(char& value: second){
            value = static_cast<char>(
                static_cast<unsigned char>(rng() % alphabet)
            );
        }
        check_against_naive(first, second);
    }

    for(int iteration = 0; iteration < 240; iteration++){
        const std::size_t first_size = 160U + rng() % 181U;
        const std::size_t second_size = 160U + rng() % 181U;
        std::string first(first_size, '\0');
        std::string second(second_size, '\0');
        for(char& value: first){
            value = static_cast<char>(
                'a' + static_cast<int>(rng() % 6U)
            );
        }
        for(char& value: second){
            value = static_cast<char>(
                'a' + static_cast<int>(rng() % 6U)
            );
        }
        check_against_naive(first, second);
    }

    {
        const std::string pattern(30000, 'a');
        const std::string text(45000, 'a');
        assert(
            myers_bit_parallel_edit_distance(pattern, text) == 15000
        );
    }
    {
        std::string pattern(20000, 'a');
        for(std::size_t index = 0; index < pattern.size(); index++){
            pattern[index] = static_cast<char>(
                'a' + static_cast<int>(index % 4)
            );
        }
        const std::string text = pattern + pattern;
        assert(
            myers_bit_parallel_edit_distance(pattern, text) ==
                pattern.size()
        );
    }
    {
        const std::string first(16000, 'a');
        const std::string second(22000, 'b');
        assert(
            myers_bit_parallel_edit_distance(first, second) ==
                second.size()
        );
    }
}

int main(){
    int case_count;
    if(!(std::cin >> case_count)){
        self_test();
        return 0;
    }

    for(int case_index = 0;
        case_index < case_count;
        case_index++
    ){
        std::size_t first_size, second_size;
        std::cin >> first_size >> second_size;
        std::string first(first_size, '\0');
        std::string second(second_size, '\0');
        for(char& value: first){
            int input;
            std::cin >> input;
            if(input < 0 || 255 < input){
                throw std::runtime_error("invalid byte in Myers test input");
            }
            value = static_cast<char>(
                static_cast<unsigned char>(input)
            );
        }
        for(char& value: second){
            int input;
            std::cin >> input;
            if(input < 0 || 255 < input){
                throw std::runtime_error("invalid byte in Myers test input");
            }
            value = static_cast<char>(
                static_cast<unsigned char>(input)
            );
        }
        std::cout <<
            myers_bit_parallel_edit_distance(first, second) <<
            '\n';
    }
}
