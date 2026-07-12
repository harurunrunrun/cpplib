// competitive-verifier: STANDALONE

#include <iostream>

#include "../../src/structure/segtree/dynamic_lazysegtree.hpp"

using Sum = long long;
using Add = long long;

constexpr Sum dynamic_lazy_op(Sum left, long long, Sum right, long long){
    return left + right;
}
constexpr Sum dynamic_lazy_e(){ return 0; }
constexpr Sum dynamic_lazy_mapping(Add add, Sum value, long long length){
    return value + add * length;
}
constexpr Add dynamic_lazy_composition(Add newer, Add older){
    return newer + older;
}
constexpr Add dynamic_lazy_id(){ return 0; }

constexpr Monoid_Act_Len<
    dynamic_lazy_op,
    dynamic_lazy_e,
    dynamic_lazy_mapping,
    dynamic_lazy_composition,
    dynamic_lazy_id
> dynamic_lazy_monoid;

int main(){
    int n, q;
    if(!(std::cin >> n >> q)) return 0;
    DynamicLazySegtree<dynamic_lazy_monoid, 1'000'000> tree(n);
    while(q--){
        char operation;
        long long l, r, value;
        std::cin >> operation;
        if(operation == 'S'){
            std::cin >> l >> value;
            tree.set(l, value);
        }else if(operation == 'A'){
            std::cin >> l >> r >> value;
            tree.apply(l, r, value);
        }else if(operation == 'P'){
            std::cin >> l >> value;
            tree.apply(l, value);
        }else if(operation == 'Q'){
            std::cin >> l >> r;
            std::cout << tree.prod(l, r) << '\n';
        }else if(operation == 'G'){
            std::cin >> l;
            std::cout << tree.get(l) << '\n';
        }else if(operation == 'R'){
            std::cin >> l >> value;
            std::cout << tree.max_right(l, [](Sum sum, Sum limit){ return sum <= limit; }, value) << '\n';
        }else if(operation == 'L'){
            std::cin >> r >> value;
            std::cout << tree.min_left(r, [](Sum sum, Sum limit){ return sum <= limit; }, value) << '\n';
        }else{
            std::cout << tree.all_prod() << '\n';
        }
    }
}
