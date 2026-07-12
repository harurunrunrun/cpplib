#pragma once

#include <algorithm>
#include <array>
#include <map>
#include <stdexcept>
#include <utility>
#include <vector>

template<int MAX_SIZE>
struct OnlineDynamicConnectivity{
    static_assert(MAX_SIZE > 0);

private:
    int _n = 0;
    int group_count = 0;
    int visit_id = 0;
    std::array<int, MAX_SIZE> component_id{};
    std::array<int, MAX_SIZE> visited{};
    std::array<std::vector<int>, MAX_SIZE> component_vertices;
    std::array<std::vector<int>, MAX_SIZE> graph;
    std::vector<int> free_component_ids;
    std::map<std::pair<int, int>, int> edge_count;

    void check_vertex(int v, const char* message) const{
        if(v < 0 || _n <= v)[[unlikely]] throw std::runtime_error(message);
    }
    std::pair<int, int> normalize(int u, int v) const{
        check_vertex(u, "library assertion fault: range violation (edge).");
        check_vertex(v, "library assertion fault: range violation (edge).");
        if(v < u) std::swap(u, v);
        return {u, v};
    }
    void add_neighbor(int u, int v){
        if(u == v) return;
        graph[u].push_back(v);
        graph[v].push_back(u);
    }
    void erase_neighbor(int u, int v){
        if(u == v) return;
        auto it_u = std::find(graph[u].begin(), graph[u].end(), v);
        auto it_v = std::find(graph[v].begin(), graph[v].end(), u);
        if(it_u != graph[u].end()){
            *it_u = graph[u].back();
            graph[u].pop_back();
        }
        if(it_v != graph[v].end()){
            *it_v = graph[v].back();
            graph[v].pop_back();
        }
    }
    void merge_components(int a, int b){
        a = component_id[a];
        b = component_id[b];
        if(a == b) return;
        if(component_vertices[a].size() < component_vertices[b].size()) std::swap(a, b);
        for(int v: component_vertices[b]){
            component_id[v] = a;
            component_vertices[a].push_back(v);
        }
        component_vertices[b].clear();
        free_component_ids.push_back(b);
        group_count--;
    }
    std::vector<int> reachable_vertices(int s){
        visit_id++;
        std::vector<int> result;
        result.reserve(component_vertices[component_id[s]].size());
        std::vector<int> stack = {s};
        visited[s] = visit_id;
        while(!stack.empty()){
            int v = stack.back();
            stack.pop_back();
            result.push_back(v);
            for(int to: graph[v]){
                if(visited[to] != visit_id){
                    visited[to] = visit_id;
                    stack.push_back(to);
                }
            }
        }
        return result;
    }
    void split_if_needed(int u, int v){
        int old_id = component_id[u];
        auto side = reachable_vertices(u);
        if(visited[v] == visit_id) return;

        if(free_component_ids.empty())[[unlikely]]{
            throw std::runtime_error("library assertion fault: no free component id (split).");
        }
        int new_id = free_component_ids.back();
        free_component_ids.pop_back();

        auto& all = component_vertices[old_id];
        if(side.size() * 2 <= all.size()){
            for(int x: side) component_id[x] = new_id;
            std::vector<int> remain;
            remain.reserve(all.size() - side.size());
            for(int x: all){
                if(visited[x] != visit_id) remain.push_back(x);
            }
            component_vertices[new_id] = std::move(side);
            all = std::move(remain);
        }else{
            std::vector<int> other;
            other.reserve(all.size() - side.size());
            for(int x: all){
                if(visited[x] != visit_id){
                    component_id[x] = new_id;
                    other.push_back(x);
                }
            }
            all = std::move(side);
            component_vertices[new_id] = std::move(other);
        }
        group_count++;
    }

public:
    explicit OnlineDynamicConnectivity(int n = MAX_SIZE): _n(n), group_count(n){
        if(n < 0 || MAX_SIZE < n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (constructor).");
        }
        for(int v = 0; v < _n; v++){
            component_id[v] = v;
            visited[v] = 0;
            component_vertices[v].push_back(v);
        }
        for(int v = _n; v < MAX_SIZE; v++) visited[v] = 0;
    }

    int size() const{ return _n; }
    int groups() const{ return group_count; }

    bool same(int u, int v) const{
        check_vertex(u, "library assertion fault: range violation (same).");
        check_vertex(v, "library assertion fault: range violation (same).");
        return component_id[u] == component_id[v];
    }
    int component_size(int v) const{
        check_vertex(v, "library assertion fault: range violation (component_size).");
        return static_cast<int>(component_vertices[component_id[v]].size());
    }
    int edge_multiplicity(int u, int v) const{
        auto edge = normalize(u, v);
        auto it = edge_count.find(edge);
        return it == edge_count.end() ? 0 : it->second;
    }

    bool add_edge(int u, int v){
        auto edge = normalize(u, v);
        int& count = edge_count[edge];
        count++;
        if(count > 1) return false;
        add_neighbor(edge.first, edge.second);
        merge_components(edge.first, edge.second);
        return true;
    }
    bool erase_edge(int u, int v){
        auto edge = normalize(u, v);
        auto it = edge_count.find(edge);
        if(it == edge_count.end() || it->second == 0) return false;
        it->second--;
        if(it->second > 0) return true;
        edge_count.erase(it);
        erase_neighbor(edge.first, edge.second);
        if(edge.first != edge.second && component_id[edge.first] == component_id[edge.second]){
            split_if_needed(edge.first, edge.second);
        }
        return true;
    }

    bool link(int u, int v){ return add_edge(u, v); }
    bool cut(int u, int v){ return erase_edge(u, v); }
};
