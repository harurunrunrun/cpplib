#pragma once

#include <algorithm>
#include <functional>
#include <limits>
#include <queue>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

template<class T>
struct KthShortestPathEdge{
    int to;
    T cost;
};

template<class T>
struct KthShortestPathResult{
    T cost;
    std::vector<int> vertices;
    std::vector<T> prefix_cost;
};

namespace kth_shortest_path_internal{

template<class T>
KthShortestPathResult<T> shortest_path(
    const std::vector<std::vector<KthShortestPathEdge<T>>>& graph,
    int source,
    int target,
    const std::vector<char>& banned_vertices,
    int banned_from,
    const std::vector<int>& banned_to,
    T inf
){
    const int n = static_cast<int>(graph.size());
    KthShortestPathResult<T> empty;
    if(banned_vertices[static_cast<std::size_t>(source)] ||
       banned_vertices[static_cast<std::size_t>(target)]){
        return empty;
    }

    std::vector<T> dist(static_cast<std::size_t>(n), inf);
    std::vector<int> parent(static_cast<std::size_t>(n), -1);
    std::vector<char> reachable(static_cast<std::size_t>(n), 0);
    using Pair = std::pair<T, int>;
    std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair>> que;
    dist[static_cast<std::size_t>(source)] = T(0);
    reachable[static_cast<std::size_t>(source)] = 1;
    que.push({T(0), source});

    while(!que.empty()){
        auto [d, v] = que.top();
        que.pop();
        if(dist[static_cast<std::size_t>(v)] != d) continue;
        if(v == target) break;
        for(const auto& e: graph[static_cast<std::size_t>(v)]){
            if(banned_vertices[static_cast<std::size_t>(e.to)]) continue;
            if(v == banned_from &&
               std::binary_search(banned_to.begin(), banned_to.end(), e.to)){
                continue;
            }
            T nd = d + e.cost;
            if(!reachable[static_cast<std::size_t>(e.to)] ||
               nd < dist[static_cast<std::size_t>(e.to)]){
                dist[static_cast<std::size_t>(e.to)] = nd;
                parent[static_cast<std::size_t>(e.to)] = v;
                reachable[static_cast<std::size_t>(e.to)] = 1;
                que.push({nd, e.to});
            }
        }
    }

    if(!reachable[static_cast<std::size_t>(target)]) return empty;

    std::vector<int> vertices;
    for(int v = target; v != -1; v = parent[static_cast<std::size_t>(v)]){
        vertices.push_back(v);
    }
    std::reverse(vertices.begin(), vertices.end());

    KthShortestPathResult<T> result;
    result.cost = dist[static_cast<std::size_t>(target)];
    result.vertices = vertices;
    result.prefix_cost.reserve(vertices.size());
    for(int v: vertices){
        result.prefix_cost.push_back(dist[static_cast<std::size_t>(v)]);
    }
    return result;
}

template<class T>
struct Candidate{
    KthShortestPathResult<T> path;
};

template<class T>
struct CandidateCompare{
    bool operator()(const Candidate<T>& a, const Candidate<T>& b) const{
        if(a.path.cost != b.path.cost) return a.path.cost > b.path.cost;
        return a.path.vertices > b.path.vertices;
    }
};

} // namespace kth_shortest_path_internal

template<class T>
std::vector<KthShortestPathResult<T>> kth_shortest_paths(
    const std::vector<std::vector<KthShortestPathEdge<T>>>& graph,
    int source,
    int target,
    int k,
    T inf = std::numeric_limits<T>::max() / 4
){
    const int n = static_cast<int>(graph.size());
    if(k < 0 || source < 0 || n <= source || target < 0 || n <= target)[[unlikely]]{
        throw std::runtime_error("library assertion fault: range violation (kth_shortest_paths).");
    }
    for(const auto& edges: graph){
        for(const auto& e: edges){
            if(e.to < 0 || n <= e.to || e.cost < T(0))[[unlikely]]{
                throw std::runtime_error("library assertion fault: range violation (kth_shortest_paths).");
            }
        }
    }
    std::vector<KthShortestPathResult<T>> result;
    if(k == 0) return result;

    std::vector<char> banned_vertices(static_cast<std::size_t>(n), 0);
    std::vector<int> banned_to;
    auto first = kth_shortest_path_internal::shortest_path(
        graph,
        source,
        target,
        banned_vertices,
        -1,
        banned_to,
        inf
    );
    if(first.vertices.empty()) return result;
    result.push_back(first);

    std::priority_queue<
        kth_shortest_path_internal::Candidate<T>,
        std::vector<kth_shortest_path_internal::Candidate<T>>,
        kth_shortest_path_internal::CandidateCompare<T>
    > candidates;
    std::set<std::vector<int>> used;
    std::set<std::vector<int>> queued;
    used.insert(first.vertices);

    while(static_cast<int>(result.size()) < k){
        const auto& base = result.back();
        for(int spur_index = 0; spur_index + 1 < static_cast<int>(base.vertices.size()); spur_index++){
            const int spur = base.vertices[static_cast<std::size_t>(spur_index)];
            banned_vertices.assign(static_cast<std::size_t>(n), 0);
            for(int i = 0; i < spur_index; i++){
                banned_vertices[static_cast<std::size_t>(base.vertices[static_cast<std::size_t>(i)])] = 1;
            }
            banned_to.clear();
            for(const auto& path: result){
                if(static_cast<int>(path.vertices.size()) <= spur_index + 1) continue;
                bool same_root = true;
                for(int i = 0; i <= spur_index; i++){
                    if(path.vertices[static_cast<std::size_t>(i)] != base.vertices[static_cast<std::size_t>(i)]){
                        same_root = false;
                        break;
                    }
                }
                if(same_root){
                    banned_to.push_back(path.vertices[static_cast<std::size_t>(spur_index + 1)]);
                }
            }
            std::sort(banned_to.begin(), banned_to.end());
            banned_to.erase(std::unique(banned_to.begin(), banned_to.end()), banned_to.end());

            auto spur_path = kth_shortest_path_internal::shortest_path(
                graph,
                spur,
                target,
                banned_vertices,
                spur,
                banned_to,
                inf
            );
            if(spur_path.vertices.empty()) continue;

            KthShortestPathResult<T> candidate;
            const T root_cost = base.prefix_cost[static_cast<std::size_t>(spur_index)];
            candidate.cost = root_cost + spur_path.cost;
            candidate.vertices.reserve(static_cast<std::size_t>(spur_index) + spur_path.vertices.size());
            candidate.prefix_cost.reserve(static_cast<std::size_t>(spur_index) + spur_path.vertices.size());
            for(int i = 0; i < spur_index; i++){
                candidate.vertices.push_back(base.vertices[static_cast<std::size_t>(i)]);
                candidate.prefix_cost.push_back(base.prefix_cost[static_cast<std::size_t>(i)]);
            }
            for(std::size_t i = 0; i < spur_path.vertices.size(); i++){
                candidate.vertices.push_back(spur_path.vertices[i]);
                candidate.prefix_cost.push_back(root_cost + spur_path.prefix_cost[i]);
            }
            if(!used.count(candidate.vertices) && !queued.count(candidate.vertices)){
                queued.insert(candidate.vertices);
                candidates.push({candidate});
            }
        }

        while(!candidates.empty() && used.count(candidates.top().path.vertices)){
            candidates.pop();
        }
        if(candidates.empty()) break;
        auto next = candidates.top().path;
        candidates.pop();
        queued.erase(next.vertices);
        used.insert(next.vertices);
        result.push_back(next);
    }
    return result;
}
