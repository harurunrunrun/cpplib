// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>
#include "../../src/structure/segtree/basic/segtree.hpp"
#include "../../src/structure/types/monoid/monoid.hpp"

long long op_sum(long long a, long long b){ return a + b; }
long long e_sum(){ return 0; }
constexpr Monoid<op_sum, e_sum> range_sum;

bool leq_limit(long long x, long long limit){ return x <= limit; }

long long naive_prod(const std::vector<long long>& a, int l, int r){
    return std::accumulate(a.begin() + l, a.begin() + r, 0LL);
}

int naive_max_right(const std::vector<long long>& a, int l, long long limit){
    long long sum = 0;
    int r = l;
    while(r < static_cast<int>(a.size()) && sum + a[static_cast<std::size_t>(r)] <= limit){
        sum += a[static_cast<std::size_t>(r)];
        r++;
    }
    return r;
}

int naive_min_left(const std::vector<long long>& a, int r, long long limit){
    long long sum = 0;
    int l = r;
    while(l > 0 && sum + a[static_cast<std::size_t>(l - 1)] <= limit){
        l--;
        sum += a[static_cast<std::size_t>(l)];
    }
    return l;
}

void check_all(const Segtree<range_sum, 128>& seg, const std::vector<long long>& a){
    assert(seg.all_prod() == naive_prod(a, 0, static_cast<int>(a.size())));
    for(int i = 0; i < static_cast<int>(a.size()); i++) assert(seg.get(i) == a[static_cast<std::size_t>(i)]);
    for(int l = 0; l <= static_cast<int>(a.size()); l += 7){
        for(int r = l; r <= static_cast<int>(a.size()); r += 9){
            assert(seg.prod(l, r) == naive_prod(a, l, r));
        }
    }
    for(int l = 0; l <= static_cast<int>(a.size()); l += 5){
        long long limit = naive_prod(a, l, static_cast<int>(a.size())) / 2;
        assert(seg.max_right(l, leq_limit, limit) == naive_max_right(a, l, limit));
    }
    for(int r = 0; r <= static_cast<int>(a.size()); r += 5){
        long long limit = naive_prod(a, 0, r) / 2;
        assert(seg.min_left(r, leq_limit, limit) == naive_min_left(a, r, limit));
    }
}

void self_test(){
    std::vector<long long> a = {1, 2, 3, 4, 5};
    Segtree<range_sum, 128> seg(a);
    check_all(seg, a);
    seg.set(2, 10);
    a[2] = 10;
    check_all(seg, a);

    std::mt19937 rng(20261117);
    a.assign(80, 0);
    for(long long& x: a) x = static_cast<int>(rng() % 20);
    Segtree<range_sum, 128> random_seg(a);
    for(int step = 0; step < 3000; step++){
        int type = static_cast<int>(rng() % 5);
        if(type < 2){
            int k = static_cast<int>(rng() % a.size());
            long long x = static_cast<int>(rng() % 20);
            random_seg.set(k, x);
            a[static_cast<std::size_t>(k)] = x;
        }else if(type == 2){
            int l = static_cast<int>(rng() % (a.size() + 1));
            int r = static_cast<int>(rng() % (a.size() + 1));
            if(l > r) std::swap(l, r);
            assert(random_seg.prod(l, r) == naive_prod(a, l, r));
        }else{
            check_all(random_seg, a);
        }
    }
    check_all(random_seg, a);

    bool thrown = false;
    try{
        (void)random_seg.get(80);
    }catch(const std::runtime_error&){
        thrown = true;
    }
    assert(thrown);
}

