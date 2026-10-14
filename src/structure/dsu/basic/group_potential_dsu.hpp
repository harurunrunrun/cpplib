#ifndef CPPLIB_SRC_STRUCTURE_DSU_BASIC_GROUP_POTENTIAL_DSU_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_DSU_BASIC_GROUP_POTENTIAL_DSU_HPP_INCLUDED

#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

// Group supplies value_type, identity(), multiply(a,b), inverse(a).
// Potentials satisfy A[v] = A[root] * weight(v), including noncommutative groups.
template<class Group>
class GroupPotentialDSU{
public:
    using value_type = typename Group::value_type;
private:
    std::vector<int> parent;
    std::vector<value_type> potential;
    void check(int v) const{
        if(v < 0 || v >= size()) throw std::out_of_range("group potential vertex");
    }
public:
    explicit GroupPotentialDSU(int n): parent(n < 0 ? 0 : n, -1),
        potential(n < 0 ? 0 : n, Group::identity()){
        if(n < 0) throw std::invalid_argument("negative group potential size");
    }
    int size() const{ return static_cast<int>(parent.size()); }
    int leader(int v){
        check(v);
        if(parent[v] < 0) return v;
        const int p = parent[v];
        const int root = leader(p);
        potential[v] = Group::multiply(potential[p], potential[v]);
        return parent[v] = root;
    }
    value_type weight(int v){ leader(v); return potential[v]; }
    std::optional<value_type> difference(int u, int v){
        if(leader(u) != leader(v)) return std::nullopt;
        return Group::multiply(Group::inverse(potential[v]), potential[u]);
    }
    // Impose A[u] = A[v] * relation. Reject an inconsistent relation.
    bool merge(int u, int v, const value_type& relation){
        int ru = leader(u), rv = leader(v);
        value_type delta = Group::multiply(
            Group::multiply(potential[v], relation), Group::inverse(potential[u]));
        if(ru == rv) return delta == Group::identity();
        if(parent[ru] < parent[rv]){
            std::swap(ru, rv);
            delta = Group::inverse(delta);
        }
        parent[rv] += parent[ru];
        parent[ru] = rv;
        potential[ru] = std::move(delta);
        return true;
    }
};

template<class Group>
class RollbackGroupPotentialDSU{
public:
    using value_type=typename Group::value_type;
private:
    struct Change{int child,root,child_size,root_size;};
    std::vector<int> parent;
    std::vector<value_type> potential;
    std::vector<Change> history;
    std::pair<int,value_type> locate(int v) const{
        if(v<0||v>=size())throw std::out_of_range("rollback group potential vertex");
        value_type w=Group::identity();
        while(parent[v]>=0){w=Group::multiply(potential[v],w);v=parent[v];}
        return {v,w};
    }
public:
    explicit RollbackGroupPotentialDSU(int n):parent(n<0?0:n,-1),potential(n<0?0:n,Group::identity()){
        if(n<0)throw std::invalid_argument("negative rollback group potential size");
    }
    int size() const{return static_cast<int>(parent.size());}
    std::size_t snapshot() const{return history.size();}
    std::optional<value_type> difference(int u,int v) const{
        auto[ru,wu]=locate(u);auto[rv,wv]=locate(v);
        if(ru!=rv)return std::nullopt;
        return Group::multiply(Group::inverse(wv),wu);
    }
    bool merge(int u,int v,const value_type& relation){
        auto[ru,wu]=locate(u);auto[rv,wv]=locate(v);
        auto delta=Group::multiply(Group::multiply(wv,relation),Group::inverse(wu));
        if(ru==rv)return delta==Group::identity();
        if(parent[ru]<parent[rv]){std::swap(ru,rv);delta=Group::inverse(delta);}
        history.push_back({ru,rv,parent[ru],parent[rv]});
        parent[rv]+=parent[ru];parent[ru]=rv;potential[ru]=std::move(delta);return true;
    }
    void rollback(std::size_t snapshot){
        if(snapshot>history.size())throw std::out_of_range("rollback group potential snapshot");
        while(history.size()>snapshot){
            auto c=history.back();history.pop_back();parent[c.child]=c.child_size;parent[c.root]=c.root_size;potential[c.child]=Group::identity();
        }
    }
};

#endif  // CPPLIB_SRC_STRUCTURE_DSU_BASIC_GROUP_POTENTIAL_DSU_HPP_INCLUDED
