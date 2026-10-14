#ifndef CPPLIB_SRC_ALGORITHM_TREE_CONSTRUCTION_PERMUTATION_INTERVAL_TREE_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_TREE_CONSTRUCTION_PERMUTATION_INTERVAL_TREE_HPP_INCLUDED

#include <algorithm>
#include <stdexcept>
#include <vector>

class PermutationIntervalTree{
public:
    struct Node{
        int left, right, minimum, maximum;
        bool linear = true;
        std::vector<int> children;
    };
private:
    std::vector<Node> nodes_;
    int root_ = -1;
    std::vector<int> leaf;
    std::vector<std::vector<int>> ancestor;
    struct RangeAddMax{
        int n;
        std::vector<int> value, lazy;
        explicit RangeAddMax(int size): n(size), value(4 * size, -1'000'000'000), lazy(4 * size){}
        void add(int v, int l, int r, int ql, int qr, int x){
            if(qr <= l || r <= ql) return;
            if(ql <= l && r <= qr){ value[v] += x; lazy[v] += x; return; }
            int m = (l + r) / 2;
            add(2*v, l, m, ql, qr, x); add(2*v+1, m, r, ql, qr, x);
            value[v] = lazy[v] + std::max(value[2*v], value[2*v+1]);
        }
        int max(int v, int l, int r, int ql, int qr){
            if(qr <= l || r <= ql) return -1'000'000'000;
            if(ql <= l && r <= qr) return value[v];
            int m = (l + r) / 2;
            return lazy[v] + std::max(max(2*v,l,m,ql,qr),max(2*v+1,m,r,ql,qr));
        }
        void add(int l, int r, int x){ if(l < r) add(1, 0, n, l, r, x); }
        int prefix_max(int r){ return r ? max(1, 0, n, 0, r) : -1'000'000'000; }
    };
    static bool adjacent(const Node& a, const Node& b){ return a.maximum == b.minimum || b.maximum == a.minimum; }
    void absorb(int v, int child){
        nodes_[v].left = std::min(nodes_[v].left, nodes_[child].left);
        nodes_[v].right = std::max(nodes_[v].right, nodes_[child].right);
        nodes_[v].minimum = std::min(nodes_[v].minimum, nodes_[child].minimum);
        nodes_[v].maximum = std::max(nodes_[v].maximum, nodes_[child].maximum);
        nodes_[v].children.push_back(child);
    }
public:
    explicit PermutationIntervalTree(const std::vector<int>& permutation){
        const int n = static_cast<int>(permutation.size());
        if(n == 0) return;
        std::vector<bool> used(n);
        for(int x: permutation){ if(x < 0 || x >= n || used[x]) throw std::invalid_argument("interval tree requires a permutation"); used[x] = true; }
        nodes_.reserve(2 * n);
        leaf.resize(n);
        RangeAddMax score(n);
        std::vector<int> minimum_stack, maximum_stack, stack;
        for(int i = 0; i < n; ++i){
            while(!minimum_stack.empty() && permutation[minimum_stack.back()] > permutation[i]){
                int p = minimum_stack.back(); minimum_stack.pop_back();
                int l = minimum_stack.empty() ? 0 : minimum_stack.back() + 1;
                score.add(l, p + 1, permutation[i] - permutation[p]);
            }
            minimum_stack.push_back(i);
            while(!maximum_stack.empty() && permutation[maximum_stack.back()] < permutation[i]){
                int p = maximum_stack.back(); maximum_stack.pop_back();
                int l = maximum_stack.empty() ? 0 : maximum_stack.back() + 1;
                score.add(l, p + 1, permutation[p] - permutation[i]);
            }
            maximum_stack.push_back(i);
            score.add(0, i, 1);
            int current = static_cast<int>(nodes_.size());
            leaf[i] = current;
            nodes_.push_back({i, i + 1, permutation[i], permutation[i] + 1, true, {}});
            while(true){
                if(!stack.empty()){
                    int top = stack.back();
                    if(nodes_[top].linear && !nodes_[top].children.empty()
                        && adjacent(nodes_[nodes_[top].children.back()], nodes_[current])){
                        absorb(top, current); current = top; stack.pop_back(); continue;
                    }
                    if(adjacent(nodes_[top], nodes_[current])){
                        int join = static_cast<int>(nodes_.size());
                        nodes_.push_back({nodes_[top].left, nodes_[current].right,
                                          std::min(nodes_[top].minimum,nodes_[current].minimum),
                                          std::max(nodes_[top].maximum,nodes_[current].maximum), true, {top,current}});
                        stack.pop_back(); current = join; continue;
                    }
                }
                stack.push_back(current);
                if(stack.size() == 1 || score.prefix_max(nodes_[current].left) != 0) break;
                int prime = static_cast<int>(nodes_.size());
                const auto base = nodes_[current];
                nodes_.push_back({base.left, base.right, base.minimum, base.maximum, false, {}});
                absorb(prime, stack.back()); stack.pop_back();
                do{ absorb(prime, stack.back()); stack.pop_back(); }
                while(nodes_[prime].right - nodes_[prime].left != nodes_[prime].maximum - nodes_[prime].minimum);
                std::reverse(nodes_[prime].children.begin(), nodes_[prime].children.end());
                current = prime;
            }
            score.add(i, i + 1, 1'000'000'000);
        }
        if(stack.size() != 1) throw std::logic_error("interval tree construction");
        root_ = stack.back();
        std::vector<int> parent(nodes_.size(), -1);
        for(int v = 0; v < static_cast<int>(nodes_.size()); ++v)
            for(int c: nodes_[v].children) parent[c] = v;
        ancestor.push_back(std::move(parent));
        for(std::size_t span = 1; span < nodes_.size(); span <<= 1){
            std::vector<int> next(nodes_.size(), -1);
            for(int v = 0; v < static_cast<int>(nodes_.size()); ++v){
                int p = ancestor.back()[v];
                if(p != -1) next[v] = ancestor.back()[p];
            }
            ancestor.push_back(std::move(next));
        }
    }
    int root() const{ return root_; }
    const std::vector<Node>& nodes() const{ return nodes_; }
    // Node representing the smallest strong interval containing [l,r).
    int smallest_container(int l, int r) const{
        if(l < 0 || l >= r || r > static_cast<int>(leaf.size())) throw std::out_of_range("interval tree container query");
        int v = leaf[l];
        if(nodes_[v].right >= r) return v;
        for(auto it = ancestor.rbegin(); it != ancestor.rend(); ++it){
            int p = (*it)[v];
            if(p != -1 && nodes_[p].right < r) v = p;
        }
        return ancestor[0][v];
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_TREE_CONSTRUCTION_PERMUTATION_INTERVAL_TREE_HPP_INCLUDED
