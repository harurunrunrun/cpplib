#ifndef CPPLIB_SRC_ALGORITHM_MATH_LINEAR_RECURRENCE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_LINEAR_RECURRENCE_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <vector>

#include "../fft/convolution.hpp"

namespace linear_recurrence_internal{

template<class T, int MAX_DEGREE>
struct BostanMoriWorkspace{
    std::array<T, static_cast<std::size_t>(MAX_DEGREE)> p{};
    std::array<T, static_cast<std::size_t>(MAX_DEGREE) + 1> q{};
    std::array<T, static_cast<std::size_t>(MAX_DEGREE) + 1> q_negative{};
    std::array<T, static_cast<std::size_t>(MAX_DEGREE)> next_p{};
    std::array<T, static_cast<std::size_t>(MAX_DEGREE) + 1> next_q{};
    std::array<T, static_cast<std::size_t>(2) * MAX_DEGREE + 1> product{};
};

template<class T, int MAX_DEGREE>
T bostan_mori_fixed(
    const T* initial,
    const T* coefficient,
    int degree,
    std::uint64_t index
){
    if(degree == 0) return T{};
    if(index < static_cast<std::uint64_t>(degree)){
        return initial[static_cast<std::size_t>(index)];
    }

    auto workspace = std::make_unique<BostanMoriWorkspace<T, MAX_DEGREE>>();
    workspace->q[0] = T(1);
    for(int i = 0; i < degree; ++i){
        workspace->q[static_cast<std::size_t>(i + 1)] =
            -coefficient[static_cast<std::size_t>(i)];
    }
    for(int i = 0; i < degree; ++i){
        T value = initial[static_cast<std::size_t>(i)];
        for(int j = 1; j <= i; ++j){
            value += workspace->q[static_cast<std::size_t>(j)] *
                initial[static_cast<std::size_t>(i - j)];
        }
        workspace->p[static_cast<std::size_t>(i)] = value;
    }

    while(index != 0){
        for(int i = 0; i <= degree; ++i){
            workspace->q_negative[static_cast<std::size_t>(i)] =
                (i & 1) ? -workspace->q[static_cast<std::size_t>(i)] :
                    workspace->q[static_cast<std::size_t>(i)];
        }

        std::fill_n(workspace->product.begin(), 2 * degree, T{});
        for(int i = 0; i < degree; ++i){
            const T value = workspace->p[static_cast<std::size_t>(i)];
            for(int j = 0; j <= degree; ++j){
                workspace->product[static_cast<std::size_t>(i + j)] +=
                    value * workspace->q_negative[static_cast<std::size_t>(j)];
            }
        }
        const int parity = static_cast<int>(index & 1);
        for(int i = 0; i < degree; ++i){
            workspace->next_p[static_cast<std::size_t>(i)] =
                workspace->product[static_cast<std::size_t>(2 * i + parity)];
        }

        std::fill_n(workspace->product.begin(), 2 * degree + 1, T{});
        for(int i = 0; i <= degree; ++i){
            const T value = workspace->q[static_cast<std::size_t>(i)];
            for(int j = 0; j <= degree; ++j){
                workspace->product[static_cast<std::size_t>(i + j)] +=
                    value * workspace->q_negative[static_cast<std::size_t>(j)];
            }
        }
        for(int i = 0; i <= degree; ++i){
            workspace->next_q[static_cast<std::size_t>(i)] =
                workspace->product[static_cast<std::size_t>(2 * i)];
        }
        std::copy_n(workspace->next_p.begin(), degree, workspace->p.begin());
        std::copy_n(workspace->next_q.begin(), degree + 1, workspace->q.begin());
        index >>= 1;
    }
    return workspace->p[0];
}

} // namespace linear_recurrence_internal

template<class T, int MAX_DEGREE>
struct LinearRecurrence{
    static_assert(MAX_DEGREE >= 0);

private:
    int _degree = 0;
    std::array<T, static_cast<std::size_t>(MAX_DEGREE)> _initial{};
    std::array<T, static_cast<std::size_t>(MAX_DEGREE)> _coefficient{};

    static void check_degree(int degree){
        if(degree < 0 || MAX_DEGREE < degree)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: capacity violation (LinearRecurrence)."
            );
        }
    }

public:
    LinearRecurrence() = default;

    LinearRecurrence(const T* initial, const T* coefficient, int degree):
        _degree(degree){
        check_degree(degree);
        if(degree != 0 && (initial == nullptr || coefficient == nullptr))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: null pointer (LinearRecurrence)."
            );
        }
        if(degree != 0){
            std::copy_n(initial, degree, _initial.begin());
            std::copy_n(coefficient, degree, _coefficient.begin());
        }
    }

    LinearRecurrence(
        const std::vector<T>& initial,
        const std::vector<T>& coefficient
    ){
        if(initial.size() != coefficient.size())[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: size violation (LinearRecurrence)."
            );
        }
        if(initial.size() > static_cast<std::size_t>(MAX_DEGREE))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: capacity violation (LinearRecurrence)."
            );
        }
        _degree = static_cast<int>(initial.size());
        std::copy_n(initial.begin(), _degree, _initial.begin());
        std::copy_n(coefficient.begin(), _degree, _coefficient.begin());
    }

    int degree() const{ return _degree; }

    const T& initial_value(int index) const{
        if(index < 0 || _degree <= index)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (initial_value)."
            );
        }
        return _initial[static_cast<std::size_t>(index)];
    }

    const T& coefficient(int index) const{
        if(index < 0 || _degree <= index)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: range violation (coefficient)."
            );
        }
        return _coefficient[static_cast<std::size_t>(index)];
    }

    T kth_term(std::uint64_t index) const{
        return linear_recurrence_internal::bostan_mori_fixed<T, MAX_DEGREE>(
            _initial.data(), _coefficient.data(), _degree, index
        );
    }
};

