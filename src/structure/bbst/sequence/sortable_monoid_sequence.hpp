#ifndef CPPLIB_SRC_STRUCTURE_BBST_SEQUENCE_SORTABLE_MONOID_SEQUENCE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_BBST_SEQUENCE_SORTABLE_MONOID_SEQUENCE_HPP_INCLUDED

#include <iterator>
#include <set>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

// Distinct integer keys, arbitrary associative aggregates. Sorted runs are
// represented by mergeable key segment trees; discarded internal nodes recycle.
template<auto Monoid>
class SortableMonoidSequence{
public:
    using S = typename std::remove_cvref_t<decltype(Monoid)>::S;
private:
    struct Node{ int left = -1, right = -1, size = 0; S forward = Monoid.e(), backward = Monoid.e(); };
    int n, key_limit, base = 1;
    std::vector<Node> nodes;
    std::vector<int> free_nodes, roots;
    std::vector<bool> reversed;
    std::set<int> starts;
    std::vector<S> segment;
    int size_of(int v) const{ return v == -1 ? 0 : nodes[v].size; }
    S aggregate(int v, bool rev = false) const{ return v == -1 ? Monoid.e() : rev ? nodes[v].backward : nodes[v].forward; }
    int allocate(){
        if(free_nodes.empty()){ nodes.push_back({}); return static_cast<int>(nodes.size()) - 1; }
        int v = free_nodes.back(); free_nodes.pop_back(); nodes[v] = Node{}; return v;
    }
    void release(int v){
        if(v == -1) return;
        release(nodes[v].left); release(nodes[v].right); free_nodes.push_back(v);
    }
    void pull(int v){
        nodes[v].size = size_of(nodes[v].left) + size_of(nodes[v].right);
        nodes[v].forward = Monoid.op(aggregate(nodes[v].left), aggregate(nodes[v].right));
        nodes[v].backward = Monoid.op(aggregate(nodes[v].right, true), aggregate(nodes[v].left, true));
    }
    int singleton(int l, int r, int key, const S& value){
        int v = allocate();
        if(r - l == 1){ nodes[v].size = 1; nodes[v].forward = nodes[v].backward = value; return v; }
        int m = l + (r-l)/2;
        int child = key < m ? singleton(l,m,key,value) : singleton(m,r,key,value);
        if(key < m) nodes[v].left = child; else nodes[v].right = child;
        pull(v); return v;
    }
    std::pair<int,int> split(int v, int k){
        if(k == 0) return {-1,v};
        if(k == size_of(v)) return {v,-1};
        const int old_left = nodes[v].left, old_right = nodes[v].right;
        int w = allocate();
        if(k <= size_of(old_left)){
            auto [a,b] = split(old_left,k);
            nodes[v].left = a; nodes[v].right = -1; nodes[w].left = b; nodes[w].right = old_right;
        }else{
            auto [a,b] = split(old_right,k-size_of(old_left));
            nodes[v].right = a; nodes[w].right = b;
        }
        pull(v); pull(w); return {v,w};
    }
    int unite(int a, int b){
        if(a == -1) return b;
        if(b == -1) return a;
        if(nodes[a].left == -1 && nodes[a].right == -1) throw std::invalid_argument("duplicate sortable sequence key");
        int l = unite(nodes[a].left,nodes[b].left), r = unite(nodes[a].right,nodes[b].right);
        nodes[a].left = l; nodes[a].right = r; pull(a); free_nodes.push_back(b); return a;
    }
    S key_product(int v,int l,int r,int ql,int qr,bool reverse) const{
        if(v == -1 || qr <= l || r <= ql) return Monoid.e();
        if(ql <= l && r <= qr) return aggregate(v,reverse);
        int m=l+(r-l)/2;
        S a=key_product(nodes[v].left,l,m,ql,qr,reverse);
        S b=key_product(nodes[v].right,m,r,ql,qr,reverse);
        return reverse ? Monoid.op(b,a) : Monoid.op(a,b);
    }
    void segment_set(int p, const S& x){
        segment[p += base] = x;
        while((p >>= 1) != 0) segment[p] = Monoid.op(segment[2*p],segment[2*p+1]);
    }
    void split_at(int p){
        auto it = starts.lower_bound(p); if(*it == p) return;
        int end = *it, begin = *std::prev(it);
        auto [a,b] = split(roots[begin],reversed[begin] ? end-p : p-begin);
        if(reversed[begin]) std::swap(a,b);
        roots[begin] = a; roots[p] = b; reversed[p] = reversed[begin]; starts.insert(p);
        segment_set(begin,aggregate(a,reversed[begin])); segment_set(p,aggregate(b,reversed[p]));
    }
    void check(int l,int r) const{ if(l < 0 || l > r || r > n) throw std::out_of_range("sortable sequence interval"); }
public:
    SortableMonoidSequence(int limit, const std::vector<int>& keys, const std::vector<S>& values):
        n(static_cast<int>(keys.size())),key_limit(limit),roots(n,-1),reversed(n){
        if(limit <= 0 || keys.size() != values.size()) throw std::invalid_argument("sortable sequence size");
        while(base < n) base <<= 1;
        segment.assign(2*base,Monoid.e());
        for(int i = 0; i < n; ++i){
            if(keys[i] < 0 || keys[i] >= limit) throw std::out_of_range("sortable sequence key");
            roots[i] = singleton(0,key_limit,keys[i],values[i]); starts.insert(i); segment[base+i] = values[i];
        }
        starts.insert(n);
        for(int p = base-1;p;--p) segment[p] = Monoid.op(segment[2*p],segment[2*p+1]);
    }
    int size() const{ return n; }
    void set(int position,int key,const S& value){
        if(position < 0 || position >= n || key < 0 || key >= key_limit) throw std::out_of_range("sortable sequence point");
        split_at(position);split_at(position+1);release(roots[position]);
        roots[position] = singleton(0,key_limit,key,value); reversed[position] = false; segment_set(position,value);
    }
    void sort(int l,int r,bool descending=false){
        check(l,r);if(l==r)return;split_at(l);split_at(r);
        auto it = std::next(starts.find(l));
        while(*it < r){
            int p = *it;roots[l] = unite(roots[l],roots[p]);roots[p] = -1;segment_set(p,Monoid.e());it = starts.erase(it);
        }
        reversed[l] = descending;segment_set(l,aggregate(roots[l],descending));
    }
    S prod(int l,int r){
        check(l,r);if(l==r)return Monoid.e();split_at(l);split_at(r);
        S a = Monoid.e(), b = Monoid.e();
        for(l+=base,r+=base;l<r;l>>=1,r>>=1){if(l&1)a=Monoid.op(a,segment[l++]);if(r&1)b=Monoid.op(segment[--r],b);}
        return Monoid.op(a,b);
    }
    S prod_key_range(int l,int r,int key_lower,int key_upper){
        check(l,r);
        if(key_lower < 0 || key_lower > key_upper || key_upper > key_limit) throw std::out_of_range("sortable sequence key interval");
        if(l == r || key_lower == key_upper) return Monoid.e();
        split_at(l);split_at(r);S answer=Monoid.e();
        for(auto it=starts.find(l);*it<r;++it){
            int p=*it;answer=Monoid.op(answer,key_product(roots[p],0,key_limit,key_lower,key_upper,reversed[p]));
        }
        return answer;
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_BBST_SEQUENCE_SORTABLE_MONOID_SEQUENCE_HPP_INCLUDED
