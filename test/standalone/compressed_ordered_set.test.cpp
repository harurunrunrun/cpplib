// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/structure/tree/compressed_ordered_set.hpp"

void self_test(){
    CompressedOrderedSet<int, 4> set({40, 10, 30, 20}, {20, 40, 20});
    assert(set.size() == 2);
    assert(!set.empty());
    assert(set.contains(20));
    assert(!set.contains(25));
    assert(set.kth_smallest(0) == 20);
    assert(set.kth_smallest(1) == 40);
    assert(!set.kth_smallest(2));
    assert(!set.kth_smallest(-1));
    assert(set.kth_smallest_one_based(1) == 20);
    assert(set.kth_smallest_one_based(2) == 40);
    assert(!set.kth_smallest_one_based(0));
    assert(!set.kth_smallest_one_based(std::numeric_limits<int>::min()));
    assert(set.order_of_key(30) == 1);
    assert(set.count_less_equal(20) == 1);
    assert(set.count_less_equal(25) == 1);
    assert(set.predecessor_or_equal(39) == 20);
    assert(set.successor_or_equal(21) == 40);
    set.insert(10);
    set.erase(20);
    assert(set.kth_smallest(0) == 10);

    bool thrown = false;
    try{
        set.insert(50);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        set.erase(50);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        [[maybe_unused]] CompressedOrderedSet<int, 3> too_many({1, 2, 3, 4});
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

int main(){
    int universe_size, initial_size, query_count;
    if(!(std::cin >> universe_size >> initial_size >> query_count)){
        self_test();
        return 0;
    }
    std::vector<int> universe(static_cast<std::size_t>(universe_size));
    std::vector<int> initial(static_cast<std::size_t>(initial_size));
    for(int& value: universe) std::cin >> value;
    for(int& value: initial) std::cin >> value;
    CompressedOrderedSet<int, 256> set(std::move(universe), initial);
    while(query_count--){
        std::string type;
        int value;
        std::cin >> type >> value;
        if(type == "I") set.insert(value);
        else if(type == "E") set.erase(value);
        else if(type == "K"){
            const auto answer = set.kth_smallest_one_based(value);
            if(answer) std::cout << *answer << '\n';
            else std::cout << "NA\n";
        }else if(type == "O"){
            std::cout << set.order_of_key(value) << '\n';
        }else if(type == "C"){
            std::cout << set.count_less_equal(value) << '\n';
        }else if(type == "P"){
            const auto answer = set.predecessor_or_equal(value);
            if(answer) std::cout << *answer << '\n';
            else std::cout << "NA\n";
        }else if(type == "S"){
            const auto answer = set.successor_or_equal(value);
            if(answer) std::cout << *answer << '\n';
            else std::cout << "NA\n";
        }else{
            std::cout << set.size() << '\n';
        }
    }
}
