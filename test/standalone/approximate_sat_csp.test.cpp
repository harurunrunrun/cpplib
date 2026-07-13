// competitive-verifier: STANDALONE

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
#include <span>
#include <stdexcept>
#include <vector>

#include "../../src/approximate/constraint/cnf.hpp"
#include "../../src/approximate/constraint/min_conflicts.hpp"
#include "../../src/approximate/constraint/sat_local_search.hpp"

namespace {

template<class Exception, class Function>
bool throws_as(Function&& function){
    try{
        function();
    }catch(const Exception&){
        return true;
    }catch(...){
    }
    return false;
}

bool brute_force_satisfiable(const approximate::constraint::CnfFormula& formula){
    if(formula.variable_count() >= std::numeric_limits<std::uint64_t>::digits){
        throw std::invalid_argument("brute-force test instance is too large");
    }
    const std::uint64_t limit = std::uint64_t{1} << formula.variable_count();
    approximate::constraint::BooleanAssignment assignment(formula.variable_count());
    for(std::uint64_t mask = 0; mask < limit; ++mask){
        for(std::size_t variable = 0; variable < formula.variable_count(); ++variable){
            assignment[variable] = static_cast<unsigned char>((mask >> variable) & 1U);
        }
        if(formula.satisfied(assignment)) return true;
    }
    return false;
}

bool check_sat_result(
    const approximate::constraint::CnfFormula& formula,
    const approximate::constraint::SatSearchResult& result,
    bool oracle
){
    using approximate::constraint::is_valid_sat_search_result;
    using approximate::constraint::validate_sat_search_result;
    if(!is_valid_sat_search_result(formula, result)) return false;
    validate_sat_search_result(formula, result);
    if(result.found && (!oracle || !formula.satisfied(result.assignment))) return false;
    if(!oracle && result.found) return false;
    return true;
}

bool run_boundary_tests(){
    using namespace approximate::constraint;
    if(!throws_as<std::invalid_argument>([]{
           static_cast<void>(CnfFormula(1, {{0}}));
       })) return false;
    if(!throws_as<std::invalid_argument>([]{
           static_cast<void>(CnfFormula(1, {{2}}));
       })) return false;
    if(!throws_as<std::invalid_argument>([]{
           static_cast<void>(CnfFormula(
               1, {{std::numeric_limits<CnfLiteral>::min()}}
           ));
       })) return false;
    const CnfFormula formula(1, {{1}});
    if(!throws_as<std::invalid_argument>([&]{
           static_cast<void>(formula.satisfied(BooleanAssignment{}));
       })) return false;
    if(!throws_as<std::invalid_argument>([&]{
           static_cast<void>(formula.satisfied(BooleanAssignment{2}));
       })) return false;
    if(!throws_as<std::out_of_range>([&]{
           static_cast<void>(formula.clause_satisfied(1, BooleanAssignment{1}));
       })) return false;

    std::mt19937_64 rng(7);
    if(!throws_as<std::invalid_argument>([&]{
           static_cast<void>(walksat(formula, rng, 1, 1,
               std::numeric_limits<double>::quiet_NaN()));
       })) return false;
    if(!throws_as<std::invalid_argument>([&]{
           static_cast<void>(walksat(formula, rng, 1, 1, 1.1));
       })) return false;
    if(!throws_as<std::invalid_argument>([&]{
           static_cast<void>(probsat(formula, rng, 1, 1, 1.0));
       })) return false;
    if(!throws_as<std::invalid_argument>([&]{
           static_cast<void>(probsat(formula, rng, 1, 1,
               std::numeric_limits<double>::infinity()));
       })) return false;

    SatSearchResult corrupt;
    corrupt.assignment = {1};
    corrupt.satisfied_clauses = 0;
    corrupt.found = false;
    if(is_valid_sat_search_result(formula, corrupt)) return false;
    if(!throws_as<std::invalid_argument>([&]{
           validate_sat_search_result(formula, corrupt);
       })) return false;

    const CnfFormula empty_clause(0, {{}});
    std::mt19937_64 empty_rng(9);
    const SatSearchResult empty_gsat = gsat(empty_clause, empty_rng, 10, 10);
    const SatSearchResult empty_walksat = walksat(empty_clause, empty_rng, 10, 10);
    const SatSearchResult empty_probsat = probsat(empty_clause, empty_rng, 10, 10);
    if(empty_gsat.found || empty_walksat.found || empty_probsat.found) return false;
    if(empty_gsat.steps != 0 || empty_walksat.steps != 0 || empty_probsat.steps != 0){
        return false;
    }

    const std::vector<std::size_t> bad_domains{2, 0};
    const auto zero_counter = [](
        std::size_t, std::size_t, std::span<const std::size_t>
    ) -> std::size_t { return 0; };
    if(!throws_as<std::invalid_argument>([&]{
           static_cast<void>(min_conflicts(bad_domains, zero_counter, rng, 1, 1));
       })) return false;
    const std::vector<std::size_t> one_domain{1};
    if(!throws_as<std::invalid_argument>([&]{
           static_cast<void>(min_conflicts(
               one_domain, zero_counter, rng, 1, 1,
               std::numeric_limits<double>::quiet_NaN()
           ));
       })) return false;
    const auto negative_counter = [](
        std::size_t, std::size_t, std::span<const std::size_t>
    ) -> int { return -1; };
    if(!throws_as<std::invalid_argument>([&]{
           static_cast<void>(min_conflicts(one_domain, negative_counter, rng, 1, 1));
       })) return false;
    const std::vector<std::size_t> two_variables{1, 1};
    const auto maximum_counter = [](
        std::size_t, std::size_t, std::span<const std::size_t>
    ) -> std::size_t { return std::numeric_limits<std::size_t>::max(); };
    if(!throws_as<std::overflow_error>([&]{
           static_cast<void>(min_conflicts(two_variables, maximum_counter, rng, 1, 1));
       })) return false;
    return true;
}

bool run_csp_tests(){
    using namespace approximate::constraint;
    const std::vector<std::size_t> domains{3, 3, 3};
    const auto triangle_conflicts = [](
        std::size_t variable,
        std::size_t candidate,
        std::span<const std::size_t> assignment
    ) -> std::size_t {
        std::size_t conflicts = 0;
        for(std::size_t other = 0; other < assignment.size(); ++other){
            if(other != variable && assignment[other] == candidate) ++conflicts;
        }
        return conflicts;
    };
    std::mt19937_64 rng(0xC5F2026ULL);
    const MinConflictsResult plain = min_conflicts(
        domains, triangle_conflicts, rng, 100, 10
    );
    const MinConflictsResult randomized = randomized_min_conflicts(
        domains, triangle_conflicts, rng, 100, 10, 0.1
    );
    if(!is_valid_min_conflicts_result(domains, triangle_conflicts, plain)) return false;
    if(!is_valid_min_conflicts_result(domains, triangle_conflicts, randomized)) return false;
    validate_min_conflicts_result(domains, triangle_conflicts, plain);
    validate_min_conflicts_result(domains, triangle_conflicts, randomized);

    const std::vector<std::size_t> impossible_domains{1, 1};
    const auto inequality_conflicts = [](
        std::size_t variable,
        std::size_t candidate,
        std::span<const std::size_t> assignment
    ) -> std::size_t {
        const std::size_t other = variable ^ std::size_t{1};
        return candidate == assignment[other] ? 1 : 0;
    };
    const MinConflictsResult impossible = min_conflicts(
        impossible_domains, inequality_conflicts, rng, 20, 3
    );
    if(impossible.found || impossible.conflict_incidence != 2) return false;
    if(!is_valid_min_conflicts_result(
           impossible_domains, inequality_conflicts, impossible
       )) return false;

    const std::vector<std::size_t> no_variables;
    const auto no_conflicts = [](
        std::size_t, std::size_t, std::span<const std::size_t>
    ) -> std::size_t { return 0; };
    const MinConflictsResult trivial = min_conflicts(
        no_variables, no_conflicts, rng, 0, 0
    );
    return trivial.found && trivial.assignment.empty();
}

}  // namespace

