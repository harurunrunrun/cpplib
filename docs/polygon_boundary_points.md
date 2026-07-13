---
title: Polygon Boundary Points
documentation_of: ../src/algorithm/geometry/polygon_boundary_points.hpp
---

Enumerates the given points that lie on the boundary of a polygon.

# Function

```cpp
enumerate_points_on_polygon_boundary(polygon, points, unique)

PolygonBoundaryPointQuery query(polygon)
query.uses_convex_fast_path()
query.enumerate(points, unique)
```

The return value contains indices into `points`. Polygon edges are processed in vertex order. Points on one edge are ordered from its first endpoint to its second endpoint. Zero-length edges are supported.

If `unique = true`, each point index occurs only at its first position. If `unique = false`, a point on a polygon vertex can occur once for each incident edge.

`PolygonBoundaryPointQuery` preserves the same edge order, order within each edge, and `unique` behavior. For a nondegenerate convex polygon it normalizes clockwise or counterclockwise weakly convex boundaries, associates every normalized side with the original edges, and uses the convex fast path. Consecutive duplicate vertices are supported.

If convexity, nondegeneracy, or the original-edge association cannot be verified, `uses_convex_fast_path()` is false and `enumerate` calls the general function. This fallback also covers nonconvex and all-collinear polygons.

# Complexity

$O(NM \log M)$ for $N$ polygon vertices and $M$ given points.

For the query class, let $K$ be the number of returned point-edge incidences.

- convex fast-path construction: $O(N \log N)$ time and $O(N)$ memory
- convex fast-path `enumerate`: $O(M \log N + K \log K)$ time and $O(N + M + K)$ memory
- fallback `enumerate`: $O(NM \log M)$ time
