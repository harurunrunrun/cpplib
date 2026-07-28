#ifndef CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_MULTIPLICATIVE_DIRICHLET_PREFIX_CONVOLUTION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_MULTIPLICATIVE_DIRICHLET_PREFIX_CONVOLUTION_HPP_INCLUDED

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace math{
namespace dirichlet_prefix_detail{

inline std::uint64_t floor_square_root(std::uint64_t value){
    std::uint64_t root = static_cast<std::uint64_t>(
        std::sqrt(static_cast<long double>(value))
    );
    while(root < value / (root + 1)) ++root;
    while(root > value / root) --root;
    return root;
}

class QuotientIndex{
    std::uint64_t maximum_;
    std::uint64_t square_root_;
    std::size_t size_;

public:
    explicit QuotientIndex(std::uint64_t maximum)
        : maximum_(maximum),
          square_root_(floor_square_root(maximum)),
          size_(static_cast<std::size_t>(
              2 * square_root_ - (maximum_ / square_root_ == square_root_)
          )) {}

    std::uint64_t square_root() const{
        return square_root_;
    }

    std::size_t size() const{
        return size_;
    }

    std::size_t ordinal(std::uint64_t value) const{
        if(value <= square_root_){
            return static_cast<std::size_t>(value);
        }
        return size_ - static_cast<std::size_t>(maximum_ / value) + 1;
    }
};

struct IndexInterval{
    std::size_t first;
    std::size_t last;
};

template<bool Reverse, class T, class Callback>
void accumulate_cross_terms(
    std::uint64_t maximum,
    const QuotientIndex& index,
    std::vector<T>& result,
    const std::vector<T>& first,
    const std::vector<T>& second,
    Callback&& finalize
){
    const std::size_t count = index.size();
    const std::size_t root = static_cast<std::size_t>(index.square_root());

    const auto add_rectangle = [&](
        IndexInterval x,
        IndexInterval y,
        IndexInterval destination
    ){
        const T first_x = first[x.last] - first[x.first - 1];
        const T first_y = first[y.last] - first[y.first - 1];
        const T second_x = Reverse
            ? second[x.first - 1] - second[x.last]
            : second[x.last] - second[x.first - 1];
        const T second_y = Reverse
            ? second[y.first - 1] - second[y.last]
            : second[y.last] - second[y.first - 1];
        T contribution = first_x * second_y;
        if(x.first != y.first || x.last != y.last){
            contribution += first_y * second_x;
        }
        result[destination.first] += contribution;
        if(destination.last < count){
            result[destination.last + 1] -= contribution;
        }
    };

    for(std::size_t order = 2; order <= count; ++order){
        if(order > root){
            const std::size_t quotient_denominator = count - order + 1;
            for(std::size_t x = 2; ; ++x){
                const std::size_t y_first =
                    std::max(x, quotient_denominator) + 1;
                const std::uint64_t product_quotient =
                    (maximum / static_cast<std::uint64_t>(x))
                    / static_cast<std::uint64_t>(quotient_denominator);
                const std::size_t y_last = index.ordinal(product_quotient);
                if(y_last < y_first) break;
                add_rectangle(
                    {x, x}, {y_first, y_last}, {order, order}
                );
            }
        }

        result[order] = finalize(order);

        if(order <= root){
            const std::size_t x = order;
            for(std::size_t y = 2; y < order; ++y){
                const std::uint64_t product =
                    static_cast<std::uint64_t>(x)
                    * static_cast<std::uint64_t>(y);
                const std::size_t destination_first =
                    index.ordinal(product);
                const std::size_t destination_last =
                    index.ordinal(maximum / static_cast<std::uint64_t>(x));
                if(destination_last < destination_first) break;
                add_rectangle(
                    {x, x},
                    {y, y},
                    {destination_first, destination_last}
                );
            }
            const std::uint64_t square =
                static_cast<std::uint64_t>(x)
                * static_cast<std::uint64_t>(x);
            add_rectangle(
                {x, x}, {x, x}, {index.ordinal(square), count}
            );
        }
    }
}

template<class T>
std::vector<T> add_sentinel(
    const std::vector<T>& values,
    std::size_t expected_size
){
    if(values.size() != expected_size){
        throw std::invalid_argument(
            "Dirichlet prefix table has an invalid size"
        );
    }
    std::vector<T> result(expected_size + 1);
    std::copy(values.begin(), values.end(), result.begin() + 1);
    return result;
}

template<class T>
std::vector<T> remove_sentinel(std::vector<T>&& values){
    return std::vector<T>(values.begin() + 1, values.end());
}

} // namespace dirichlet_prefix_detail

template<class T>
std::vector<T> dirichlet_prefix_convolution(
    std::uint64_t maximum,
    const std::vector<T>& first_prefix,
    const std::vector<T>& second_prefix
){
    if(maximum == 0){
        throw std::invalid_argument(
            "Dirichlet prefix convolution requires a positive maximum"
        );
    }
    const dirichlet_prefix_detail::QuotientIndex index(maximum);
    const std::size_t count = index.size();
    const std::vector<T> first =
        dirichlet_prefix_detail::add_sentinel(first_prefix, count);
    const std::vector<T> second =
        dirichlet_prefix_detail::add_sentinel(second_prefix, count);
    std::vector<T> result(count + 1);
    result[1] = first[1] * second[1];
    dirichlet_prefix_detail::accumulate_cross_terms<false>(
        maximum,
        index,
        result,
        first,
        second,
        [&](std::size_t order){
            return result[order]
                + (first[order] - first[order - 1]) * second[1]
                + (second[order] - second[order - 1]) * first[1];
        }
    );
    for(std::size_t order = 1; order <= count; ++order){
        result[order] += result[order - 1];
    }
    return dirichlet_prefix_detail::remove_sentinel(std::move(result));
}

template<class T>
std::vector<T> dirichlet_prefix_division(
    std::uint64_t maximum,
    const std::vector<T>& dividend_prefix,
    const std::vector<T>& divisor_prefix
){
    if(maximum == 0){
        throw std::invalid_argument(
            "Dirichlet prefix division requires a positive maximum"
        );
    }
    const dirichlet_prefix_detail::QuotientIndex index(maximum);
    const std::size_t count = index.size();
    std::vector<T> result =
        dirichlet_prefix_detail::add_sentinel(dividend_prefix, count);
    const std::vector<T> divisor =
        dirichlet_prefix_detail::add_sentinel(divisor_prefix, count);
    const T unit_inverse = T{1} / divisor[1];
    for(std::size_t order = count; order >= 2; --order){
        result[order] -= result[order - 1];
    }
    result[0] = T{};
    result[1] *= unit_inverse;
    dirichlet_prefix_detail::accumulate_cross_terms<true>(
        maximum,
        index,
        result,
        result,
        divisor,
        [&](std::size_t order){
            return unit_inverse * (
                result[order]
                - (divisor[order] - divisor[order - 1]) * result[1]
            ) + result[order - 1];
        }
    );
    return dirichlet_prefix_detail::remove_sentinel(std::move(result));
}

template<class T>
std::vector<T> dirichlet_prefix_inverse(
    std::uint64_t maximum,
    const std::vector<T>& prefix
){
    if(maximum == 0){
        throw std::invalid_argument(
            "Dirichlet prefix inverse requires a positive maximum"
        );
    }
    const dirichlet_prefix_detail::QuotientIndex index(maximum);
    return dirichlet_prefix_division(
        maximum,
        std::vector<T>(index.size(), T{1}),
        prefix
    );
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_NUMBER_THEORY_MULTIPLICATIVE_DIRICHLET_PREFIX_CONVOLUTION_HPP_INCLUDED
