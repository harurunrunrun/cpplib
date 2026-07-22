#ifndef CPPLIB_SRC_ALGORITHM_MATCHING_GENERAL_GENERAL_MATCHING_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATCHING_GENERAL_GENERAL_MATCHING_HPP_INCLUDED

#include <algorithm>
#include <queue>
#include <stdexcept>
#include <vector>

struct GeneralMatchingResult{
    int size;
    std::vector<int> match;
};

struct GeneralMatching{
    int n;
    std::vector<std::vector<int>> graph;
    std::vector<int> match;
    std::vector<int> parent;
    std::vector<int> base;
    std::vector<char> used;
    std::vector<char> blossom;
    std::queue<int> que;

    explicit GeneralMatching(int n_)
        : n(n_ < 0
            ? throw std::runtime_error(
                "library assertion fault: range violation (GeneralMatching)."
            )
            : n_),
          graph(static_cast<std::size_t>(n)),
          match(static_cast<std::size_t>(n), -1),
          parent(static_cast<std::size_t>(n)),
          base(static_cast<std::size_t>(n)),
          used(static_cast<std::size_t>(n)),
          blossom(static_cast<std::size_t>(n)){}

    void add_edge(int u, int v){
        if(u < 0 || n <= u || v < 0 || n <= v)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (GeneralMatching::add_edge).");
        }
        if(u == v) return;
        graph[static_cast<std::size_t>(u)].push_back(v);
        graph[static_cast<std::size_t>(v)].push_back(u);
    }

    int lca(int a, int b){
        std::vector<char> seen(static_cast<std::size_t>(n), 0);
        while(true){
            a = base[static_cast<std::size_t>(a)];
            seen[static_cast<std::size_t>(a)] = 1;
            if(match[static_cast<std::size_t>(a)] == -1) break;
            a = parent[static_cast<std::size_t>(match[static_cast<std::size_t>(a)])];
        }
        while(true){
            b = base[static_cast<std::size_t>(b)];
            if(seen[static_cast<std::size_t>(b)]) return b;
            b = parent[static_cast<std::size_t>(match[static_cast<std::size_t>(b)])];
        }
    }

    void mark_path(int v, int b, int child){
        while(base[static_cast<std::size_t>(v)] != b){
            blossom[static_cast<std::size_t>(base[static_cast<std::size_t>(v)])] = 1;
            blossom[static_cast<std::size_t>(base[static_cast<std::size_t>(match[static_cast<std::size_t>(v)])])] = 1;
            parent[static_cast<std::size_t>(v)] = child;
            child = match[static_cast<std::size_t>(v)];
            v = parent[static_cast<std::size_t>(match[static_cast<std::size_t>(v)])];
        }
    }

    int find_path(int root){
        std::fill(used.begin(), used.end(), 0);
        std::fill(parent.begin(), parent.end(), -1);
        for(int i = 0; i < n; i++) base[static_cast<std::size_t>(i)] = i;
        while(!que.empty()) que.pop();
        que.push(root);
        used[static_cast<std::size_t>(root)] = 1;
        while(!que.empty()){
            int v = que.front();
            que.pop();
            for(int to: graph[static_cast<std::size_t>(v)]){
                if(base[static_cast<std::size_t>(v)] == base[static_cast<std::size_t>(to)] ||
                   match[static_cast<std::size_t>(v)] == to){
                    continue;
                }
                if(to == root ||
                   (match[static_cast<std::size_t>(to)] != -1 &&
                    parent[static_cast<std::size_t>(match[static_cast<std::size_t>(to)])] != -1)){
                    int curbase = lca(v, to);
                    std::fill(blossom.begin(), blossom.end(), 0);
                    mark_path(v, curbase, to);
                    mark_path(to, curbase, v);
                    for(int i = 0; i < n; i++){
                        if(blossom[static_cast<std::size_t>(base[static_cast<std::size_t>(i)])]){
                            base[static_cast<std::size_t>(i)] = curbase;
                            if(!used[static_cast<std::size_t>(i)]){
                                used[static_cast<std::size_t>(i)] = 1;
                                que.push(i);
                            }
                        }
                    }
                }else if(parent[static_cast<std::size_t>(to)] == -1){
                    parent[static_cast<std::size_t>(to)] = v;
                    if(match[static_cast<std::size_t>(to)] == -1){
                        return to;
                    }
                    int next = match[static_cast<std::size_t>(to)];
                    used[static_cast<std::size_t>(next)] = 1;
                    que.push(next);
                }
            }
        }
        return -1;
    }

    GeneralMatchingResult solve(){
        int result = 0;
        for(int v = 0; v < n; v++){
            if(match[static_cast<std::size_t>(v)] != -1) result++;
        }
        result /= 2;
        for(int i = 0; i < n; i++){
            if(match[static_cast<std::size_t>(i)] != -1) continue;
            int v = find_path(i);
            if(v == -1) continue;
            while(v != -1){
                int pv = parent[static_cast<std::size_t>(v)];
                int nv = match[static_cast<std::size_t>(pv)];
                match[static_cast<std::size_t>(v)] = pv;
                match[static_cast<std::size_t>(pv)] = v;
                v = nv;
            }
            result++;
        }
        return {result, match};
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_MATCHING_GENERAL_GENERAL_MATCHING_HPP_INCLUDED
