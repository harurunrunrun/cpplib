#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_POLYHEDRON_POLYHEDRON_POLYHEDRA_INTERSECT_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_POLYHEDRON_POLYHEDRON_POLYHEDRA_INTERSECT_HPP_INCLUDED

#include <algorithm>
#include <cstddef>
#include <numeric>
#include <utility>
#include <variant>
#include <vector>

#include "../../predicate/polyhedron_point/polyhedron_contains.hpp"
#include "../../result/triangle_triangle/triangle_triangle_intersection.hpp"
#include "../../type/definition/polyhedron3.hpp"
#include "../../void/polyhedron/polyhedron3_validate.hpp"

namespace polyhedra_intersect_detail{

struct BoxedFace{
    std::size_t face = 0;
    Point3 minimum{};
    Point3 maximum{};
};

struct Node{
    Point3 minimum{};
    Point3 maximum{};
    std::size_t begin = 0;
    std::size_t end = 0;
    int left = -1;
    int right = -1;
    bool leaf() const noexcept{ return left == -1; }
};

inline std::vector<BoxedFace> boxed_faces(const Polyhedron3& polyhedron){
    std::vector<BoxedFace> result;
    result.reserve(polyhedron.faces.size());
    for(std::size_t index = 0; index < polyhedron.faces.size(); ++index){
        const auto& face = polyhedron.faces[index];
        const Point3& first = polyhedron.vertices[face[0]];
        const Point3& second = polyhedron.vertices[face[1]];
        const Point3& third = polyhedron.vertices[face[2]];
        result.push_back({
            index,
            {std::min({first.x, second.x, third.x}),
             std::min({first.y, second.y, third.y}),
             std::min({first.z, second.z, third.z})},
            {std::max({first.x, second.x, third.x}),
             std::max({first.y, second.y, third.y}),
             std::max({first.z, second.z, third.z})},
        });
    }
    return result;
}

inline int build(
    std::vector<BoxedFace>& faces,
    std::vector<Node>& nodes,
    std::size_t begin,
    std::size_t end
){
    Node node;
    node.begin = begin;
    node.end = end;
    node.minimum = faces[begin].minimum;
    node.maximum = faces[begin].maximum;
    for(std::size_t index = begin + 1; index < end; ++index){
        node.minimum.x = std::min(node.minimum.x, faces[index].minimum.x);
        node.minimum.y = std::min(node.minimum.y, faces[index].minimum.y);
        node.minimum.z = std::min(node.minimum.z, faces[index].minimum.z);
        node.maximum.x = std::max(node.maximum.x, faces[index].maximum.x);
        node.maximum.y = std::max(node.maximum.y, faces[index].maximum.y);
        node.maximum.z = std::max(node.maximum.z, faces[index].maximum.z);
    }
    const int result = static_cast<int>(nodes.size());
    nodes.push_back(node);
    if(end - begin <= 8) return result;
    const long double extent_x = node.maximum.x - node.minimum.x;
    const long double extent_y = node.maximum.y - node.minimum.y;
    const long double extent_z = node.maximum.z - node.minimum.z;
    int axis = extent_y > extent_x ? 1 : 0;
    if((axis == 0 ? extent_x : extent_y) < extent_z) axis = 2;
    const std::size_t middle = begin + (end - begin) / 2;
    std::nth_element(
        faces.begin() + static_cast<std::ptrdiff_t>(begin),
        faces.begin() + static_cast<std::ptrdiff_t>(middle),
        faces.begin() + static_cast<std::ptrdiff_t>(end),
        [axis](const BoxedFace& left, const BoxedFace& right){
            const auto center = [axis](const BoxedFace& face){
                if(axis == 0) return face.minimum.x / 2.0L + face.maximum.x / 2.0L;
                if(axis == 1) return face.minimum.y / 2.0L + face.maximum.y / 2.0L;
                return face.minimum.z / 2.0L + face.maximum.z / 2.0L;
            };
            return center(left) < center(right);
        }
    );
    const int left = build(faces, nodes, begin, middle);
    const int right = build(faces, nodes, middle, end);
    nodes[static_cast<std::size_t>(result)].left = left;
    nodes[static_cast<std::size_t>(result)].right = right;
    return result;
}

inline bool boxes_intersect(const Node& first, const Node& second){
    return first.minimum.x <= second.maximum.x
        && second.minimum.x <= first.maximum.x
        && first.minimum.y <= second.maximum.y
        && second.minimum.y <= first.maximum.y
        && first.minimum.z <= second.maximum.z
        && second.minimum.z <= first.maximum.z;
}

inline Triangle3 triangle(const Polyhedron3& polyhedron, std::size_t face_index){
    const auto& face = polyhedron.faces[face_index];
    return {polyhedron.vertices[face[0]], polyhedron.vertices[face[1]],
        polyhedron.vertices[face[2]]};
}

class DisjointSet{
    std::vector<std::size_t> parent_;
public:
    explicit DisjointSet(std::size_t size): parent_(size){
        std::iota(parent_.begin(), parent_.end(), 0);
    }
    std::size_t root(std::size_t vertex){
        if(parent_[vertex] == vertex) return vertex;
        return parent_[vertex] = root(parent_[vertex]);
    }
    void unite(std::size_t first, std::size_t second){
        first = root(first);
        second = root(second);
        if(first != second) parent_[second] = first;
    }
};

inline std::vector<std::size_t> component_representatives(
    const Polyhedron3& polyhedron
){
    DisjointSet components(polyhedron.vertices.size());
    std::vector<bool> used(polyhedron.vertices.size());
    for(const auto& face: polyhedron.faces){
        used[face[0]] = used[face[1]] = used[face[2]] = true;
        components.unite(face[0], face[1]);
        components.unite(face[1], face[2]);
    }
    std::vector<bool> emitted(polyhedron.vertices.size());
    std::vector<std::size_t> result;
    for(std::size_t vertex = 0; vertex < polyhedron.vertices.size(); ++vertex){
        if(!used[vertex]) continue;
        const std::size_t root = components.root(vertex);
        if(!emitted[root]){
            emitted[root] = true;
            result.push_back(vertex);
        }
    }
    return result;
}

}  // namespace polyhedra_intersect_detail