template<class T, int MAX_DEGREE>
LinearRecurrence<T, MAX_DEGREE> berlekamp_massey(
    const T* sequence,
    int sequence_size
){
    static_assert(MAX_DEGREE >= 0);
    if(sequence_size < 0 || (sequence_size != 0 && sequence == nullptr))[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: invalid input (berlekamp_massey)."
        );
    }

    std::array<T, static_cast<std::size_t>(MAX_DEGREE) + 1> current{};
    std::array<T, static_cast<std::size_t>(MAX_DEGREE) + 1> previous{};
    std::array<T, static_cast<std::size_t>(MAX_DEGREE) + 1> backup{};
    current[0] = previous[0] = T(1);
    int current_size = 1;
    int previous_size = 1;
    int degree = 0;
    int shift = 1;
    T last_difference = T(1);

    for(int i = 0; i < sequence_size; ++i){
        T difference = sequence[static_cast<std::size_t>(i)];
        for(int j = 1; j <= degree; ++j){
            difference += current[static_cast<std::size_t>(j)] *
                sequence[static_cast<std::size_t>(i - j)];
        }
        if(difference == T{}){
            ++shift;
            continue;
        }

        std::copy_n(current.begin(), current_size, backup.begin());
        const int backup_size = current_size;
        const int required_size = previous_size + shift;
        if(MAX_DEGREE + 1 < required_size)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: capacity violation (berlekamp_massey)."
            );
        }
        current_size = std::max(current_size, required_size);
        const T multiplier = difference / last_difference;
        for(int j = 0; j < previous_size; ++j){
            current[static_cast<std::size_t>(j + shift)] -=
                multiplier * previous[static_cast<std::size_t>(j)];
        }

        if(2 * degree <= i){
            degree = i + 1 - degree;
            if(MAX_DEGREE < degree)[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: capacity violation (berlekamp_massey)."
                );
            }
            std::copy_n(backup.begin(), backup_size, previous.begin());
            previous_size = backup_size;
            last_difference = difference;
            shift = 1;
        }else{
            ++shift;
        }
    }

    std::array<T, static_cast<std::size_t>(MAX_DEGREE)> initial{};
    std::array<T, static_cast<std::size_t>(MAX_DEGREE)> coefficient{};
    for(int i = 0; i < degree; ++i){
        initial[static_cast<std::size_t>(i)] = sequence[static_cast<std::size_t>(i)];
        coefficient[static_cast<std::size_t>(i)] =
            -current[static_cast<std::size_t>(i + 1)];
    }
    return LinearRecurrence<T, MAX_DEGREE>(initial.data(), coefficient.data(), degree);
}

template<class T, int MAX_DEGREE>
LinearRecurrence<T, MAX_DEGREE> berlekamp_massey(
    const std::vector<T>& sequence
){
    return berlekamp_massey<T, MAX_DEGREE>(
        sequence.data(), static_cast<int>(sequence.size())
    );
}

template<int MOD>
Modint<MOD> bostan_mori(
    const std::vector<Modint<MOD>>& initial,
    const std::vector<Modint<MOD>>& coefficient,
    std::uint64_t index
){
    using mint = Modint<MOD>;
    if(initial.size() != coefficient.size())[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: size violation (bostan_mori)."
        );
    }
    const std::size_t degree = initial.size();
    if(degree == 0) return mint{};
    if(index < degree) return initial[static_cast<std::size_t>(index)];

    std::vector<mint> q(degree + 1);
    q[0] = mint(1);
    for(std::size_t i = 0; i < degree; ++i) q[i + 1] = -coefficient[i];
    std::vector<mint> p = arbitrary_mod_convolution(initial, q);
    p.resize(degree);

    std::vector<mint> q_negative(degree + 1);
    while(index != 0){
        for(std::size_t i = 0; i <= degree; ++i){
            q_negative[i] = (i & 1) ? -q[i] : q[i];
        }
        const std::vector<mint> product_p = arbitrary_mod_convolution(p, q_negative);
        const std::vector<mint> product_q = arbitrary_mod_convolution(q, q_negative);
        const std::size_t parity = static_cast<std::size_t>(index & 1);
        for(std::size_t i = 0; i < degree; ++i) p[i] = product_p[2 * i + parity];
        for(std::size_t i = 0; i <= degree; ++i) q[i] = product_q[2 * i];
        index >>= 1;
    }
    return p[0];
}

#endif  // CPPLIB_SRC_ALGORITHM_MATH_LINEAR_RECURRENCE_HPP_INCLUDED
