#ifndef CPPLIB_SRC_ALGORITHM_MATCHING_BIPARTITE_BIPARTITE_MATCHING_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_MATCHING_BIPARTITE_BIPARTITE_MATCHING_HPP_INCLUDED

#include <queue>
#include <stdexcept>
#include <vector>

struct BipartiteMatchingResult{
    int size;
    std::vector<int> left_match;
    std::vector<int> right_match;
};

struct BipartiteMatching{
    int left_size;
    int right_size;
    std::vector<std::vector<int>> graph;
    std::vector<int> left_match;
    std::vector<int> right_match;
    std::vector<int> dist;

    BipartiteMatching(int left_size_, int right_size_)
        : left_size(left_size_ < 0
            ? throw std::runtime_error(
                "library assertion fault: range violation (BipartiteMatching)."
            )
            : left_size_),
          right_size(right_size_ < 0
            ? throw std::runtime_error(
                "library assertion fault: range violation (BipartiteMatching)."
            )
            : right_size_),
          graph(static_cast<std::size_t>(left_size)),
          left_match(static_cast<std::size_t>(left_size), -1),
          right_match(static_cast<std::size_t>(right_size), -1),
          dist(static_cast<std::size_t>(left_size), -1){}

    void add_edge(int left, int right){
        if(left < 0 || left_size <= left || right < 0 || right_size <= right)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (BipartiteMatching::add_edge).");
        }
        graph[static_cast<std::size_t>(left)].push_back(right);
    }

    bool bfs(){
        std::queue<int> que;
        bool found = false;
        for(int v = 0; v < left_size; v++){
            if(left_match[static_cast<std::size_t>(v)] == -1){
                dist[static_cast<std::size_t>(v)] = 0;
                que.push(v);
            }else{
                dist[static_cast<std::size_t>(v)] = -1;
            }
        }
        while(!que.empty()){
            int v = que.front();
            que.pop();
            for(int to: graph[static_cast<std::size_t>(v)]){
                int next = right_match[static_cast<std::size_t>(to)];
                if(next == -1){
                    found = true;
                }else if(dist[static_cast<std::size_t>(next)] == -1){
                    dist[static_cast<std::size_t>(next)] = dist[static_cast<std::size_t>(v)] + 1;
                    que.push(next);
                }
            }
        }
        return found;
    }

    bool dfs(int v){
        for(int to: graph[static_cast<std::size_t>(v)]){
            int next = right_match[static_cast<std::size_t>(to)];
            if(next == -1 || (dist[static_cast<std::size_t>(next)] == dist[static_cast<std::size_t>(v)] + 1 && dfs(next))){
                left_match[static_cast<std::size_t>(v)] = to;
                right_match[static_cast<std::size_t>(to)] = v;
                return true;
            }
        }
        dist[static_cast<std::size_t>(v)] = -1;
        return false;
    }

    BipartiteMatchingResult solve(){
        int result = 0;
        for(int left = 0; left < left_size; ++left){
            result += left_match[static_cast<std::size_t>(left)] != -1;
        }
        while(bfs()){
            for(int v = 0; v < left_size; v++){
                if(left_match[static_cast<std::size_t>(v)] == -1 && dfs(v)){
                    result++;
                }
            }
        }
        return {result, left_match, right_match};
    }
};

#endif  // CPPLIB_SRC_ALGORITHM_MATCHING_BIPARTITE_BIPARTITE_MATCHING_HPP_INCLUDED
