#ifndef CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_INDEX_SET_POLYHEDRON_POLYHEDRON_FACE_ADJACENCY_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_INDEX_SET_POLYHEDRON_POLYHEDRON_FACE_ADJACENCY_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <cstddef>
#include <stdexcept>
#include <vector>

#include "../../type/definition/polyhedron3.hpp"
#include "../../void/polyhedron/polyhedron3_validate.hpp"

namespace polyhedron_face_adjacency_detail{

struct Incident{
    std::size_t lower;
    std::size_t upper;
    std::size_t face;
    std::size_t edge;
};

template<class Key>
inline void counting_sort(
    std::vector<Incident>& incidents,
    std::vector<Incident>& buffer,
    std::size_t vertex_count,
    Key key
){
    std::vector<std::size_t> offset(vertex_count + 1);
    for(const Incident& incident: incidents) ++offset[key(incident) + 1];
    for(std::size_t index = 1; index < offset.size(); ++index){
        offset[index] += offset[index - 1];
    }
    for(const Incident& incident: incidents){
        buffer[offset[key(incident)]++] = incident;
    }
    incidents.swap(buffer);
}

}  // namespace polyhedron_face_adjacency_detail

inline std::vector<std::array<std::size_t, 3>> polyhedron_face_adjacency(
    const Polyhedron3& polyhedron
){
    using namespace polyhedron_face_adjacency_detail;
    geometry3d_validate(polyhedron);
    const std::size_t sentinel = polyhedron.faces.size();
    std::vector<std::array<std::size_t, 3>> result(
        polyhedron.faces.size(), {sentinel, sentinel, sentinel}
    );
    std::vector<Incident> incidents;
    incidents.reserve(polyhedron.faces.size() * 3);
    for(std::size_t face_index = 0; face_index < polyhedron.faces.size(); ++face_index){
        const auto& face = polyhedron.faces[face_index];
        for(std::size_t edge = 0; edge < 3; ++edge){
            const std::size_t first = face[edge];
            const std::size_t second = face[(edge + 1) % 3];
            incidents.push_back({
                std::min(first, second), std::max(first, second),
                face_index, edge,
            });
        }
    }
    std::vector<Incident> buffer(incidents.size());
    counting_sort(incidents, buffer, polyhedron.vertices.size(),
        [](const Incident& incident){ return incident.upper; });
    counting_sort(incidents, buffer, polyhedron.vertices.size(),
        [](const Incident& incident){ return incident.lower; });
    for(std::size_t begin = 0; begin < incidents.size();){
        std::size_t end = begin + 1;
        while(end < incidents.size()
            && incidents[end].lower == incidents[begin].lower
            && incidents[end].upper == incidents[begin].upper){
            ++end;
        }
        if(end - begin > 2)[[unlikely]]{
            throw std::domain_error("non-manifold polyhedron edge");
        }
        if(end - begin == 2){
            const Incident& first = incidents[begin];
            const Incident& second = incidents[begin + 1];
            result[first.face][first.edge] = second.face;
            result[second.face][second.edge] = first.face;
        }
        begin = end;
    }
    return result;
}

#endif  // CPPLIB_SRC_ALGORITHM_GEOMETRY_3D_INDEX_SET_POLYHEDRON_POLYHEDRON_FACE_ADJACENCY_HPP_INCLUDED
