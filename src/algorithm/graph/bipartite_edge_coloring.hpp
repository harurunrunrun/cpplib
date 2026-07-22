#ifndef CPPLIB_SRC_ALGORITHM_GRAPH_BIPARTITE_EDGE_COLORING_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GRAPH_BIPARTITE_EDGE_COLORING_HPP_INCLUDED

#include <algorithm>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

struct BipartiteEdgeColoringEdge{
    int left;
    int right;
};

struct BipartiteEdgeColoringResult{
    int color_count;
    std::vector<int> color;
};

namespace bipartite_edge_coloring_internal{

struct RegularEdge{
    int left;
    int right;
    int original_id;
};

struct PackedVertices{
    int count;
    std::vector<int> id;
};

inline PackedVertices pack_vertices(
    const std::vector<int>& degree,
    int capacity
){
    PackedVertices result{0, std::vector<int>(degree.size(), -1)};
    int current = -1;
    int current_degree = 0;
    for(int vertex = 0; vertex < static_cast<int>(degree.size()); ++vertex){
        const int value = degree[static_cast<std::size_t>(vertex)];
        if(value == 0) continue;
        if(current == -1 || current_degree + value > capacity){
            current = result.count++;
            current_degree = 0;
        }
        result.id[static_cast<std::size_t>(vertex)] = current;
        current_degree += value;
    }
    return result;
}

class RegularBipartiteEdgeColoring{
    int part_size;
    const std::vector<RegularEdge>& edges;
    std::vector<int>& answer;
    std::vector<unsigned char> selected;

    std::pair<std::vector<int>, std::vector<int>> split_even(
        const std::vector<int>& edge_ids,
        int degree
    ) const{
        const int vertex_count = 2 * part_size;
        if(static_cast<long long>(vertex_count) * degree !=
           static_cast<long long>(2 * edge_ids.size()))[[unlikely]]{
            throw std::logic_error(
                "library internal fault: non-regular bipartite graph."
            );
        }
        std::vector<int> adjacency(2 * edge_ids.size(), -1);
        std::vector<int> filled(static_cast<std::size_t>(vertex_count));
        for(int local_edge = 0;
            local_edge < static_cast<int>(edge_ids.size()); ++local_edge){
            const RegularEdge& edge = edges[static_cast<std::size_t>(
                edge_ids[static_cast<std::size_t>(local_edge)]
            )];
            const int right = part_size + edge.right;
            adjacency[static_cast<std::size_t>(
                edge.left * degree +
                filled[static_cast<std::size_t>(edge.left)]++
            )] = local_edge;
            adjacency[static_cast<std::size_t>(
                right * degree + filled[static_cast<std::size_t>(right)]++
            )] = local_edge;
        }
        for(int value: filled){
            if(value != degree)[[unlikely]]{
                throw std::logic_error(
                    "library internal fault: non-regular bipartite graph."
                );
            }
        }

        std::vector<int> next_edge(static_cast<std::size_t>(vertex_count));
        std::vector<unsigned char> used(edge_ids.size());
        std::vector<int> first;
        std::vector<int> second;
        first.reserve(edge_ids.size() / 2);
        second.reserve(edge_ids.size() / 2);
        std::vector<int> vertex_stack;
        std::vector<int> edge_stack;
        std::vector<int> tour;

        for(int start = 0; start < vertex_count; ++start){
            while(next_edge[static_cast<std::size_t>(start)] < degree &&
                  used[static_cast<std::size_t>(adjacency[
                      static_cast<std::size_t>(
                          start * degree +
                          next_edge[static_cast<std::size_t>(start)]
                      )
                  ])]){
                ++next_edge[static_cast<std::size_t>(start)];
            }
            if(next_edge[static_cast<std::size_t>(start)] == degree) continue;

            vertex_stack.push_back(start);
            tour.clear();
            while(!vertex_stack.empty()){
                const int vertex = vertex_stack.back();
                int& position = next_edge[static_cast<std::size_t>(vertex)];
                while(position < degree &&
                      used[static_cast<std::size_t>(adjacency[
                          static_cast<std::size_t>(vertex * degree + position)
                      ])]){
                    ++position;
                }
                if(position == degree){
                    vertex_stack.pop_back();
                    if(!edge_stack.empty()){
                        tour.push_back(edge_stack.back());
                        edge_stack.pop_back();
                    }
                    continue;
                }
                const int local_edge = adjacency[static_cast<std::size_t>(
                    vertex * degree + position++
                )];
                if(used[static_cast<std::size_t>(local_edge)]) continue;
                used[static_cast<std::size_t>(local_edge)] = 1;
                const RegularEdge& edge = edges[static_cast<std::size_t>(
                    edge_ids[static_cast<std::size_t>(local_edge)]
                )];
                const int to = vertex == edge.left
                    ? part_size + edge.right : edge.left;
                vertex_stack.push_back(to);
                edge_stack.push_back(local_edge);
            }
            if(tour.size() % 2 != 0)[[unlikely]]{
                throw std::logic_error(
                    "library internal fault: odd Euler circuit."
                );
            }
            for(std::size_t index = 0; index < tour.size(); ++index){
                const int global_edge = edge_ids[
                    static_cast<std::size_t>(tour[index])
                ];
                if(index % 2 == 0) first.push_back(global_edge);
                else second.push_back(global_edge);
            }
        }
        if(first.size() != edge_ids.size() / 2 ||
           second.size() != edge_ids.size() / 2)[[unlikely]]{
            throw std::logic_error(
                "library internal fault: Euler split failed."
            );
        }
        return {std::move(first), std::move(second)};
    }

