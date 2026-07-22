#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_CONVEX_POLYHEDRON_FACETS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_CONVEX_POLYHEDRON_FACETS_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../predicate/adaptive_orient3d.hpp"
#include "../core/convex_polyhedron3.hpp"

struct ConvexPolyhedronFacet3{
    std::vector<std::size_t> boundary;
    std::vector<std::size_t> triangles;
};

namespace convex_polyhedron_facets_detail{

inline constexpr std::size_t no_index =
    std::numeric_limits<std::size_t>::max();

struct EdgeUse{
    std::size_t triangle = 0;
    std::size_t opposite = 0;
    std::size_t from = 0;
    std::size_t to = 0;
    std::size_t lower = 0;
    std::size_t upper = 0;
};

inline EdgeUse edge_use(
    std::size_t triangle,
    std::size_t opposite,
    std::size_t from,
    std::size_t to
){
    return {
        triangle, opposite, from, to,
        std::min(from, to), std::max(from, to)
    };
}

template<class Key>
void stable_counting_sort_edges(
    std::vector<EdgeUse>& edges,
    std::vector<EdgeUse>& buffer,
    std::size_t vertex_count,
    Key key
){
    std::vector<std::size_t> offset(vertex_count + 1, 0);
    for(const EdgeUse& edge: edges) ++offset[key(edge) + 1];
    for(std::size_t index = 1; index < offset.size(); ++index){
        offset[index] += offset[index - 1];
    }
    for(const EdgeUse& edge: edges){
        buffer[offset[key(edge)]++] = edge;
    }
    edges.swap(buffer);
}

inline void radix_sort_edges(
    std::vector<EdgeUse>& edges,
    std::size_t vertex_count
){
    std::vector<EdgeUse> buffer(edges.size());
    stable_counting_sort_edges(
        edges, buffer, vertex_count,
        [](const EdgeUse& edge){ return edge.upper; }
    );
    stable_counting_sort_edges(
        edges, buffer, vertex_count,
        [](const EdgeUse& edge){ return edge.lower; }
    );
}

inline bool same_edge(const EdgeUse& first, const EdgeUse& second){
    return first.lower == second.lower && first.upper == second.upper;
}

inline bool opposite_directions(const EdgeUse& first, const EdgeUse& second){
    return first.from == second.to && first.to == second.from;
}

}  // namespace convex_polyhedron_facets_detail

