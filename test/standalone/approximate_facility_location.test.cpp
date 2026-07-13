// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "../../src/approximate/facility/facility_location.hpp"

using approximate::facility::FacilityLocationResult;

long double evaluate(
    const std::vector<std::vector<long double>>& distance,
    const std::vector<long double>& opening,
    const std::vector<std::size_t>& facilities
){
    if(facilities.empty()) return 0.0L;
    long double result = 0.0L;
    for(std::size_t facility: facilities) result += opening[facility];
    for(std::size_t client = 0; client < distance.front().size(); ++client){
        long double best = std::numeric_limits<long double>::infinity();
        for(std::size_t facility: facilities) best = std::min(best, distance[facility][client]);
        result += best;
    }
    return result;
}

void validate_result(
    const std::vector<std::vector<long double>>& distance,
    const std::vector<long double>& opening,
    const FacilityLocationResult& result
){
    assert(std::is_sorted(result.facilities.begin(), result.facilities.end()));
    assert(result.labels.size() == distance.front().size());
    assert(result.cost == evaluate(distance, opening, result.facilities));
    for(std::size_t client = 0; client < result.labels.size(); ++client){
        assert(result.labels[client] < result.facilities.size());
        const std::size_t assigned = result.facilities[result.labels[client]];
        for(std::size_t facility: result.facilities){
            assert(distance[assigned][client] <= distance[facility][client]);
        }
    }
}

void print_result(const std::string& name, const FacilityLocationResult& result){
    std::cout << name << ' ' << std::llround(result.cost) << ' '
              << result.facilities.size();
    for(std::size_t facility: result.facilities) std::cout << ' ' << facility;
    std::cout << '\n';
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::size_t facility_count;
    std::size_t client_count;
    if(!(std::cin >> facility_count >> client_count)) return 0;
    std::vector<long double> opening(facility_count);
    for(long double& value: opening) std::cin >> value;
    std::vector<std::vector<long double>> distance(
        facility_count, std::vector<long double>(client_count)
    );
    for(auto& row: distance) for(long double& value: row) std::cin >> value;
    const auto greedy = approximate::facility::greedy_facility_location(distance, opening);
    const auto add_drop = approximate::facility::add_drop_facility_location(distance, opening);
    validate_result(distance, opening, greedy);
    validate_result(distance, opening, add_drop);
    assert(add_drop.cost <= greedy.cost);

    if(facility_count >= std::numeric_limits<std::uint64_t>::digits){
        throw std::invalid_argument("standalone oracle requires fewer than 64 facilities");
    }
    FacilityLocationResult optimum;
    optimum.cost = std::numeric_limits<long double>::infinity();
    const std::uint64_t first_mask = client_count == 0 ? 0U : 1U;
    const std::uint64_t limit = std::uint64_t{1} << facility_count;
    for(std::uint64_t mask = first_mask; mask < limit; ++mask){
        std::vector<std::size_t> facilities;
        for(std::size_t facility = 0; facility < facility_count; ++facility){
            if(((mask >> facility) & std::uint64_t{1}) != 0U) facilities.push_back(facility);
        }
        const long double candidate_cost = evaluate(distance, opening, facilities);
        if(candidate_cost < optimum.cost
           || (candidate_cost == optimum.cost && facilities < optimum.facilities)){
            optimum.cost = candidate_cost;
            optimum.facilities = std::move(facilities);
        }
    }
    assert(optimum.cost <= greedy.cost);
    assert(optimum.cost <= add_drop.cost);
    print_result("GREEDY", greedy);
    print_result("ADD_DROP", add_drop);
    print_result("OPT", optimum);

    bool invalid = false;
    try{ static_cast<void>(approximate::facility::greedy_facility_location(
        std::vector<std::vector<double>>{{0.0}, {}}, std::vector<double>{1.0, 1.0}
    )); }catch(const std::invalid_argument&){ invalid = true; }
    assert(invalid);
    invalid = false;
    try{ static_cast<void>(approximate::facility::greedy_facility_location(
        std::vector<std::vector<double>>{}, std::vector<double>{}
    )); }catch(const std::invalid_argument&){ invalid = true; }
    assert(invalid);
    invalid = false;
    try{ static_cast<void>(approximate::facility::greedy_facility_location(
        std::vector<std::vector<double>>{{std::numeric_limits<double>::quiet_NaN()}},
        std::vector<double>{1.0}
    )); }catch(const std::invalid_argument&){ invalid = true; }
    assert(invalid);
    invalid = false;
    try{ static_cast<void>(approximate::facility::greedy_facility_location(
        std::vector<std::vector<double>>{{0.0}}, std::vector<double>{-1.0}
    )); }catch(const std::invalid_argument&){ invalid = true; }
    assert(invalid);
    invalid = false;
    try{ static_cast<void>(approximate::facility::greedy_facility_location(
        std::vector<std::vector<double>>{{-1.0}}, std::vector<double>{1.0}
    )); }catch(const std::invalid_argument&){ invalid = true; }
    assert(invalid);
    invalid = false;
    try{ static_cast<void>(approximate::facility::greedy_facility_location(
        std::vector<std::vector<long double>>{{std::numeric_limits<long double>::max(),
            std::numeric_limits<long double>::max()}}, std::vector<long double>{0.0L}
    )); }catch(const std::overflow_error&){ invalid = true; }
    assert(invalid);
}
