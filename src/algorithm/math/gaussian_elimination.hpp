#ifndef CPPLIB_SRC_ALGORITHM_MATH_GAUSSIAN_ELIMINATION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_GAUSSIAN_ELIMINATION_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <bitset>
#include <cstddef>
#include <memory>
#include <optional>
#include <stdexcept>
#include <vector>

#include "../../structure/matrix/matrix.hpp"

template<class T>
struct GaussianLinearSystemSolution{
    bool consistent = false;
    std::vector<T> particular;
    std::vector<std::vector<T>> nullspace_basis;
    std::vector<int> pivot_columns;
};

namespace gaussian_elimination_internal{

template<class T, int MAX_ROW, int MAX_COLUMN>
struct Workspace{
    static_assert(MAX_ROW >= 0);
    static_assert(MAX_COLUMN >= 0);
    std::array<T, static_cast<std::size_t>(MAX_ROW) * MAX_COLUMN> data{};
    int stride = 0;

    T* row(int index){
        if constexpr(MAX_COLUMN == 0) return nullptr;
        else return data.data() + static_cast<std::size_t>(index) * MAX_COLUMN;
    }
    const T* row(int index) const{
        if constexpr(MAX_COLUMN == 0) return nullptr;
        else return data.data() + static_cast<std::size_t>(index) * MAX_COLUMN;
    }
};

template<class T, int MAX_ROW, int MAX_COL, int WORK_COL>
std::unique_ptr<Workspace<T, MAX_ROW, WORK_COL>> copy_matrix(
    const Matrix<T, MAX_ROW, MAX_COL>& matrix,
    int stride
){
    auto workspace = std::make_unique<Workspace<T, MAX_ROW, WORK_COL>>();
    workspace->stride = stride;
    for(int i = 0; i < matrix.rows(); ++i){
        T* destination = workspace->row(i);
        for(int j = 0; j < matrix.cols(); ++j) destination[j] = matrix(i, j);
    }
    return workspace;
}

template<class T>
void swap_row(T* lhs, T* rhs, int first, int last){
    for(int j = first; j < last; ++j) std::swap(lhs[j], rhs[j]);
}

template<class T>
struct DynamicWorkspace{
    std::vector<T> data;
    int stride = 0;

    DynamicWorkspace(int rows, int columns):
        data(static_cast<std::size_t>(rows) * columns), stride(columns){}

