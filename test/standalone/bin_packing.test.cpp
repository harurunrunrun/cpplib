#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "../../src/approximate/packing/bin_packing.hpp"

namespace {

template<class Size>
bool valid_result(
    const approximate::packing::BinPackingResult<Size>& result,
    const std::vector<Size>& item_size,
    const Size& capacity
){
    if(result.bin_of_item.size() != item_size.size()) return false;
    std::vector<Size> remaining(result.bin_count(), capacity);
    for(std::size_t item = 0; item < item_size.size(); ++item){
        const std::size_t bin = result.bin_of_item[item];
        if(bin >= result.bin_count() || remaining[bin] < item_size[item]) return false;
        remaining[bin] -= item_size[item];
    }
    return remaining == result.remaining_capacity;
}

template<class Size>
void print_result(const approximate::packing::BinPackingResult<Size>& result){
    std::cout << result.bin_count();
    for(const std::size_t bin: result.bin_of_item) std::cout << ' ' << bin;
    std::cout << '\n';
}

}  // namespace

int main(){
    using approximate::packing::best_fit_bin_packing;
    using approximate::packing::first_fit_bin_packing;
    using approximate::packing::next_fit_bin_packing;
    using approximate::packing::worst_fit_bin_packing;

    bool rejected = false;
    try{
        (void)first_fit_bin_packing(std::vector<long long>{1}, 0LL);
    }catch(const std::invalid_argument&){
        rejected = true;
    }
    if(!rejected) return 2;
    rejected = false;
    try{
        (void)best_fit_bin_packing(std::vector<long long>{11}, 10LL);
    }catch(const std::invalid_argument&){
        rejected = true;
    }
    if(!rejected) return 3;
    rejected = false;
    try{
        (void)worst_fit_bin_packing(std::vector<long long>{-1}, 10LL);
    }catch(const std::invalid_argument&){
        rejected = true;
    }
    if(!rejected) return 4;

    std::size_t test_count = 0;
    if(!(std::cin >> test_count)) return 1;
    for(std::size_t test = 0; test < test_count; ++test){
        std::size_t item_count = 0;
        long long capacity = 0;
        std::cin >> item_count >> capacity;
        std::vector<long long> item_size(item_count);
        for(long long& value: item_size) std::cin >> value;

        const std::vector results{
            next_fit_bin_packing(item_size, capacity, false),
            first_fit_bin_packing(item_size, capacity, false),
            best_fit_bin_packing(item_size, capacity, false),
            worst_fit_bin_packing(item_size, capacity, false),
            next_fit_bin_packing(item_size, capacity, true),
            first_fit_bin_packing(item_size, capacity, true),
            best_fit_bin_packing(item_size, capacity, true),
            worst_fit_bin_packing(item_size, capacity, true),
        };
        for(const auto& result: results){
            if(!valid_result(result, item_size, capacity)) return 5;
            print_result(result);
        }
    }
}
