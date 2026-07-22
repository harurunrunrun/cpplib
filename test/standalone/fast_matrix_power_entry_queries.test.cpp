// competitive-verifier: STANDALONE

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/math/linear_algebra/fast_matrix_multiply_mod.hpp"
#include "../../src/algorithm/math/linear_algebra/fast_matrix_power_entry_queries.hpp"

namespace{

template<class Exception, class Function>
bool throws_exception(Function&& function){
    try{
        function();
    }catch(const Exception&){
        return true;
    }
    return false;
}

void self_check(){
    const std::int64_t maximum_modulus =
        std::numeric_limits<std::int64_t>::max();
    const auto product = math::fast_matrix_multiply_mod(
        std::vector<std::int64_t>{
            std::numeric_limits<std::int64_t>::min(),
            std::numeric_limits<std::int64_t>::max(),
        },
        1,
        2,
        std::vector<std::int64_t>{-1, 2},
        1,
        maximum_modulus
    );
    if(product != std::vector<std::int64_t>{1}){
        throw std::runtime_error("full-range modular product failed");
    }

    const auto zero_inner = math::fast_matrix_multiply_mod(
        std::vector<std::int64_t>{},
        3,
        0,
        std::vector<std::int64_t>{},
        4,
        17
    );
    if(zero_inner != std::vector<std::int64_t>(12, 0)){
        throw std::runtime_error("zero-inner matrix product failed");
    }

    if(!throws_exception<std::invalid_argument>([]{
        static_cast<void>(math::fast_matrix_multiply_mod(
            std::vector<std::int64_t>{1},
            1,
            2,
            std::vector<std::int64_t>{1, 2},
            1,
            7
        ));
    })){
        throw std::runtime_error("invalid flat shape was accepted");
    }
    if(!throws_exception<std::invalid_argument>([]{
        static_cast<void>(math::fast_matrix_multiply_mod(
            std::vector<std::int64_t>{1},
            1,
            1,
            std::vector<std::int64_t>{1},
            1,
            0
        ));
    })){
        throw std::runtime_error("nonpositive product modulus was accepted");
    }
    if(!throws_exception<std::length_error>([]{
        const std::size_t huge =
            std::numeric_limits<std::size_t>::max() / 2 + 1;
        static_cast<void>(math::fast_matrix_multiply_mod(
            std::vector<std::int64_t>{},
            huge,
            2,
            std::vector<std::int64_t>{},
            0,
            7
        ));
    })){
        throw std::runtime_error("flat shape overflow was accepted");
    }

    math::FastMatrixPowerEntryQueries empty({}, 7);
    if(!throws_exception<std::out_of_range>([&]{
        static_cast<void>(empty.entry(0, 0, 0));
    })){
        throw std::runtime_error("empty matrix query was accepted");
    }
    if(!throws_exception<std::invalid_argument>([]{
        math::FastMatrixPowerEntryQueries invalid({{1, 2}}, 7);
    })){
        throw std::runtime_error("nonsquare matrix was accepted");
    }
    if(!throws_exception<std::invalid_argument>([]{
        math::FastMatrixPowerEntryQueries invalid({{1}}, -1);
    })){
        throw std::runtime_error("nonpositive query modulus was accepted");
    }
    math::FastMatrixPowerEntryQueries identity({{1, 0}, {0, 1}}, 1);
    if(identity.entry(0, 0, 0) != 0 || identity.entry(0, 1, 0) != 0){
        throw std::runtime_error("modulus-one identity failed");
    }
    if(!throws_exception<std::out_of_range>([&]{
        static_cast<void>(identity.entry(2, 0, 1));
    })){
        throw std::runtime_error("out-of-range row was accepted");
    }
}

}  // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    self_check();
    std::size_t case_count = 0;
    if(!(std::cin >> case_count)) return 2;
    for(std::size_t case_index = 0; case_index < case_count; ++case_index){
        std::size_t size = 0;
        std::int64_t modulus = 0;
        std::size_t query_count = 0;
        if(!(std::cin >> size >> modulus >> query_count)) return 2;
        math::FastMatrixPowerEntryQueries::input_matrix_type matrix(
            size, std::vector<std::int64_t>(size)
        );
        for(auto& row: matrix){
            for(std::int64_t& value: row){
                if(!(std::cin >> value)) return 2;
            }
        }
        math::FastMatrixPowerEntryQueries queries(matrix, modulus);
        for(std::size_t query = 0; query < query_count; ++query){
            std::size_t row = 0;
            std::size_t column = 0;
            std::uint64_t exponent = 0;
            if(!(std::cin >> row >> column >> exponent)) return 2;
            std::cout << queries.entry(row, column, exponent) << '\n';
        }
    }
}