    T* row(int index){
        return stride == 0 ? nullptr :
            data.data() + static_cast<std::size_t>(index) * stride;
    }
    const T* row(int index) const{
        return stride == 0 ? nullptr :
            data.data() + static_cast<std::size_t>(index) * stride;
    }
};

template<class T>
std::pair<int, int> check_rectangular(const std::vector<std::vector<T>>& matrix){
    const int rows = static_cast<int>(matrix.size());
    const int cols = rows == 0 ? 0 : static_cast<int>(matrix[0].size());
    for(const auto& row: matrix){
        if(static_cast<int>(row.size()) != cols)[[unlikely]]{
            throw std::runtime_error("library assertion fault: rectangular violation (gaussian elimination).");
        }
    }
    return {rows, cols};
}

template<class T>
DynamicWorkspace<T> copy_dynamic(
    const std::vector<std::vector<T>>& matrix,
    int extra_columns
){
    const auto [rows, cols] = check_rectangular(matrix);
    DynamicWorkspace<T> workspace(rows, cols + extra_columns);
    for(int i = 0; i < rows; ++i){
        std::copy(matrix[static_cast<std::size_t>(i)].begin(),
                  matrix[static_cast<std::size_t>(i)].end(), workspace.row(i));
    }
    return workspace;
}

template<class T, class WORKSPACE>
int rank_core(WORKSPACE& workspace, int rows, int cols){
    int rank = 0;
    for(int column = 0; column < cols && rank < rows; ++column){
        int pivot = rank;
        while(pivot < rows && workspace.row(pivot)[column] == T{}) ++pivot;
        if(pivot == rows) continue;
        if(pivot != rank) swap_row(workspace.row(pivot), workspace.row(rank), column, cols);
        T* pivot_row = workspace.row(rank);
        const T inverse = T{1} / pivot_row[column];
        for(int i = rank + 1; i < rows; ++i){
            T* current = workspace.row(i);
            if(current[column] == T{}) continue;
            const T factor = current[column] * inverse;
            current[column] = T{};
            for(int j = column + 1; j < cols; ++j) current[j] -= factor * pivot_row[j];
        }
        ++rank;
    }
    return rank;
}

template<class T, class WORKSPACE>
T determinant_core(WORKSPACE& workspace, int n){
    T determinant = T{1};
    for(int column = 0; column < n; ++column){
        int pivot = column;
        while(pivot < n && workspace.row(pivot)[column] == T{}) ++pivot;
        if(pivot == n) return T{};
        if(pivot != column){
            swap_row(workspace.row(pivot), workspace.row(column), column, n);
            determinant = -determinant;
        }
        T* pivot_row = workspace.row(column);
        const T pivot_value = pivot_row[column];
        determinant *= pivot_value;
        const T inverse = T{1} / pivot_value;
        for(int i = column + 1; i < n; ++i){
            T* current = workspace.row(i);
            if(current[column] == T{}) continue;
            const T factor = current[column] * inverse;
            current[column] = T{};
            for(int j = column + 1; j < n; ++j) current[j] -= factor * pivot_row[j];
        }
    }
    return determinant;
}

template<class T, class WORKSPACE>
GaussianLinearSystemSolution<T> solve_core(WORKSPACE& workspace, int rows, int cols){
    std::vector<int> pivot_columns;
    pivot_columns.reserve(static_cast<std::size_t>(std::min(rows, cols)));
    int rank = 0;
    for(int column = 0; column < cols && rank < rows; ++column){
        int pivot = rank;
        while(pivot < rows && workspace.row(pivot)[column] == T{}) ++pivot;
        if(pivot == rows) continue;
        if(pivot != rank) swap_row(workspace.row(pivot), workspace.row(rank), column, cols + 1);
        T* pivot_row = workspace.row(rank);
        const T inverse = T{1} / pivot_row[column];
        for(int j = column; j <= cols; ++j) pivot_row[j] *= inverse;
        for(int i = 0; i < rows; ++i){
            if(i == rank) continue;
            T* current = workspace.row(i);
            const T factor = current[column];
            if(factor == T{}) continue;
            current[column] = T{};
            for(int j = column + 1; j <= cols; ++j) current[j] -= factor * pivot_row[j];
        }
        pivot_columns.push_back(column);
        ++rank;
    }

    GaussianLinearSystemSolution<T> result;
    for(int i = rank; i < rows; ++i) if(workspace.row(i)[cols] != T{}) return result;
    result.consistent = true;
    result.pivot_columns = pivot_columns;
    result.particular.assign(static_cast<std::size_t>(cols), T{});
    std::vector<char> is_pivot(static_cast<std::size_t>(cols), 0);
    for(int i = 0; i < rank; ++i){
        const int column = pivot_columns[static_cast<std::size_t>(i)];
        is_pivot[static_cast<std::size_t>(column)] = 1;
        result.particular[static_cast<std::size_t>(column)] = workspace.row(i)[cols];
    }
    result.nullspace_basis.reserve(static_cast<std::size_t>(cols - rank));
    for(int free_column = 0; free_column < cols; ++free_column){
        if(is_pivot[static_cast<std::size_t>(free_column)]) continue;
        std::vector<T> basis(static_cast<std::size_t>(cols), T{});
        basis[static_cast<std::size_t>(free_column)] = T{1};
        for(int i = 0; i < rank; ++i){
            basis[static_cast<std::size_t>(pivot_columns[static_cast<std::size_t>(i)])] =
                -workspace.row(i)[free_column];
        }
        result.nullspace_basis.push_back(std::move(basis));
    }
    return result;
}

template<class T, class WORKSPACE>
std::optional<std::vector<T>> inverse_core(WORKSPACE& workspace, int n){
    for(int i = 0; i < n; ++i) workspace.row(i)[n + i] = T{1};
    for(int column = 0; column < n; ++column){
        int pivot = column;
        while(pivot < n && workspace.row(pivot)[column] == T{}) ++pivot;
        if(pivot == n) return std::nullopt;
        if(pivot != column) swap_row(workspace.row(pivot), workspace.row(column), column, 2 * n);
        T* pivot_row = workspace.row(column);
        const T inverse = T{1} / pivot_row[column];
        for(int j = column; j < 2 * n; ++j) pivot_row[j] *= inverse;
        for(int i = 0; i < n; ++i){
            if(i == column) continue;
            T* current = workspace.row(i);
            const T factor = current[column];
            if(factor == T{}) continue;
            current[column] = T{};
            for(int j = column + 1; j < 2 * n; ++j) current[j] -= factor * pivot_row[j];
        }
    }
    std::vector<T> result(static_cast<std::size_t>(n) * n);
    for(int i = 0; i < n; ++i){
        for(int j = 0; j < n; ++j){
            result[static_cast<std::size_t>(i) * n + j] = workspace.row(i)[n + j];
        }
    }
    return result;
}

} // namespace gaussian_elimination_internal

