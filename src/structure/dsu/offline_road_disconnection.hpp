#pragma once

#include <algorithm>
#include <stdexcept>
#include <utility>
#include <vector>

#include "dsu.hpp"

template<int MAX_SIZE>
struct OfflineRoadDisconnection{
private:
    struct Query{
        bool ask;
        int road;
    };

    int _n;
    std::vector<std::pair<int, int>> roads;
    std::vector<Query> queries;
    std::vector<bool> removed;

public:
    explicit OfflineRoadDisconnection(int n):
        _n(n), removed(static_cast<std::size_t>(std::max(0, n - 1)), false){
        if(n < 0 || MAX_SIZE < n) throw std::runtime_error("library assertion fault: range violation (constructor).");
        roads.reserve(static_cast<std::size_t>(std::max(0, n - 1)));
    }

    int add_road(int left, int right){
        if(static_cast<int>(roads.size()) >= _n - 1) throw std::runtime_error("library assertion fault: too many roads (add_road).");
        if(left < 0 || _n <= left || right < 0 || _n <= right) throw std::runtime_error("library assertion fault: range violation (add_road).");
        roads.emplace_back(left, right);
        return static_cast<int>(roads.size()) - 1;
    }

    void remove_road(int road){
        if(road < 0 || static_cast<int>(roads.size()) <= road) throw std::runtime_error("library assertion fault: range violation (remove_road).");
        if(removed[static_cast<std::size_t>(road)]) throw std::runtime_error("library assertion fault: road removed twice (remove_road).");
        removed[static_cast<std::size_t>(road)] = true;
        queries.push_back({false, road});
    }

    void ask(){ queries.push_back({true, -1}); }

    std::vector<long long> solve() const{
        if(static_cast<int>(roads.size()) != std::max(0, _n - 1)) throw std::runtime_error("library assertion fault: road count mismatch (solve).");
        DSU<MAX_SIZE> dsu(_n);
        long long disconnected = static_cast<long long>(_n) * (_n - 1LL) / 2LL;
        const auto restore = [&](int road){
            const auto [left, right] = roads[static_cast<std::size_t>(road)];
            if(dsu.same(left, right)) return;
            disconnected -= static_cast<long long>(dsu.component_size(left))
                * dsu.component_size(right);
            dsu.merge(left, right);
        };
        for(int road = 0; road < static_cast<int>(roads.size()); ++road){
            if(!removed[static_cast<std::size_t>(road)]) restore(road);
        }
        std::vector<long long> answer;
        for(auto iterator = queries.rbegin(); iterator != queries.rend(); ++iterator){
            if(iterator->ask) answer.push_back(disconnected);
            else restore(iterator->road);
        }
        std::reverse(answer.begin(), answer.end());
        return answer;
    }
};
