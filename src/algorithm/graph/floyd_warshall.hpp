#pragma once

#include <algorithm>
#include <stdexcept>
#include <vector>

template<class T>
struct FloydWarshallResult{
    std::vector<std::vector<T>> dist;
    std::vector<std::vector<int>> next;
    std::vector<char> negative;
    bool has_negative_cycle = false;

    std::vector<int> path(int from, int to) const{
        const int n = static_cast<int>(dist.size());
        if(from < 0 || n <= from || to < 0 || n <= to)[[unlikely]]{
            throw std::runtime_error("library assertion fault: range violation (FloydWarshallResult::path).");
        }
        if(next[static_cast<std::size_t>(from)][static_cast<std::size_t>(to)] == -1) return {};
        std::vector<int> result{from};
        while(from != to){
            from = next[static_cast<std::size_t>(from)][static_cast<std::size_t>(to)];
            if(from == -1) return {};
            result.push_back(from);
        }
        return result;
    }
};

template<class T>
FloydWarshallResult<T> floyd_warshall(std::vector<std::vector<T>> dist, T inf){
    const int n = static_cast<int>(dist.size());
    for(const auto& row: dist){
        if(static_cast<int>(row.size()) != n)[[unlikely]]{
            throw std::runtime_error("library assertion fault: matrix must be square (floyd_warshall).");
        }
    }

    FloydWarshallResult<T> result;
    result.dist = std::move(dist);
    result.next.assign(static_cast<std::size_t>(n), std::vector<int>(static_cast<std::size_t>(n), -1));
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            if(result.dist[static_cast<std::size_t>(i)][static_cast<std::size_t>(j)] != inf){
                result.next[static_cast<std::size_t>(i)][static_cast<std::size_t>(j)] = j;
            }
        }
        if(T(0) < result.dist[static_cast<std::size_t>(i)][static_cast<std::size_t>(i)]){
            result.dist[static_cast<std::size_t>(i)][static_cast<std::size_t>(i)] = T(0);
        }
        result.next[static_cast<std::size_t>(i)][static_cast<std::size_t>(i)] = i;
    }

    for(int k = 0; k < n; k++){
        for(int i = 0; i < n; i++){
            if(result.dist[static_cast<std::size_t>(i)][static_cast<std::size_t>(k)] == inf) continue;
            for(int j = 0; j < n; j++){
                if(result.dist[static_cast<std::size_t>(k)][static_cast<std::size_t>(j)] == inf) continue;
                T nd = result.dist[static_cast<std::size_t>(i)][static_cast<std::size_t>(k)] +
                       result.dist[static_cast<std::size_t>(k)][static_cast<std::size_t>(j)];
                if(nd < result.dist[static_cast<std::size_t>(i)][static_cast<std::size_t>(j)]){
                    result.dist[static_cast<std::size_t>(i)][static_cast<std::size_t>(j)] = nd;
                    result.next[static_cast<std::size_t>(i)][static_cast<std::size_t>(j)] =
                        result.next[static_cast<std::size_t>(i)][static_cast<std::size_t>(k)];
                }
            }
        }
    }

    result.negative.assign(static_cast<std::size_t>(n), 0);
    for(int v = 0; v < n; v++){
        if(result.dist[static_cast<std::size_t>(v)][static_cast<std::size_t>(v)] < T(0)){
            result.negative[static_cast<std::size_t>(v)] = 1;
            result.has_negative_cycle = true;
        }
    }
    return result;
}
