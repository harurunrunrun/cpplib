// competitive-verifier: STANDALONE
#include <cassert>
#include <iostream>
#include <stdexcept>
#include "../../src/structure/segtree/partially_persistent_dynamic_lazysegtree.hpp"

constexpr long long partial_lazy_op(long long a, long long, long long b, long long){ return a + b; }
constexpr long long partial_lazy_e(){ return 0; }
constexpr long long partial_lazy_mapping(long long f, long long x, long long len){ return x + f * len; }
constexpr long long partial_lazy_composition(long long f, long long g){ return f + g; }
constexpr long long partial_lazy_id(){ return 0; }
constexpr Monoid_Act_Len<partial_lazy_op, partial_lazy_e, partial_lazy_mapping, partial_lazy_composition, partial_lazy_id> partial_lazy_monoid;
using Tree = PartiallyPersistentDynamicLazySegtree<partial_lazy_monoid, 1000000000039LL, 20000, 256, 60000>;

int main(){
    int q;
    if(std::cin >> q){
        long long n; std::cin >> n;
        Tree tree(n);
        while(q--){
            char type; std::cin >> type;
            if(type == 'S' || type == 'D'){
                long long index, value; std::cin >> index >> value;
                std::cout << (type == 'S' ? tree.set(index, value) : tree.apply(index, value)) << '\n';
            }else if(type == 'R'){
                long long left, right, value; std::cin >> left >> right >> value;
                std::cout << tree.apply(left, right, value) << '\n';
            }else if(type == 'G'){
                int version; long long index; std::cin >> version >> index; std::cout << tree.get(version, index) << '\n';
            }else if(type == 'P'){
                int version; long long left, right; std::cin >> version >> left >> right; std::cout << tree.prod(version, left, right) << '\n';
            }else{
                int version; std::cin >> version; std::cout << tree.all_prod(version) << '\n';
            }
        }
        return 0;
    }
    constexpr long long n = 1000000000039LL;
    Tree tree(n);
    int v1 = tree.apply(0, n, 1);
    int v2 = tree.set(999999999999LL, 5);
    int v3 = tree.apply(10, 20, 3);
    assert(tree.all_prod(0) == 0 && tree.all_prod(v1) == n && tree.all_prod(v2) == n + 4);
    assert(tree.get(v1, 999999999999LL) == 1 && tree.get(v2, 999999999999LL) == 5);
    assert(tree.prod(v3, 10, 20) == 40);
    PartiallyPersistentDynamicLazySegtree<partial_lazy_monoid, 2, 3, 3, 5> limited;
    int only_version = limited.set(0, 6);
    bool thrown = false;
    try{ (void)limited.set(1, 2); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown && limited.latest_version() == only_version && limited.all_prod() == 6);
}
