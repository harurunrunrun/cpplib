#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

namespace minimum_weight_general_matching_internal{

using Wide = __int128_t;

struct WeightedEdge{
    int from;
    int to;
    Wide weight;
};

class WeightedBlossom{
private:
    int n;
    int edge_count;
    std::vector<WeightedEdge> edges;
    std::vector<int> endpoint;
    std::vector<std::vector<int>> neighbour_endpoints;
    std::vector<int> mate;
    std::vector<int> label;
    std::vector<int> label_end;
    std::vector<int> in_blossom;
    std::vector<int> blossom_parent;
    std::vector<std::vector<int>> blossom_children;
    std::vector<int> blossom_base;
    std::vector<std::vector<int>> blossom_endpoints;
    std::vector<int> best_edge;
    std::vector<std::vector<int>> blossom_best_edges;
    std::vector<int> unused_blossoms;
    std::vector<Wide> dual;
    std::vector<char> allowed_edge;
    std::vector<int> queue;

    Wide slack(int edge) const{
        const auto& e = edges[static_cast<std::size_t>(edge)];
        return dual[static_cast<std::size_t>(e.from)] + dual[static_cast<std::size_t>(e.to)] -
               static_cast<Wide>(2) * e.weight;
    }

    void collect_leaves(int blossom, std::vector<int>& leaves) const{
        if(blossom < n){
            leaves.push_back(blossom);
            return;
        }
        for(int child: blossom_children[static_cast<std::size_t>(blossom)]){
            collect_leaves(child, leaves);
        }
    }

    std::vector<int> leaves(int blossom) const{
        std::vector<int> result;
        collect_leaves(blossom, result);
        return result;
    }

    static int cyclic(const std::vector<int>& values, int index){
        if(index < 0) index += static_cast<int>(values.size());
        return values[static_cast<std::size_t>(index)];
    }

    static int child_index(const std::vector<int>& values, int value){
        return static_cast<int>(std::find(values.begin(), values.end(), value) - values.begin());
    }

    static void rotate_to(std::vector<int>& values, int index){
        std::rotate(values.begin(), values.begin() + index, values.end());
    }

    void assign_label(int vertex, int type, int remote_endpoint){
        const int blossom = in_blossom[static_cast<std::size_t>(vertex)];
        label[static_cast<std::size_t>(vertex)] = type;
        label[static_cast<std::size_t>(blossom)] = type;
        label_end[static_cast<std::size_t>(vertex)] = remote_endpoint;
        label_end[static_cast<std::size_t>(blossom)] = remote_endpoint;
        best_edge[static_cast<std::size_t>(vertex)] = -1;
        best_edge[static_cast<std::size_t>(blossom)] = -1;
        if(type == 1){
            const auto blossom_leaves = leaves(blossom);
            queue.insert(queue.end(), blossom_leaves.begin(), blossom_leaves.end());
        }else{
            const int base = blossom_base[static_cast<std::size_t>(blossom)];
            const int matched_endpoint = mate[static_cast<std::size_t>(base)];
            assign_label(endpoint[static_cast<std::size_t>(matched_endpoint)], 1, matched_endpoint ^ 1);
        }
    }

    int scan_blossom(int vertex, int other){
        std::vector<int> path;
        int base = -1;
        while(vertex != -1 || other != -1){
            const int blossom = in_blossom[static_cast<std::size_t>(vertex)];
            if(label[static_cast<std::size_t>(blossom)] & 4){
                base = blossom_base[static_cast<std::size_t>(blossom)];
                break;
            }
            path.push_back(blossom);
            label[static_cast<std::size_t>(blossom)] = 5;
            if(label_end[static_cast<std::size_t>(blossom)] == -1){
                vertex = -1;
            }else{
                vertex = endpoint[static_cast<std::size_t>(label_end[static_cast<std::size_t>(blossom)])];
                const int parent_blossom = in_blossom[static_cast<std::size_t>(vertex)];
                vertex = endpoint[static_cast<std::size_t>(label_end[static_cast<std::size_t>(parent_blossom)])];
            }
            if(other != -1) std::swap(vertex, other);
        }
        for(int blossom: path) label[static_cast<std::size_t>(blossom)] = 1;
        return base;
    }

