// competitive-verifier: STANDALONE

#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <vector>

#include "../../src/algorithm/other/utility/ordered_bound.hpp"

int main(){
    std::vector<int> values{1, 3, 3, 7};
    if(ordered_lower_bound(values, 3) - values.begin() != 1 ||
       ordered_upper_bound(values, 3) - values.begin() != 3 ||
       ordered_last_less_equal(values, 6) - values.begin() != 2 ||
       ordered_last_less(values, 1) != values.end()){
        throw std::runtime_error("ordered vector bound failed");
    }

    const std::set<int> set_values{2, 5, 9};
    if(*ordered_lower_bound(set_values, 3) != 5 ||
       *ordered_upper_bound(set_values, 5) != 9 ||
       *ordered_last_less_equal(set_values, 5) != 5 ||
       ordered_last_less(set_values, 2) != set_values.end()){
        throw std::runtime_error("ordered set bound failed");
    }

    std::map<int, int> mapping{{1, 10}, {4, 40}};
    if(ordered_lower_bound(mapping, 2)->first != 4 ||
       ordered_last_less(mapping, 4)->first != 1){
        throw std::runtime_error("ordered map bound failed");
    }

    std::cout << "OK\n";
}