template<class T, int MAX_ROW, int MAX_COL>
int gaussian_rank(const Matrix<T, MAX_ROW, MAX_COL>& matrix){
    auto workspace = gaussian_elimination_internal::copy_matrix<
        T, MAX_ROW, MAX_COL, MAX_COL
    >(matrix, matrix.cols());
    const int rows = matrix.rows();
    const int cols = matrix.cols();
    int rank = 0;
    for(int column = 0; column < cols && rank < rows; ++column){
        int pivot = rank;
        while(pivot < rows && workspace->row(pivot)[column] == T{}) ++pivot;
        if(pivot == rows) continue;
        if(pivot != rank){
            gaussian_elimination_internal::swap_row(
                workspace->row(pivot), workspace->row(rank), column, cols
            );
        }
        T* pivot_row = workspace->row(rank);
        const T inverse = T{1} / pivot_row[column];
        for(int i = rank + 1; i < rows; ++i){
            T* current = workspace->row(i);
            if(current[column] == T{}) continue;
            const T factor = current[column] * inverse;
            current[column] = T{};
            for(int j = column + 1; j < cols; ++j){
                current[j] -= factor * pivot_row[j];
            }
        }
        ++rank;
    }
    return rank;
}

template<class T, int MAX_ROW, int MAX_COL>
T gaussian_determinant(const Matrix<T, MAX_ROW, MAX_COL>& matrix){
    if(matrix.rows() != matrix.cols())[[unlikely]]{
        throw std::runtime_error("library assertion fault: square violation (gaussian_determinant).");
    }
    auto workspace = gaussian_elimination_internal::copy_matrix<
        T, MAX_ROW, MAX_COL, MAX_COL
    >(matrix, matrix.cols());
    const int n = matrix.rows();
    T determinant = T{1};
    for(int column = 0; column < n; ++column){
        int pivot = column;
        while(pivot < n && workspace->row(pivot)[column] == T{}) ++pivot;
        if(pivot == n) return T{};
        if(pivot != column){
            gaussian_elimination_internal::swap_row(
                workspace->row(pivot), workspace->row(column), column, n
            );
            determinant = -determinant;
        }
        T* pivot_row = workspace->row(column);
        const T pivot_value = pivot_row[column];
        determinant *= pivot_value;
        const T inverse = T{1} / pivot_value;
        for(int i = column + 1; i < n; ++i){
            T* current = workspace->row(i);
            if(current[column] == T{}) continue;
            const T factor = current[column] * inverse;
            current[column] = T{};
            for(int j = column + 1; j < n; ++j){
                current[j] -= factor * pivot_row[j];
            }
        }
    }
    return determinant;
}

