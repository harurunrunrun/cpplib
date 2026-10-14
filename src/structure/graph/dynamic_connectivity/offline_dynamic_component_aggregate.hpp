#ifndef CPPLIB_SRC_STRUCTURE_GRAPH_DYNAMIC_CONNECTIVITY_OFFLINE_DYNAMIC_COMPONENT_AGGREGATE_HPP_INCLUDED
#define CPPLIB_SRC_STRUCTURE_GRAPH_DYNAMIC_CONNECTIVITY_OFFLINE_DYNAMIC_COMPONENT_AGGREGATE_HPP_INCLUDED

#include <algorithm>
#include <map>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

template<auto Monoid>struct DynamicComponentQuery{
    using S=typename std::remove_cvref_t<decltype(Monoid)>::S;
    int type,first,second=-1;
    S value=Monoid.e();
};

template<auto Monoid>
std::vector<typename DynamicComponentQuery<Monoid>::S> offline_dynamic_component_aggregate(
    const std::vector<typename DynamicComponentQuery<Monoid>::S>& initial,
    const std::vector<DynamicComponentQuery<Monoid>>& queries
){
    using S=typename DynamicComponentQuery<Monoid>::S;
    const int n=static_cast<int>(initial.size()),q=static_cast<int>(queries.size());
    if(q==0)return {};
    int base=1;while(base<q)base*=2;
    struct Event{int first,second;S value;};
    std::vector<std::vector<Event>> segment(2*base);
    const auto add_interval=[&](int left,int right,Event event){
        for(left+=base,right+=base;left<right;left/=2,right/=2){
            if(left&1)segment[left++].push_back(event);
            if(right&1)segment[--right].push_back(event);
        }
    };
    std::map<std::pair<int,int>,std::vector<int>> active;
    for(int t=0;t<q;++t){
        const auto& query=queries[t];
        if(query.type<0 || query.type>3 || query.first<0 || query.first>=n)
            throw std::invalid_argument("offline_dynamic_component_aggregate: invalid query");
        if(query.type<=1){
            if(query.second<0 || query.second>=n)
                throw std::out_of_range("offline_dynamic_component_aggregate: invalid endpoint");
            auto key=std::minmax(query.first,query.second);
            auto& starts=active[{key.first,key.second}];
            if(query.type==0)starts.push_back(t);
            else{
                if(starts.empty())throw std::invalid_argument("offline_dynamic_component_aggregate: absent edge removal");
                add_interval(starts.back(),t,{key.first,key.second,Monoid.e()});starts.pop_back();
            }
        }else if(query.type==2)add_interval(t,q,{query.first,-1,query.value});
    }
    for(const auto& [edge,starts]:active)for(int start:starts)
        add_interval(start,q,{edge.first,edge.second,Monoid.e()});
    std::vector<int> parent(n,-1);std::vector<S> aggregate=initial,answer;
    const auto leader=[&](int vertex){while(parent[vertex]>=0)vertex=parent[vertex];return vertex;};
    struct Change{int first,second,parent_first,parent_second;S value;};
    std::vector<Change> history;
    const auto dfs=[&](auto&& self,int node)->void{
        const std::size_t snapshot=history.size();
        for(const auto& event:segment[node]){
            int first=leader(event.first);
            if(event.second<0){
                history.push_back({first,-1,0,0,aggregate[first]});aggregate[first]=Monoid.op(aggregate[first],event.value);
            }else{
                int second=leader(event.second);if(first==second)continue;
                if(parent[first]>parent[second])std::swap(first,second);
                history.push_back({first,second,parent[first],parent[second],aggregate[first]});
                parent[first]+=parent[second];parent[second]=first;aggregate[first]=Monoid.op(aggregate[first],aggregate[second]);
            }
        }
        if(node>=base){
            const int t=node-base;if(t<q && queries[t].type==3)answer.push_back(aggregate[leader(queries[t].first)]);
        }else{self(self,node*2);self(self,node*2+1);}
        while(history.size()>snapshot){
            const auto change=history.back();history.pop_back();aggregate[change.first]=change.value;
            if(change.second>=0){parent[change.first]=change.parent_first;parent[change.second]=change.parent_second;}
        }
    };
    dfs(dfs,1);return answer;
}

#endif  // CPPLIB_SRC_STRUCTURE_GRAPH_DYNAMIC_CONNECTIVITY_OFFLINE_DYNAMIC_COMPONENT_AGGREGATE_HPP_INCLUDED
