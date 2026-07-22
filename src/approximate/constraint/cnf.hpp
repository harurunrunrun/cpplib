#ifndef CPPLIB_SRC_APPROXIMATE_CONSTRAINT_CNF_HPP_INCLUDED
#define CPPLIB_SRC_APPROXIMATE_CONSTRAINT_CNF_HPP_INCLUDED

#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <utility>
#include <vector>

namespace approximate::constraint {

using CnfLiteral = std::int64_t;
using BooleanAssignment = std::vector<unsigned char>;

class CnfFormula {
public:
    using Clause = std::vector<CnfLiteral>;

private:
    std::size_t variable_count_ = 0;
    std::vector<Clause> clauses_;

    [[nodiscard]] static std::uint64_t literal_magnitude(CnfLiteral literal){
        if(literal >= 0) return static_cast<std::uint64_t>(literal);
        return static_cast<std::uint64_t>(-(literal + 1)) + std::uint64_t{1};
    }

    void validate_literals() const {
        for(const Clause& clause : clauses_){
            for(const CnfLiteral literal : clause){
                const std::uint64_t magnitude = literal_magnitude(literal);
                if(magnitude == 0 || magnitude > variable_count_){
                    throw std::invalid_argument("CNF literal is outside the variable range");
                }
            }
        }
    }

    void validate_assignment(std::span<const unsigned char> assignment) const {
        if(assignment.size() != variable_count_){
            throw std::invalid_argument("Boolean assignment has the wrong size");
        }
        for(const unsigned char value : assignment){
            if(value > 1){
                throw std::invalid_argument("Boolean assignment values must be zero or one");
            }
        }
    }

public:
    CnfFormula() = default;

    CnfFormula(std::size_t variable_count, std::vector<Clause> clauses)
        : variable_count_(variable_count), clauses_(std::move(clauses)) {
        validate_literals();
    }

    [[nodiscard]] std::size_t variable_count() const noexcept {
        return variable_count_;
    }

    [[nodiscard]] std::size_t clause_count() const noexcept {
        return clauses_.size();
    }

    [[nodiscard]] const std::vector<Clause>& clauses() const noexcept {
        return clauses_;
    }

    [[nodiscard]] bool clause_satisfied(
        std::size_t clause_index,
        std::span<const unsigned char> assignment
    ) const {
        validate_assignment(assignment);
        if(clause_index >= clauses_.size()){
            throw std::out_of_range("CNF clause index is out of range");
        }
        for(const CnfLiteral literal : clauses_[clause_index]){
            const std::size_t variable = static_cast<std::size_t>(literal_magnitude(literal) - 1);
            const bool value = assignment[variable] != 0;
            if((literal > 0 && value) || (literal < 0 && !value)) return true;
        }
        return false;
    }

    [[nodiscard]] std::size_t satisfied_clause_count(
        std::span<const unsigned char> assignment
    ) const {
        validate_assignment(assignment);
        std::size_t result = 0;
        for(const Clause& clause : clauses_){
            bool satisfied = false;
            for(const CnfLiteral literal : clause){
                const std::size_t variable = static_cast<std::size_t>(literal_magnitude(literal) - 1);
                const bool value = assignment[variable] != 0;
                if((literal > 0 && value) || (literal < 0 && !value)){
                    satisfied = true;
                    break;
                }
            }
            if(satisfied) ++result;
        }
        return result;
    }

    [[nodiscard]] bool satisfied(std::span<const unsigned char> assignment) const {
        return satisfied_clause_count(assignment) == clauses_.size();
    }

    [[nodiscard]] std::vector<std::size_t> unsatisfied_clause_indices(
        std::span<const unsigned char> assignment
    ) const {
        validate_assignment(assignment);
        std::vector<std::size_t> result;
        for(std::size_t index = 0; index < clauses_.size(); ++index){
            bool satisfied = false;
            for(const CnfLiteral literal : clauses_[index]){
                const std::size_t variable = static_cast<std::size_t>(literal_magnitude(literal) - 1);
                const bool value = assignment[variable] != 0;
                if((literal > 0 && value) || (literal < 0 && !value)){
                    satisfied = true;
                    break;
                }
            }
            if(!satisfied) result.push_back(index);
        }
        return result;
    }
};

}  // namespace approximate::constraint

#endif  // CPPLIB_SRC_APPROXIMATE_CONSTRAINT_CNF_HPP_INCLUDED
