---
title: Geometry
documentation_of: ../src/algorithm/geometry/geometry.hpp
---

Two-dimensional geometry with `long double` coordinates. Comparisons use the absolute tolerance `GEOMETRY_EPS = 1e-10` unless otherwise noted. `Point::operator<` is exact lexicographical order so that it can be used by standard sorting algorithms.

# Types

```cpp
Point
Line
Segment
Circle
```

`Segment` is an alias of `Line`. A line passed to a line operation must have distinct endpoints. A circle radius must be nonnegative.

# Points and vectors

```cpp
dot(a, b)
cross(a, b)
norm(p)
abs(p)
arg(p)
unit(p)
rotate90(p)
rotate(p, theta)
polar(r, theta)
ccw(a, b, c)
orthogonal(a, b)
parallel(a, b)
```

`unit` throws `std::invalid_argument` for a zero vector. `ccw` returns one of the following values.

```cpp
CLOCKWISE = -1
COUNTER_CLOCKWISE = 1
ONLINE_BACK = 2
ONLINE_FRONT = -2
ON_SEGMENT = 0
```

# Lines and segments

```cpp
projection(line, p)
reflection(line, p)
on_segment(segment, p)
intersect(s, t)
intersect(line, p)
same_line(a, b)
intersect_line_line(a, b)
intersect_line_segment(line, segment)
distance(a, b)
distance(line, p)
distance_segment_point(segment, p)
distance(s, t)
distance_line_line(a, b)
distance_line_segment(line, segment)
cross_point(a, b)
line_intersection(a, b)
segment_intersection(a, b)
```

`cross_point` and `line_intersection` return the unique intersection of two lines. They throw `std::domain_error` for parallel or coincident lines. `segment_intersection` returns the unique common point and throws `std::domain_error` if there is no intersection or the intersection is a segment. A degenerate segment is treated as a point; a degenerate line passed to a line operation causes `std::invalid_argument`.

# Polygons

```cpp
area(polygon)
contains(polygon, p)
contains_convex(polygon, p)
ConvexPolygonQuery query(polygon)
query.contains(p)
is_convex(polygon)
convex_hull(points)
convex_diameter(convex_polygon)
convex_cut(polygon, line)
closest_pair(points)
```

`area` is signed. `contains`, `contains_convex`, and `ConvexPolygonQuery::contains` return `0` outside, `1` on the boundary, and `2` inside. `ConvexPolygonQuery` accepts clockwise and counterclockwise convex polygons, removes consecutive duplicate and intermediate collinear vertices, and reduces an all-collinear polygon to its two endpoints. Use it when querying the same polygon more than once. `contains_convex` performs the same normalization for a single query.

`is_convex` and `convex_diameter` accept either orientation. `convex_diameter` supports collinear and repeated consecutive vertices. `convex_hull` returns a counterclockwise hull without intermediate collinear points. `convex_cut` keeps the left side of the directed line.

# Circles

```cpp
circle_relation(a, b)
intersect_circle_line(circle, line)
intersect_circle_segment(circle, segment)
intersect_circle_circle(a, b)
circle_line_cross_points(circle, line)
circle_segment_cross_points(circle, segment)
circle_circle_cross_points(a, b)
tangent_points(circle, p)
tangent_lines(circle, p)
common_tangents(a, b)
circumcenter(a, b, c)
circumcircle(a, b, c)
incenter(a, b, c)
incircle(a, b, c)
```

`circle_relation` returns:

```cpp
CIRCLE_INSIDE = 0
CIRCLE_INTERNALLY_TANGENT = 1
CIRCLE_INTERSECT = 2
CIRCLE_EXTERNALLY_TANGENT = 3
CIRCLE_SEPARATE = 4
CIRCLE_COINCIDENT = 5
```

`circle_circle_cross_points` and `common_tangents` throw `std::domain_error` for coincident circles because the result is infinite. `circumcenter` and `incenter` throw `std::invalid_argument` for a collinear triangle.

# Complexity

- `contains`: $O(N)$
- `contains_convex`: $O(N)$ including normalization
- `ConvexPolygonQuery` construction: $O(N)$ time and memory
- `ConvexPolygonQuery::contains`: $O(\log N)$
- `is_convex`: $O(N)$
- `convex_hull`: $O(N \log N)$
- `convex_diameter`: $O(N)$
- `convex_cut`: $O(N)$
- `closest_pair`: $O(N \log N)$
- other operations: $O(1)$, excluding output size