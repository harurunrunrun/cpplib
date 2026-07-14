// competitive-verifier: STANDALONE

#include <cstddef>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "../../src/approximate/scheduling/flow_shop_heuristics.hpp"

namespace {

bool permutation(const std::vector<std::size_t>& order, std::size_t size){
    if(order.size() != size) return false;
    std::vector<unsigned char> used(size, 0);
    for(const std::size_t job: order){
        if(job >= size || used[job] != 0) return false;
        used[job] = 1;
    }
    return true;
}

template<class Function>
bool throws_invalid(Function&& function){
    try{
        function();
    }catch(const std::invalid_argument&){
        return true;
    }
    return false;
}

bool boundary_tests(){
    using namespace approximate::scheduling;
    if(!cds_flow_shop_order(std::vector<std::vector<long long>>{}).empty()) return false;
    if(!throws_invalid([]{
        (void)palmer_flow_shop_order(
            std::vector<std::vector<long long>>{{1}, {1, 2}}
        );
    })) return false;
    if(!throws_invalid([]{
        (void)gupta_flow_shop_order(
            std::vector<std::vector<long long>>{{-1, 2}}
        );
    })) return false;
    bool overflow = false;
    try{
        const long long maximum = std::numeric_limits<long long>::max();
        (void)cds_flow_shop_order(
            std::vector<std::vector<long long>>{{maximum, maximum}, {1, 1}}
        );
    }catch(const std::overflow_error&){
        overflow = true;
    }
    return overflow;
}

void print_result(
    const std::vector<std::vector<long long>>& processing_time,
    const std::vector<std::size_t>& order,
    std::size_t machine_count
){
    const long long makespan = approximate::scheduling::scheduling_internal::
        checked_flow_shop_makespan(processing_time, order, machine_count);
    std::cout << makespan << ' ' << order.size();
    for(const std::size_t job: order) std::cout << ' ' << job;
    std::cout << '\n';
}

}  // namespace

int main(){
    using namespace approximate::scheduling;
    if(!boundary_tests()) return 2;
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
        const auto cds = cds_flow_shop_order(processing_time);
        const auto palmer = palmer_flow_shop_order(processing_time);
        const auto gupta = gupta_flow_shop_order(processing_time);
        if(!permutation(cds, job_count) || !permutation(palmer, job_count)
            || !permutation(gupta, job_count)) return 3;
        print_result(processing_time, cds, machine_count);
        print_result(processing_time, palmer, machine_count);
        print_result(processing_time, gupta, machine_count);
    }
}
