// competitive-verifier: STANDALONE

#define PROBLEM "https://example.com/standalone/square_matrix"

#include <cassert>
#include <iostream>
#include <stdexcept>
#include "../../src/structure/matrix/square_matrix.hpp"
#include "../../src/structure/modint/modint.hpp"

int main(){
    int query_count;
    if(std::cin >> query_count){
        using external_mint = Modint<998244353>;
        while(query_count--){
            int n;
            long long exponent;
            std::cin >> n >> exponent;
            SquareMatrix<external_mint, 8> matrix(n);
            for(int i = 0; i < n; i++){
                for(int j = 0; j < n; j++){
                    long long value;
                    std::cin >> value;
                    matrix(i, j) = value;
                }
            }
            const auto power = matrix.pow(exponent);
            std::cout << n;
            for(int i = 0; i < n; i++){
                for(int j = 0; j < n; j++) std::cout << ' ' << power(i, j).val();
            }
            std::cout << '\n';
        }
        return 0;
    }
    SquareMatrix<long long, 4> a({{1, 2}, {3, 4}});
    assert(a.size() == 2);
    assert(!a.empty());

    SquareMatrix<long long, 4> id = SquareMatrix<long long, 4>::identity(2);
    assert(a * id == a);
    assert(id * a == a);
    assert(a.pow(0) == id);
    assert(a.pow(1) == a);

    SquareMatrix<long long, 4> a2 = a * a;
    assert(a2(0, 0) == 7);
    assert(a2(0, 1) == 10);
    assert(a2(1, 0) == 15);
    assert(a2(1, 1) == 22);
    assert(a.pow(2) == a2);

    SquareMatrix<long long, 4> sum = a + a;
    assert(sum(0, 0) == 2);
    assert(sum(1, 1) == 8);
    assert(sum - a == a);

    SquareMatrix<long long, 4> scaled = 6 * a;
    scaled /= 3;
    assert(scaled == a * 2);

    SquareMatrix<long long, 4> fib({{1, 1}, {1, 0}});
    SquareMatrix<long long, 4> f10 = fib.pow(10);
    assert(f10(0, 0) == 89);
    assert(f10(0, 1) == 55);
    assert(f10(1, 0) == 55);
    assert(f10(1, 1) == 34);

    auto tr = a.transposed();
    assert(tr(0, 1) == 3);
    assert(tr(1, 0) == 2);

    bool thrown = false;
    try{
        SquareMatrix<int, 2> bad(3);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        SquareMatrix<int, 2> bad({{1}, {2, 3}});
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);

    thrown = false;
    try{
        a.pow(-1);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}
