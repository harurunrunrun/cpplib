#ifndef CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_ASSIGNMENT_SUBSET_DANCING_LINKS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_ASSIGNMENT_SUBSET_DANCING_LINKS_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <stdexcept>
#include <vector>

struct DancingLinks{
    int column_count;
    std::vector<int> left;
    std::vector<int> right;
    std::vector<int> up;
    std::vector<int> down;
    std::vector<int> column;
    std::vector<int> row;
    std::vector<int> column_size;
    std::vector<int> current;
    std::vector<std::vector<int>> solutions;

    int row_count = 0;
    explicit DancingLinks(int column_count_)
        : column_count(column_count_ < 0
            ? throw std::runtime_error(
                "library assertion fault: negative column count "
                "(DancingLinks)."
            )
            : column_count_),
          left(static_cast<std::size_t>(column_count + 1)),
          right(static_cast<std::size_t>(column_count + 1)),
          up(static_cast<std::size_t>(column_count + 1)),
          down(static_cast<std::size_t>(column_count + 1)),
          column(static_cast<std::size_t>(column_count + 1)),
          row(static_cast<std::size_t>(column_count + 1), -1),
          column_size(static_cast<std::size_t>(column_count + 1), 0){
        for(int index = 0; index <= column_count; ++index){
            left[static_cast<std::size_t>(index)] =
                index == 0 ? column_count : index - 1;
            right[static_cast<std::size_t>(index)] =
                index == column_count ? 0 : index + 1;
            up[static_cast<std::size_t>(index)] = index;
            down[static_cast<std::size_t>(index)] = index;
            column[static_cast<std::size_t>(index)] = index;
        }
    }

    int add_row(const std::vector<int>& columns){
        const int row_id = row_count++;
        std::vector<int> sorted = columns;
        std::sort(sorted.begin(), sorted.end());
        if(std::adjacent_find(sorted.begin(), sorted.end()) != sorted.end())
            [[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: duplicate column "
                "(DancingLinks::add_row)."
            );
        }
        int first_node = -1;
        int previous_node = -1;
        for(int zero_based_column: columns){
            if(zero_based_column < 0 || column_count <= zero_based_column)
                [[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: range violation "
                    "(DancingLinks::add_row)."
                );
            }
            const int header = zero_based_column + 1;
            const int node = static_cast<int>(left.size());
            left.push_back(node);
            right.push_back(node);
            up.push_back(up[static_cast<std::size_t>(header)]);
            down.push_back(header);
            column.push_back(header);
            row.push_back(row_id);
            down[static_cast<std::size_t>(
                up[static_cast<std::size_t>(header)]
            )] = node;
            up[static_cast<std::size_t>(header)] = node;
            ++column_size[static_cast<std::size_t>(header)];
            if(first_node == -1){
                first_node = node;
            }else{
                right[static_cast<std::size_t>(previous_node)] = node;
                left[static_cast<std::size_t>(node)] = previous_node;
                right[static_cast<std::size_t>(node)] = first_node;
                left[static_cast<std::size_t>(first_node)] = node;
            }
            previous_node = node;
        }
        return row_id;
    }

private:
    void cover(int header){
        right[static_cast<std::size_t>(
            left[static_cast<std::size_t>(header)]
        )] = right[static_cast<std::size_t>(header)];
        left[static_cast<std::size_t>(
            right[static_cast<std::size_t>(header)]
        )] = left[static_cast<std::size_t>(header)];
        for(int row_node = down[static_cast<std::size_t>(header)];
            row_node != header;
            row_node = down[static_cast<std::size_t>(row_node)]){
            for(int node = right[static_cast<std::size_t>(row_node)];
                node != row_node;
                node = right[static_cast<std::size_t>(node)]){
                down[static_cast<std::size_t>(
                    up[static_cast<std::size_t>(node)]
                )] = down[static_cast<std::size_t>(node)];
                up[static_cast<std::size_t>(
                    down[static_cast<std::size_t>(node)]
                )] = up[static_cast<std::size_t>(node)];
                --column_size[static_cast<std::size_t>(
                    column[static_cast<std::size_t>(node)]
                )];
            }
        }
    }

    void uncover(int header){
        for(int row_node = up[static_cast<std::size_t>(header)];
            row_node != header;
            row_node = up[static_cast<std::size_t>(row_node)]){
            for(int node = left[static_cast<std::size_t>(row_node)];
                node != row_node;
                node = left[static_cast<std::size_t>(node)]){
                ++column_size[static_cast<std::size_t>(
                    column[static_cast<std::size_t>(node)]
                )];
                down[static_cast<std::size_t>(
                    up[static_cast<std::size_t>(node)]
                )] = node;
                up[static_cast<std::size_t>(
                    down[static_cast<std::size_t>(node)]
                )] = node;
            }
        }
        right[static_cast<std::size_t>(
            left[static_cast<std::size_t>(header)]
        )] = header;
        left[static_cast<std::size_t>(
            right[static_cast<std::size_t>(header)]
        )] = header;
    }

    void search(std::size_t limit){
        if(solutions.size() >= limit) return;
        if(right[0] == 0){
            solutions.push_back(current);
            return;
        }
        int selected = right[0];
        for(int header = right[static_cast<std::size_t>(selected)];
            header != 0;
            header = right[static_cast<std::size_t>(header)]){
            if(column_size[static_cast<std::size_t>(header)]
                < column_size[static_cast<std::size_t>(selected)]){
                selected = header;
            }
        }
        if(column_size[static_cast<std::size_t>(selected)] == 0) return;
        cover(selected);
        for(int row_node = down[static_cast<std::size_t>(selected)];
            row_node != selected && solutions.size() < limit;
            row_node = down[static_cast<std::size_t>(row_node)]){
            current.push_back(row[static_cast<std::size_t>(row_node)]);
            for(int node = right[static_cast<std::size_t>(row_node)];
                node != row_node;
                node = right[static_cast<std::size_t>(node)]){
                cover(column[static_cast<std::size_t>(node)]);
            }
            search(limit);
            for(int node = left[static_cast<std::size_t>(row_node)];
                node != row_node;
                node = left[static_cast<std::size_t>(node)]){
                uncover(column[static_cast<std::size_t>(node)]);
            }
            current.pop_back();
        }
        uncover(selected);
    }

public:
    std::vector<std::vector<int>> solve(
        std::size_t solution_limit = 1
    ){
        solutions.clear();
        current.clear();
        if(solution_limit != 0) search(solution_limit);
        return solutions;
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_OTHER_DYNAMIC_PROGRAMMING_ASSIGNMENT_SUBSET_DANCING_LINKS_HPP_INCLUDED
