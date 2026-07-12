---
title: Polygon Boundary Points
documentation_of: ../src/algorithm/geometry/polygon_boundary_points.hpp
---

Enumerates the given points that lie on the boundary of a polygon.

# Function

```cpp
enumerate_points_on_polygon_boundary(polygon, points, unique)
```

The return value contains indices into `points`. Polygon edges are processed in vertex order. Points on one edge are ordered from its first endpoint to its second endpoint. Zero-length edges are supported.

If `unique = true`, each point index occurs only at its first position. If `unique = false`, a point on a polygon vertex can occur once for each incident edge.

# Complexity

$O(NM \log M)$ for $N$ polygon vertices and $M$ given points.