    std::vector<int> perfect_matching(
        const std::vector<int>& edge_ids,
        int degree
    ) const{
        std::vector<int> adjacency(edge_ids.size(), -1);
        std::vector<int> filled(static_cast<std::size_t>(part_size));
        for(int local_edge = 0;
            local_edge < static_cast<int>(edge_ids.size()); ++local_edge){
            const int global_edge = edge_ids[static_cast<std::size_t>(local_edge)];
            const int left = edges[static_cast<std::size_t>(global_edge)].left;
            adjacency[static_cast<std::size_t>(
                left * degree + filled[static_cast<std::size_t>(left)]++
            )] = local_edge;
        }
        for(int value: filled){
            if(value != degree)[[unlikely]]{
                throw std::logic_error(
                    "library internal fault: non-regular bipartite graph."
                );
            }
        }

        std::vector<int> left_match(static_cast<std::size_t>(part_size), -1);
        std::vector<int> right_match(static_cast<std::size_t>(part_size), -1);
        std::vector<int> matching_edge(
            static_cast<std::size_t>(part_size), -1
        );
        std::vector<int> distance(static_cast<std::size_t>(part_size));
        std::vector<int> queue;
        queue.reserve(static_cast<std::size_t>(part_size));
        std::vector<int> next_edge(static_cast<std::size_t>(part_size));
        std::vector<int> left_stack;
        std::vector<int> transition_edge;

        int matching_size = 0;
        while(matching_size < part_size){
            std::fill(distance.begin(), distance.end(), -1);
            queue.clear();
            for(int left = 0; left < part_size; ++left){
                if(left_match[static_cast<std::size_t>(left)] == -1){
                    distance[static_cast<std::size_t>(left)] = 0;
                    queue.push_back(left);
                }
            }
            int shortest = std::numeric_limits<int>::max();
            for(std::size_t head = 0; head < queue.size(); ++head){
                const int left = queue[head];
                if(distance[static_cast<std::size_t>(left)] + 1 > shortest){
                    continue;
                }
                for(int position = 0; position < degree; ++position){
                    const int local_edge = adjacency[static_cast<std::size_t>(
                        left * degree + position
                    )];
                    const int global_edge = edge_ids[
                        static_cast<std::size_t>(local_edge)
                    ];
                    const int right = edges[
                        static_cast<std::size_t>(global_edge)
                    ].right;
                    const int next_left = right_match[
                        static_cast<std::size_t>(right)
                    ];
                    if(next_left == -1){
                        shortest = std::min(
                            shortest,
                            distance[static_cast<std::size_t>(left)] + 1
                        );
                    }else if(distance[static_cast<std::size_t>(next_left)] == -1){
                        distance[static_cast<std::size_t>(next_left)] =
                            distance[static_cast<std::size_t>(left)] + 1;
                        queue.push_back(next_left);
                    }
                }
            }
            if(shortest == std::numeric_limits<int>::max())[[unlikely]]{
                throw std::logic_error(
                    "library internal fault: perfect matching was not found."
                );
            }
            std::fill(next_edge.begin(), next_edge.end(), 0);

            for(int root = 0; root < part_size; ++root){
                if(left_match[static_cast<std::size_t>(root)] != -1 ||
                   distance[static_cast<std::size_t>(root)] != 0){
                    continue;
                }
                left_stack.clear();
                transition_edge.clear();
                left_stack.push_back(root);
                bool augmented = false;
                while(!left_stack.empty() && !augmented){
                    const int left = left_stack.back();
                    bool advanced = false;
                    int& position = next_edge[static_cast<std::size_t>(left)];
                    while(position < degree){
                        const int local_edge = adjacency[
                            static_cast<std::size_t>(left * degree + position++)
                        ];
                        const int global_edge = edge_ids[
                            static_cast<std::size_t>(local_edge)
                        ];
                        const int right = edges[
                            static_cast<std::size_t>(global_edge)
                        ].right;
                        const int next_left = right_match[
                            static_cast<std::size_t>(right)
                        ];
                        if(next_left == -1){
                            if(distance[static_cast<std::size_t>(left)] + 1 !=
                               shortest){
                                continue;
                            }
                            left_match[static_cast<std::size_t>(left)] = right;
                            right_match[static_cast<std::size_t>(right)] = left;
                            matching_edge[static_cast<std::size_t>(left)] =
                                global_edge;
                            for(int index =
                                    static_cast<int>(transition_edge.size()) - 1;
                                index >= 0; --index){
                                const int parent_left = left_stack[
                                    static_cast<std::size_t>(index)
                                ];
                                const int parent_edge = transition_edge[
                                    static_cast<std::size_t>(index)
                                ];
                                const int parent_right = edges[
                                    static_cast<std::size_t>(parent_edge)
                                ].right;
                                left_match[static_cast<std::size_t>(parent_left)] =
                                    parent_right;
                                right_match[static_cast<std::size_t>(parent_right)] =
                                    parent_left;
                                matching_edge[
                                    static_cast<std::size_t>(parent_left)
                                ] = parent_edge;
                            }
                            augmented = true;
                            ++matching_size;
                            break;
                        }
                        if(distance[static_cast<std::size_t>(next_left)] ==
                               distance[static_cast<std::size_t>(left)] + 1 &&
                           distance[static_cast<std::size_t>(next_left)] <
                               shortest){
                            transition_edge.push_back(global_edge);
                            left_stack.push_back(next_left);
                            advanced = true;
                            break;
                        }
                    }
                    if(augmented || advanced) continue;
                    distance[static_cast<std::size_t>(left)] = -1;
                    left_stack.pop_back();
                    if(!transition_edge.empty()) transition_edge.pop_back();
                }
            }
        }
        return matching_edge;
    }