#include "../../src/structure/segtree/basic/dualsegtree.hpp"
#include "../../src/structure/segtree/basic/lazysegtree.hpp"
#include "../../src/structure/segtree/basic/segtree.hpp"
#include "../../src/structure/segtree/basic/segtree_2d.hpp"
#include "../../src/structure/segtree/basic/segtree_beats.hpp"
#include "../../src/structure/segtree/basic/widesegtree.hpp"
#include "../../src/structure/segtree/detail/partially_persistent_bit_range_tree.hpp"
#include "../../src/structure/segtree/detail/persistent_bit_range_tree.hpp"
#include "../../src/structure/segtree/detail/rollback_bit_range_tree.hpp"
#include "../../src/structure/segtree/detail/segtree_beats_node.hpp"
#include "../../src/structure/segtree/detail/wide_segtree_persistence_common.hpp"
#include "../../src/structure/segtree/dynamic/dynamic_bitassign_rangesum.hpp"
#include "../../src/structure/segtree/dynamic/dynamic_bitassign_rangesum_rangeflip.hpp"
#include "../../src/structure/segtree/dynamic/dynamic_bitoverwrite_rangesum.hpp"
#include "../../src/structure/segtree/dynamic/dynamic_lazysegtree.hpp"
#include "../../src/structure/segtree/dynamic/dynamic_segtree.hpp"
#include "../../src/structure/segtree/dynamic/dynamic_segtree_2d.hpp"
#include "../../src/structure/segtree/partially_persistent/partially_persistent_bitassign_rangesum.hpp"
#include "../../src/structure/segtree/partially_persistent/partially_persistent_bitassign_rangesum_rangeflip.hpp"
#include "../../src/structure/segtree/partially_persistent/partially_persistent_bitoverwrite_rangesum.hpp"
#include "../../src/structure/segtree/partially_persistent/partially_persistent_dualsegtree.hpp"
#include "../../src/structure/segtree/partially_persistent/partially_persistent_dynamic_bitassign_rangesum.hpp"
#include "../../src/structure/segtree/partially_persistent/partially_persistent_dynamic_bitassign_rangesum_rangeflip.hpp"
#include "../../src/structure/segtree/partially_persistent/partially_persistent_dynamic_bitoverwrite_rangesum.hpp"
#include "../../src/structure/segtree/partially_persistent/partially_persistent_dynamic_lazysegtree.hpp"
#include "../../src/structure/segtree/partially_persistent/partially_persistent_dynamic_segtree.hpp"
#include "../../src/structure/segtree/partially_persistent/partially_persistent_dynamic_segtree_2d.hpp"
#include "../../src/structure/segtree/partially_persistent/partially_persistent_lazysegtree.hpp"
#include "../../src/structure/segtree/partially_persistent/partially_persistent_segtree.hpp"
#include "../../src/structure/segtree/partially_persistent/partially_persistent_segtree_2d.hpp"
#include "../../src/structure/segtree/partially_persistent/partially_persistent_segtree_beats.hpp"
#include "../../src/structure/segtree/partially_persistent/partially_persistent_widesegtree.hpp"
#include "../../src/structure/segtree/persistent/persistent_bitassign_rangesum.hpp"
#include "../../src/structure/segtree/persistent/persistent_bitassign_rangesum_rangeflip.hpp"
#include "../../src/structure/segtree/persistent/persistent_bitoverwrite_rangesum.hpp"
#include "../../src/structure/segtree/persistent/persistent_dualsegtree.hpp"
#include "../../src/structure/segtree/persistent/persistent_dynamic_bitassign_rangesum.hpp"
#include "../../src/structure/segtree/persistent/persistent_dynamic_bitassign_rangesum_rangeflip.hpp"
#include "../../src/structure/segtree/persistent/persistent_dynamic_bitoverwrite_rangesum.hpp"
#include "../../src/structure/segtree/persistent/persistent_dynamic_lazysegtree.hpp"
#include "../../src/structure/segtree/persistent/persistent_dynamic_segtree.hpp"
#include "../../src/structure/segtree/persistent/persistent_dynamic_segtree_2d.hpp"
#include "../../src/structure/segtree/persistent/persistent_lazysegtree.hpp"
#include "../../src/structure/segtree/persistent/persistent_point_add_range_sum.hpp"
#include "../../src/structure/segtree/persistent/persistent_range_affine_sum.hpp"
#include "../../src/structure/segtree/persistent/persistent_segtree.hpp"
#include "../../src/structure/segtree/persistent/persistent_segtree_2d.hpp"
#include "../../src/structure/segtree/persistent/persistent_segtree_beats.hpp"
#include "../../src/structure/segtree/persistent/persistent_widesegtree.hpp"
#include "../../src/structure/segtree/rollback/rollback_bitassign_rangesum.hpp"
#include "../../src/structure/segtree/rollback/rollback_bitassign_rangesum_rangeflip.hpp"
#include "../../src/structure/segtree/rollback/rollback_bitoverwrite_rangesum.hpp"
#include "../../src/structure/segtree/rollback/rollback_dualsegtree.hpp"
#include "../../src/structure/segtree/rollback/rollback_dynamic_bitassign_rangesum.hpp"
#include "../../src/structure/segtree/rollback/rollback_dynamic_bitassign_rangesum_rangeflip.hpp"
#include "../../src/structure/segtree/rollback/rollback_dynamic_bitoverwrite_rangesum.hpp"
#include "../../src/structure/segtree/rollback/rollback_dynamic_lazysegtree.hpp"
#include "../../src/structure/segtree/rollback/rollback_dynamic_segtree.hpp"
#include "../../src/structure/segtree/rollback/rollback_dynamic_segtree_2d.hpp"
#include "../../src/structure/segtree/rollback/rollback_lazysegtree.hpp"
#include "../../src/structure/segtree/rollback/rollback_segtree.hpp"
#include "../../src/structure/segtree/rollback/rollback_segtree_2d.hpp"
#include "../../src/structure/segtree/rollback/rollback_segtree_beats.hpp"
#include "../../src/structure/segtree/rollback/rollback_widesegtree.hpp"
#include "../../src/structure/segtree/specialized/bitassign_rangesum.hpp"
#include "../../src/structure/segtree/specialized/bitoverwrite_rangesum.hpp"
#include "../../src/structure/segtree/specialized/dynamic_parentheses_sequence.hpp"
#include "../../src/structure/segtree/specialized/dynamic_prime_count_range_assign.hpp"
#include "../../src/structure/segtree/specialized/maximum_subarray_sum_segtree.hpp"
#include "../../src/structure/segtree/specialized/point_set_range_composite.hpp"
#include "../../src/structure/segtree/specialized/point_set_range_composite_large_array.hpp"
#include "../../src/structure/segtree/specialized/range_add_range_sum.hpp"
#include "../../src/structure/segtree/specialized/range_affine_point_get.hpp"
#include "../../src/structure/segtree/specialized/range_affine_range_sum.hpp"
#include "../../src/structure/segtree/specialized/range_affine_range_sum_large_array.hpp"
#include "../../src/structure/segtree/specialized/range_assign_add_sum_squares.hpp"
#include "../../src/structure/segtree/specialized/range_integer_sqrt_sum.hpp"
#include "../../src/structure/segtree/specialized/range_set_range_composite.hpp"
#include "../../src/structure/segtree/specialized/residue_count_range_tree.hpp"
#include "../../src/structure/segtree/specialized/static_two_ranges_maximum_subarray_sum.hpp"
#include "../../src/structure/segtree/specialized/two_maximum_segtree.hpp"
#include "../../src/structure/types/monoid/common_monoids.hpp"

