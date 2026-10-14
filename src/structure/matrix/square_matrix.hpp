#ifndef CPPLIB_SRC_STRUCTURE_MATRIX_SQUARE_MATRIX_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_MATRIX_SQUARE_MATRIX_HPP_INCLUDED

#include <array>
#include <cstddef>
#include <limits>
#include <type_traits>
#include <stdexcept>
#include <vector>

template<class T, int MAX_SIZE = -1>
struct SquareMatrix{
    static_assert(MAX_SIZE >= -1);

private:
    int _n = 0;
    using DynamicValue = std::conditional_t<std::is_same_v<T, bool>, std::array<T, 1>, T>;
    std::conditional_t<MAX_SIZE < 0, std::vector<DynamicValue>,
        std::array<T, static_cast<std::size_t>(MAX_SIZE < 0 ? 0 : MAX_SIZE) * (MAX_SIZE < 0 ? 0 : MAX_SIZE)>> data{};

    constexpr int stride() const{return MAX_SIZE < 0 ? _n : MAX_SIZE;}

    constexpr std::size_t index(int i, int j) const{
        return static_cast<std::size_t>(i) * stride() + j;
    }

    void allocate(){
        if constexpr(MAX_SIZE < 0){
            if(_n && static_cast<std::size_t>(_n) > data.max_size() / static_cast<std::size_t>(_n))
                throw std::length_error("matrix storage is too large");
            data.resize(static_cast<std::size_t>(_n) * _n);
        }
    }

    void check_size(int n, const char* message) const{
        if(n < 0 || (MAX_SIZE >= 0 && MAX_SIZE < n))[[unlikely]] throw std::runtime_error(message);
    }

    void check_index(int i, int j, const char* message) const{
        if(i < 0 || _n <= i || j < 0 || _n <= j)[[unlikely]]{
            throw std::runtime_error(message);
        }
    }

    void check_same_size(const SquareMatrix& rhs, const char* message) const{
        if(_n != rhs._n)[[unlikely]] throw std::runtime_error(message);
    }

public:
    SquareMatrix() = default;

    explicit SquareMatrix(int n): _n(n){
        check_size(n, "library assertion fault: range violation (constructor).");
        allocate();
    }

    explicit SquareMatrix(const std::vector<std::vector<T>>& values){
        if(values.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()))
            throw std::length_error("too many matrix rows");
        int n = static_cast<int>(values.size());
        check_size(n, "library assertion fault: range violation (constructor).");

        for(int i = 0; i < n; i++){
            if(values[static_cast<std::size_t>(i)].size() != static_cast<std::size_t>(n)){
                throw std::runtime_error(
                    "library assertion fault: square violation (constructor)."
                );
            }
        }

