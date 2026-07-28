#ifndef CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_ASSIGNMENT_SUBSET_EXACT_COVER_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_ASSIGNMENT_SUBSET_EXACT_COVER_HPP_INCLUDED

#include <algorithm>
#include <functional>
#include <limits>
#include <optional>
#include <stdexcept>
#include <vector>

inline std::optional<std::vector<int>> exact_cover(
    int column_count,
    const std::vector<std::vector<int>>& rows
){
    if(column_count < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: negative column count (exact_cover)."
        );
    }
    if(rows.size() > static_cast<std::size_t>(
        std::numeric_limits<int>::max()
    ))[[unlikely]]{
        throw std::length_error("too many rows (exact_cover)");
    }
    std::vector<std::vector<int>> column_rows(
        static_cast<std::size_t>(column_count)
    );
    std::vector<int> last_row(
        static_cast<std::size_t>(column_count), -1
    );
    for(int row_id = 0; row_id < static_cast<int>(rows.size()); ++row_id){
        for(int column: rows[static_cast<std::size_t>(row_id)]){
            if(column < 0 || column >= column_count)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: column out of range "
                    "(exact_cover)."
                );
            }
            if(last_row[static_cast<std::size_t>(column)] == row_id)
                [[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: duplicate column in a row "
                    "(exact_cover)."
                );
            }
            last_row[static_cast<std::size_t>(column)] = row_id;
            column_rows[static_cast<std::size_t>(column)].push_back(row_id);
        }
    }

    std::vector<unsigned char> covered(
        static_cast<std::size_t>(column_count), 0
    );
    std::vector<int> solution;
    const auto compatible = [&](int row_id){
        for(int column: rows[static_cast<std::size_t>(row_id)]){
            if(covered[static_cast<std::size_t>(column)] != 0) return false;
        }
        return true;
    };
    const std::function<bool()> search = [&]{
        int selected_column = -1;
        std::size_t selected_count = rows.size() + 1;
        for(int column = 0; column < column_count; ++column){
            if(covered[static_cast<std::size_t>(column)] != 0) continue;
            std::size_t count = 0;
            for(int row_id:
                column_rows[static_cast<std::size_t>(column)]){
                if(compatible(row_id)) ++count;
            }
            if(count == 0) return false;
            if(count < selected_count){
                selected_count = count;
                selected_column = column;
            }
        }
        if(selected_column == -1) return true;

        for(int row_id:
            column_rows[static_cast<std::size_t>(selected_column)]){
            if(!compatible(row_id)) continue;
            for(int column: rows[static_cast<std::size_t>(row_id)]){
                covered[static_cast<std::size_t>(column)] = 1;
            }
            solution.push_back(row_id);
            if(search()) return true;
            solution.pop_back();
            for(int column: rows[static_cast<std::size_t>(row_id)]){
                covered[static_cast<std::size_t>(column)] = 0;
            }
        }
        return false;
    };
    if(!search()) return std::nullopt;
    return solution;
}

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_ASSIGNMENT_SUBSET_EXACT_COVER_HPP_INCLUDED
