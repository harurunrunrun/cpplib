#pragma once

#include <array>
#include <stdexcept>
#include <vector>

struct ThreeSatLiteral{
    int variable;
    bool value;
};

struct ThreeSatClause{
    std::array<ThreeSatLiteral, 3> literal;
};

struct ThreeSatResult{
    bool satisfiable;
    std::vector<int> assignment;
};

namespace three_sat_internal{

bool solve_dfs(
    int n,
    const std::vector<ThreeSatClause>& clauses,
    std::vector<int>& assignment
){
    while(true){
        bool changed = false;
        for(const auto& clause: clauses){
            bool satisfied = false;
            int undecided_count = 0;
            ThreeSatLiteral last_undecided{-1, false};
            for(auto lit: clause.literal){
                int current = assignment[static_cast<std::size_t>(lit.variable)];
                if(current == -1){
                    undecided_count++;
                    last_undecided = lit;
                }else if(static_cast<bool>(current) == lit.value){
                    satisfied = true;
                    break;
                }
            }
            if(satisfied) continue;
            if(undecided_count == 0) return false;
            if(undecided_count == 1){
                int& slot = assignment[static_cast<std::size_t>(last_undecided.variable)];
                int value = static_cast<int>(last_undecided.value);
                if(slot != -1 && slot != value) return false;
                if(slot == -1){
                    slot = value;
                    changed = true;
                }
            }
        }
        if(!changed) break;
    }

    int variable = -1;
    std::vector<int> score(static_cast<std::size_t>(n), 0);
    for(const auto& clause: clauses){
        bool satisfied = false;
        for(auto lit: clause.literal){
            int current = assignment[static_cast<std::size_t>(lit.variable)];
            if(current != -1 && static_cast<bool>(current) == lit.value){
                satisfied = true;
                break;
            }
        }
        if(satisfied) continue;
        for(auto lit: clause.literal){
            if(assignment[static_cast<std::size_t>(lit.variable)] == -1){
                score[static_cast<std::size_t>(lit.variable)]++;
            }
        }
    }
    for(int i = 0; i < n; i++){
        if(assignment[static_cast<std::size_t>(i)] == -1 &&
           (variable == -1 || score[static_cast<std::size_t>(variable)] < score[static_cast<std::size_t>(i)])){
            variable = i;
        }
    }
    if(variable == -1){
        for(int& value: assignment){
            if(value == -1) value = 0;
        }
        return true;
    }

    for(int value: {1, 0}){
        auto next_assignment = assignment;
        next_assignment[static_cast<std::size_t>(variable)] = value;
        if(solve_dfs(n, clauses, next_assignment)){
            assignment.swap(next_assignment);
            return true;
        }
    }
    return false;
}

} // namespace three_sat_internal

ThreeSatResult three_sat(int n, const std::vector<ThreeSatClause>& clauses){
    if(n < 0)[[unlikely]]{
        throw std::runtime_error("library assertion fault: range violation (three_sat).");
    }
    for(const auto& clause: clauses){
        for(auto lit: clause.literal){
            if(lit.variable < 0 || n <= lit.variable)[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (three_sat).");
            }
        }
    }
    ThreeSatResult result;
    result.assignment.assign(static_cast<std::size_t>(n), -1);
    result.satisfiable = three_sat_internal::solve_dfs(n, clauses, result.assignment);
    if(!result.satisfiable) result.assignment.clear();
    return result;
}