    void add_blossom(int base, int edge){
        int vertex = edges[static_cast<std::size_t>(edge)].from;
        int other = edges[static_cast<std::size_t>(edge)].to;
        const int base_blossom = in_blossom[static_cast<std::size_t>(base)];
        int vertex_blossom = in_blossom[static_cast<std::size_t>(vertex)];
        int other_blossom = in_blossom[static_cast<std::size_t>(other)];
        const int blossom = unused_blossoms.back();
        unused_blossoms.pop_back();

        blossom_base[static_cast<std::size_t>(blossom)] = base;
        blossom_parent[static_cast<std::size_t>(blossom)] = -1;
        blossom_parent[static_cast<std::size_t>(base_blossom)] = blossom;
        auto& path = blossom_children[static_cast<std::size_t>(blossom)];
        auto& end_points = blossom_endpoints[static_cast<std::size_t>(blossom)];
        path.clear();
        end_points.clear();
        while(vertex_blossom != base_blossom){
            blossom_parent[static_cast<std::size_t>(vertex_blossom)] = blossom;
            path.push_back(vertex_blossom);
            end_points.push_back(label_end[static_cast<std::size_t>(vertex_blossom)]);
            vertex = endpoint[static_cast<std::size_t>(label_end[static_cast<std::size_t>(vertex_blossom)])];
            vertex_blossom = in_blossom[static_cast<std::size_t>(vertex)];
        }
        path.push_back(base_blossom);
        std::reverse(path.begin(), path.end());
        std::reverse(end_points.begin(), end_points.end());
        end_points.push_back(2 * edge);
        while(other_blossom != base_blossom){
            blossom_parent[static_cast<std::size_t>(other_blossom)] = blossom;
            path.push_back(other_blossom);
            end_points.push_back(label_end[static_cast<std::size_t>(other_blossom)] ^ 1);
            other = endpoint[static_cast<std::size_t>(label_end[static_cast<std::size_t>(other_blossom)])];
            other_blossom = in_blossom[static_cast<std::size_t>(other)];
        }

        label[static_cast<std::size_t>(blossom)] = 1;
        label_end[static_cast<std::size_t>(blossom)] = label_end[static_cast<std::size_t>(base_blossom)];
        dual[static_cast<std::size_t>(blossom)] = 0;
        for(int leaf: leaves(blossom)){
            if(label[static_cast<std::size_t>(in_blossom[static_cast<std::size_t>(leaf)])] == 2){
                queue.push_back(leaf);
            }
            in_blossom[static_cast<std::size_t>(leaf)] = blossom;
        }

        std::vector<int> best_to(static_cast<std::size_t>(2 * n), -1);
        for(int sub_blossom: path){
            std::vector<int> candidate_edges;
            if(blossom_best_edges[static_cast<std::size_t>(sub_blossom)].empty()){
                for(int leaf: leaves(sub_blossom)){
                    for(int remote_endpoint: neighbour_endpoints[static_cast<std::size_t>(leaf)]){
                        candidate_edges.push_back(remote_endpoint / 2);
                    }
                }
            }else{
                candidate_edges = blossom_best_edges[static_cast<std::size_t>(sub_blossom)];
            }
            for(int candidate: candidate_edges){
                int from = edges[static_cast<std::size_t>(candidate)].from;
                int to = edges[static_cast<std::size_t>(candidate)].to;
                if(in_blossom[static_cast<std::size_t>(to)] == blossom) std::swap(from, to);
                const int to_blossom = in_blossom[static_cast<std::size_t>(to)];
                if(to_blossom != blossom && label[static_cast<std::size_t>(to_blossom)] == 1 &&
                   (best_to[static_cast<std::size_t>(to_blossom)] == -1 ||
                    slack(candidate) < slack(best_to[static_cast<std::size_t>(to_blossom)]))){
                    best_to[static_cast<std::size_t>(to_blossom)] = candidate;
                }
            }
            blossom_best_edges[static_cast<std::size_t>(sub_blossom)].clear();
            best_edge[static_cast<std::size_t>(sub_blossom)] = -1;
        }
        auto& blossom_edges = blossom_best_edges[static_cast<std::size_t>(blossom)];
        blossom_edges.clear();
        for(int candidate: best_to){
            if(candidate != -1) blossom_edges.push_back(candidate);
        }
        best_edge[static_cast<std::size_t>(blossom)] = -1;
        for(int candidate: blossom_edges){
            if(best_edge[static_cast<std::size_t>(blossom)] == -1 ||
               slack(candidate) < slack(best_edge[static_cast<std::size_t>(blossom)])){
                best_edge[static_cast<std::size_t>(blossom)] = candidate;
            }
        }
    }

