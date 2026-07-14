// competitive-verifier: STANDALONE
#include <cassert>
#include <iostream>
#include <stdexcept>
#include "../../src/structure/segtree/persistent_dynamic_lazysegtree.hpp"

constexpr long long lazy_op(long long a, long long, long long b, long long){ return a + b; }
constexpr long long lazy_e(){ return 0; }
constexpr long long lazy_mapping(long long f, long long x, long long len){ return x + f * len; }
constexpr long long lazy_composition(long long f, long long g){ return f + g; }
constexpr long long lazy_id(){ return 0; }
constexpr Monoid_Act_Len<lazy_op, lazy_e, lazy_mapping, lazy_composition, lazy_id> lazy_monoid;
using Tree = PersistentDynamicLazySegtree<lazy_monoid, 1000000000039LL, 30000, 256>;
struct Affine{ long long a; long long b; };
constexpr long long affine_op(long long x, long long, long long y, long long){ return x + y; }
constexpr long long affine_e(){ return 0; }
constexpr long long affine_mapping(Affine f, long long x, long long len){ return f.a * x + f.b * len; }
constexpr Affine affine_composition(Affine f, Affine g){ return {f.a * g.a, f.a * g.b + f.b}; }
constexpr Affine affine_id(){ return {1, 0}; }
constexpr Monoid_Act_Len<affine_op, affine_e, affine_mapping, affine_composition, affine_id> affine_monoid;

int main(){
    int q;
    if(std::cin >> q){
        long long n; std::cin >> n;
        Tree tree(n);
        while(q--){
            char type; std::cin >> type;
            if(type == 'S' || type == 'D'){
                int version; long long index, value; std::cin >> version >> index >> value;
                std::cout << (type == 'S' ? tree.set(version, index, value) : tree.apply(version, index, value)) << '\n';
            }else if(type == 'R'){
                int version; long long left, right, value; std::cin >> version >> left >> right >> value;
                std::cout << tree.apply(version, left, right, value) << '\n';
            }else if(type == 'F'){
                int version; std::cin >> version; std::cout << tree.fork(version) << '\n';
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
    int v1 = tree.apply(0, 0, n, 1);
    int v2 = tree.set(v1, 999999999999LL, 5);
    int v3 = tree.apply(0, 10, 20, 3);
    assert(tree.all_prod(0) == 0 && tree.all_prod(v1) == n && tree.all_prod(v2) == n + 4);
    assert(tree.get(v1, 999999999999LL) == 1 && tree.get(v2, 999999999999LL) == 5);
    assert(tree.all_prod(v3) == 30 && tree.max_right(v3, 10, [](long long x){ return x <= 15; }) == 15);
    assert(tree.min_left(v3, 20, [](long long x){ return x <= 15; }) == 15);
    assert(tree.size() == n);
    assert(tree.versions() == 4 && tree.latest_version() == v3);
    assert(tree.nodes_used() > 0);
    int nodes_before_fork = tree.nodes_used();
    int v4 = tree.fork(v2);
    assert(v4 == 4 && tree.versions() == 5 && tree.latest_version() == v4);
    assert(tree.nodes_used() == nodes_before_fork);
    assert(tree.all_prod(v4) == tree.all_prod(v2));
    PersistentDynamicLazySegtree<affine_monoid, 8, 100, 8> affine;
    int a1 = affine.set(0, 2, 1);
    int a2 = affine.apply(a1, 0, 8, Affine{2, 3});
    int a3 = affine.apply(a2, 0, 8, Affine{3, 4});
    assert(affine.get(a3, 2) == 19 && affine.get(a3, 0) == 13 && affine.all_prod(a3) == 8 * 13 + 6);    PersistentDynamicLazySegtree<lazy_monoid, 8, 3, 2> small;
    bool thrown = false;
    try{ (void)small.set(0, 2, 1); }catch(const std::runtime_error&){ thrown = true; }
    assert(thrown && small.versions() == 1 && small.nodes_used() == 0 && small.all_prod(0) == 0);
}