template<class T, int MAX_ROW, int MAX_COL>
GaussianLinearSystemSolution<T> solve_linear_system(
    const Matrix<T, MAX_ROW, MAX_COL>& matrix,
    const std::vector<T>& rhs
){
    const int rows = matrix.rows();
    const int cols = matrix.cols();
    if(static_cast<int>(rhs.size()) != rows)[[unlikely]]{
        throw std::runtime_error("library assertion fault: shape violation (solve_linear_system).");
    }
    auto workspace = gaussian_elimination_internal::copy_matrix<
        T, MAX_ROW, MAX_COL, MAX_COL + 1
    >(matrix, cols + 1);
    for(int i = 0; i < rows; ++i) workspace->row(i)[cols] = rhs[static_cast<std::size_t>(i)];

    std::vector<int> pivot_columns;
    pivot_columns.reserve(static_cast<std::size_t>(std::min(rows, cols)));
    int rank = 0;
    for(int column = 0; column < cols && rank < rows; ++column){
        int pivot = rank;
        while(pivot < rows && workspace->row(pivot)[column] == T{}) ++pivot;
        if(pivot == rows) continue;
        if(pivot != rank){
            gaussian_elimination_internal::swap_row(
                workspace->row(pivot), workspace->row(rank), column, cols + 1
            );
        }
        T* pivot_row = workspace->row(rank);
        const T inverse = T{1} / pivot_row[column];
        for(int j = column; j <= cols; ++j) pivot_row[j] *= inverse;
        for(int i = 0; i < rows; ++i){
            if(i == rank) continue;
            T* current = workspace->row(i);
            const T factor = current[column];
            if(factor == T{}) continue;
            current[column] = T{};
            for(int j = column + 1; j <= cols; ++j){
                current[j] -= factor * pivot_row[j];
            }
        }
        pivot_columns.push_back(column);
        ++rank;
    }

    GaussianLinearSystemSolution<T> result;
    for(int i = rank; i < rows; ++i){
        if(workspace->row(i)[cols] != T{}) return result;
    }
    result.consistent = true;
    result.pivot_columns = pivot_columns;
    result.particular.assign(static_cast<std::size_t>(cols), T{});
    std::vector<char> is_pivot(static_cast<std::size_t>(cols), 0);
    for(int i = 0; i < rank; ++i){
        const int column = pivot_columns[static_cast<std::size_t>(i)];
        is_pivot[static_cast<std::size_t>(column)] = 1;
        result.particular[static_cast<std::size_t>(column)] = workspace->row(i)[cols];
    }
    result.nullspace_basis.reserve(static_cast<std::size_t>(cols - rank));
    for(int free_column = 0; free_column < cols; ++free_column){
        if(is_pivot[static_cast<std::size_t>(free_column)]) continue;
        std::vector<T> basis(static_cast<std::size_t>(cols), T{});
        basis[static_cast<std::size_t>(free_column)] = T{1};
        for(int i = 0; i < rank; ++i){
            basis[static_cast<std::size_t>(pivot_columns[static_cast<std::size_t>(i)])] =
                -workspace->row(i)[free_column];
        }
        result.nullspace_basis.push_back(std::move(basis));
    }
    return result;
}

template<class T, int MAX_ROW, int MAX_COL>
std::optional<Matrix<T, MAX_ROW, MAX_COL>> gaussian_inverse(
    const Matrix<T, MAX_ROW, MAX_COL>& matrix
){
    if(matrix.rows() != matrix.cols())[[unlikely]]{
        throw std::runtime_error("library assertion fault: square violation (gaussian_inverse).");
    }
    const int n = matrix.rows();
    auto workspace = gaussian_elimination_internal::copy_matrix<
        T, MAX_ROW, MAX_COL, 2 * MAX_COL
    >(matrix, 2 * n);
    for(int i = 0; i < n; ++i) workspace->row(i)[n + i] = T{1};

    for(int column = 0; column < n; ++column){
        int pivot = column;
        while(pivot < n && workspace->row(pivot)[column] == T{}) ++pivot;
        if(pivot == n) return std::nullopt;
        if(pivot != column){
            gaussian_elimination_internal::swap_row(
                workspace->row(pivot), workspace->row(column), column, 2 * n
            );
        }
        T* pivot_row = workspace->row(column);
        const T inverse = T{1} / pivot_row[column];
        for(int j = column; j < 2 * n; ++j) pivot_row[j] *= inverse;
        for(int i = 0; i < n; ++i){
            if(i == column) continue;
            T* current = workspace->row(i);
            const T factor = current[column];
            if(factor == T{}) continue;
            current[column] = T{};
            for(int j = column + 1; j < 2 * n; ++j){
                current[j] -= factor * pivot_row[j];
            }
        }
    }

    Matrix<T, MAX_ROW, MAX_COL> result(n, n);
    for(int i = 0; i < n; ++i){
        for(int j = 0; j < n; ++j) result(i, j) = workspace->row(i)[n + j];
    }
    return result;
}

