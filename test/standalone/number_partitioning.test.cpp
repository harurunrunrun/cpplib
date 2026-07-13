#include <cstddef>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../../src/approximate/packing/number_partitioning.hpp"

namespace {

template<class Weight>
bool valid_result(
    const approximate::packing::PartitionResult<Weight>& result,
    const std::vector<Weight>& item_weight,
    std::size_t part_count
){
    if(result.part_count() != part_count
        || result.part_of_item.size() != item_weight.size()) return false;
    std::vector<Weight> sum(part_count, Weight{});
    for(std::size_t item = 0; item < item_weight.size(); ++item){
        const std::size_t part = result.part_of_item[item];
        if(part >= part_count
            || std::numeric_limits<Weight>::max() - sum[part] < item_weight[item]){
            return false;
        }
        sum[part] += item_weight[item];
    }
    return sum == result.part_sum;
}

template<class Weight>
void print_result(const approximate::packing::PartitionResult<Weight>& result){
    std::cout << result.part_count();
    for(const std::size_t part: result.part_of_item) std::cout << ' ' << part;
    for(const Weight& sum: result.part_sum) std::cout << ' ' << sum;
    std::cout << '\n';
}

}  // namespace

int main(){
    using approximate::packing::greedy_load_balancing;
    using approximate::packing::greedy_multiway_partition;
    using approximate::packing::greedy_number_partition;
    using approximate::packing::greedy_two_way_partition;
    using approximate::packing::karmarkar_karp_partition;
    using approximate::packing::largest_differencing_partition;
    using approximate::packing::round_robin_partition;

    bool rejected = false;
    try{
        (void)greedy_load_balancing(std::vector<unsigned long long>{1}, 0);
    }catch(const std::invalid_argument&){
        rejected = true;
    }
    if(!rejected) return 2;

    rejected = false;
    try{
        (void)greedy_number_partition(std::vector<long long>{-1});
    }catch(const std::invalid_argument&){
        rejected = true;
    }
    if(!rejected) return 3;

    rejected = false;
    try{
        (void)round_robin_partition(
            std::vector<double>{std::numeric_limits<double>::infinity()},
            1
        );
    }catch(const std::invalid_argument&){
        rejected = true;
    }
    if(!rejected) return 8;

    const std::vector<unsigned long long> alias_weight{4, 3, 2, 1};
    const auto two_way_alias = greedy_two_way_partition(alias_weight);
    if(!valid_result(two_way_alias, alias_weight, 2)) return 9;

    rejected = false;
    try{
        (void)round_robin_partition(
            std::vector<unsigned long long>{
                std::numeric_limits<unsigned long long>::max(),
                1,
            },
            1
        );
    }catch(const std::overflow_error&){
        rejected = true;
    }
    if(!rejected) return 4;

    rejected = false;
    try{
        (void)karmarkar_karp_partition(
            std::vector<unsigned long long>{
                std::numeric_limits<unsigned long long>::max(),
                std::numeric_limits<unsigned long long>::max(),
                std::numeric_limits<unsigned long long>::max(),
            }
        );
    }catch(const std::overflow_error&){
        rejected = true;
    }
    if(!rejected) return 5;

    std::size_t test_count = 0;
    if(!(std::cin >> test_count)) return 1;
    for(std::size_t test = 0; test < test_count; ++test){
        std::size_t item_count = 0;
        std::size_t part_count = 0;
        std::cin >> item_count >> part_count;
        std::vector<unsigned long long> item_weight(item_count);
        for(auto& weight: item_weight) std::cin >> weight;

        const auto load = greedy_load_balancing(item_weight, part_count, false);
        const auto load_decreasing =
            greedy_load_balancing(item_weight, part_count, true);
        const auto multiway =
            greedy_multiway_partition(item_weight, part_count);
        const auto round_robin =
            round_robin_partition(item_weight, part_count, false);
        const auto round_robin_decreasing =
            round_robin_partition(item_weight, part_count, true);
        const auto greedy_two_way = greedy_number_partition(item_weight);
        const auto differencing = largest_differencing_partition(item_weight);
        const auto karmarkar_karp = karmarkar_karp_partition(item_weight);

        if(!valid_result(load, item_weight, part_count)
            || !valid_result(load_decreasing, item_weight, part_count)
            || !valid_result(multiway, item_weight, part_count)
            || !valid_result(round_robin, item_weight, part_count)
            || !valid_result(round_robin_decreasing, item_weight, part_count)
            || !valid_result(greedy_two_way, item_weight, 2)
            || !valid_result(differencing, item_weight, 2)
            || !valid_result(karmarkar_karp, item_weight, 2)) return 6;
        if(multiway.part_of_item != load_decreasing.part_of_item
            || differencing.part_of_item != karmarkar_karp.part_of_item) return 7;

        print_result(load);
        print_result(load_decreasing);
        print_result(multiway);
        print_result(round_robin);
        print_result(round_robin_decreasing);
        print_result(greedy_two_way);
        print_result(differencing);
        print_result(karmarkar_karp);
    }
}
