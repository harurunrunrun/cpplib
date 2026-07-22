#ifndef CPPLIB_SRC_ALGORITHM_MATCHING_ASSIGNMENT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATCHING_ASSIGNMENT_HPP_INCLUDED

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <vector>

template<class T>
struct AssignmentResult{
    T cost;
    std::vector<int> row_to_column;
    std::vector<int> column_to_row;
    std::vector<T> row_potential;
    std::vector<T> column_potential;
};

template<class T>
AssignmentResult<T> minimum_cost_assignment(
    const std::vector<std::vector<T>>& cost,
    T infinity = std::numeric_limits<T>::max() / 4
){
    const int row_count = static_cast<int>(cost.size());
    const int column_count = row_count == 0 ? 0 : static_cast<int>(cost[0].size());
    for(const auto& row: cost){
        if(static_cast<int>(row.size()) != column_count)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: non-rectangular matrix (minimum_cost_assignment)."
            );
        }
    }
    if(row_count > column_count)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: rows exceed columns (minimum_cost_assignment)."
        );
    }

    AssignmentResult<T> result;
    result.cost = T();
    result.row_to_column.assign(static_cast<std::size_t>(row_count), -1);
    result.column_to_row.assign(static_cast<std::size_t>(column_count), -1);
    result.row_potential.assign(static_cast<std::size_t>(row_count), T());
    result.column_potential.assign(static_cast<std::size_t>(column_count), T());
    if(row_count == 0) return result;

    std::vector<T> row_potential(static_cast<std::size_t>(row_count + 1), T());
    std::vector<T> column_potential(static_cast<std::size_t>(column_count + 1), T());
    std::vector<int> matched_row(static_cast<std::size_t>(column_count + 1), 0);
    std::vector<int> previous_column(static_cast<std::size_t>(column_count + 1), 0);

    for(int row = 1; row <= row_count; ++row){
        matched_row[0] = row;
        int current_column = 0;
        std::vector<T> minimum_slack(static_cast<std::size_t>(column_count + 1), infinity);
        std::vector<unsigned char> used(static_cast<std::size_t>(column_count + 1), 0);
        do{
            used[static_cast<std::size_t>(current_column)] = 1;
            const int current_row = matched_row[static_cast<std::size_t>(current_column)];
            T delta = infinity;
            int next_column = -1;
            for(int column = 1; column <= column_count; ++column){
                if(used[static_cast<std::size_t>(column)]) continue;
                const T slack =
                    cost[static_cast<std::size_t>(current_row - 1)]
                        [static_cast<std::size_t>(column - 1)] -
                    row_potential[static_cast<std::size_t>(current_row)] -
                    column_potential[static_cast<std::size_t>(column)];
                if(slack < minimum_slack[static_cast<std::size_t>(column)]){
                    minimum_slack[static_cast<std::size_t>(column)] = slack;
                    previous_column[static_cast<std::size_t>(column)] = current_column;
                }
                if(minimum_slack[static_cast<std::size_t>(column)] < delta){
                    delta = minimum_slack[static_cast<std::size_t>(column)];
                    next_column = column;
                }
            }
            if(next_column == -1 || !(delta < infinity))[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: finite assignment does not exist "
                    "(minimum_cost_assignment)."
                );
            }
            for(int column = 0; column <= column_count; ++column){
                if(used[static_cast<std::size_t>(column)]){
                    row_potential[static_cast<std::size_t>(
                        matched_row[static_cast<std::size_t>(column)]
                    )] += delta;
                    column_potential[static_cast<std::size_t>(column)] -= delta;
                }else{
                    minimum_slack[static_cast<std::size_t>(column)] -= delta;
                }
            }
            current_column = next_column;
        }while(matched_row[static_cast<std::size_t>(current_column)] != 0);

        do{
            const int predecessor = previous_column[static_cast<std::size_t>(current_column)];
            matched_row[static_cast<std::size_t>(current_column)] =
                matched_row[static_cast<std::size_t>(predecessor)];
            current_column = predecessor;
        }while(current_column != 0);
    }

    for(int column = 1; column <= column_count; ++column){
        const int row = matched_row[static_cast<std::size_t>(column)];
        if(row == 0) continue;
        result.row_to_column[static_cast<std::size_t>(row - 1)] = column - 1;
        result.column_to_row[static_cast<std::size_t>(column - 1)] = row - 1;
    }
    for(int row = 0; row < row_count; ++row){
        const int column = result.row_to_column[static_cast<std::size_t>(row)];
        result.cost += cost[static_cast<std::size_t>(row)][static_cast<std::size_t>(column)];
        result.row_potential[static_cast<std::size_t>(row)] =
            row_potential[static_cast<std::size_t>(row + 1)];
    }
    for(int column = 0; column < column_count; ++column){
        result.column_potential[static_cast<std::size_t>(column)] =
            column_potential[static_cast<std::size_t>(column + 1)];
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_MATCHING_ASSIGNMENT_HPP_INCLUDED
