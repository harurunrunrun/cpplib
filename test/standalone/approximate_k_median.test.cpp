// competitive-verifier: STANDALONE

#include <algorithm>
#include <bit>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#include "../../src/approximate/facility/add_drop_k_median.hpp"
#include "../../src/approximate/facility/drop_k_median.hpp"
#include "../../src/approximate/facility/local_search_k_median.hpp"
#include "../../src/approximate/facility/vertex_substitution.hpp"

using approximate::facility::KMedianResult;

void validate_result(
    const std::vector<std::vector<long double>>& distance,
    const KMedianResult& result,
    std::size_t k
){
    assert(result.facilities.size() == k);
    assert(std::is_sorted(result.facilities.begin(), result.facilities.end()));
    assert(std::adjacent_find(result.facilities.begin(), result.facilities.end())
           == result.facilities.end());
    const auto evaluated = approximate::facility::evaluate_k_median(
        distance, result.facilities
    );
    assert(evaluated.cost == result.cost);
    assert(evaluated.assignment == result.assignment);
}

void print_result(const std::string& name, const KMedianResult& result){
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
    std::size_t k;
    if(!(std::cin >> facility_count >> client_count >> k)) return 0;
    std::vector<std::vector<long double>> distance(
        facility_count, std::vector<long double>(client_count)
    );
    for(auto& row: distance) for(long double& value: row) std::cin >> value;

    const auto added = approximate::facility::add_k_median(distance, k);
    const auto dropped = approximate::facility::drop_k_median(distance, k);
    std::vector<std::size_t> initial(k);
    for(std::size_t index = 0; index < k; ++index) initial[index] = index;
    const auto teitz = approximate::facility::teitz_bart_k_median(distance, initial);
    const auto vertex = approximate::facility::vertex_substitution_k_median(distance, initial);
    const auto add_drop = approximate::facility::add_drop_k_median(distance, k);
    const auto local = approximate::facility::local_search_k_median(distance, k);
    validate_result(distance, added, k);
    validate_result(distance, dropped, k);
    validate_result(distance, teitz, k);
    validate_result(distance, add_drop, k);
    assert(vertex.facilities == teitz.facilities && vertex.cost == teitz.cost);
    assert(local.facilities == add_drop.facilities && local.cost == add_drop.cost);

    if(facility_count >= std::numeric_limits<std::uint64_t>::digits){
        throw std::invalid_argument("standalone oracle requires fewer than 64 facilities");
    }
    KMedianResult optimum;
    optimum.cost = std::numeric_limits<long double>::infinity();
    const std::uint64_t limit = std::uint64_t{1} << facility_count;
    for(std::uint64_t mask = 0; mask < limit; ++mask){
        if(static_cast<std::size_t>(std::popcount(mask)) != k) continue;
        std::vector<std::size_t> facilities;
        for(std::size_t facility = 0; facility < facility_count; ++facility){
            if(((mask >> facility) & std::uint64_t{1}) != 0U) facilities.push_back(facility);
        }
        const auto candidate = approximate::facility::evaluate_k_median(distance, facilities);
        if(approximate::facility::detail::better_result(candidate, optimum)) optimum = candidate;
    }
    assert(optimum.cost <= added.cost);
    assert(optimum.cost <= dropped.cost);
    assert(optimum.cost <= teitz.cost);
    assert(optimum.cost <= add_drop.cost);
    for(std::size_t removed: add_drop.facilities){
        for(std::size_t added_facility = 0; added_facility < facility_count; ++added_facility){
            if(std::binary_search(
                add_drop.facilities.begin(), add_drop.facilities.end(), added_facility
            )) continue;
            auto trial = add_drop.facilities;
            *std::lower_bound(trial.begin(), trial.end(), removed) = added_facility;
            const auto candidate = approximate::facility::evaluate_k_median(distance, trial);
            assert(candidate.cost >= add_drop.cost);
        }
    }
    print_result("ADD", added);
    print_result("DROP", dropped);
    print_result("TEITZ", teitz);
    print_result("ADD_DROP", add_drop);
    print_result("OPT", optimum);

    bool invalid = false;
    try{ static_cast<void>(approximate::facility::add_k_median(distance, 0)); }
    catch(const std::invalid_argument&){ invalid = true; }
    assert(invalid);
    invalid = false;
    try{ static_cast<void>(approximate::facility::add_k_median(
        distance, facility_count + 1
    )); }catch(const std::invalid_argument&){ invalid = true; }
    assert(invalid);
    invalid = false;
    try{ static_cast<void>(approximate::facility::add_k_median(
        std::vector<std::vector<double>>{}, 1
    )); }catch(const std::invalid_argument&){ invalid = true; }
    assert(invalid);
    invalid = false;
    try{ static_cast<void>(approximate::facility::drop_k_median(
        std::vector<std::vector<double>>{{0.0}, {}}, 1
    )); }catch(const std::invalid_argument&){ invalid = true; }
    assert(invalid);
    invalid = false;
    try{ static_cast<void>(approximate::facility::add_k_median(
        std::vector<std::vector<double>>{{-1.0}}, 1
    )); }catch(const std::invalid_argument&){ invalid = true; }
    assert(invalid);
    invalid = false;
    try{ static_cast<void>(approximate::facility::add_k_median(
        std::vector<std::vector<double>>{{std::numeric_limits<double>::infinity()}}, 1
    )); }catch(const std::invalid_argument&){ invalid = true; }
    assert(invalid);
    invalid = false;
    try{ static_cast<void>(approximate::facility::evaluate_k_median(
        std::vector<std::vector<double>>{{0.0}, {1.0}}, {0, 0}
    )); }catch(const std::invalid_argument&){ invalid = true; }
    assert(invalid);
    invalid = false;
    try{ static_cast<void>(approximate::facility::add_k_median(
        std::vector<std::vector<long double>>{{std::numeric_limits<long double>::max(),
            std::numeric_limits<long double>::max()}}, 1
    )); }catch(const std::overflow_error&){ invalid = true; }
    assert(invalid);
}
