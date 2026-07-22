#ifndef CPPLIB_SRC_ALGORITHM_MATH_MULTIPOINT_EVALUATION_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_MULTIPOINT_EVALUATION_HPP_INCLUDED

#include <cstddef>
#include <utility>
#include <vector>

#include "polynomial_division.hpp"
#include "polynomial_product.hpp"

namespace math{
namespace polynomial_internal{

template<int MOD>
class ProductTree{
public:
    using mint = Modint<MOD>;

private:
    std::size_t point_count_ = 0;
    std::size_t leaf_count_ = 1;
    std::vector<Polynomial<MOD>> nodes_;

public:
    explicit ProductTree(const std::vector<mint>& points):
        point_count_(points.size())
    {
        while(leaf_count_ < point_count_) leaf_count_ *= 2;
        nodes_.assign(leaf_count_ * 2, Polynomial<MOD>{mint(1)});
        for(std::size_t index = 0; index < point_count_; ++index){
            nodes_[leaf_count_ + index] = {-points[index], mint(1)};
        }
        for(std::size_t node = leaf_count_; node > 1; --node){
            const std::size_t parent = node - 1;
            nodes_[parent] = polynomial_multiply<MOD>(
                nodes_[parent * 2], nodes_[parent * 2 + 1]
            );
        }
    }

    std::size_t point_count() const{ return point_count_; }
    std::size_t leaf_count() const{ return leaf_count_; }
    const Polynomial<MOD>& operator[](std::size_t node) const{ return nodes_[node]; }
};

template<int MOD>
void evaluate_product_tree_node(
    const ProductTree<MOD>& tree,
    std::size_t node,
    std::size_t left,
    std::size_t right,
    const Polynomial<MOD>& polynomial,
    std::vector<Modint<MOD>>& result
){
    if(left >= tree.point_count()) return;
    if(right - left == 1){
        result[left] = polynomial.empty() ? Modint<MOD>(0) : polynomial[0];
        return;
    }
    const std::size_t middle = left + (right - left) / 2;
    const Polynomial<MOD> left_remainder = polynomial_remainder<MOD>(
        polynomial, tree[node * 2]
    );
    evaluate_product_tree_node(
        tree, node * 2, left, middle, left_remainder, result
    );
    if(middle < tree.point_count()){
        const Polynomial<MOD> right_remainder = polynomial_remainder<MOD>(
            polynomial, tree[node * 2 + 1]
        );
        evaluate_product_tree_node(
            tree, node * 2 + 1, middle, right, right_remainder, result
        );
    }
}

template<int MOD>
std::vector<Modint<MOD>> evaluate_with_product_tree(
    Polynomial<MOD> polynomial,
    const ProductTree<MOD>& tree
){
    std::vector<Modint<MOD>> result(tree.point_count());
    if(tree.point_count() == 0) return result;
    polynomial_internal::trim(polynomial);
    if(polynomial.size() >= tree[1].size()){
        polynomial = polynomial_remainder<MOD>(std::move(polynomial), tree[1]);
    }
    evaluate_product_tree_node(
        tree, 1, 0, tree.leaf_count(), polynomial, result
    );
    return result;
}

} // namespace polynomial_internal

template<int MOD>
std::vector<Modint<MOD>> multipoint_evaluation(
    Polynomial<MOD> polynomial,
    const std::vector<Modint<MOD>>& points
){
    const polynomial_internal::ProductTree<MOD> tree(points);
    return polynomial_internal::evaluate_with_product_tree<MOD>(
        std::move(polynomial), tree
    );
}

} // namespace math

#endif  // CPPLIB_SRC_ALGORITHM_MATH_MULTIPOINT_EVALUATION_HPP_INCLUDED
