#ifndef CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_PERMUTATION_GROUP_ORDER_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_PERMUTATION_GROUP_ORDER_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <numeric>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../integer/big_integer.hpp"

struct PermutationGroupOrderResult{
    BigInteger order;
    std::vector<std::size_t> stabilizer_orbit_sizes;
};

namespace permutation_group_order_internal{

using Permutation = std::vector<int>;

inline Permutation identity(int degree){
    Permutation result(static_cast<std::size_t>(degree));
    std::iota(result.begin(), result.end(), 0);
    return result;
}

// compose(first, second) applies second and then first.
inline Permutation compose(
    const Permutation& first,
    const Permutation& second
){
    Permutation result(first.size());
    for(std::size_t point = 0; point < first.size(); ++point){
        result[point] = first[static_cast<std::size_t>(second[point])];
    }
    return result;
}

inline Permutation inverse(const Permutation& permutation){
    Permutation result(permutation.size());
    for(std::size_t point = 0; point < permutation.size(); ++point){
        result[static_cast<std::size_t>(permutation[point])] =
            static_cast<int>(point);
    }
    return result;
}

inline bool is_identity(const Permutation& permutation){
    for(std::size_t point = 0; point < permutation.size(); ++point){
        if(permutation[point] != static_cast<int>(point)) return false;
    }
    return true;
}

inline void sort_unique(std::vector<Permutation>& permutations){
    std::sort(permutations.begin(), permutations.end());
    permutations.erase(
        std::unique(permutations.begin(), permutations.end()),
        permutations.end()
    );
    permutations.erase(
        std::remove_if(
            permutations.begin(), permutations.end(), is_identity
        ),
        permutations.end()
    );
}

} // namespace permutation_group_order_internal

inline PermutationGroupOrderResult permutation_group_order(
    int degree,
    const std::vector<std::vector<int>>& input_generators
){
    using namespace permutation_group_order_internal;
    if(degree < 0)[[unlikely]]{
        throw std::runtime_error(
            "library assertion fault: negative degree (permutation_group_order)."
        );
    }
    for(const Permutation& permutation: input_generators){
        if(permutation.size() != static_cast<std::size_t>(degree))[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: permutation size mismatch "
                "(permutation_group_order)."
            );
        }
        std::vector<char> used(static_cast<std::size_t>(degree), 0);
        for(const int image: permutation){
            if(image < 0 || image >= degree || used[static_cast<std::size_t>(image)])[[unlikely]]{
                throw std::runtime_error(
                    "library assertion fault: input is not a permutation "
                    "(permutation_group_order)."
                );
            }
            used[static_cast<std::size_t>(image)] = 1;
        }
    }

    std::vector<Permutation> generators;
    generators.reserve(input_generators.size() * 2);
    for(const Permutation& generator: input_generators){
        if(is_identity(generator)) continue;
        generators.push_back(generator);
        generators.push_back(inverse(generator));
    }
    sort_unique(generators);

    PermutationGroupOrderResult result{BigInteger(1), {}};
    result.stabilizer_orbit_sizes.reserve(static_cast<std::size_t>(degree));
    const Permutation identity_permutation = identity(degree);

    for(int base = 0; base < degree; ++base){
        std::vector<char> reached(static_cast<std::size_t>(degree), 0);
        std::vector<Permutation> transversal(
            static_cast<std::size_t>(degree), identity_permutation
        );
        std::vector<int> orbit;
        orbit.reserve(static_cast<std::size_t>(degree));
        std::queue<int> queue;
        reached[static_cast<std::size_t>(base)] = 1;
        orbit.push_back(base);
        queue.push(base);
        while(!queue.empty()){
            const int point = queue.front();
            queue.pop();
            for(const Permutation& generator: generators){
                const int next = generator[static_cast<std::size_t>(point)];
                if(reached[static_cast<std::size_t>(next)]) continue;
                reached[static_cast<std::size_t>(next)] = 1;
                transversal[static_cast<std::size_t>(next)] = compose(
                    generator,
                    transversal[static_cast<std::size_t>(point)]
                );
                orbit.push_back(next);
                queue.push(next);
            }
        }
        result.stabilizer_orbit_sizes.push_back(orbit.size());
        result.order *= static_cast<unsigned long long>(orbit.size());
        if(orbit.size() == 1) continue;

        std::vector<Permutation> inverse_transversal(
            static_cast<std::size_t>(degree)
        );
        for(const int point: orbit){
            inverse_transversal[static_cast<std::size_t>(point)] =
                inverse(transversal[static_cast<std::size_t>(point)]);
        }
        std::vector<Permutation> stabilizer_generators;
        stabilizer_generators.reserve(orbit.size() * generators.size());
        for(const int point: orbit){
            for(const Permutation& generator: generators){
                const int next = generator[static_cast<std::size_t>(point)];
                Permutation schreier = compose(
                    inverse_transversal[static_cast<std::size_t>(next)],
                    compose(
                        generator,
                        transversal[static_cast<std::size_t>(point)]
                    )
                );
                if(!is_identity(schreier)){
                    stabilizer_generators.push_back(std::move(schreier));
                }
            }
        }
        sort_unique(stabilizer_generators);
        generators = std::move(stabilizer_generators);
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_PERMUTATION_GROUP_ORDER_HPP_INCLUDED
