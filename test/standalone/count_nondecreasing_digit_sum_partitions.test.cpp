// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

#include "../../src/algorithm/string/sequence/count_nondecreasing_digit_sum_partitions.hpp"

namespace{

std::string decode(std::string value){
    return value == "-" ? std::string{} : value;
}

} // namespace

int main(){
    assert(count_nondecreasing_digit_sum_partitions("") == 1);
    assert(count_nondecreasing_digit_sum_partitions("111") == 3);
    assert(count_nondecreasing_digit_sum_partitions("123") == 4);
    assert(count_nondecreasing_digit_sum_partitions("90") == 1);

    bool invalid_threw = false;
    try{
        (void)count_nondecreasing_digit_sum_partitions("12a3");
    }catch(const std::runtime_error&){
        invalid_threw = true;
    }
    assert(invalid_threw);

    bool overflow_threw = false;
    try{
        (void)count_nondecreasing_digit_sum_partitions(std::string(65, '0'));
    }catch(const std::overflow_error&){
        overflow_threw = true;
    }
    assert(overflow_threw);

    int case_count;
    if(!(std::cin >> case_count)) return 0;
    for(int index = 0; index < case_count; ++index){
        std::string digits;
        std::cin >> digits;
        std::cout << count_nondecreasing_digit_sum_partitions(
            decode(std::move(digits))
        ) << '\n';
    }
}