template<class T>
int gaussian_rank(const std::vector<std::vector<T>>& matrix){
    const auto [rows, cols] = gaussian_elimination_internal::check_rectangular(matrix);
    auto workspace = gaussian_elimination_internal::copy_dynamic(matrix, 0);
    return gaussian_elimination_internal::rank_core<T>(workspace, rows, cols);
}

template<class T>
T gaussian_determinant(const std::vector<std::vector<T>>& matrix){
    const auto [rows, cols] = gaussian_elimination_internal::check_rectangular(matrix);
    if(rows != cols)[[unlikely]]{
        throw std::runtime_error("library assertion fault: square violation (gaussian_determinant).");
    }
    auto workspace = gaussian_elimination_internal::copy_dynamic(matrix, 0);
    return gaussian_elimination_internal::determinant_core<T>(workspace, rows);
}

template<class T>
GaussianLinearSystemSolution<T> solve_linear_system(
    const std::vector<std::vector<T>>& matrix,
    const std::vector<T>& rhs
){
    const auto [rows, cols] = gaussian_elimination_internal::check_rectangular(matrix);
    if(static_cast<int>(rhs.size()) != rows)[[unlikely]]{
        throw std::runtime_error("library assertion fault: shape violation (solve_linear_system).");
    }
    auto workspace = gaussian_elimination_internal::copy_dynamic(matrix, 1);
    for(int i = 0; i < rows; ++i) workspace.row(i)[cols] = rhs[static_cast<std::size_t>(i)];
    return gaussian_elimination_internal::solve_core<T>(workspace, rows, cols);
}

template<class T>
std::optional<std::vector<std::vector<T>>> gaussian_inverse(
    const std::vector<std::vector<T>>& matrix
){
    const auto [rows, cols] = gaussian_elimination_internal::check_rectangular(matrix);
    if(rows != cols)[[unlikely]]{
        throw std::runtime_error("library assertion fault: square violation (gaussian_inverse).");
    }
    auto workspace = gaussian_elimination_internal::copy_dynamic(matrix, cols);
    auto flat = gaussian_elimination_internal::inverse_core<T>(workspace, rows);
    if(!flat) return std::nullopt;
    std::vector<std::vector<T>> result(
        static_cast<std::size_t>(rows),
        std::vector<T>(static_cast<std::size_t>(rows))
    );
    for(int i = 0; i < rows; ++i){
        for(int j = 0; j < rows; ++j){
            result[static_cast<std::size_t>(i)][static_cast<std::size_t>(j)] =
                (*flat)[static_cast<std::size_t>(i) * rows + j];
        }
    }
    return result;
}

template<std::size_t MAX_COL>
struct GF2LinearSystemSolution{
    bool consistent = false;
    std::bitset<MAX_COL> particular;
    std::vector<std::bitset<MAX_COL>> nullspace_basis;
    std::vector<int> pivot_columns;
};