        _n = n;
        allocate();
        for(int i = 0; i < _n; i++){
            for(int j = 0; j < _n; j++){
                (*this)(i, j) = values[static_cast<std::size_t>(i)][
                    static_cast<std::size_t>(j)
                ];
            }
        }
    }

    int size() const{ return _n; }
    bool empty() const{ return _n == 0; }

    T& operator()(int i, int j){
        check_index(i, j, "library assertion fault: range violation (operator()).");
        if constexpr((MAX_SIZE < 0) && std::is_same_v<T, bool>) return data[index(i, j)][0];
        else return data[index(i, j)];
    }

    const T& operator()(int i, int j) const{
        check_index(i, j, "library assertion fault: range violation (operator()).");
        if constexpr((MAX_SIZE < 0) && std::is_same_v<T, bool>) return data[index(i, j)][0];
        else return data[index(i, j)];
    }

    static SquareMatrix zero(int n){
        return SquareMatrix(n);
    }

    static SquareMatrix identity(int n){
        SquareMatrix res(n);
        for(int i = 0; i < n; i++) res(i, i) = T(1);
        return res;
    }

    SquareMatrix transposed() const{
        SquareMatrix res(_n);
        for(int i = 0; i < _n; i++){
            for(int j = 0; j < _n; j++){
                res(j, i) = (*this)(i, j);
            }
        }
        return res;
    }

    bool operator==(const SquareMatrix& rhs) const{
        if(_n != rhs._n) return false;
        for(int i = 0; i < _n; i++){
            for(int j = 0; j < _n; j++){
                if((*this)(i, j) != rhs(i, j)) return false;
            }
        }
        return true;
    }

    bool operator!=(const SquareMatrix& rhs) const{
        return !(*this == rhs);
    }

    SquareMatrix& operator+=(const SquareMatrix& rhs){
        check_same_size(rhs, "library assertion fault: shape violation (operator+=).");
        for(int i = 0; i < _n; i++){
            for(int j = 0; j < _n; j++){
                (*this)(i, j) += rhs(i, j);
            }
        }
        return *this;
    }

    SquareMatrix& operator-=(const SquareMatrix& rhs){
        check_same_size(rhs, "library assertion fault: shape violation (operator-=).");
        for(int i = 0; i < _n; i++){
            for(int j = 0; j < _n; j++){
                (*this)(i, j) -= rhs(i, j);
            }
        }
        return *this;
    }

    SquareMatrix& operator*=(const T& scalar){
        for(int i = 0; i < _n; i++){
            for(int j = 0; j < _n; j++){
                (*this)(i, j) *= scalar;
            }
        }
        return *this;
    }

    SquareMatrix& operator/=(const T& scalar){
        for(int i = 0; i < _n; i++){
            for(int j = 0; j < _n; j++){
                (*this)(i, j) /= scalar;
            }
        }
        return *this;
    }

    SquareMatrix& operator*=(const SquareMatrix& rhs){
        *this = *this * rhs;
        return *this;
    }

    SquareMatrix operator+() const{ return *this; }

    SquareMatrix operator-() const{
        SquareMatrix res(_n);
        for(int i = 0; i < _n; i++){
            for(int j = 0; j < _n; j++){
                res(i, j) = -(*this)(i, j);
            }
        }
        return res;
    }

    friend SquareMatrix operator+(SquareMatrix lhs, const SquareMatrix& rhs){
        lhs += rhs;
        return lhs;
    }

    friend SquareMatrix operator-(SquareMatrix lhs, const SquareMatrix& rhs){
        lhs -= rhs;
        return lhs;
    }

    friend SquareMatrix operator*(const SquareMatrix& lhs, const SquareMatrix& rhs){
        lhs.check_same_size(
            rhs,
            "library assertion fault: shape violation (operator*)."
        );

        SquareMatrix res(lhs._n);
        const int n = lhs._n;
        if constexpr(std::is_same_v<T, bool>){
            for(int i = 0; i < n; i++){
                for(int k = 0; k < n; k++){
                    for(int j = 0; j < n; j++) res(i, j) += lhs(i, k) * rhs(k, j);
                }
            }
        }else for(int i = 0; i < n; i++){
            T* result_row = res.data.data() +
                static_cast<std::size_t>(i) * res.stride();
            const T* left_row = lhs.data.data() +
                static_cast<std::size_t>(i) * lhs.stride();
            for(int k = 0; k < n; k++){
                const T& left_value = left_row[k];
                const T* right_row = rhs.data.data() +
                    static_cast<std::size_t>(k) * rhs.stride();
                for(int j = 0; j < n; j++){
                    result_row[j] += left_value * right_row[j];
                }
            }
        }
        return res;
    }

    friend SquareMatrix operator*(SquareMatrix lhs, const T& scalar){
        lhs *= scalar;
        return lhs;
    }

    friend SquareMatrix operator*(const T& scalar, SquareMatrix rhs){
        rhs *= scalar;
        return rhs;
    }

    friend SquareMatrix operator/(SquareMatrix lhs, const T& scalar){
        lhs /= scalar;
        return lhs;
    }

    SquareMatrix pow(long long exponent) const{
        if(exponent < 0)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: negative exponent violation (pow)."
            );
        }

        SquareMatrix base = *this;
        SquareMatrix res = identity(_n);
        while(exponent > 0){
            if(exponent & 1LL) res *= base;
            exponent >>= 1;
            if(exponent > 0) base *= base;
        }
        return res;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_MATRIX_SQUARE_MATRIX_HPP_INCLUDED
