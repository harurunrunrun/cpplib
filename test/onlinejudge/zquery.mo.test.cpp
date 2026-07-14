// competitive-verifier: PROBLEM https://www.spoj.com/problems/ZQUERY/

#include <algorithm>
#include <cmath>
#include <deque>
#include <iostream>
#include <vector>

#include "../../src/algorithm/other/mo.hpp"

class DistanceMaximum{
    int limit;
    int block_size;
    std::vector<int> frequency;
    std::vector<int> block_frequency;

public:
    explicit DistanceMaximum(int limit_)
        : limit(limit_),
          block_size(std::max(1, static_cast<int>(std::sqrt(static_cast<long double>(limit + 1))))),
          frequency(static_cast<std::size_t>(limit + 1), 0),
          block_frequency(static_cast<std::size_t>(limit / block_size + 1), 0){}

    void add(int distance){
        ++frequency[static_cast<std::size_t>(distance)];
        ++block_frequency[static_cast<std::size_t>(distance / block_size)];
    }

    void erase(int distance){
        --frequency[static_cast<std::size_t>(distance)];
        --block_frequency[static_cast<std::size_t>(distance / block_size)];
    }

    int maximum() const{
        for(int block = static_cast<int>(block_frequency.size()) - 1; block >= 0; --block){
            if(block_frequency[static_cast<std::size_t>(block)] == 0) continue;
            int upper = std::min(limit, (block + 1) * block_size - 1);
            int lower = block * block_size;
            for(int distance = upper; distance >= lower; --distance){
                if(frequency[static_cast<std::size_t>(distance)] != 0) return distance;
            }
        }
        return 0;
    }
};

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, query_count;
    std::cin >> n >> query_count;

    std::vector<int> prefix(static_cast<std::size_t>(n + 1), 0);
    for(int index = 0; index < n; ++index){
        int value;
        std::cin >> value;
        prefix[static_cast<std::size_t>(index + 1)] = prefix[static_cast<std::size_t>(index)] + value;
    }

    Mo mo(n + 1);
    for(int query = 0; query < query_count; ++query){
        int left, right;
        std::cin >> left >> right;
        mo.add_query(left - 1, right + 1);
    }

    std::vector<std::deque<int>> positions(static_cast<std::size_t>(2 * n + 1));
    DistanceMaximum distances(n);
    std::vector<int> answer(static_cast<std::size_t>(query_count), 0);

    const auto value_id = [&](int index){
        return prefix[static_cast<std::size_t>(index)] + n;
    };
    const auto erase_span = [&](const std::deque<int>& indices){
        if(!indices.empty()) distances.erase(indices.back() - indices.front());
    };
    const auto add_span = [&](const std::deque<int>& indices){
        if(!indices.empty()) distances.add(indices.back() - indices.front());
    };
    const auto add_left = [&](int index){
        std::deque<int>& indices = positions[static_cast<std::size_t>(value_id(index))];
        erase_span(indices);
        indices.push_front(index);
        add_span(indices);
    };
    const auto add_right = [&](int index){
        std::deque<int>& indices = positions[static_cast<std::size_t>(value_id(index))];
        erase_span(indices);
        indices.push_back(index);
        add_span(indices);
    };
    const auto erase_left = [&](int index){
        std::deque<int>& indices = positions[static_cast<std::size_t>(value_id(index))];
        erase_span(indices);
        indices.pop_front();
        add_span(indices);
    };
    const auto erase_right = [&](int index){
        std::deque<int>& indices = positions[static_cast<std::size_t>(value_id(index))];
        erase_span(indices);
        indices.pop_back();
        add_span(indices);
    };

    mo.solve(add_left, add_right, erase_left, erase_right, [&](int query){
        answer[static_cast<std::size_t>(query)] = distances.maximum();
    });

    for(int value: answer) std::cout << value << '\n';
}
