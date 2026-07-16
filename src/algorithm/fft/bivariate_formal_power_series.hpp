#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>

template<class T, std::size_t XDegree, std::size_t YDegree>
struct BivariateFormalPowerSeries{
    static constexpr std::size_t x_degree = XDegree;
    static constexpr std::size_t y_degree = YDegree;
    static constexpr std::size_t x_size = XDegree + 1;
    static constexpr std::size_t y_size = YDegree + 1;
    static constexpr std::size_t coefficient_count = x_size * y_size;

private:
    std::array<T, coefficient_count> coefficients{};

    static constexpr std::size_t flat_index(
        std::size_t x,
        std::size_t y
    ) noexcept{
        return x * y_size + y;
    }

    static void check_index(std::size_t x, std::size_t y){
        if(x > XDegree || y > YDegree)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: coefficient out of range "
                "(BivariateFormalPowerSeries)."
            );
        }
    }

    BivariateFormalPowerSeries& divide_scalar(const T& scalar){
        for(T& coefficient: coefficients) coefficient /= scalar;
        return *this;
    }

public:
    BivariateFormalPowerSeries() = default;

    explicit BivariateFormalPowerSeries(
        const std::array<T, coefficient_count>& values
    ): coefficients(values){}

    const std::array<T, coefficient_count>& data() const noexcept{
        return coefficients;
    }

    T& operator()(std::size_t x, std::size_t y){
        check_index(x, y);
        return coefficients[flat_index(x, y)];
    }

    const T& operator()(std::size_t x, std::size_t y) const{
        check_index(x, y);
        return coefficients[flat_index(x, y)];
    }

    const T& constant_term() const noexcept{
        return coefficients[0];
    }

    static BivariateFormalPowerSeries constant(const T& value){
        BivariateFormalPowerSeries result;
        result.coefficients[0] = value;
        return result;
    }

    BivariateFormalPowerSeries& operator+=(
        const BivariateFormalPowerSeries& other
    ){
        for(std::size_t index = 0; index < coefficient_count; ++index){
            coefficients[index] += other.coefficients[index];
        }
        return *this;
    }

    BivariateFormalPowerSeries& operator-=(
        const BivariateFormalPowerSeries& other
    ){
        for(std::size_t index = 0; index < coefficient_count; ++index){
            coefficients[index] -= other.coefficients[index];
        }
        return *this;
    }

    BivariateFormalPowerSeries& operator*=(
        const BivariateFormalPowerSeries& other
    ){
        std::array<T, coefficient_count> product{};
        for(std::size_t first_x = 0; first_x <= XDegree; ++first_x){
            for(std::size_t first_y = 0; first_y <= YDegree; ++first_y){
                const T& first = coefficients[flat_index(first_x, first_y)];
                for(std::size_t second_x = 0;
                    second_x + first_x <= XDegree;
                    ++second_x
                ){
                    for(std::size_t second_y = 0;
                        second_y + first_y <= YDegree;
                        ++second_y
                    ){
                        product[flat_index(
                            first_x + second_x,
                            first_y + second_y
                        )] += first * other.coefficients[
                            flat_index(second_x, second_y)
                        ];
                    }
                }
            }
        }
        coefficients = std::move(product);
        return *this;
    }

    BivariateFormalPowerSeries inverse() const{
        if(constant_term() == T{})[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: zero constant term "
                "(BivariateFormalPowerSeries::inverse)."
            );
        }
        BivariateFormalPowerSeries result;
        result.coefficients[0] = T{1} / constant_term();
        for(std::size_t total = 1; total <= XDegree + YDegree; ++total){
            const std::size_t first_x = total > YDegree ? total - YDegree : 0;
            const std::size_t last_x = total < XDegree ? total : XDegree;
            for(std::size_t x = first_x; x <= last_x; ++x){
                const std::size_t y = total - x;
                T convolution{};
                for(std::size_t source_x = 0; source_x <= x; ++source_x){
                    for(std::size_t source_y = 0; source_y <= y; ++source_y){
                        if(source_x == 0 && source_y == 0) continue;
                        convolution += coefficients[flat_index(source_x, source_y)]
                            * result.coefficients[flat_index(
                                x - source_x, y - source_y
                            )];
                    }
                }
                result.coefficients[flat_index(x, y)] =
                    -convolution / constant_term();
            }
        }
        return result;
    }

    BivariateFormalPowerSeries logarithm() const{
        if(!(constant_term() == T{1}))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: constant term is not one "
                "(BivariateFormalPowerSeries::logarithm)."
            );
        }
        BivariateFormalPowerSeries term = *this;
        term.coefficients[0] -= T{1};
        BivariateFormalPowerSeries power = term;
        BivariateFormalPowerSeries result;
        constexpr std::size_t maximum_total_degree = XDegree + YDegree;
        for(std::size_t exponent = 1;
            exponent <= maximum_total_degree;
            ++exponent
        ){
            BivariateFormalPowerSeries contribution = power;
            contribution.divide_scalar(T{static_cast<long long>(exponent)});
            if((exponent & 1U) != 0) result += contribution;
            else result -= contribution;
            if(exponent != maximum_total_degree) power *= term;
        }
        return result;
    }

    BivariateFormalPowerSeries exponential() const{
        if(!(constant_term() == T{}))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: constant term is not zero "
                "(BivariateFormalPowerSeries::exponential)."
            );
        }
        BivariateFormalPowerSeries result = constant(T{1});
        BivariateFormalPowerSeries power = constant(T{1});
        T factorial{1};
        constexpr std::size_t maximum_total_degree = XDegree + YDegree;
        for(std::size_t exponent = 1;
            exponent <= maximum_total_degree;
            ++exponent
        ){
            power *= *this;
            factorial *= T{static_cast<long long>(exponent)};
            BivariateFormalPowerSeries contribution = power;
            contribution.divide_scalar(factorial);
            result += contribution;
        }
        return result;
    }

    BivariateFormalPowerSeries pow(long long exponent) const{
        bool negative = exponent < 0;
        std::uint64_t magnitude = static_cast<std::uint64_t>(exponent);
        if(negative) magnitude = 0 - magnitude;
        BivariateFormalPowerSeries base = negative ? inverse() : *this;
        BivariateFormalPowerSeries result = constant(T{1});
        while(magnitude != 0){
            if((magnitude & 1U) != 0) result *= base;
            magnitude >>= 1;
            if(magnitude != 0) base *= base;
        }
        return result;
    }

    BivariateFormalPowerSeries square_root(
        const T& constant_square_root
    ) const{
        if(!(constant_square_root * constant_square_root == constant_term()))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: invalid constant square root "
                "(BivariateFormalPowerSeries::square_root)."
            );
        }
        const T twice_root = constant_square_root + constant_square_root;
        if(twice_root == T{})[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: non-invertible doubled root "
                "(BivariateFormalPowerSeries::square_root)."
            );
        }
        BivariateFormalPowerSeries result;
        result.coefficients[0] = constant_square_root;
        for(std::size_t total = 1; total <= XDegree + YDegree; ++total){
            const std::size_t first_x = total > YDegree ? total - YDegree : 0;
            const std::size_t last_x = total < XDegree ? total : XDegree;
            for(std::size_t x = first_x; x <= last_x; ++x){
                const std::size_t y = total - x;
                T known{};
                for(std::size_t first_term_x = 0;
                    first_term_x <= x;
                    ++first_term_x
                ){
                    for(std::size_t first_term_y = 0;
                        first_term_y <= y;
                        ++first_term_y
                    ){
                        if(first_term_x == 0 && first_term_y == 0) continue;
                        if(first_term_x == x && first_term_y == y) continue;
                        known += result.coefficients[flat_index(
                            first_term_x, first_term_y
                        )] * result.coefficients[flat_index(
                            x - first_term_x, y - first_term_y
                        )];
                    }
                }
                result.coefficients[flat_index(x, y)] =
                    (coefficients[flat_index(x, y)] - known) / twice_root;
            }
        }
        return result;
    }

    friend bool operator==(
        const BivariateFormalPowerSeries& left,
        const BivariateFormalPowerSeries& right
    ){
        return left.coefficients == right.coefficients;
    }

    friend BivariateFormalPowerSeries operator+(
        BivariateFormalPowerSeries left,
        const BivariateFormalPowerSeries& right
    ){
        left += right;
        return left;
    }

    friend BivariateFormalPowerSeries operator-(
        BivariateFormalPowerSeries left,
        const BivariateFormalPowerSeries& right
    ){
        left -= right;
        return left;
    }

    friend BivariateFormalPowerSeries operator*(
        BivariateFormalPowerSeries left,
        const BivariateFormalPowerSeries& right
    ){
        left *= right;
        return left;
    }
};