namespace vector_segtree_checks{
inline constexpr AddMonoid<long long> sum{};
inline constexpr AffineSumAggregateMonoidAct<long long> aggregate_add{};
inline constexpr AddSumMonoidAct<long long> add{};
using Agg=SumLengthAggregate<long long>;
std::vector<Agg> aggregates(const std::vector<long long>& a){std::vector<Agg> out;for(auto v:a)out.push_back({v,1});return out;}

template<class Tree> void persistent_point(){
    Tree tree(17); std::vector<std::vector<long long>> states(1,std::vector<long long>(17));
    std::mt19937 rng(12345);
    for(int q=0;q<250;++q){
        int from=rng()%states.size(),at=rng()%17;long long value=rng()%100;
        auto next=states[from];next[at]=value;
        assert(tree.set(from,at,value)==static_cast<int>(states.size()));states.push_back(next);
        int v=rng()%states.size();
        for(int i=0;i<17;++i)assert(tree.get(v,i)==states[v][i]);
        assert(tree.prod(v,0,17)==std::accumulate(states[v].begin(),states[v].end(),0LL));
    }
}
template<class Tree> void partial_point(){
    Tree tree(17);std::vector<std::vector<long long>> states(1,std::vector<long long>(17));
    for(int q=0;q<180;++q){int at=(q*7)%17;auto next=states.back();next[at]=q;assert(tree.set(at,q)==static_cast<int>(states.size()));states.push_back(next);int v=q/2;for(int i=0;i<17;++i)assert(tree.get(v,i)==states[v][i]);}
}
template<class Tree> void rollback_point(){
    Tree tree(17); std::vector<long long>a(17);
    for(int q=0;q<120;++q){int i=q%17;auto snap=tree.snapshot();auto old=a[i];tree.set(i,q);a[i]=q;assert(tree.prod(0,17)==std::accumulate(a.begin(),a.end(),0LL));tree.rollback(snap);a[i]=old;assert(tree.get(i)==old);tree.set(i,q+1);a[i]=q+1;}
}
template<class Tree> void persistent_2d(){
    Tree tree(3,5);int version=0;for(int q=0;q<90;++q){int r=q%3,c=(q*7)%5;auto old=tree.get(version,r,c);int next=tree.set(version,r,c,q);assert(tree.get(version,r,c)==old);assert(tree.get(next,r,c)==q);version=next;}
}
template<class Tree> void partial_2d(){
    Tree tree(3,5);for(int q=0;q<90;++q){int r=q%3,c=(q*7)%5;auto old=tree.get(q,r,c);tree.set(r,c,q);assert(tree.get(q,r,c)==old);assert(tree.get(q+1,r,c)==q);}
}
template<class Tree> void rollback_2d(){
    Tree tree(3,5);for(int q=0;q<90;++q){int r=q%3,c=(q*7)%5;auto old=tree.get(r,c);auto snap=tree.snapshot();tree.set(r,c,q);assert(tree.get(r,c)==q);tree.rollback(snap);assert(tree.get(r,c)==old);tree.set(r,c,q+1);}
}
template<class Tree> void persistent_bit(){
    Tree tree(37);int version=0;for(int q=0;q<100;++q){int next=tree.assign(version,q%37,37,(q&1)!=0);auto old=tree.sum(version,0,37);assert(tree.sum(next,q%37,37)==((q&1)?37-q%37:0));assert(tree.sum(version,0,37)==old);version=next;}assert(tree.sum(0,0,37)==0);
}
template<class Tree> void partial_bit(){
    Tree tree(37);for(int q=0;q<100;++q){tree.assign(q%37,37,(q&1)!=0);assert(tree.sum(q+1,q%37,37)==((q&1)?37-q%37:0));}assert(tree.sum(0,0,37)==0);
}
template<class Tree> void rollback_bit(){
    Tree tree(37);for(int q=0;q<100;++q){int v=tree.snapshot();auto old=tree.sum(0,37);tree.assign(q%37,37,(q&1)!=0);assert(tree.sum(q%37,37)==((q&1)?37-q%37:0));tree.rollback(v);assert(tree.sum(0,37)==old);tree.flip(0,37);}
}
template<class Value> long long scalar(Value value){if constexpr(std::is_arithmetic_v<Value>) return value;else return value.sum;}
template<class Tree> void check_persistent_lazy(Tree& tree){
    std::vector<std::vector<long long>> states(1,std::vector<long long>(17));std::mt19937 rng(76543);
    for(int q=0;q<180;++q){int from=rng()%states.size(),l=rng()%18,r=rng()%18;if(l>r)std::swap(l,r);long long delta=rng()%20;
        typename Tree::T action{};if constexpr(std::is_arithmetic_v<typename Tree::T>)action=delta;else action={1,delta};
        auto next=states[from];for(int i=l;i<r;++i)next[i]+=delta;int version=tree.apply(from,l,r,action);assert(version==static_cast<int>(states.size()));states.push_back(next);
        int v=rng()%states.size();assert(scalar(tree.prod(v,0,17))==std::accumulate(states[v].begin(),states[v].end(),0LL));for(int i=0;i<17;++i)assert(scalar(tree.get(v,i))==states[v][i]);
    }
}
void check(){
 persistent_point<PersistentSegtree<sum>>();
 persistent_point<PersistentDynamicSegtree<sum>>();
 partial_point<PartiallyPersistentSegtree<sum>>();
 partial_point<PartiallyPersistentDynamicSegtree<sum>>();
 rollback_point<RollbackSegtree<sum>>();
 rollback_point<RollbackDynamicSegtree<sum>>();
 persistent_2d<PersistentSegtree2D<sum>>();
 persistent_2d<PersistentDynamicSegtree2D<sum>>();
 partial_2d<PartiallyPersistentSegtree2D<sum>>();
 partial_2d<PartiallyPersistentDynamicSegtree2D<sum>>();
 rollback_2d<RollbackSegtree2D<sum>>();
 rollback_2d<RollbackDynamicSegtree2D<sum>>();
 persistent_bit<PersistentBitAssignRangeSum<>>();
 persistent_bit<PersistentBitAssignRangeSumRangeFlip<>>();
 persistent_bit<PersistentBitOverwriteRangeSum<>>();
 partial_bit<PartiallyPersistentBitAssignRangeSum<>>();
 partial_bit<PartiallyPersistentBitAssignRangeSumRangeFlip<>>();
 partial_bit<PartiallyPersistentBitOverwriteRangeSum<>>();
 rollback_bit<RollbackBitAssignRangeSum<>>();
 rollback_bit<RollbackBitAssignRangeSumRangeFlip<>>();
 rollback_bit<RollbackBitOverwriteRangeSum<>>();


 std::vector<long long>a{1,2,3,4,5};
 Segtree<sum> point(a);assert(point.prod(1,4)==9);point.set(2,7);assert(point.all_prod()==19);
 LazySegtree<aggregate_add> lazy(aggregates(a));lazy.apply(1,4,{1,2});assert(lazy.prod(0,5).sum==21);
 DualSegtree<aggregate_add> dual(aggregates(a));dual.apply(1,4,{1,2});assert(dual.get(2).sum==5);
 SegmentTreeBeats<long long> beats(a);beats.range_chmin(0,5,3);assert(beats.all_sum()==12);
 Segtree2D<sum> matrix(3,5);matrix.set(2,4,9);assert(matrix.prod(0,0,3,5)==9);
 DynamicSegtree<sum> sparse(1000000000000LL);sparse.set(999999999999LL,13);assert(sparse.all_prod()==13);
 DynamicLazySegtree<add> sparse_lazy(1000000000000LL);sparse_lazy.apply(10,20,3);assert(sparse_lazy.prod(0,100)==30);
 DynamicSegtree2D<sum> sparse_matrix(1000000000000LL,1000000000000LL);sparse_matrix.set(77,88,12);assert(sparse_matrix.prod(0,0,100,100)==12);
 Wide_Segtree<> wide(37);wide.add(36,8);wide.set(3,4);assert(wide.prod(0,37)==12);

  PersistentLazySegtree<aggregate_add> pl(aggregates(a));int p1=pl.apply(0,1,4,{1,2});assert(pl.all_prod(p1).sum==21);assert(pl.all_prod(0).sum==15);
  PartiallyPersistentLazySegtree<aggregate_add> ql(aggregates(a));ql.apply(1,4,{1,2});assert(ql.all_prod(1).sum==21);assert(ql.all_prod(0).sum==15);
  RollbackLazySegtree<aggregate_add> rl(aggregates(a));rl.apply(1,4,{1,2});assert(rl.all_prod().sum==21);rl.undo();assert(rl.all_prod().sum==15);
  PersistentDualSegtree<aggregate_add> pd(aggregates(a));int d1=pd.apply(0,1,4,{1,2});assert(pd.get(d1,2).sum==5);assert(pd.get(0,2).sum==3);
  PartiallyPersistentDualSegtree<aggregate_add> qd(aggregates(a));qd.apply(1,4,{1,2});assert(qd.get(1,2).sum==5);assert(qd.get(0,2).sum==3);
  RollbackDualSegtree<aggregate_add> rd(aggregates(a));rd.apply(1,4,{1,2});assert(rd.get(2).sum==5);rd.undo();assert(rd.get(2).sum==3);
  PersistentDynamicLazySegtree<add> pdl(37);int l1=pdl.apply(0,1,30,3);assert(pdl.prod(l1,0,37)==87);assert(pdl.prod(0,0,37)==0);
  PartiallyPersistentDynamicLazySegtree<add> qdl(37);qdl.apply(1,30,3);assert(qdl.prod(1,0,37)==87);assert(qdl.prod(0,0,37)==0);
  RollbackDynamicLazySegtree<add> rdl(37);rdl.apply(1,30,3);assert(rdl.prod(0,37)==87);rdl.undo();assert(rdl.prod(0,37)==0);

  PersistentSegmentTreeBeats<long long> pb(a);int b1=pb.range_chmin(0,0,5,3);assert(pb.range_sum(b1,0,5)==12);assert(pb.range_sum(0,0,5)==15);
  PartiallyPersistentSegmentTreeBeats<long long> qb(a);qb.range_chmin(0,5,3);assert(qb.range_sum(1,0,5)==12);assert(qb.range_sum(0,0,5)==15);
  RollbackSegmentTreeBeats<long long> rb(a);rb.range_chmin(0,5,3);assert(rb.range_sum(0,5)==12);rb.rollback(0);assert(rb.range_sum(0,5)==15);

  PersistentLazySegtree<aggregate_add> branch_dense(aggregates(std::vector<long long>(17)));check_persistent_lazy(branch_dense);
  PersistentDynamicLazySegtree<add> branch_sparse(17);check_persistent_lazy(branch_sparse);
  PersistentSegmentTreeBeats<long long> branch_beats(std::vector<long long>(17));std::vector<std::vector<long long>> beat_states(1,std::vector<long long>(17));std::mt19937 beat_rng(778899);
 for(int q=0;q<200;++q){int from=beat_rng()%beat_states.size(),l=beat_rng()%18,r=beat_rng()%18;if(l>r)std::swap(l,r);long long value=static_cast<int>(beat_rng()%31)-15;auto next=beat_states[from];int version;
  if(q%3==0){version=branch_beats.range_add(from,l,r,value);for(int i=l;i<r;++i)next[i]+=value;}
  else if(q%3==1){version=branch_beats.range_chmin(from,l,r,value);for(int i=l;i<r;++i)next[i]=std::min(next[i],value);}
  else{version=branch_beats.range_chmax(from,l,r,value);for(int i=l;i<r;++i)next[i]=std::max(next[i],value);}
  assert(version==static_cast<int>(beat_states.size()));beat_states.push_back(next);int v=beat_rng()%beat_states.size();assert(branch_beats.range_sum(v,0,17)==std::accumulate(beat_states[v].begin(),beat_states[v].end(),0LL));for(int i=0;i<17;++i)assert(branch_beats.get(v,i)==beat_states[v][i]);
 }

 DynamicParenthesesSequence<> parentheses("()()");assert(parentheses.balanced());parentheses.toggle(0);assert(!parentheses.balanced());parentheses.toggle(0);assert(parentheses.balanced());
 DynamicPrimeCountRangeAssign<> primes(std::vector<int>{2,4,5,9});assert(primes.count_primes(0,4)==2);primes.assign(1,4,7);assert(primes.count_primes(0,4)==4);primes.assign(0,4,-3);assert(primes.count_primes(0,4)==0);
 MaximumSubarraySumSegtree<long long> maximum(a);assert(maximum.maximum_sum(0,5)==15);maximum.set(2,-10);assert(maximum.maximum_sum(0,5)==9);
 TwoMaximumSegtree<long long> two(a);assert(two.sum_of_two_largest(0,5)==9);two.set(4,-1);assert(two.sum_of_two_largest(0,5)==7);
 std::vector<MonoidAffine<long long>> functions{{2,1},{3,4}};
 PointSetRangeComposite<long long> compose(functions);assert(compose.evaluate(0,2,2)==19);
 PointSetRangeCompositeLargeArray<long long> large_compose(1000000000000LL);large_compose.set(10,2,1);large_compose.set(20,3,4);assert(large_compose.evaluate(0,100,2)==19);
 RangeSetRangeComposite<long long> assign_compose(functions);assign_compose.assign(0,2,2,3);assert(assign_compose.evaluate(0,2,1)==13);
 RangeAffinePointGet<long long> affine_get(a);affine_get.apply(1,4,2,1);assert(affine_get.get(2)==7);
 RangeAffineRangeSum<long long> affine_sum(a);affine_sum.apply(1,4,2,1);assert(affine_sum.range_sum(0,5)==27);
 RangeAffineRangeSumLargeArray<long long> large_affine(1000000000000LL);large_affine.apply(10,20,0,3);assert(large_affine.range_sum(0,100)==30);
 RangeAddRangeSum<long long> add_sum(100);add_sum.add(10,20,3);assert(add_sum.sum(0,100)==30);
 RangeAssignAddSumSquares<long long> squares(std::vector<long long>{1,2,3});squares.add(0,3,2);assert(squares.square_sum(0,3)==50);squares.assign(1,3,-1);assert(squares.square_sum(0,3)==11);
 RangeIntegerSqrtSum<> square_root(std::vector<long long>{1,2,16,100});square_root.apply_sqrt(0,4);assert(square_root.sum(0,4)==16);
 ResidueCountRangeTree<int,3> residues(a);assert(residues.count_residue(0,5,0)==1);residues.increment(0,5);assert(residues.count_residue(0,5,0)==2);
 StaticTwoRangesMaximumSubarraySum<long long> endpoint_ranges(a);assert(endpoint_ranges.maximum_sum(0,2,3,5)==15);
  PersistentPointAddRangeSum<long long> point_add(a);int pa=point_add.add(0,2,7);assert(point_add.sum(pa,0,5)==22);assert(point_add.sum(0,0,5)==15);

  PersistentWideSegtree<> pw(37);int w1=pw.set(0,3,8);assert(pw.prod(w1,0,37)==8);
  PartiallyPersistentWideSegtree<> qw(37);qw.set(3,8);assert(qw.prod(1,0,37)==8);
  RollbackWideSegtree<> rw(37);rw.set(3,8);assert(rw.prod(0,37)==8);rw.undo();assert(rw.prod(0,37)==0);
}
}

int main(){
    vector_segtree_checks::check();
    int n, q;
    if(!(std::cin >> n >> q)){
        self_test();
        return 0;
    }
    std::vector<long long> a(static_cast<std::size_t>(n));
    for(auto& x: a) std::cin >> x;
    Segtree<range_sum, 256> seg(a);
    while(q--){
        std::string type;
        std::cin >> type;
        if(type == "SET"){
            int k;
            long long x;
            std::cin >> k >> x;
            seg.set(k, x);
        }else if(type == "GET"){
            int k;
            std::cin >> k;
            std::cout << seg.get(k) << '\n';
        }else if(type == "PROD"){
            int l, r;
            std::cin >> l >> r;
            std::cout << seg.prod(l, r) << '\n';
        }else if(type == "ALL"){
            std::cout << seg.all_prod() << '\n';
        }else if(type == "MR"){
            int l;
            long long limit;
            std::cin >> l >> limit;
            std::cout << seg.max_right(l, leq_limit, limit) << '\n';
        }else if(type == "ML"){
            int r;
            long long limit;
            std::cin >> r >> limit;
            std::cout << seg.min_left(r, leq_limit, limit) << '\n';
        }
    }
}