inline std::vector<ConvexPolyhedronFacet3> convex_polyhedron_facets(
    const ConvexPolyhedron3& polyhedron
){
    using namespace convex_polyhedron_facets_detail;
    if(polyhedron.affine_dimension < 2 || polyhedron.faces.empty()) return {};

    const std::size_t vertex_count = polyhedron.vertices.size();
    const std::size_t triangle_count = polyhedron.faces.size();
    if(triangle_count > std::vector<EdgeUse>().max_size() / 3)[[unlikely]]{
        throw std::length_error("convex polyhedron has too many triangle edges");
    }

    std::vector<EdgeUse> edge_uses;
    edge_uses.reserve(triangle_count * 3);
    for(std::size_t triangle = 0; triangle < triangle_count; ++triangle){
        const auto& face = polyhedron.faces[triangle];
        for(const std::size_t vertex: face){
            if(vertex >= vertex_count)[[unlikely]]{
                throw std::out_of_range("convex polyhedron face index");
            }
        }
        if(face[0] == face[1] || face[1] == face[2] || face[2] == face[0]){
            throw std::domain_error("degenerate convex polyhedron triangle");
        }
        for(std::size_t index = 0; index < 3; ++index){
            edge_uses.push_back(edge_use(
                triangle,
                face[(index + 2) % 3],
                face[index],
                face[(index + 1) % 3]
            ));
        }
    }
    radix_sort_edges(edge_uses, vertex_count);

    std::vector<std::vector<std::size_t>> coplanar_adjacency(triangle_count);
    for(std::size_t begin = 0; begin < edge_uses.size();){
        std::size_t end = begin + 1;
        while(end < edge_uses.size()
            && same_edge(edge_uses[begin], edge_uses[end])){
            ++end;
        }
        const std::size_t use_count = end - begin;
        if(use_count > 2
            || (polyhedron.affine_dimension >= 3 && use_count != 2))
            [[unlikely]]{
            throw std::domain_error("non-manifold convex polyhedron edge");
        }
        if(use_count == 2){
            const EdgeUse& first = edge_uses[begin];
            const EdgeUse& second = edge_uses[begin + 1];
            if(adaptive_orient3d(
                polyhedron.vertices[first.lower],
                polyhedron.vertices[first.upper],
                polyhedron.vertices[first.opposite],
                polyhedron.vertices[second.opposite]
            ) == 0){
                if(!opposite_directions(first, second))[[unlikely]]{
                    throw std::domain_error(
                        "inconsistently oriented facet triangles"
                    );
                }
                coplanar_adjacency[first.triangle].push_back(second.triangle);
                coplanar_adjacency[second.triangle].push_back(first.triangle);
            }
        }
        begin = end;
    }

    std::vector<std::size_t> component(triangle_count, no_index);
    std::vector<std::size_t> stack;
    std::size_t component_count = 0;
    for(std::size_t start = 0; start < triangle_count; ++start){
        if(component[start] != no_index) continue;
        component[start] = component_count;
        stack.push_back(start);
        while(!stack.empty()){
            const std::size_t triangle = stack.back();
            stack.pop_back();
            for(const std::size_t adjacent: coplanar_adjacency[triangle]){
                if(component[adjacent] != no_index) continue;
                component[adjacent] = component_count;
                stack.push_back(adjacent);
            }
        }
        ++component_count;
    }

    std::vector<std::vector<std::size_t>> component_triangles(component_count);
    for(std::size_t triangle = 0; triangle < triangle_count; ++triangle){
        component_triangles[component[triangle]].push_back(triangle);
    }
    std::vector<std::vector<EdgeUse>> component_boundary(component_count);
    for(std::size_t begin = 0; begin < edge_uses.size();){
        std::size_t end = begin + 1;
        while(end < edge_uses.size()
            && same_edge(edge_uses[begin], edge_uses[end])){
            ++end;
        }
        if(end - begin == 2
            && component[edge_uses[begin].triangle]
                == component[edge_uses[begin + 1].triangle]){
            begin = end;
            continue;
        }
        for(std::size_t index = begin; index < end; ++index){
            const EdgeUse& edge = edge_uses[index];
            component_boundary[component[edge.triangle]].push_back(edge);
        }
        begin = end;
    }

    std::vector<std::size_t> next(vertex_count, no_index);
    std::vector<std::size_t> previous(vertex_count, no_index);
    std::vector<std::size_t> touched;
    std::vector<ConvexPolyhedronFacet3> result;
    result.reserve(component_count);
    for(std::size_t identifier = 0; identifier < component_count; ++identifier){
        const std::vector<EdgeUse>& boundary_edges =
            component_boundary[identifier];
        if(boundary_edges.size() < 3)[[unlikely]]{
            throw std::domain_error("geometric facet has no boundary loop");
        }
        touched.clear();
        touched.reserve(boundary_edges.size());
        for(const EdgeUse& edge: boundary_edges){
            if(next[edge.from] != no_index || previous[edge.to] != no_index)
                [[unlikely]]{
                throw std::domain_error(
                    "inconsistently oriented facet triangles"
                );
            }
            next[edge.from] = edge.to;
            previous[edge.to] = edge.from;
            touched.push_back(edge.from);
            touched.push_back(edge.to);
        }
        for(const std::size_t vertex: touched){
            if(next[vertex] == no_index || previous[vertex] == no_index)
                [[unlikely]]{
                throw std::domain_error("open geometric facet boundary");
            }
        }

        const std::size_t start = boundary_edges.front().from;
        std::vector<std::size_t> boundary;
        boundary.reserve(boundary_edges.size());
        std::size_t current = start;
        for(std::size_t step = 0; step < boundary_edges.size(); ++step){
            boundary.push_back(current);
            current = next[current];
            if(current == start){
                if(step + 1 != boundary_edges.size())[[unlikely]]{
                    throw std::domain_error(
                        "geometric facet has multiple boundary loops"
                    );
                }
                break;
            }
            if(step + 1 == boundary_edges.size())[[unlikely]]{
                throw std::domain_error(
                    "self-intersecting geometric facet boundary"
                );
            }
        }
        for(const std::size_t vertex: touched){
            next[vertex] = no_index;
            previous[vertex] = no_index;
        }
        result.push_back({
            std::move(boundary),
            std::move(component_triangles[identifier])
        });
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_QUERY_CONVEX_POLYHEDRON_FACETS_HPP_INCLUDED
