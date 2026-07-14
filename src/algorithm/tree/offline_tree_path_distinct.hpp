#pragma once

#include <algorithm>
#include <vector>

#include "tree_mo.hpp"

template<class Value, int MAX_SIZE>
struct OfflineTreePathDistinct{
private:
    int _n;
    TreeMo<MAX_SIZE> mo;
    std::vector<Value> values;
    std::vector<int> ranks;
    int query_count = 0;

public:
    explicit OfflineTreePathDistinct(const std::vector<Value>& vertex_values):
        _n(static_cast<int>(vertex_values.size())), mo(_n), values(vertex_values){}

    void add_edge(int left, int right){ mo.add_edge(left, right); }

    void build(int root = 0){
        mo.build(root);
        std::vector<Value> compressed = values;
        std::sort(compressed.begin(), compressed.end());
        compressed.erase(std::unique(compressed.begin(), compressed.end()), compressed.end());
        ranks.resize(static_cast<std::size_t>(_n));
        for(int vertex = 0; vertex < _n; ++vertex){
            ranks[static_cast<std::size_t>(vertex)] = static_cast<int>(
                std::lower_bound(compressed.begin(), compressed.end(), values[static_cast<std::size_t>(vertex)])
                    - compressed.begin()
            );
        }
    }

    int add_query(int left, int right){
        mo.add_query(left, right);
        return query_count++;
    }

    std::vector<int> solve() const{
        int value_count = 0;
        for(int rank: ranks) value_count = std::max(value_count, rank + 1);
        std::vector<int> frequency(static_cast<std::size_t>(value_count), 0);
        std::vector<int> answer(static_cast<std::size_t>(query_count));
        int distinct = 0;
        mo.solve(
            [&](int vertex){
                const int rank = ranks[static_cast<std::size_t>(vertex)];
                if(frequency[static_cast<std::size_t>(rank)]++ == 0) ++distinct;
            },
            [&](int vertex){
                const int rank = ranks[static_cast<std::size_t>(vertex)];
                if(--frequency[static_cast<std::size_t>(rank)] == 0) --distinct;
            },
            [&](int query){ answer[static_cast<std::size_t>(query)] = distinct; }
        );
        return answer;
    }
};
