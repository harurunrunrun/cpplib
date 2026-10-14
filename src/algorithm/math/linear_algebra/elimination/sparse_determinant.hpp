#ifndef CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_ELIMINATION_SPARSE_DETERMINANT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_ELIMINATION_SPARSE_DETERMINANT_HPP_INCLUDED

#include <algorithm>
#include <cstdint>
#include <random>
#include <stdexcept>
#include <vector>
#include "../../sequence/linear_recurrence.hpp"

template<class T>struct SparseMatrixEntry{int row,column;T value;};

template<class T,int MAX_SIZE=-1>
T sparse_determinant(int size,const std::vector<SparseMatrixEntry<T>>& entries,
    std::uint64_t seed=std::random_device{}()){
    if(size<0 || (MAX_SIZE>=0 && size>MAX_SIZE))throw std::length_error("sparse_determinant: invalid matrix size");
    for(const auto& e:entries)
        if(e.row<0 || e.row>=size || e.column<0 || e.column>=size)
            throw std::out_of_range("sparse_determinant: invalid entry");
    if(size==0)return T(1);
    if(entries.empty())return T(0);
    std::mt19937_64 rng(seed);
    const auto random_nonzero=[&](){return T(static_cast<long long>(rng()%(T::get_mod()-1)+1));};
    std::vector<T> diagonal(size),projection(size),state(size),next(size),sequence(2*size);
    for(int attempt=0;attempt<2;++attempt){
        T diagonal_product(1);
        for(int i=0;i<size;++i){
            diagonal[i]=random_nonzero();projection[i]=random_nonzero();state[i]=random_nonzero();
            diagonal_product*=diagonal[i];
        }
        for(int step=0;step<2*size;++step){
            T dot{};for(int i=0;i<size;++i)dot+=projection[i]*state[i];
            sequence[step]=dot;
            if(step+1==2*size)break;
            std::fill(next.begin(),next.end(),T{});
            for(const auto& e:entries)next[e.row]+=e.value*state[e.column];
            for(int i=0;i<size;++i)next[i]*=diagonal[i];
            state.swap(next);
        }
        const auto recurrence=berlekamp_massey<T,MAX_SIZE>(sequence);
        if(recurrence.degree()==size){
            T result=recurrence.coefficient(size-1)/diagonal_product;
            if((size&1)==0)result=-result;
            return result;
        }
    }
    return T{};
}

#endif  // CPPLIB_SRC_ALGORITHM_MATH_LINEAR_ALGEBRA_ELIMINATION_SPARSE_DETERMINANT_HPP_INCLUDED
