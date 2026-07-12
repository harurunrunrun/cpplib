#define PROBLEM "https://example.com/standalone/matrix"

#include <cassert>
#include <stdexcept>
#include <vector>
#include "../../src/structure/matrix/matrix.hpp"

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
}
