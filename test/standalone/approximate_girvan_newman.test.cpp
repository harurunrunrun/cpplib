// competitive-verifier: STANDALONE

#include <cstddef>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../../src/approximate/graph/girvan_newman.hpp"

namespace {

void require(bool condition){
    if(!condition) throw std::runtime_error("Girvan-Newman test failed");
}

template<class Exception, class Function>
void require_throws(Function&& function){
    bool thrown = false;
    try{
        std::forward<Function>(function)();
    }catch(const Exception&){
        thrown = true;
    }
    require(thrown);
}

void check_canonical(const approximate::graph::GirvanNewmanResult& result, std::size_t size){
    require(result.labels.size() == size);
    require(result.rounds == result.removed_edges.size());
    std::size_t next = 0;
    std::vector<bool> seen(result.community_count, false);
    for(const std::size_t label : result.labels){
        require(label < result.community_count);
        if(!seen[label]){
            require(label == next);
            seen[label] = true;
            ++next;
        }
    }
    require(next == result.community_count);
    for(const auto [left, right] : result.removed_edges) require(left < right);
}

void boundary_tests(){
    using approximate::graph::girvan_newman;
    require_throws<std::invalid_argument>([]{
        static_cast<void>(girvan_newman(0, {}, 1));
    });
    require_throws<std::invalid_argument>([]{
        static_cast<void>(girvan_newman(2, {}, 0));
    });
    require_throws<std::invalid_argument>([]{
        static_cast<void>(girvan_newman(2, {}, 3));
    });
    require_throws<std::out_of_range>([]{
        static_cast<void>(girvan_newman(2, {{0, 2}}, 1));
    });
    require_throws<std::invalid_argument>([]{
        static_cast<void>(girvan_newman(4, {{0, 1}, {2, 3}}, 1));
    });
    const auto empty = girvan_newman(0, {}, 0);
    check_canonical(empty, 0);
    require(empty.community_count == 0);

    const auto normalized = girvan_newman(
        3, {{1, 0}, {0, 1}, {1, 1}, {1, 2}, {2, 1}}, 3
    );
    check_canonical(normalized, 3);
    require(normalized.removed_edges.size() == 2);
    require(normalized.removed_edges[0] == std::pair<std::size_t, std::size_t>{0, 1});
    require(normalized.removed_edges[1] == std::pair<std::size_t, std::size_t>{1, 2});

    const auto limited = girvan_newman(3, {{0, 1}, {1, 2}}, 3, 0);
    require(limited.community_count == 1);
    require(limited.removed_edges.empty());

    std::vector<std::pair<std::size_t, std::size_t>> cycle{
        {0, 1}, {1, 2}, {2, 3}, {3, 0}
    };
    const auto first = girvan_newman(4, cycle, 2);
    const auto second = girvan_newman(4, cycle, 2);
    require(first.labels == second.labels);
    require(first.removed_edges == second.removed_edges);
}

}  // namespace

int main(){
    using approximate::graph::girvan_newman;
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    boundary_tests();

    std::size_t case_count = 0;
    std::cin >> case_count;
    for(std::size_t case_index = 0; case_index < case_count; ++case_index){
        std::size_t identifier = 0;
        std::size_t vertex_count = 0;
        std::size_t edge_count = 0;
        std::size_t target = 0;
        std::size_t maximum_rounds = 0;
        std::cin >> identifier >> vertex_count >> edge_count >> target >> maximum_rounds;
        std::vector<std::pair<std::size_t, std::size_t>> edges(edge_count);
        for(auto& [left, right] : edges) std::cin >> left >> right;
        const auto result = girvan_newman(
            vertex_count, edges, target, maximum_rounds
        );
        check_canonical(result, vertex_count);
        require(result.rounds <= maximum_rounds);
        std::cout << "CASE " << identifier << ' ' << result.community_count << ' '
                  << result.rounds;
        for(const std::size_t label : result.labels) std::cout << ' ' << label;
        for(const auto [left, right] : result.removed_edges){
            std::cout << ' ' << left << ' ' << right;
        }
        std::cout << '\n';
    }
}