    void expand_blossom(int blossom, bool end_stage){
        for(int sub_blossom: blossom_children[static_cast<std::size_t>(blossom)]){
            blossom_parent[static_cast<std::size_t>(sub_blossom)] = -1;
            if(sub_blossom < n){
                in_blossom[static_cast<std::size_t>(sub_blossom)] = sub_blossom;
            }else if(end_stage && dual[static_cast<std::size_t>(sub_blossom)] == 0){
                expand_blossom(sub_blossom, end_stage);
            }else{
                for(int leaf: leaves(sub_blossom)) in_blossom[static_cast<std::size_t>(leaf)] = sub_blossom;
            }
        }

        if(!end_stage && label[static_cast<std::size_t>(blossom)] == 2){
            const int entry_child = in_blossom[static_cast<std::size_t>(
                endpoint[static_cast<std::size_t>(label_end[static_cast<std::size_t>(blossom)] ^ 1)]
            )];
            auto& children = blossom_children[static_cast<std::size_t>(blossom)];
            auto& end_points = blossom_endpoints[static_cast<std::size_t>(blossom)];
            int index = child_index(children, entry_child);
            int step;
            int endpoint_trick;
            if(index & 1){
                index -= static_cast<int>(children.size());
                step = 1;
                endpoint_trick = 0;
            }else{
                step = -1;
                endpoint_trick = 1;
            }
            int remote_endpoint = label_end[static_cast<std::size_t>(blossom)];
            while(index != 0){
                label[static_cast<std::size_t>(endpoint[static_cast<std::size_t>(remote_endpoint ^ 1)])] = 0;
                const int bridge = cyclic(end_points, index - endpoint_trick) ^ endpoint_trick ^ 1;
                label[static_cast<std::size_t>(endpoint[static_cast<std::size_t>(bridge)])] = 0;
                assign_label(endpoint[static_cast<std::size_t>(remote_endpoint ^ 1)], 2, remote_endpoint);
                allowed_edge[static_cast<std::size_t>(cyclic(end_points, index - endpoint_trick) / 2)] = 1;
                index += step;
                remote_endpoint = cyclic(end_points, index - endpoint_trick) ^ endpoint_trick;
                allowed_edge[static_cast<std::size_t>(remote_endpoint / 2)] = 1;
                index += step;
            }
            int sub_blossom = cyclic(children, index);
            label[static_cast<std::size_t>(endpoint[static_cast<std::size_t>(remote_endpoint ^ 1)])] = 2;
            label[static_cast<std::size_t>(sub_blossom)] = 2;
            label_end[static_cast<std::size_t>(endpoint[static_cast<std::size_t>(remote_endpoint ^ 1)])] = remote_endpoint;
            label_end[static_cast<std::size_t>(sub_blossom)] = remote_endpoint;
            best_edge[static_cast<std::size_t>(sub_blossom)] = -1;
            index += step;
            while(cyclic(children, index) != entry_child){
                sub_blossom = cyclic(children, index);
                if(label[static_cast<std::size_t>(sub_blossom)] == 1){
                    index += step;
                    continue;
                }
                int reached_vertex = -1;
                for(int leaf: leaves(sub_blossom)){
                    if(label[static_cast<std::size_t>(leaf)] != 0){
                        reached_vertex = leaf;
                        break;
                    }
                }
                if(reached_vertex != -1){
                    label[static_cast<std::size_t>(reached_vertex)] = 0;
                    const int base = blossom_base[static_cast<std::size_t>(sub_blossom)];
                    label[static_cast<std::size_t>(endpoint[static_cast<std::size_t>(mate[static_cast<std::size_t>(base)])])] = 0;
                    assign_label(reached_vertex, 2, label_end[static_cast<std::size_t>(reached_vertex)]);
                }
                index += step;
            }
        }

        label[static_cast<std::size_t>(blossom)] = -1;
        label_end[static_cast<std::size_t>(blossom)] = -1;
        blossom_children[static_cast<std::size_t>(blossom)].clear();
        blossom_endpoints[static_cast<std::size_t>(blossom)].clear();
        blossom_base[static_cast<std::size_t>(blossom)] = -1;
        blossom_best_edges[static_cast<std::size_t>(blossom)].clear();
        best_edge[static_cast<std::size_t>(blossom)] = -1;
        unused_blossoms.push_back(blossom);
    }