int main(){
    using namespace approximate::constraint;
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    if(!run_boundary_tests() || !run_csp_tests()) return 2;

    std::size_t case_count = 0;
    std::cin >> case_count;
    std::size_t oracle_satisfiable_count = 0;
    for(std::size_t case_index = 0; case_index < case_count; ++case_index){
        std::size_t variable_count = 0;
        std::size_t clause_count = 0;
        unsigned int oracle_value = 0;
        std::cin >> variable_count >> clause_count >> oracle_value;
        std::vector<CnfFormula::Clause> clauses(clause_count);
        for(CnfFormula::Clause& clause : clauses){
            std::size_t literal_count = 0;
            std::cin >> literal_count;
            clause.resize(literal_count);
            for(CnfLiteral& literal : clause) std::cin >> literal;
        }
        const CnfFormula formula(variable_count, std::move(clauses));
        const bool oracle = oracle_value != 0;
        if(brute_force_satisfiable(formula) != oracle) return 3;
        if(oracle) ++oracle_satisfiable_count;

        std::mt19937_64 gsat_rng(0x6A5A7000ULL + case_index);
        std::mt19937_64 walksat_rng(0xA17A7000ULL + case_index);
        std::mt19937_64 probsat_rng(0xB0B5A700ULL + case_index);
        const SatSearchResult gsat_result = gsat(formula, gsat_rng, 80, 8);
        const SatSearchResult walksat_result = walksat(
            formula, walksat_rng, 80, 8, 0.45
        );
        const SatSearchResult probsat_result = probsat(
            formula, probsat_rng, 80, 8, 2.3
        );
        if(!check_sat_result(formula, gsat_result, oracle)
           || !check_sat_result(formula, walksat_result, oracle)
           || !check_sat_result(formula, probsat_result, oracle)){
            return 4;
        }

        std::mt19937_64 zero_rng(123);
        const SatSearchResult zero_budget = gsat(formula, zero_rng, 0, 0);
        if(!check_sat_result(formula, zero_budget, oracle)) return 5;
        if(oracle && !formula.satisfied(BooleanAssignment(variable_count, 0))
           && zero_budget.found){
            return 6;
        }
    }
    std::cout << "OK " << case_count << ' ' << oracle_satisfiable_count << '\n';
}
