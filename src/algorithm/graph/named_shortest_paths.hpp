#pragma once

#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "dijkstra.hpp"

template<class T>
class NamedShortestPaths{
    std::vector<std::vector<DijkstraEdge<T>>> graph_;
    std::unordered_map<std::string, int> index_;
    mutable std::vector<std::optional<DijkstraResult<T>>> cache_;

    int vertex_index(const std::string& name) const{
        const auto iterator = index_.find(name);
        if(iterator == index_.end())[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: unknown name (NamedShortestPaths)."
            );
        }
        return iterator->second;
    }

public:
    NamedShortestPaths(
        std::vector<std::string> names,
        std::vector<std::vector<DijkstraEdge<T>>> graph
    ): graph_(std::move(graph)){
        const int n = static_cast<int>(names.size());
        if(static_cast<int>(graph_.size()) != n)[[unlikely]]{
            throw std::runtime_error(
                "library assertion fault: size mismatch (NamedShortestPaths)."
            );
        }
        index_.reserve(names.size() * 2);
        for(int vertex = 0; vertex < n; ++vertex){
            if(!index_.emplace(std::move(names[static_cast<std::size_t>(vertex)]), vertex).second){
                throw std::runtime_error(
                    "library assertion fault: duplicate name (NamedShortestPaths)."
                );
            }
        }
        for(const auto& edges: graph_){
            for(const auto& edge: edges){
                if(edge.to < 0 || n <= edge.to || edge.cost < T(0))[[unlikely]]{
                    throw std::runtime_error(
                        "library assertion fault: range violation (NamedShortestPaths)."
                    );
                }
            }
        }
        cache_.resize(names.size());
    }

    int size() const{
        return static_cast<int>(graph_.size());
    }

    bool contains(const std::string& name) const{
        return index_.contains(name);
    }

    std::optional<T> distance(
        const std::string& source_name,
        const std::string& target_name
    ) const{
        const int source = vertex_index(source_name);
        const int target = vertex_index(target_name);
        auto& result = cache_[static_cast<std::size_t>(source)];
        if(!result) result = dijkstra<T>(graph_, source);
        if(!result->reachable[static_cast<std::size_t>(target)]) return std::nullopt;
        return result->dist[static_cast<std::size_t>(target)];
    }

    void clear_cache(){
        for(auto& result: cache_) result.reset();
    }
};
