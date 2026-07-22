#ifndef CPPLIB_SRC_ALGORITHM_OTHER_UTILITY_SMAWK_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_UTILITY_SMAWK_HPP_INCLUDED

#include <cstddef>
#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>

namespace smawk_internal{

template<class Value>
class Solver{
    Value value;
    std::vector<int> answer;

    bool strictly_better(int row, int candidate, int current){
        return std::invoke(value, row, candidate)
            < std::invoke(value, row, current);
    }

    void solve(const std::vector<int>& rows, const std::vector<int>& columns){
        if(rows.empty()) return;

        std::vector<int> reduced;
        reduced.reserve(columns.size());
        for(int column: columns){
            while(!reduced.empty()){
                const int row = rows[reduced.size() - 1];
                if(!strictly_better(row, column, reduced.back())) break;
                reduced.pop_back();
            }
            if(reduced.size() < rows.size()) reduced.push_back(column);
        }

        std::vector<int> odd_rows;
        odd_rows.reserve(rows.size() / 2);
        for(std::size_t index = 1; index < rows.size(); index += 2){
            odd_rows.push_back(rows[index]);
        }
        solve(odd_rows, reduced);

        std::size_t left = 0;
        for(std::size_t index = 0; index < rows.size(); index += 2){
            std::size_t right = reduced.size() - 1;
            if(index + 1 < rows.size()){
                const int boundary = answer[static_cast<std::size_t>(rows[index + 1])];
                right = left;
                while(reduced[right] != boundary) ++right;
            }

            std::size_t best = left;
            for(std::size_t position = left + 1; position <= right; ++position){
                if(strictly_better(rows[index], reduced[position], reduced[best])){
                    best = position;
                }
            }
            answer[static_cast<std::size_t>(rows[index])] = reduced[best];
            left = right;
        }
    }

public:
    Solver(int row_count, Value value_):
        value(std::move(value_)),
        answer(static_cast<std::size_t>(row_count), -1){}

    std::vector<int> run(int column_count){
        std::vector<int> rows(answer.size());
        for(int row = 0; row < static_cast<int>(rows.size()); ++row){
            rows[static_cast<std::size_t>(row)] = row;
        }
        std::vector<int> columns(static_cast<std::size_t>(column_count));
        for(int column = 0; column < column_count; ++column){
            columns[static_cast<std::size_t>(column)] = column;
        }
        solve(rows, columns);
        return answer;
    }
};

} // namespace smawk_internal

// Returns the leftmost minimum column of every row of a totally monotone
// matrix. value(row, column) is evaluated lazily.
template<class Value>
std::vector<int> smawk_row_minima(int row_count, int column_count, Value value){
    if(row_count < 0 || column_count < 0)[[unlikely]]{
        throw std::invalid_argument("smawk dimensions must be nonnegative");
    }
    if(row_count == 0) return {};
    if(column_count == 0)[[unlikely]]{
        throw std::invalid_argument("a nonempty SMAWK matrix needs a column");
    }
    return smawk_internal::Solver<Value>(row_count, std::move(value)).run(column_count);
}

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_UTILITY_SMAWK_HPP_INCLUDED
