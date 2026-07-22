#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_HORN_SAT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_HORN_SAT_HPP_INCLUDED

#include <cstddef>
#include <deque>
#include <stdexcept>
#include <utility>
#include <vector>

struct HornClause{
    // Represents (not x_1) or ... or (not x_k) or positive_variable.
    // positive_variable == -1 means that the positive literal is absent.
    std::vector<int> negative_variables;
    int positive_variable = -1;
};

struct HornSatResult{
    bool satisfiable = false;
    std::vector<char> minimum_model;
};

inline HornSatResult horn_sat(
    int variable_count,
    const std::vector<HornClause>& clauses
){
    if(variable_count < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: range violation (horn_sat)."
        );
    }

    std::vector<std::vector<std::size_t>> containing(
        static_cast<std::size_t>(variable_count)
    );
    std::vector<std::size_t> remaining(clauses.size());
    for(std::size_t clause_index = 0;
        clause_index < clauses.size();
        ++clause_index
    ){
        const HornClause& clause = clauses[clause_index];
        if(clause.positive_variable < -1
            || variable_count <= clause.positive_variable)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (horn_sat)."
            );
        }
        remaining[clause_index] = clause.negative_variables.size();
        for(int variable: clause.negative_variables){
            if(variable < 0 || variable_count <= variable)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: range violation (horn_sat)."
                );
            }
            containing[static_cast<std::size_t>(variable)].push_back(clause_index);
        }
    }

    std::vector<char> model(static_cast<std::size_t>(variable_count), 0);
    std::deque<int> queue;
    const auto force = [&](int variable) -> void {
        char& value = model[static_cast<std::size_t>(variable)];
        if(value == 0){
            value = 1;
            queue.push_back(variable);
        }
    };

    for(std::size_t clause_index = 0;
        clause_index < clauses.size();
        ++clause_index
    ){
        if(remaining[clause_index] != 0) continue;
        const int positive = clauses[clause_index].positive_variable;
        if(positive == -1) return {false, {}};
        force(positive);
    }

    while(!queue.empty()){
        const int variable = queue.front();
        queue.pop_front();
        for(std::size_t clause_index:
            containing[static_cast<std::size_t>(variable)]
        ){
            std::size_t& count = remaining[clause_index];
            --count;
            if(count != 0) continue;
            const int positive = clauses[clause_index].positive_variable;
            if(positive == -1) return {false, {}};
            force(positive);
        }
    }
    return {true, std::move(model)};
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_HORN_SAT_HPP_INCLUDED
