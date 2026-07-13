#pragma once

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "multipoint_evaluation.hpp"

namespace math{

template<int MOD>
Polynomial<MOD> polynomial_interpolation(
    const std::vector<Modint<MOD>>& points,
    const std::vector<Modint<MOD>>& values
){
    if(points.size() != values.size())[[unlikely]]{
        throw std::invalid_argument("polynomial interpolation size mismatch");
    }
    if(points.empty()) return {};

    const polynomial_internal::ProductTree<MOD> tree(points);
    const Polynomial<MOD> derivative = polynomial_derivative<MOD>(tree[1]);
    const std::vector<Modint<MOD>> denominators =
        polynomial_internal::evaluate_with_product_tree<MOD>(derivative, tree);

    const std::size_t leaf_count = tree.leaf_count();
    std::vector<Polynomial<MOD>> interpolation(leaf_count * 2);
    for(std::size_t index = 0; index < points.size(); ++index){
        if(denominators[index] == Modint<MOD>(0))[[unlikely]]{
            throw std::invalid_argument("duplicate polynomial interpolation points");
        }
        interpolation[leaf_count + index] = {
            values[index] / denominators[index]
        };
    }

    for(std::size_t node = leaf_count; node > 1; --node){
        const std::size_t parent = node - 1;
        const Polynomial<MOD> from_left = polynomial_multiply<MOD>(
            interpolation[parent * 2], tree[parent * 2 + 1]
        );
        const Polynomial<MOD> from_right = polynomial_multiply<MOD>(
            interpolation[parent * 2 + 1], tree[parent * 2]
        );
        interpolation[parent] = polynomial_add<MOD>(from_left, from_right);
    }
    polynomial_internal::trim(interpolation[1]);
    return std::move(interpolation[1]);
}

} // namespace math
