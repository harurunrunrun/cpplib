---
title: Polygon Boundary Points (多角形の辺上の点列挙)
documentation_of: ../src/algorithm/geometry/2d/core/polygon_boundary_points.hpp
---

Enumerates the given points that lie on the boundary of a polygon.

## Function

```cpp
enumerate_points_on_polygon_boundary(polygon, points, unique)

PolygonBoundaryPointQuery query(polygon)
query.uses_convex_fast_path()
query.enumerate(points, unique)
```

The return value contains indices into `points`. Polygon edges are processed in vertex order. Points on one edge are ordered from its first endpoint to its second endpoint. Zero-length edges are supported.

If `unique = true`, each point index occurs only at its first position. If `unique = false`, a point on a polygon vertex can occur once for each incident edge.

`PolygonBoundaryPointQuery` preserves the same edge order, order within each edge, and `unique` behavior. For a nondegenerate convex polygon it normalizes clockwise or counterclockwise weakly convex boundaries, associates every normalized side with the original edges, and uses the convex fast path. Consecutive duplicate vertices are supported.

If convexity, nondegeneracy, or the original-edge association cannot be verified, `uses_convex_fast_path()` is false and `enumerate` calls the general function. This fallback also covers nonconvex and all-collinear polygons. Enumeration also falls back when tolerance makes the candidate-side set ambiguous.

## 計算量の概要

$O(NM+K\log(K+1))$ for $N$ polygon vertices, $M$ given points, and $K$ returned point-edge incidences.

For the query class, let $K$ be the number of returned point-edge incidences.

- convex fast-path construction: $O(N \log N)$ time and $O(N)$ memory
- convex fast-path `enumerate`: $O(M \log N + K \log K)$ time and $O(N + M + K)$ memory
- fallback `enumerate`: $O(NM+K\log(K+1))$ time

## API別の時間計算量・空間計算量

$N$ をpolygon辺数、$M$ を与えるpoint数、$K$ を返すpoint-edge incidence数とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `enumerate_points_on_polygon_boundary(...)` | $O(NM+K\log(K+1))$ | $O(M+K)$ |
| `PolygonBoundaryPointQuery(polygon)` | $O(N\log N)$（fast-path検証・前処理） | $O(N)$ |
| `uses_convex_fast_path()` | $O(1)$（metadata query） | $O(1)$ |
| `polygon()` | $O(1)$（内部vectorの `const&` を返す） | $O(1)$ |
| `enumerate(points,unique)`（convex fast path） | $O(M\log N+K\log(K+1))$ | $O(N+M+K)$ |
| `enumerate(points,unique)`（fallback） | $O(NM+K\log(K+1))$ | $O(M+K)$ |

constructorはfallbackを選ぶ場合もfast pathの検証を試みる。`polygon()` の戻り値を全列挙する場合は別途 $O(N)$。両enumerateは長さ $K$ のindex列を返す。

## 注意点

座標と中間演算は有限な `long double` の範囲に収まる必要がある。境界・退化判定には各APIで明記した許容誤差を用いる。
