// competitive-verifier: STANDALONE

#define PROBLEM "https://example.com/standalone/matrix"

#include <cassert>
#include <stdexcept>
#include <vector>
#include "../../src/structure/matrix/matrix.hpp"
#include "../../src/structure/modint/modint.hpp"


template<class T, int N>
Matrix<T, N, N> identity_matrix(int n){
    Matrix<T, N, N> res(n, n);
    for(int i = 0; i < n; i++){
        res(i, i) = T(1);
    }
    return res;
}

template<class T, int N>
Matrix<T, N, N> matrix_pow(Matrix<T, N, N> base, long long exponent){
    Matrix<T, N, N> res = identity_matrix<T, N>(base.rows());
    while(exponent > 0){
        if(exponent & 1) res = res * base;
        base = base * base;
        exponent >>= 1;
    }
    return res;
}

int main(){
    Matrix<long long, 3, 4> a(2, 3);
    assert(a.rows() == 2);
    assert(a.cols() == 3);
    assert(!a.empty());

    long long value = 1;
    for(int i = 0; i < a.rows(); i++){
        for(int j = 0; j < a.cols(); j++){
            a(i, j) = value++;
        }
    }

    Matrix<long long, 3, 4> b({{6, 5, 4}, {3, 2, 1}});
    Matrix<long long, 3, 4> sum = a + b;
    assert(sum(0, 0) == 7);
    assert(sum(0, 1) == 7);
    assert(sum(1, 2) == 7);

    Matrix<long long, 3, 4> diff = sum - b;
    assert(diff == a);

    Matrix<long long, 3, 4> neg = -a;
    for(int i = 0; i < a.rows(); i++){
        for(int j = 0; j < a.cols(); j++){
            assert(neg(i, j) == -a(i, j));
        }
    }

    Matrix<long long, 3, 4> scaled = a * 3;
    scaled /= 3;
    assert(scaled == a);

    Matrix<long long, 3, 2> c({{1, 2}, {3, 4}, {5, 6}});
    Matrix<long long, 3, 2> prod = a * c;
    assert(prod.rows() == 2);
    assert(prod.cols() == 2);
    assert(prod(0, 0) == 22);
    assert(prod(0, 1) == 28);
    assert(prod(1, 0) == 49);
    assert(prod(1, 1) == 64);

    auto tr = a.transposed();
    assert(tr.rows() == 3);
    assert(tr.cols() == 2);
    for(int i = 0; i < a.rows(); i++){
        for(int j = 0; j < a.cols(); j++){
            assert(tr(j, i) == a(i, j));
        }
    }

    using mint = Modint998244353;
    Matrix<mint, 4, 4> fib({{1, 1}, {1, 0}});
    Matrix<mint, 4, 4> fib_10 = matrix_pow(fib, 10);
    assert(fib.pow_entry_bmbm(10, 0, 0) == fib_10(0, 0));
    assert(fib.pow_entry_bmbm(10, 1) == fib_10(0, 1));

    long long big_exponent = 1000000000000LL;
    Matrix<mint, 4, 4> fib_big = matrix_pow(fib, big_exponent);
    assert(fib.pow_entry_bmbm(big_exponent, 0, 0) == fib_big(0, 0));
    assert(fib.pow_entry_bmbm(big_exponent, 1, 1) == fib_big(1, 1));

    Matrix<mint, 4, 4> d({{2, 3, 5}, {7, 11, 13}, {17, 19, 23}});
    for(long long exponent = 0; exponent <= 40; exponent++){
        Matrix<mint, 4, 4> expected = matrix_pow(d, exponent);
        for(int i = 0; i < d.rows(); i++){
            for(int j = 0; j < d.cols(); j++){
                assert(d.pow_entry_bmbm(exponent, i, j) == expected(i, j));
                assert(d.pow_entry_bmbm(exponent, i * d.cols() + j) == expected(i, j));
            }
        }
    }

    Matrix<mint, 3, 3> zero(2, 2);
    assert(zero.pow_entry_bmbm(0, 0, 0) == mint(1));
    assert(zero.pow_entry_bmbm(100, 0, 0) == mint(0));
    assert(zero.pow_entry_bmbm(100, 0, 1) == mint(0));

    bool thrown = false;
    try{
        Matrix<int, 2, 2> bad(3, 1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        Matrix<int, 2, 2> bad({{1}, {2, 3}});
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        fib.pow_entry_bmbm(-1, 0, 0);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        Matrix<mint, 3, 4> nonsquare(2, 3);
        nonsquare.pow_entry_bmbm(5, 0, 0);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        fib.pow_entry_bmbm(5, 4);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}
