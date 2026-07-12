#pragma once

#include <array>
#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

template<class T, int MAX_ROW, int MAX_COL>
struct Matrix{
    static_assert(MAX_ROW >= 0);
    static_assert(MAX_COL >= 0);

private:
    int _rows = 0;
    int _cols = 0;
    std::array<T, static_cast<std::size_t>(MAX_ROW) * MAX_COL> data{};

    static constexpr std::size_t index(int i, int j){
        return static_cast<std::size_t>(i) * MAX_COL + j;
    }

    void check_shape(int rows, int cols, const char* message) const{
        if(rows < 0 || MAX_ROW < rows || cols < 0 || MAX_COL < cols)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    void check_index(int i, int j, const char* message) const{
        if(i < 0 || _rows <= i || j < 0 || _cols <= j)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    template<int R, int C>
    void check_same_shape(const Matrix<T, R, C>& rhs, const char* message) const{
        if(_rows != rhs.rows() || _cols != rhs.cols())[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

public:
    Matrix() = default;

    Matrix(int rows, int cols): _rows(rows), _cols(cols){
        check_shape(
            rows,
            cols,
            "library assertion fault: range violation (constructor)."
        );
    }

    explicit Matrix(const std::vector<std::vector<T>>& values){
        int rows = static_cast<int>(values.size());
        int cols = rows == 0 ? 0 : static_cast<int>(values[0].size());
        check_shape(
            rows,
            cols,
            "library assertion fault: range violation (constructor)."
        );

        for(int i = 0; i < rows; i++){
            if(static_cast<int>(values[static_cast<std::size_t>(i)].size()) != cols){
                throw std::runtime_error(
                    "library assertion fault: rectangular violation (constructor)."
                );
            }
        }

        _rows = rows;
        _cols = cols;
        for(int i = 0; i < _rows; i++){
            for(int j = 0; j < _cols; j++){
                (*this)(i, j) = values[static_cast<std::size_t>(i)][
                    static_cast<std::size_t>(j)
                ];
            }
        }
    }

    int rows() const{ return _rows; }
    int cols() const{ return _cols; }
    bool empty() const{ return _rows == 0 || _cols == 0; }

    T& operator()(int i, int j){
        check_index(i, j, "library assertion fault: range violation (operator()).");
        return data[index(i, j)];
    }

    const T& operator()(int i, int j) const{
        check_index(i, j, "library assertion fault: range violation (operator()).");
        return data[index(i, j)];
    }

    static Matrix zero(int rows, int cols){
        return Matrix(rows, cols);
    }

    Matrix<T, MAX_COL, MAX_ROW> transposed() const{
        Matrix<T, MAX_COL, MAX_ROW> res(_cols, _rows);
        for(int i = 0; i < _rows; i++){
            for(int j = 0; j < _cols; j++){
                res(j, i) = (*this)(i, j);
            }
        }
        return res;
    }

    template<int R, int C>
    bool operator==(const Matrix<T, R, C>& rhs) const{
        if(_rows != rhs.rows() || _cols != rhs.cols()) return false;
        for(int i = 0; i < _rows; i++){
            for(int j = 0; j < _cols; j++){
                if((*this)(i, j) != rhs(i, j)) return false;
            }
        }
        return true;
    }

    template<int R, int C>
    bool operator!=(const Matrix<T, R, C>& rhs) const{
        return !(*this == rhs);
    }

    Matrix& operator+=(const Matrix& rhs){
        check_same_shape(rhs, "library assertion fault: shape violation (operator+=).");
        for(int i = 0; i < _rows; i++){
            for(int j = 0; j < _cols; j++){
                (*this)(i, j) += rhs(i, j);
            }
        }
        return *this;
    }

    Matrix& operator-=(const Matrix& rhs){
        check_same_shape(rhs, "library assertion fault: shape violation (operator-=).");
        for(int i = 0; i < _rows; i++){
            for(int j = 0; j < _cols; j++){
                (*this)(i, j) -= rhs(i, j);
            }
        }
        return *this;
    }

    Matrix& operator*=(const T& scalar){
        for(int i = 0; i < _rows; i++){
            for(int j = 0; j < _cols; j++){
                (*this)(i, j) *= scalar;
            }
        }
        return *this;
    }

    Matrix& operator/=(const T& scalar){
        for(int i = 0; i < _rows; i++){
            for(int j = 0; j < _cols; j++){
                (*this)(i, j) /= scalar;
            }
        }
        return *this;
    }

    Matrix operator+() const{ return *this; }

    Matrix operator-() const{
        Matrix res(_rows, _cols);
        for(int i = 0; i < _rows; i++){
            for(int j = 0; j < _cols; j++){
                res(i, j) = -(*this)(i, j);
            }
        }
        return res;
    }

    friend Matrix operator+(Matrix lhs, const Matrix& rhs){
        lhs += rhs;
        return lhs;
    }

    friend Matrix operator-(Matrix lhs, const Matrix& rhs){
        lhs -= rhs;
        return lhs;
    }

    friend Matrix operator*(Matrix lhs, const T& scalar){
        lhs *= scalar;
        return lhs;
    }

    friend Matrix operator*(const T& scalar, Matrix rhs){
        rhs *= scalar;
        return rhs;
    }

    friend Matrix operator/(Matrix lhs, const T& scalar){
        lhs /= scalar;
        return lhs;
    }

    template<int RHS_MAX_ROW, int RHS_MAX_COL>
    Matrix<T, MAX_ROW, RHS_MAX_COL> operator*(
        const Matrix<T, RHS_MAX_ROW, RHS_MAX_COL>& rhs
    ) const{
        if(_cols != rhs.rows())[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: shape violation (operator*)."
            );
        }

        Matrix<T, MAX_ROW, RHS_MAX_COL> res(_rows, rhs.cols());
        for(int i = 0; i < _rows; i++){
            for(int k = 0; k < _cols; k++){
                for(int j = 0; j < rhs.cols(); j++){
                    res(i, j) += (*this)(i, k) * rhs(k, j);
                }
            }
        }
        return res;
    }
};