    void color_regular(
        std::vector<int> edge_ids,
        int degree,
        int offset
    ){
        if(degree == 1){
            for(int edge_id: edge_ids){
                const int original = edges[
                    static_cast<std::size_t>(edge_id)
                ].original_id;
                if(original != -1){
                    answer[static_cast<std::size_t>(original)] = offset;
                }
            }
            return;
        }
        if(degree % 2 == 0){
            auto [first, second] = split_even(edge_ids, degree);
            std::vector<int>().swap(edge_ids);
            color_regular(std::move(first), degree / 2, offset);
            color_regular(
                std::move(second), degree / 2, offset + degree / 2
            );
            return;
        }

        const std::vector<int> matching = perfect_matching(edge_ids, degree);
        for(int edge_id: matching){
            selected[static_cast<std::size_t>(edge_id)] = 1;
            const int original = edges[
                static_cast<std::size_t>(edge_id)
            ].original_id;
            if(original != -1){
                answer[static_cast<std::size_t>(original)] = offset;
            }
        }
        std::vector<int> remaining;
        remaining.reserve(
            edge_ids.size() - static_cast<std::size_t>(part_size)
        );
        for(int edge_id: edge_ids){
            if(!selected[static_cast<std::size_t>(edge_id)]){
                remaining.push_back(edge_id);
            }
        }
        for(int edge_id: matching){
            selected[static_cast<std::size_t>(edge_id)] = 0;
        }
        std::vector<int>().swap(edge_ids);
        color_regular(std::move(remaining), degree - 1, offset + 1);
    }

public:
    RegularBipartiteEdgeColoring(
        int part_size_,
        const std::vector<RegularEdge>& edges_,
        std::vector<int>& answer_
    ):
        part_size(part_size_), edges(edges_), answer(answer_),
        selected(edges_.size()){}