inline bool polyhedra_intersect(
    const Polyhedron3& first,
    const Polyhedron3& second
){
    using namespace polyhedra_intersect_detail;
    geometry3d_validate(first);
    geometry3d_validate(second);
    if(first.faces.empty() || second.faces.empty()) return false;

    std::vector<BoxedFace> first_faces = boxed_faces(first);
    std::vector<BoxedFace> second_faces = boxed_faces(second);
    std::vector<Node> first_nodes;
    std::vector<Node> second_nodes;
    first_nodes.reserve(first_faces.size() * 2);
    second_nodes.reserve(second_faces.size() * 2);
    const int first_root = build(first_faces, first_nodes, 0, first_faces.size());
    const int second_root = build(second_faces, second_nodes, 0, second_faces.size());
    std::vector<std::pair<int, int>> stack{{first_root, second_root}};
    while(!stack.empty()){
        const auto [first_index, second_index] = stack.back();
        stack.pop_back();
        const Node& first_node = first_nodes[static_cast<std::size_t>(first_index)];
        const Node& second_node = second_nodes[static_cast<std::size_t>(second_index)];
        if(!boxes_intersect(first_node, second_node)) continue;
        if(first_node.leaf() && second_node.leaf()){
            for(std::size_t left = first_node.begin; left < first_node.end; ++left){
                for(std::size_t right = second_node.begin; right < second_node.end; ++right){
                    if(!std::holds_alternative<std::monostate>(
                        triangle_triangle_intersection(
                            triangle(first, first_faces[left].face),
                            triangle(second, second_faces[right].face)
                        )
                    )) return true;
                }
            }
            continue;
        }
        const bool split_first = !first_node.leaf() && (
            second_node.leaf()
            || first_node.end - first_node.begin >= second_node.end - second_node.begin
        );
        if(split_first){
            stack.push_back({first_node.left, second_index});
            stack.push_back({first_node.right, second_index});
        }else{
            stack.push_back({first_index, second_node.left});
            stack.push_back({first_index, second_node.right});
        }
    }

    for(const std::size_t vertex: component_representatives(first)){
        if(polyhedron_contains_detail::unchecked(second, first.vertices[vertex])){
            return true;
        }
    }
    for(const std::size_t vertex: component_representatives(second)){
        if(polyhedron_contains_detail::unchecked(first, second.vertices[vertex])){
            return true;
        }
    }
    return false;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_PREDICATE_POLYHEDRON_POLYHEDRON_POLYHEDRA_INTERSECT_HPP_INCLUDED
