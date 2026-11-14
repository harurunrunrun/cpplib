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

// Stabilizer chain using all distinct Schreier generators of each level as the
// generators of the next one (no sifting).  This is fast when duplicates keep the
// generator sets small (e.g. a cycle and a transposition), but the sets can grow
// multiplicatively from level to level.  Gives up and returns false as soon as a
// set exceeds limit (checked after every limit new candidates, so the work wasted
// on giving up stays small); otherwise appends the orbit sizes to orbit_sizes.
inline bool schreier_generator_chain(
    int degree,
    const std::vector<Permutation>& input_generators,
    std::size_t limit,
    std::vector<std::size_t>& orbit_sizes
){
    std::vector<Permutation> generators;
    generators.reserve(input_generators.size() * 2);
    for(const Permutation& generator: input_generators){
        if(is_identity(generator)) continue;
        generators.push_back(generator);
        Permutation inverted(generator.size());
        for(std::size_t point = 0; point < generator.size(); ++point){
            inverted[static_cast<std::size_t>(generator[point])] = static_cast<int>(point);
        }
        generators.push_back(std::move(inverted));
    }
    sort_unique(generators);
    if(generators.size() > limit) return false;

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
                const Permutation& from = transversal[static_cast<std::size_t>(point)];
                Permutation& to = transversal[static_cast<std::size_t>(next)];
                for(std::size_t x = 0; x < to.size(); ++x){
                    to[x] = generator[static_cast<std::size_t>(from[x])];
                }
                orbit.push_back(next);
                queue.push(next);
            }
        }
        orbit_sizes.push_back(orbit.size());
        if(orbit.size() == 1) continue;

        std::vector<Permutation> inverse_transversal(
            static_cast<std::size_t>(degree), identity_permutation
        );
        for(const int point: orbit){
            const Permutation& element = transversal[static_cast<std::size_t>(point)];
            Permutation& inverted = inverse_transversal[static_cast<std::size_t>(point)];
            for(std::size_t x = 0; x < element.size(); ++x){
                inverted[static_cast<std::size_t>(element[x])] = static_cast<int>(x);
            }
        }
        std::vector<Permutation> stabilizer_generators;
        std::size_t checked_size = 0;
        Permutation schreier(static_cast<std::size_t>(degree));
        for(const int point: orbit){
            const Permutation& representative =
                transversal[static_cast<std::size_t>(point)];
            for(const Permutation& generator: generators){
                const Permutation& back = inverse_transversal[static_cast<std::size_t>(
                    generator[static_cast<std::size_t>(point)])];
                for(std::size_t x = 0; x < schreier.size(); ++x){
                    schreier[x] = back[static_cast<std::size_t>(
                        generator[static_cast<std::size_t>(representative[x])])];
                }
                if(is_identity(schreier)) continue;
                stabilizer_generators.push_back(schreier);
                if(stabilizer_generators.size() >= checked_size + limit){
                    sort_unique(stabilizer_generators);
                    if(stabilizer_generators.size() > limit) return false;
                    checked_size = stabilizer_generators.size();
                }
            }
        }
        sort_unique(stabilizer_generators);
        if(stabilizer_generators.size() > limit) return false;
        generators = std::move(stabilizer_generators);
    }
    return true;
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
    // inverse_transversal[level][point] is the inverse of transversal[level][point],
    // and empty exactly where the latter is.  build_orbit does not clear it: the
    // generators used at a level only increase, so a rebuilt orbit contains the
    // previous one and every entry of the new orbit is overwritten.
    std::vector<std::vector<Permutation>> inverse_transversal;

    SchreierSims(int degree_, const std::vector<Permutation>& generators)
        : degree(degree_),
          transversal(
              static_cast<std::size_t>(degree_),
              std::vector<Permutation>(static_cast<std::size_t>(degree_))
          ),
          inverse_transversal(transversal){
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
        auto& inverse_table = inverse_transversal[static_cast<std::size_t>(level)];
        std::fill(table.begin(), table.end(), Permutation());
        table[static_cast<std::size_t>(level)] = identity(degree);
        inverse_table[static_cast<std::size_t>(level)] = identity(degree);
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
                inverse_table[static_cast<std::size_t>(next)] =
                    inverse(table[static_cast<std::size_t>(next)]);
                queue.push(next);
            }
        }
    }

    // Sifts every Schreier generator of the level through the higher levels.  On the
    // first one that does not sift, adds its residue as a strong generator and returns
    // the level where sifting stopped; returns -1 if all of them sift.
    int check_level(int level){
        const auto& table = transversal[static_cast<std::size_t>(level)];
        const auto& inverse_table = inverse_transversal[static_cast<std::size_t>(level)];
        Permutation residue(static_cast<std::size_t>(degree));
        Permutation buffer(static_cast<std::size_t>(degree));
        for(int point = 0; point < degree; ++point){
            const Permutation& representative = table[static_cast<std::size_t>(point)];
            if(representative.empty()) continue;
            for(std::size_t index = 0; index < strong_generators.size(); ++index){
                if(generator_levels[index] < level) continue;
                // residue = t_{g(point)}^{-1} g t_point, then sifted by the higher levels.
                const Permutation& generator = strong_generators[index];
                const Permutation& back = inverse_table[static_cast<std::size_t>(
                    generator[static_cast<std::size_t>(point)])];
                for(std::size_t x = 0; x < residue.size(); ++x){
                    residue[x] = back[static_cast<std::size_t>(
                        generator[static_cast<std::size_t>(representative[x])])];
                }
                int stop = level + 1;
                for(; stop < degree; ++stop){
                    const std::size_t image =
                        static_cast<std::size_t>(residue[static_cast<std::size_t>(stop)]);
                    const Permutation& element =
                        inverse_transversal[static_cast<std::size_t>(stop)][image];
                    if(element.empty()) break;
                    for(std::size_t x = 0; x < residue.size(); ++x){
                        buffer[x] = element[static_cast<std::size_t>(residue[x])];
                    }
                    residue.swap(buffer);
                }
                if(stop < degree){
                    strong_generators.push_back(residue);
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
    // depend on the generators chosen or on the method.  The Schreier generator
    // chain is tried first while its generator sets stay small; otherwise the
    // Schreier-Sims algorithm is used.
    PermutationGroupOrderResult result{BigInteger(1), {}};
    result.stabilizer_orbit_sizes.reserve(static_cast<std::size_t>(degree));
    const std::size_t limit = 2 * static_cast<std::size_t>(degree);
    if(!schreier_generator_chain(
        degree, input_generators, limit, result.stabilizer_orbit_sizes
    )){
        const SchreierSims chain(degree, input_generators);
        result.stabilizer_orbit_sizes.clear();
        for(int level = 0; level < degree; ++level){
            std::size_t orbit_size = 0;
            for(const auto& element: chain.transversal[static_cast<std::size_t>(level)]){
                if(!element.empty()) ++orbit_size;
            }
            result.stabilizer_orbit_sizes.push_back(orbit_size);
        }
    }
    for(const std::size_t orbit_size: result.stabilizer_orbit_sizes){
        result.order *= static_cast<unsigned long long>(orbit_size);
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_MATH_COMBINATORICS_PERMUTATION_GROUP_ORDER_HPP_INCLUDED
