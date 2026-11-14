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

inline int first_moved_point(const Permutation& permutation){
    for(std::size_t point = 0; point < permutation.size(); ++point){
        if(permutation[point] != static_cast<int>(point)) return static_cast<int>(point);
    }
    return static_cast<int>(permutation.size());
}

// Deterministic Schreier-Sims algorithm with the base 0, 1, ..., degree - 1.
// transversal[level][point] maps level to point and fixes 0, ..., level - 1
// (empty if point is not in the orbit).  The strong generators used at a level
// are those fixing every smaller base point.
struct SchreierSims{
    int degree;
    std::vector<Permutation> strong_generators;
    std::vector<int> generator_levels;  // first moved point of each strong generator
    std::vector<std::vector<Permutation>> transversal;

    SchreierSims(int degree_, const std::vector<Permutation>& generators)
        : degree(degree_),
          transversal(
              static_cast<std::size_t>(degree_),
              std::vector<Permutation>(static_cast<std::size_t>(degree_))
          ){
        for(const Permutation& generator: generators){
            if(!is_identity(generator)){
                strong_generators.push_back(generator);
                generator_levels.push_back(first_moved_point(generator));
            }
        }
        int level = degree - 1;
        while(level >= 0){
            build_orbit(level);
            const int failed = check_level(level);
            if(failed < 0) --level;
            else level = failed;
        }
    }

    void build_orbit(int level){
        auto& table = transversal[static_cast<std::size_t>(level)];
        std::fill(table.begin(), table.end(), Permutation());
        table[static_cast<std::size_t>(level)] = identity(degree);
        std::queue<int> queue;
        queue.push(level);
        while(!queue.empty()){
            const int point = queue.front();
            queue.pop();
            for(std::size_t index = 0; index < strong_generators.size(); ++index){
                if(generator_levels[index] < level) continue;
                const Permutation& generator = strong_generators[index];
                const int next = generator[static_cast<std::size_t>(point)];
                if(!table[static_cast<std::size_t>(next)].empty()) continue;
                table[static_cast<std::size_t>(next)] =
                    compose(generator, table[static_cast<std::size_t>(point)]);
                queue.push(next);
            }
        }
    }

    // Sifts every Schreier generator of the level through the higher levels.  On the
    // first one that does not sift, adds its residue as a strong generator and returns
    // the level where sifting stopped; returns -1 if all of them sift.
    int check_level(int level){
        const auto& table = transversal[static_cast<std::size_t>(level)];
        for(int point = 0; point < degree; ++point){
            const Permutation& representative = table[static_cast<std::size_t>(point)];
            if(representative.empty()) continue;
            for(std::size_t index = 0; index < strong_generators.size(); ++index){
                if(generator_levels[index] < level) continue;
                const Permutation moved = compose(strong_generators[index], representative);
                Permutation residue = compose(
                    inverse(table[static_cast<std::size_t>(
                        moved[static_cast<std::size_t>(level)])]),
                    moved
                );
                int stop = level + 1;
                for(; stop < degree; ++stop){
                    const auto& element = transversal[static_cast<std::size_t>(stop)][
                        static_cast<std::size_t>(residue[static_cast<std::size_t>(stop)])];
                    if(element.empty()) break;
                    residue = compose(inverse(element), residue);
                }
                if(stop < degree){
                    strong_generators.push_back(std::move(residue));
                    generator_levels.push_back(stop);
                    return stop;
                }
            }
        }
        return -1;
    }
};

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

    // The orbit sizes are those of the pointwise stabilizer chain, so they do not
    // depend on the generators chosen.
    const SchreierSims chain(degree, input_generators);

    PermutationGroupOrderResult result{BigInteger(1), {}};
    result.stabilizer_orbit_sizes.reserve(static_cast<std::size_t>(degree));
    for(int level = 0; level < degree; ++level){
        std::size_t orbit_size = 0;
        for(const auto& element: chain.transversal[static_cast<std::size_t>(level)]){
            if(!element.empty()) ++orbit_size;
        }
        result.stabilizer_orbit_sizes.push_back(orbit_size);
        result.order *= static_cast<unsigned long long>(orbit_size);
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_PERMUTATION_GROUP_ORDER_HPP_INCLUDED