    void augment_blossom(int blossom, int vertex){
        int sub_blossom = vertex;
        while(blossom_parent[static_cast<std::size_t>(sub_blossom)] != blossom){
            sub_blossom = blossom_parent[static_cast<std::size_t>(sub_blossom)];
        }
        if(sub_blossom >= n) augment_blossom(sub_blossom, vertex);
        auto& children = blossom_children[static_cast<std::size_t>(blossom)];
        auto& end_points = blossom_endpoints[static_cast<std::size_t>(blossom)];
        const int start = child_index(children, sub_blossom);
        int index = start;
        int step;
        int endpoint_trick;
        if(index & 1){
            index -= static_cast<int>(children.size());
            step = 1;
            endpoint_trick = 0;
        }else{
            step = -1;
            endpoint_trick = 1;
        }
        while(index != 0){
            index += step;
            sub_blossom = cyclic(children, index);
            const int remote_endpoint = cyclic(end_points, index - endpoint_trick) ^ endpoint_trick;
            if(sub_blossom >= n) augment_blossom(sub_blossom, endpoint[static_cast<std::size_t>(remote_endpoint)]);
            index += step;
            sub_blossom = cyclic(children, index);
            if(sub_blossom >= n) augment_blossom(sub_blossom, endpoint[static_cast<std::size_t>(remote_endpoint ^ 1)]);
            mate[static_cast<std::size_t>(endpoint[static_cast<std::size_t>(remote_endpoint)])] = remote_endpoint ^ 1;
            mate[static_cast<std::size_t>(endpoint[static_cast<std::size_t>(remote_endpoint ^ 1)])] = remote_endpoint;
        }
        rotate_to(children, start);
        rotate_to(end_points, start);
        blossom_base[static_cast<std::size_t>(blossom)] = blossom_base[static_cast<std::size_t>(children[0])];
    }

    void augment_matching(int edge){
        const int starts[2] = {
            edges[static_cast<std::size_t>(edge)].from,
            edges[static_cast<std::size_t>(edge)].to
        };
        const int endpoints[2] = {2 * edge + 1, 2 * edge};
        for(int side = 0; side < 2; ++side){
            int vertex = starts[side];
            int remote_endpoint = endpoints[side];
            while(true){
                const int vertex_blossom = in_blossom[static_cast<std::size_t>(vertex)];
                if(vertex_blossom >= n) augment_blossom(vertex_blossom, vertex);
                mate[static_cast<std::size_t>(vertex)] = remote_endpoint;
                if(label_end[static_cast<std::size_t>(vertex_blossom)] == -1) break;
                const int matched_vertex = endpoint[static_cast<std::size_t>(label_end[static_cast<std::size_t>(vertex_blossom)])];
                const int matched_blossom = in_blossom[static_cast<std::size_t>(matched_vertex)];
                vertex = endpoint[static_cast<std::size_t>(label_end[static_cast<std::size_t>(matched_blossom)])];
                const int blossom_vertex = endpoint[static_cast<std::size_t>(label_end[static_cast<std::size_t>(matched_blossom)] ^ 1)];
                if(matched_blossom >= n) augment_blossom(matched_blossom, blossom_vertex);
                mate[static_cast<std::size_t>(blossom_vertex)] = label_end[static_cast<std::size_t>(matched_blossom)];
                remote_endpoint = label_end[static_cast<std::size_t>(matched_blossom)] ^ 1;
            }
        }
    }

public:
    WeightedBlossom(int vertex_count, std::vector<WeightedEdge> input_edges)
        : n(vertex_count),
          edge_count(static_cast<int>(input_edges.size())),
          edges(std::move(input_edges)),
          endpoint(static_cast<std::size_t>(2 * edge_count)),
          neighbour_endpoints(static_cast<std::size_t>(n)),
          mate(static_cast<std::size_t>(n), -1),
          label(static_cast<std::size_t>(2 * n)),
          label_end(static_cast<std::size_t>(2 * n), -1),
          in_blossom(static_cast<std::size_t>(n)),
          blossom_parent(static_cast<std::size_t>(2 * n), -1),
          blossom_children(static_cast<std::size_t>(2 * n)),
          blossom_base(static_cast<std::size_t>(2 * n), -1),
          blossom_endpoints(static_cast<std::size_t>(2 * n)),
          best_edge(static_cast<std::size_t>(2 * n), -1),
          blossom_best_edges(static_cast<std::size_t>(2 * n)),
          dual(static_cast<std::size_t>(2 * n)),
          allowed_edge(static_cast<std::size_t>(edge_count)){
        Wide maximum_weight = 0;
        for(int edge = 0; edge < edge_count; ++edge){
            const auto& e = edges[static_cast<std::size_t>(edge)];
            endpoint[static_cast<std::size_t>(2 * edge)] = e.from;
            endpoint[static_cast<std::size_t>(2 * edge + 1)] = e.to;
            neighbour_endpoints[static_cast<std::size_t>(e.from)].push_back(2 * edge + 1);
            neighbour_endpoints[static_cast<std::size_t>(e.to)].push_back(2 * edge);
            if(maximum_weight < e.weight) maximum_weight = e.weight;
        }
        for(int vertex = 0; vertex < n; ++vertex){
            in_blossom[static_cast<std::size_t>(vertex)] = vertex;
            blossom_base[static_cast<std::size_t>(vertex)] = vertex;
            dual[static_cast<std::size_t>(vertex)] = maximum_weight;
        }
        for(int blossom = n; blossom < 2 * n; ++blossom) unused_blossoms.push_back(blossom);
    }

