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
circle_circle_intersection_area(a, b)
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

`circle_circle_intersection_area` は2つの円盤の共通部分の面積を返す。離れている場合、
外接する場合、または少なくとも一方の半径が0の場合は0、包含・内接・同心円・
一致する円では小さい円盤の面積を返す。通常の2点交差では余弦を
$[-1,1]$ に収め、外接に近い小さな弓形には級数を用いる。
どちらかの半径が負なら `std::invalid_argument` を送出する。

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

## API別計算量

$N$ を入力polygon・point列の要素数、$H$ を返すhull頂点数とする。座標の算術・三角関数・平方根を $O(1)$ と数える。

| API | 時間計算量 | 出力・追加領域 |
| --- | --- | --- |
| `geometry_sign(x)` / `GEOMETRY_EPS` 参照 | $O(1)$ | $O(1)$ |
| `Point` の各constructor、単項・二項・複合代入・比較operator | $O(1)$ | $O(1)$ |
| `operator*(k,p)`、`Point` / `Line` / `Circle` のfield参照・aggregate構築 | $O(1)$ | $O(1)$ |
| `dot` / `cross` / `norm` / `abs` / `arg` | $O(1)$ | $O(1)$ |
| `unit` / `rotate90` / `rotate` / `polar` / `ccw` | $O(1)$ | $O(1)$ |
| `orthogonal` / `parallel`（Point・Line overload） | $O(1)$ | $O(1)$ |
| `projection` / `reflection` / `on_segment` | $O(1)$ | $O(1)$ |
| `intersect` の両overload / `same_line` / `intersect_line_line` / `intersect_line_segment` | $O(1)$ | $O(1)$ |
| `distance` の全overload / `distance_segment_point` / `distance_line_line` / `distance_line_segment` | $O(1)$ | $O(1)$ |
| `cross_point` / `line_intersection` / `segment_intersection` | $O(1)$ | $O(1)$ |
| `area(polygon)` / `contains(polygon,p)` / `is_convex(polygon)` | $O(N)$ | $O(1)$ |
| `ConvexPolygonQuery(polygon)` | $O(N)$（正規化前処理） | $O(N)$ |
| `ConvexPolygonQuery::size()` / `vertices()` | $O(1)$（metadata・参照返却） | $O(1)$ |
| `ConvexPolygonQuery::contains(p)` | $O(\log N)$ | $O(1)$ |
| `contains_convex(polygon,p)` | $O(N)$ | $O(N)$ |
| `convex_hull(points)` | $O(N\log N)$ | 出力 $O(H)$、作業領域 $O(N)$ |
| `convex_diameter(convex_polygon)` | $O(N)$ | $O(N)$ |
| `convex_cut(polygon,line)` | $O(N)$ | 出力 $O(N)$ |
| `closest_pair(points)` | $O(N\log N)$ | $O(N)$ |
| `validate_circle(circle)` / `circle_relation(a,b)` | $O(1)$ | $O(1)$ |
| `circle_circle_intersection_area(a,b)` | $O(1)$ | $O(1)$ |
| `intersect_circle_line` / `intersect_circle_segment` / `intersect_circle_circle` | $O(1)$ | $O(1)$ |
| `circle_line_cross_points` / `circle_segment_cross_points` / `circle_circle_cross_points` | $O(1)$ | 最大2点 |
| `tangent_points` / `tangent_lines` | $O(1)$ | 最大2要素 |
| `common_tangents(a,b)` | $O(1)$ | 最大4本 |
| `circumcenter` / `circumcircle` / `incenter` / `incircle` | $O(1)$ | $O(1)$ |

`vertices()` は内部vectorへの `const&` を返すだけで、全頂点を列挙する場合は別途 $O(N)$。circle系のvector出力は要素数が定数上限なので $O(1)$ に含める。
- point・line・circleの各操作: $O(1)$（polygon走査と列挙操作は上のAPI表を参照）
