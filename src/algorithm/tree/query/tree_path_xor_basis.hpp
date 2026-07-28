#ifndef CPPLIB_SRC_ALGORITHM_TREE_QUERY_TREE_PATH_XOR_BASIS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_TREE_QUERY_TREE_PATH_XOR_BASIS_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <bit>
#include <cstdint>
#include <stdexcept>
#include <vector>

template<int Bits = 64>
class TreePathXorBasis{
    static_assert(1 <= Bits && Bits <= 64);
public:
    using Basis = std::array<std::uint64_t, Bits>;

private:
    int n_ = 0, log_ = 0;
    std::vector<int> depth_;
    std::vector<std::vector<int>> up_;
    std::vector<std::vector<Basis>> basis_;

    static void insert(Basis& basis, std::uint64_t value){
        for(int bit = Bits - 1; bit >= 0; --bit){
            if(((value >> bit) & 1U) == 0) continue;
            if(basis[bit] == 0){
                basis[bit] = value;
                return;
            }
            value ^= basis[bit];
        }
    }
    static void merge(Basis& lhs, const Basis& rhs){
        for(const auto value: rhs) if(value != 0) insert(lhs, value);
    }

public:
    TreePathXorBasis() = default;
    TreePathXorBasis(const std::vector<std::vector<int>>& graph,
                     const std::vector<std::uint64_t>& values, int root = 0)
        : n_(static_cast<int>(graph.size())){
        if(static_cast<int>(values.size()) != n_) throw std::invalid_argument("TreePathXorBasis: size mismatch");
        if(n_ == 0) return;
        if(root < 0 || root >= n_) throw std::out_of_range("TreePathXorBasis: invalid root");
        log_ = static_cast<int>(std::bit_width(static_cast<unsigned int>(n_)));
        depth_.assign(n_, -1);
        up_.assign(log_, std::vector<int>(n_, root));
        basis_.assign(log_, std::vector<Basis>(n_));
        std::vector<int> order{root};
        depth_[root] = 0;
        for(std::size_t i = 0; i < order.size(); ++i){
            const int v = order[i];
            insert(basis_[0][v], values[v]);
            for(const int to: graph[v]) if(depth_[to] == -1){
                depth_[to] = depth_[v] + 1;
                up_[0][to] = v;
                order.push_back(to);
            }
        }
        if(static_cast<int>(order.size()) != n_) throw std::invalid_argument("TreePathXorBasis: disconnected graph");
        for(int k = 1; k < log_; ++k){
            for(int v = 0; v < n_; ++v){
                up_[k][v] = up_[k - 1][up_[k - 1][v]];
                basis_[k][v] = basis_[k - 1][v];
                merge(basis_[k][v], basis_[k - 1][up_[k - 1][v]]);
            }
        }
    }

    Basis path_basis(int u, int v) const {
        check(u); check(v);
        Basis result{};
        if(depth_[u] < depth_[v]) std::swap(u, v);
        int difference = depth_[u] - depth_[v];
        for(int k = 0; difference != 0; ++k, difference >>= 1) if(difference & 1){
            merge(result, basis_[k][u]);
            u = up_[k][u];
        }
        if(u == v){
            merge(result, basis_[0][u]);
            return result;
        }
        for(int k = log_ - 1; k >= 0; --k) if(up_[k][u] != up_[k][v]){
            merge(result, basis_[k][u]);
            merge(result, basis_[k][v]);
            u = up_[k][u];
            v = up_[k][v];
        }
        merge(result, basis_[0][u]);
        merge(result, basis_[0][v]);
        merge(result, basis_[0][up_[0][u]]);
        return result;
    }
    std::uint64_t maximum_xor(int u, int v, std::uint64_t seed = 0) const {
        const Basis basis = path_basis(u, v);
        for(int bit = Bits - 1; bit >= 0; --bit) seed = std::max(seed, seed ^ basis[bit]);
        return seed;
    }

private:
    void check(int v) const {
        if(v < 0 || v >= n_) throw std::out_of_range("TreePathXorBasis: invalid vertex");
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_TREE_QUERY_TREE_PATH_XOR_BASIS_HPP_INCLUDED
