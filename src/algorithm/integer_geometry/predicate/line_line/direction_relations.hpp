#ifndef CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_PREDICATE_LINE_LINE_DIRECTION_RELATIONS_HPP_INCLUDED
#define CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_PREDICATE_LINE_LINE_DIRECTION_RELATIONS_HPP_INCLUDED

#include "../../vector/line/direction_line.hpp"
#include "../vector_vector/direction_relations.hpp"

namespace integer_geometry{

inline bool parallel(const Line& first, const Line& second){
    return parallel(direction(first), direction(second));
}

inline bool orthogonal(const Line& first, const Line& second){
    return orthogonal(direction(first), direction(second));
}

}  // namespace integer_geometry

#endif  // CPPLIB_SRC_ALGORITHM_INTEGER_GEOMETRY_PREDICATE_LINE_LINE_DIRECTION_RELATIONS_HPP_INCLUDED
