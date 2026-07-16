// competitive-verifier: STANDALONE

#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/structure/other/threshold_updated_indexed_multiset.hpp"

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int case_count;
    std::cin >> case_count;
    while(case_count--){
        int size, operation_count;
        std::cin >> size >> operation_count;
        std::vector<long long> values(static_cast<std::size_t>(size));
        for(long long& value: values) std::cin >> value;
        ThresholdUpdatedIndexedMultiset<> multiset(values);
        while(operation_count--){
            char operation;
            long long argument;
            std::cin >> operation >> argument;
            if(operation == 'I'){
                try{
                    multiset.increment(static_cast<int>(argument));
                    std::cout << "ok\n";
                }catch(const std::overflow_error&){
                    std::cout << "overflow\n";
                }
            }else if(operation == 'C'){
                std::cout << multiset.count_at_least(argument) << '\n';
            }else if(operation == 'D'){
                try{
                    multiset.decrement_at_least(argument);
                    std::cout << "ok\n";
                }catch(const std::overflow_error&){
                    std::cout << "overflow\n";
                }
            }else{
                std::cout << multiset.get(static_cast<int>(argument)) << '\n';
            }
        }
    }
}
