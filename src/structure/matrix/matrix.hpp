#pragma once

#include <array>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

template<class T, int MAX_ROW, int MAX_COL>
struct Matrix{
    static_assert(MAX_ROW >= 0);
    static_assert(MAX_COL >= 0);

private:
    template<class, int, int>
    friend struct Matrix;

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

    void check_square(const char* message) const{
        if(_rows != _cols)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    static void convolution(
        const std::vector<T>& lhs,
        const std::vector<T>& rhs,
        std::vector<T>& res
    ){
        if(lhs.empty() || rhs.empty()){
            res.clear();
            return;
        }
        res.assign(lhs.size() + rhs.size() - 1, T());
        for(std::size_t i = 0; i < lhs.size(); i++){
            for(std::size_t j = 0; j < rhs.size(); j++){
                res[i + j] += lhs[i] * rhs[j];
            }
        }
    }

    static std::vector<T> berlekamp_massey(
        const T* sequence,
        int sequence_size
    ){
        std::vector<T> current;
        std::vector<T> previous;
        std::vector<T> backup;
        const std::size_t capacity = static_cast<std::size_t>(sequence_size + 1);
        current.reserve(capacity);
        previous.reserve(capacity);
        backup.reserve(capacity);
        current.push_back(T(1));
        previous.push_back(T(1));
        int degree = 0;
        int shift = 1;
        T last = T(1);

        for(int i = 0; i < sequence_size; i++){
            T diff = sequence[static_cast<std::size_t>(i)];
            for(int j = 1; j <= degree; j++){
                diff += current[static_cast<std::size_t>(j)] *
                    sequence[static_cast<std::size_t>(i - j)];
            }
            if(diff == T()){
                shift++;
                continue;
            }

            backup.assign(current.begin(), current.end());
            T coeff = diff / last;
            if(current.size() < previous.size() + static_cast<std::size_t>(shift)){
                current.resize(previous.size() + static_cast<std::size_t>(shift));
            }
            for(std::size_t j = 0; j < previous.size(); j++){
                current[j + static_cast<std::size_t>(shift)] -= coeff * previous[j];
            }

            if(2 * degree <= i){
                degree = i + 1 - degree;
                previous = backup;
                last = diff;
                shift = 1;
            }else{
                shift++;
            }
        }

        std::vector<T> recurrence(static_cast<std::size_t>(degree));
        for(int i = 0; i < degree; i++){
            recurrence[static_cast<std::size_t>(i)] =
                -current[static_cast<std::size_t>(i + 1)];
        }
        return recurrence;
    }

    static T bostan_mori(
        const T* initial,
        int initial_size,
        const std::vector<T>& recurrence,
        long long k
    ){
        int degree = static_cast<int>(recurrence.size());
        if(degree == 0) return T();
        if(k < initial_size){
            return initial[static_cast<std::size_t>(k)];
        }

        std::vector<T> q(static_cast<std::size_t>(degree + 1));
        q[0] = T(1);
        for(int i = 0; i < degree; i++){
            q[static_cast<std::size_t>(i + 1)] =
                -recurrence[static_cast<std::size_t>(i)];
        }

        std::vector<T> p(static_cast<std::size_t>(degree));
        for(int i = 0; i < degree; i++){
            T value = initial[static_cast<std::size_t>(i)];
            for(int j = 1; j <= i; j++){
                value += q[static_cast<std::size_t>(j)] *
                    initial[static_cast<std::size_t>(i - j)];
            }
            p[static_cast<std::size_t>(i)] = value;
        }

        std::vector<T> q_neg;
        std::vector<T> next_p_full;
        std::vector<T> next_q_full;
        std::vector<T> next_p;
        std::vector<T> next_q;
        q_neg.reserve(static_cast<std::size_t>(degree + 1));
        next_p_full.reserve(static_cast<std::size_t>(2 * degree));
        next_q_full.reserve(static_cast<std::size_t>(2 * degree + 1));
        next_p.reserve(static_cast<std::size_t>(degree));
        next_q.reserve(static_cast<std::size_t>(degree + 1));

        while(k > 0){
            q_neg.assign(q.begin(), q.end());
            for(std::size_t i = 1; i < q_neg.size(); i += 2){
                q_neg[i] = -q_neg[i];
            }

            convolution(p, q_neg, next_p_full);
            convolution(q, q_neg, next_q_full);

            next_p.clear();
            for(std::size_t i = static_cast<std::size_t>(k & 1);
                i < next_p_full.size();
                i += 2
            ){
                next_p.push_back(next_p_full[i]);
            }

            next_q.clear();
            for(std::size_t i = 0; i < next_q_full.size(); i += 2){
                next_q.push_back(next_q_full[i]);
            }

            p = std::move(next_p);
            q = std::move(next_q);
            k >>= 1;
        }

        return p[0] / q[0];
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
        const int rhs_cols = rhs._cols;
        if(_rows == 0 || _cols == 0 || rhs_cols == 0) return res;
        for(int i = 0; i < _rows; i++){
            T* result_row = res.data.data() +
                static_cast<std::size_t>(i) * RHS_MAX_COL;
            const T* left_row = data.data() +
                static_cast<std::size_t>(i) * MAX_COL;
            for(int k = 0; k < _cols; k++){
                const T& left_value = left_row[k];
                const T* right_row = rhs.data.data() +
                    static_cast<std::size_t>(k) * RHS_MAX_COL;
                for(int j = 0; j < rhs_cols; j++){
                    result_row[j] += left_value * right_row[j];
                }
            }
        }
        return res;
    }

    T pow_entry_bmbm(long long exponent, int row, int col) const{
        if(exponent < 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: negative exponent (pow_entry_bmbm)."
            );
        }
        check_square("library assertion fault: shape violation (pow_entry_bmbm).");
        check_index(row, col, "library assertion fault: range violation (pow_entry_bmbm).");

        int n = _rows;
        const int sample_size = 2 * n;
        auto workspace = std::make_unique_for_overwrite<T[]>(
            static_cast<std::size_t>(n) * 4
        );
        T* samples = workspace.get();
        T* vec = samples + sample_size;
        T* next = vec + n;
        for(int i = 0; i < n; i++) vec[i] = T();
        vec[col] = T(1);
        for(int step = 0; step < 2 * n; step++){
            samples[step] = vec[row];
            for(int i = 0; i < n; i++){
                T value{};
                const T* matrix_row = data.data() +
                    static_cast<std::size_t>(i) * MAX_COL;
                for(int j = 0; j < n; j++){
                    value += matrix_row[j] * vec[j];
                }
                next[i] = value;
            }
            std::swap(vec, next);
        }

        if(exponent < sample_size){
            return samples[exponent];
        }

        std::vector<T> recurrence = berlekamp_massey(samples, sample_size);
        return bostan_mori(samples, sample_size, recurrence, exponent);
    }

    T pow_entry_bmbm(long long exponent, int k) const{
        if(exponent < 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: negative exponent (pow_entry_bmbm)."
            );
        }
        check_square("library assertion fault: shape violation (pow_entry_bmbm).");
        if(k < 0 || _rows * _cols <= k)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (pow_entry_bmbm)."
            );
        }
        return pow_entry_bmbm(exponent, k / _cols, k % _cols);
    }
};