template<std::size_t MAX_COL>
int gf2_rank(
    const std::vector<std::bitset<MAX_COL>>& input,
    int columns
){
    if(columns < 0 || MAX_COL < static_cast<std::size_t>(columns))[[unlikely]]{
        throw std::runtime_error("library assertion fault: range violation (gf2_rank).");
    }
    std::vector<std::bitset<MAX_COL>> matrix = input;
    const int rows = static_cast<int>(matrix.size());
    int rank = 0;
    for(int column = 0; column < columns && rank < rows; ++column){
        int pivot = rank;
        while(pivot < rows && !matrix[static_cast<std::size_t>(pivot)][column]) ++pivot;
        if(pivot == rows) continue;
        if(pivot != rank){
            std::swap(matrix[static_cast<std::size_t>(pivot)], matrix[static_cast<std::size_t>(rank)]);
        }
        for(int i = rank + 1; i < rows; ++i){
            if(matrix[static_cast<std::size_t>(i)][column]){
                matrix[static_cast<std::size_t>(i)] ^= matrix[static_cast<std::size_t>(rank)];
            }
        }
        ++rank;
    }
    return rank;
}

template<std::size_t MAX_COL, class RHS>
GF2LinearSystemSolution<MAX_COL> gf2_solve_linear_system(
    const std::vector<std::bitset<MAX_COL>>& input,
    const std::vector<RHS>& rhs,
    int columns
){
    const int rows = static_cast<int>(input.size());
    if(columns < 0 || MAX_COL < static_cast<std::size_t>(columns) ||
       static_cast<int>(rhs.size()) != rows)[[unlikely]]{
        throw std::runtime_error("library assertion fault: shape violation (gf2_solve_linear_system).");
    }
    std::vector<std::bitset<MAX_COL + 1>> matrix(static_cast<std::size_t>(rows));
    for(int i = 0; i < rows; ++i){
        for(int j = 0; j < columns; ++j){
            matrix[static_cast<std::size_t>(i)][j] = input[static_cast<std::size_t>(i)][j];
        }
        matrix[static_cast<std::size_t>(i)][columns] = static_cast<bool>(rhs[static_cast<std::size_t>(i)]);
    }

    std::vector<int> pivot_columns;
    pivot_columns.reserve(static_cast<std::size_t>(std::min(rows, columns)));
    int rank = 0;
    for(int column = 0; column < columns && rank < rows; ++column){
        int pivot = rank;
        while(pivot < rows && !matrix[static_cast<std::size_t>(pivot)][column]) ++pivot;
        if(pivot == rows) continue;
        if(pivot != rank){
            std::swap(matrix[static_cast<std::size_t>(pivot)], matrix[static_cast<std::size_t>(rank)]);
        }
        for(int i = 0; i < rows; ++i){
            if(i != rank && matrix[static_cast<std::size_t>(i)][column]){
                matrix[static_cast<std::size_t>(i)] ^= matrix[static_cast<std::size_t>(rank)];
            }
        }
        pivot_columns.push_back(column);
        ++rank;
    }

    GF2LinearSystemSolution<MAX_COL> result;
    for(int i = rank; i < rows; ++i){
        if(matrix[static_cast<std::size_t>(i)][columns]) return result;
    }
    result.consistent = true;
    result.pivot_columns = pivot_columns;
    std::vector<char> is_pivot(static_cast<std::size_t>(columns), 0);
    for(int i = 0; i < rank; ++i){
        int column = pivot_columns[static_cast<std::size_t>(i)];
        is_pivot[static_cast<std::size_t>(column)] = 1;
        result.particular[column] = matrix[static_cast<std::size_t>(i)][columns];
    }
    result.nullspace_basis.reserve(static_cast<std::size_t>(columns - rank));
    for(int free_column = 0; free_column < columns; ++free_column){
        if(is_pivot[static_cast<std::size_t>(free_column)]) continue;
        std::bitset<MAX_COL> basis;
        basis[free_column] = true;
        for(int i = 0; i < rank; ++i){
            basis[pivot_columns[static_cast<std::size_t>(i)]] =
                matrix[static_cast<std::size_t>(i)][free_column];
        }
        result.nullspace_basis.push_back(basis);
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_MATH_GAUSSIAN_ELIMINATION_HPP_INCLUDED
