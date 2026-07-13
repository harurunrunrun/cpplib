#include <cstddef>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <vector>

#include "../../src/approximate/scheduling/flow_shop.hpp"

namespace {

bool is_permutation(const std::vector<std::size_t>& order, std::size_t size){
    if(order.size() != size) return false;
    std::vector<unsigned char> used(size, 0);
    for(const std::size_t value: order){
        if(value >= size || used[value] != 0) return false;
        used[value] = 1;
    }
    return true;
}

void print_order(const std::vector<std::size_t>& order){
    std::cout << order.size();
    for(const std::size_t job: order) std::cout << ' ' << job;
}

}  // namespace

int main(){
    using approximate::scheduling::flow_shop_makespan;
    using approximate::scheduling::johnson_two_machine_order;
    using approximate::scheduling::neh_flow_shop_order;

    bool rejected = false;
    try{
        (void)flow_shop_makespan(
            std::vector<std::vector<long long>>{{1}, {2}},
            std::vector<std::size_t>{0, 0}
        );
    }catch(const std::invalid_argument&){
        rejected = true;
    }
    if(!rejected) return 2;
    rejected = false;
    try{
        (void)neh_flow_shop_order(
            std::vector<std::vector<long long>>{{1}, {2, 3}}
        );
    }catch(const std::invalid_argument&){
        rejected = true;
    }
    if(!rejected) return 3;
    rejected = false;
    try{
        (void)johnson_two_machine_order(
            std::vector<long long>{1}, std::vector<long long>{}
        );
    }catch(const std::invalid_argument&){
        rejected = true;
    }
    if(!rejected) return 4;

    std::size_t test_count = 0;
    if(!(std::cin >> test_count)) return 1;
    for(std::size_t test = 0; test < test_count; ++test){
        std::size_t job_count = 0;
        std::size_t machine_count = 0;
        std::cin >> job_count >> machine_count;
        std::vector<std::vector<long long>> processing_time(
            job_count, std::vector<long long>(machine_count)
        );
        for(auto& row: processing_time){
            for(long long& value: row) std::cin >> value;
        }

        std::vector<std::size_t> identity(job_count);
        std::iota(identity.begin(), identity.end(), std::size_t{0});
        const auto neh = neh_flow_shop_order(processing_time);
        if(!is_permutation(neh, job_count)) return 5;
        std::cout << flow_shop_makespan(processing_time, identity) << ' '
                  << flow_shop_makespan(processing_time, neh) << ' ';
        print_order(neh);

        if(machine_count == 2){
            std::vector<long long> first(job_count);
            std::vector<long long> second(job_count);
            for(std::size_t job = 0; job < job_count; ++job){
                first[job] = processing_time[job][0];
                second[job] = processing_time[job][1];
            }
            const auto johnson = johnson_two_machine_order(first, second);
            if(!is_permutation(johnson, job_count)) return 6;
            std::cout << ' ' << flow_shop_makespan(processing_time, johnson) << ' ';
            print_order(johnson);
        }else{
            std::cout << " -1 0";
        }
        std::cout << '\n';
    }
}
