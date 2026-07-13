#pragma once

#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

struct STNumberingResult{
    std::vector<int> number;
    std::vector<int> order;
};

inline std::optional<STNumberingResult> st_numbering(
    int vertex_count,
    const std::vector<std::pair<int, int>>& edges,
    int source,
    int sink
){
    constexpr const char* message =
        "library assertion fault: range violation (st_numbering).";
    if(vertex_count < 0)[[unlikely]] throw std::runtime_error(message);
    if(source < 0 || vertex_count <= source ||
       sink < 0 || vertex_count <= sink)[[unlikely]]{
        throw std::runtime_error(message);
    }
    if(vertex_count == 1){
        if(source != sink) return std::nullopt;
        return STNumberingResult{{0}, {0}};
    }
    if(source == sink) return std::nullopt;

    std::vector<std::vector<int>> graph(
        static_cast<std::size_t>(vertex_count)
    );
    for(const auto& [left, right]: edges){
        if(left < 0 || vertex_count <= left ||
           right < 0 || vertex_count <= right || left == right)[[unlikely]]{
            throw std::runtime_error(message);
        }
        graph[static_cast<std::size_t>(left)].push_back(right);
        graph[static_cast<std::size_t>(right)].push_back(left);
    }

    std::vector<int> parent(static_cast<std::size_t>(vertex_count), -1);
    std::vector<int> preorder(static_cast<std::size_t>(vertex_count), -1);
    std::vector<int> low(static_cast<std::size_t>(vertex_count), -1);
    std::vector<int> discovery_order;
    discovery_order.reserve(static_cast<std::size_t>(vertex_count));
    preorder[static_cast<std::size_t>(source)] = 0;
    low[static_cast<std::size_t>(source)] = source;
    discovery_order.push_back(source);

    struct Frame{
        int vertex;
        std::size_t next_edge;
    };
    preorder[static_cast<std::size_t>(sink)] = 1;
    low[static_cast<std::size_t>(sink)] = sink;
    discovery_order.push_back(sink);
    std::vector<Frame> stack{{sink, 0}};

    while(!stack.empty()){
        Frame& frame = stack.back();
        const int vertex = frame.vertex;
        const auto& adjacent = graph[static_cast<std::size_t>(vertex)];
        if(frame.next_edge == adjacent.size()){
            stack.pop_back();
            const int p = parent[static_cast<std::size_t>(vertex)];
            if(p != -1 &&
               preorder[static_cast<std::size_t>(
                   low[static_cast<std::size_t>(vertex)]
               )] < preorder[static_cast<std::size_t>(
                   low[static_cast<std::size_t>(p)]
               )]){
                low[static_cast<std::size_t>(p)] =
                    low[static_cast<std::size_t>(vertex)];
            }
            continue;
        }

        const int to = adjacent[frame.next_edge++];
        if(preorder[static_cast<std::size_t>(to)] == -1){
            parent[static_cast<std::size_t>(to)] = vertex;
            preorder[static_cast<std::size_t>(to)] =
                static_cast<int>(discovery_order.size());
            low[static_cast<std::size_t>(to)] = to;
            discovery_order.push_back(to);
            stack.push_back({to, 0});
        }else if(preorder[static_cast<std::size_t>(to)] <
                 preorder[static_cast<std::size_t>(
                     low[static_cast<std::size_t>(vertex)]
                 )]){
            low[static_cast<std::size_t>(vertex)] = to;
        }
    }

    if(discovery_order.size() != static_cast<std::size_t>(vertex_count)){
        return std::nullopt;
    }

    std::vector<int> next(static_cast<std::size_t>(vertex_count), -1);
    std::vector<int> previous(static_cast<std::size_t>(vertex_count), -1);
    next[static_cast<std::size_t>(source)] = sink;
    previous[static_cast<std::size_t>(sink)] = source;
    std::vector<int> sign(static_cast<std::size_t>(vertex_count));
    sign[static_cast<std::size_t>(source)] = -1;

    for(std::size_t index = 2; index < discovery_order.size(); ++index){
        const int vertex = discovery_order[index];
        const int p = parent[static_cast<std::size_t>(vertex)];
        if(p == -1) return std::nullopt;
        if(sign[static_cast<std::size_t>(
               low[static_cast<std::size_t>(vertex)]
           )] == -1){
            const int q = previous[static_cast<std::size_t>(p)];
            if(q == -1) return std::nullopt;
            next[static_cast<std::size_t>(q)] = vertex;
            next[static_cast<std::size_t>(vertex)] = p;
            previous[static_cast<std::size_t>(vertex)] = q;
            previous[static_cast<std::size_t>(p)] = vertex;
            sign[static_cast<std::size_t>(p)] = 1;
        }else{
            const int q = next[static_cast<std::size_t>(p)];
            if(q == -1) return std::nullopt;
            next[static_cast<std::size_t>(p)] = vertex;
            next[static_cast<std::size_t>(vertex)] = q;
            previous[static_cast<std::size_t>(vertex)] = p;
            previous[static_cast<std::size_t>(q)] = vertex;
            sign[static_cast<std::size_t>(p)] = -1;
        }
    }

    std::vector<int> order;
    order.reserve(static_cast<std::size_t>(vertex_count));
    int current = source;
    while(current != -1 &&
          order.size() < static_cast<std::size_t>(vertex_count)){
        order.push_back(current);
        if(current == sink) break;
        current = next[static_cast<std::size_t>(current)];
    }
    if(order.size() != static_cast<std::size_t>(vertex_count) ||
       order.front() != source || order.back() != sink){
        return std::nullopt;
    }

    std::vector<int> number(static_cast<std::size_t>(vertex_count), -1);
    for(int index = 0; index < vertex_count; ++index){
        number[static_cast<std::size_t>(
            order[static_cast<std::size_t>(index)]
        )] = index;
    }
    for(int vertex = 0; vertex < vertex_count; ++vertex){
        bool has_lower = false;
        bool has_higher = false;
        for(int to: graph[static_cast<std::size_t>(vertex)]){
            has_lower = has_lower ||
                number[static_cast<std::size_t>(to)] <
                number[static_cast<std::size_t>(vertex)];
            has_higher = has_higher ||
                number[static_cast<std::size_t>(vertex)] <
                number[static_cast<std::size_t>(to)];
        }
        if(vertex != source && !has_lower) return std::nullopt;
        if(vertex != sink && !has_higher) return std::nullopt;
    }
    return STNumberingResult{std::move(number), std::move(order)};
}
