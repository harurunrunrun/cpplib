// competitive-verifier: STANDALONE

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stdexcept>

#include "../../src/algorithm/math/fast_matrix_power_mod.hpp"

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
    if(!math::fast_matrix_power_mod({}, 123, 7).empty()){
        throw std::runtime_error("empty matrix power failed");
    }
    if(!throws_exception<std::invalid_argument>([]{
        static_cast<void>(
            math::fast_matrix_power_mod({{1, 2}}, 3, 7)
        );
    })){
        throw std::runtime_error("nonsquare matrix was accepted");
    }
    if(!throws_exception<std::invalid_argument>([]{
        static_cast<void>(
            math::fast_matrix_power_mod({{1}}, 3, 0)
        );
    })){
        throw std::runtime_error("nonpositive modulus was accepted");
    }
}

}  // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    self_check();
    std::size_t query_count;
    if(!(std::cin >> query_count)) return 2;
    for(std::size_t query = 0; query < query_count; ++query){
        std::size_t size;
        std::uint64_t exponent;
        std::int64_t modulus;
        if(!(std::cin >> size >> exponent >> modulus)) return 2;
        math::DynamicModMatrix matrix(
            size, std::vector<std::int64_t>(size)
        );
        for(auto& row: matrix){
            for(std::int64_t& value: row){
                if(!(std::cin >> value)) return 2;
            }
        }
        const math::DynamicModMatrix result =
            math::fast_matrix_power_mod(
                std::move(matrix), exponent, modulus
            );
        bool first = true;
        for(const auto& row: result){
            for(const std::int64_t value: row){
                if(!first) std::cout << ' ';
                first = false;
                std::cout << value;
            }
        }
        std::cout << '\n';
    }
}