    std::vector<int> solve(bool maximize_cardinality = true){
        if(edge_count == 0) return mate;
        for(int stage = 0; stage < n; ++stage){
            std::fill(label.begin(), label.end(), 0);
            std::fill(best_edge.begin(), best_edge.end(), -1);
            for(int blossom = n; blossom < 2 * n; ++blossom){
                blossom_best_edges[static_cast<std::size_t>(blossom)].clear();
            }
            std::fill(allowed_edge.begin(), allowed_edge.end(), 0);
            queue.clear();
            for(int vertex = 0; vertex < n; ++vertex){
                if(mate[static_cast<std::size_t>(vertex)] == -1 &&
                   label[static_cast<std::size_t>(in_blossom[static_cast<std::size_t>(vertex)])] == 0){
                    assign_label(vertex, 1, -1);
                }
            }

            bool augmented = false;
            while(true){
                while(!queue.empty() && !augmented){
                    const int vertex = queue.back();
                    queue.pop_back();
                    for(int remote_endpoint: neighbour_endpoints[static_cast<std::size_t>(vertex)]){
                        const int edge = remote_endpoint / 2;
                        const int other = endpoint[static_cast<std::size_t>(remote_endpoint)];
                        if(in_blossom[static_cast<std::size_t>(vertex)] == in_blossom[static_cast<std::size_t>(other)]) continue;
                        Wide edge_slack = 0;
                        if(!allowed_edge[static_cast<std::size_t>(edge)]){
                            edge_slack = slack(edge);
                            if(edge_slack <= 0) allowed_edge[static_cast<std::size_t>(edge)] = 1;
                        }
                        if(allowed_edge[static_cast<std::size_t>(edge)]){
                            if(label[static_cast<std::size_t>(in_blossom[static_cast<std::size_t>(other)])] == 0){
                                assign_label(other, 2, remote_endpoint ^ 1);
                            }else if(label[static_cast<std::size_t>(in_blossom[static_cast<std::size_t>(other)])] == 1){
                                const int base = scan_blossom(vertex, other);
                                if(base >= 0) add_blossom(base, edge);
                                else{
                                    augment_matching(edge);
                                    augmented = true;
                                    break;
                                }
                            }else if(label[static_cast<std::size_t>(other)] == 0){
                                label[static_cast<std::size_t>(other)] = 2;
                                label_end[static_cast<std::size_t>(other)] = remote_endpoint ^ 1;
                            }
                        }else if(label[static_cast<std::size_t>(in_blossom[static_cast<std::size_t>(other)])] == 1){
                            const int blossom = in_blossom[static_cast<std::size_t>(vertex)];
                            if(best_edge[static_cast<std::size_t>(blossom)] == -1 ||
                               edge_slack < slack(best_edge[static_cast<std::size_t>(blossom)])){
                                best_edge[static_cast<std::size_t>(blossom)] = edge;
                            }
                        }else if(label[static_cast<std::size_t>(other)] == 0){
                            if(best_edge[static_cast<std::size_t>(other)] == -1 ||
                               edge_slack < slack(best_edge[static_cast<std::size_t>(other)])){
                                best_edge[static_cast<std::size_t>(other)] = edge;
                            }
                        }
                    }
                }
                if(augmented) break;

                int delta_type = -1;
                Wide delta = 0;
                int delta_edge = -1;
                int delta_blossom = -1;
                if(!maximize_cardinality){
                    delta_type = 1;
                    delta = dual[0];
                    for(int vertex = 1; vertex < n; ++vertex){
                        if(dual[static_cast<std::size_t>(vertex)] < delta){
                            delta = dual[static_cast<std::size_t>(vertex)];
                        }
                    }
                }
                for(int vertex = 0; vertex < n; ++vertex){
                    if(label[static_cast<std::size_t>(in_blossom[static_cast<std::size_t>(vertex)])] == 0 &&
                       best_edge[static_cast<std::size_t>(vertex)] != -1){
                        const Wide candidate = slack(best_edge[static_cast<std::size_t>(vertex)]);
                        if(delta_type == -1 || candidate < delta){
                            delta = candidate;
                            delta_type = 2;
                            delta_edge = best_edge[static_cast<std::size_t>(vertex)];
                        }
                    }
                }
                for(int blossom = 0; blossom < 2 * n; ++blossom){
                    if(blossom_parent[static_cast<std::size_t>(blossom)] == -1 &&
                       label[static_cast<std::size_t>(blossom)] == 1 &&
                       best_edge[static_cast<std::size_t>(blossom)] != -1){
                        const Wide candidate = slack(best_edge[static_cast<std::size_t>(blossom)]) / 2;
                        if(delta_type == -1 || candidate < delta){
                            delta = candidate;
                            delta_type = 3;
                            delta_edge = best_edge[static_cast<std::size_t>(blossom)];
                        }
                    }
                }
                for(int blossom = n; blossom < 2 * n; ++blossom){
                    if(blossom_base[static_cast<std::size_t>(blossom)] >= 0 &&
                       blossom_parent[static_cast<std::size_t>(blossom)] == -1 &&
                       label[static_cast<std::size_t>(blossom)] == 2 &&
                       (delta_type == -1 || dual[static_cast<std::size_t>(blossom)] < delta)){
                        delta = dual[static_cast<std::size_t>(blossom)];
                        delta_type = 4;
                        delta_blossom = blossom;
                    }
                }
                if(delta_type == -1){
                    delta_type = 1;
                    delta = dual[0];
                    for(int vertex = 1; vertex < n; ++vertex){
                        if(dual[static_cast<std::size_t>(vertex)] < delta) delta = dual[static_cast<std::size_t>(vertex)];
                    }
                    if(delta < 0) delta = 0;
                }

                for(int vertex = 0; vertex < n; ++vertex){
                    const int type = label[static_cast<std::size_t>(in_blossom[static_cast<std::size_t>(vertex)])];
                    if(type == 1) dual[static_cast<std::size_t>(vertex)] -= delta;
                    else if(type == 2) dual[static_cast<std::size_t>(vertex)] += delta;
                }
                for(int blossom = n; blossom < 2 * n; ++blossom){
                    if(blossom_base[static_cast<std::size_t>(blossom)] >= 0 &&
                       blossom_parent[static_cast<std::size_t>(blossom)] == -1){
                        if(label[static_cast<std::size_t>(blossom)] == 1) dual[static_cast<std::size_t>(blossom)] += delta;
                        else if(label[static_cast<std::size_t>(blossom)] == 2) dual[static_cast<std::size_t>(blossom)] -= delta;
                    }
                }

                if(delta_type == 1) break;
                if(delta_type == 2){
                    allowed_edge[static_cast<std::size_t>(delta_edge)] = 1;
                    int from = edges[static_cast<std::size_t>(delta_edge)].from;
                    int to = edges[static_cast<std::size_t>(delta_edge)].to;
                    if(label[static_cast<std::size_t>(in_blossom[static_cast<std::size_t>(from)])] == 0) std::swap(from, to);
                    queue.push_back(from);
                }else if(delta_type == 3){
                    allowed_edge[static_cast<std::size_t>(delta_edge)] = 1;
                    queue.push_back(edges[static_cast<std::size_t>(delta_edge)].from);
                }else{
                    expand_blossom(delta_blossom, false);
                }
            }
            if(!augmented) break;
            for(int blossom = n; blossom < 2 * n; ++blossom){
                if(blossom_parent[static_cast<std::size_t>(blossom)] == -1 &&
                   blossom_base[static_cast<std::size_t>(blossom)] >= 0 &&
                   label[static_cast<std::size_t>(blossom)] == 1 &&
                   dual[static_cast<std::size_t>(blossom)] == 0){
                    expand_blossom(blossom, true);
                }
            }
        }
        for(int vertex = 0; vertex < n; ++vertex){
            if(mate[static_cast<std::size_t>(vertex)] >= 0){
                mate[static_cast<std::size_t>(vertex)] = endpoint[static_cast<std::size_t>(mate[static_cast<std::size_t>(vertex)])];
            }
        }
        return mate;
    }
};

} // namespace minimum_weight_general_matching_internal