    void solve(int degree){
        std::vector<int> edge_ids(edges.size());
        std::iota(edge_ids.begin(), edge_ids.end(), 0);
        color_regular(std::move(edge_ids), degree, 0);
    }
};

} // namespace bipartite_edge_coloring_internal

inline BipartiteEdgeColoringResult bipartite_edge_coloring(
    int left_size,
    int right_size,
    const std::vector<BipartiteEdgeColoringEdge>& input_edges
){
    constexpr const char* message =
        "library assertion fault: range violation "
        "(bipartite_edge_coloring).";
    if(left_size < 0 || right_size < 0)[[unlikely]]{
        throw std::runtime_error(message);
    }
    std::vector<int> left_degree(static_cast<std::size_t>(left_size));
    std::vector<int> right_degree(static_cast<std::size_t>(right_size));
    int maximum_degree = 0;
    for(const auto& edge: input_edges){
        if(edge.left < 0 || left_size <= edge.left ||
           edge.right < 0 || right_size <= edge.right)[[unlikely]]{
            throw std::runtime_error(message);
        }
        maximum_degree = std::max(
            maximum_degree,
            ++left_degree[static_cast<std::size_t>(edge.left)]
        );
        maximum_degree = std::max(
            maximum_degree,
            ++right_degree[static_cast<std::size_t>(edge.right)]
        );
    }

    BipartiteEdgeColoringResult result{
        maximum_degree, std::vector<int>(input_edges.size(), -1)
    };
    if(maximum_degree == 0) return result;

    auto packed_left = bipartite_edge_coloring_internal::pack_vertices(
        left_degree, maximum_degree
    );
    auto packed_right = bipartite_edge_coloring_internal::pack_vertices(
        right_degree, maximum_degree
    );
    const int part_size = std::max(packed_left.count, packed_right.count);
    const long long regular_edge_count =
        static_cast<long long>(part_size) * maximum_degree;
    if(regular_edge_count > std::numeric_limits<int>::max())[[unlikely]]{
        throw std::runtime_error(message);
    }

    std::vector<int> packed_left_degree(
        static_cast<std::size_t>(part_size)
    );
    std::vector<int> packed_right_degree(
        static_cast<std::size_t>(part_size)
    );
    std::vector<bipartite_edge_coloring_internal::RegularEdge> edges;
    edges.reserve(static_cast<std::size_t>(regular_edge_count));
    for(int edge_id = 0;
        edge_id < static_cast<int>(input_edges.size()); ++edge_id){
        const auto& edge = input_edges[static_cast<std::size_t>(edge_id)];
        const int left = packed_left.id[static_cast<std::size_t>(edge.left)];
        const int right = packed_right.id[static_cast<std::size_t>(edge.right)];
        edges.push_back({left, right, edge_id});
        ++packed_left_degree[static_cast<std::size_t>(left)];
        ++packed_right_degree[static_cast<std::size_t>(right)];
    }

    int left = 0;
    int right = 0;
    while(left < part_size && right < part_size){
        while(left < part_size &&
              packed_left_degree[static_cast<std::size_t>(left)] ==
                  maximum_degree){
            ++left;
        }
        while(right < part_size &&
              packed_right_degree[static_cast<std::size_t>(right)] ==
                  maximum_degree){
            ++right;
        }
        if(left == part_size || right == part_size) break;
        const int count = std::min(
            maximum_degree -
                packed_left_degree[static_cast<std::size_t>(left)],
            maximum_degree -
                packed_right_degree[static_cast<std::size_t>(right)]
        );
        for(int copy = 0; copy < count; ++copy){
            edges.push_back({left, right, -1});
        }
        packed_left_degree[static_cast<std::size_t>(left)] += count;
        packed_right_degree[static_cast<std::size_t>(right)] += count;
    }
    if(static_cast<long long>(edges.size()) != regular_edge_count)[[unlikely]]{
        throw std::logic_error(
            "library internal fault: regularization failed."
        );
    }

    bipartite_edge_coloring_internal::RegularBipartiteEdgeColoring solver(
        part_size, edges, result.color
    );
    solver.solve(maximum_degree);
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GRAPH_BIPARTITE_EDGE_COLORING_HPP_INCLUDED
