---
title: Geometry
documentation_of: ../src/algorithm/geometry/geometry.hpp
---

二次元幾何の基本関数。

# 型

```cpp
Point
Line
Segment
Circle
```

# 点・ベクトル

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

`ccw` は次を返す。

```cpp
CLOCKWISE = -1
COUNTER_CLOCKWISE = 1
ONLINE_BACK = 2
ONLINE_FRONT = -2
ON_SEGMENT = 0
```

# 直線・線分

```cpp
projection(line, p)
reflection(line, p)
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

# 多角形・凸包

```cpp
area(polygon)
contains(polygon, p)
contains_convex(polygon, p)
is_convex(polygon)
convex_hull(points)
convex_diameter(convex_polygon)
convex_cut(polygon, line)
closest_pair(points)
```

`contains` と `contains_convex` は外部なら `0`、辺上なら `1`、内部なら `2` を返す。

`contains_convex` は反時計回り凸多角形を渡す。

# 円

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

`circle_relation(a, b)` は次を返す。

```cpp
0: 一方が他方の内部
1: 内接
2: 2点で交わる
3: 外接
4: 離れている
```

## 時間計算量

- `convex_hull`: $O(N \log N)$
- `convex_diameter`: $O(N)$
- `convex_cut`: $O(N)$
- `contains_convex`: $O(\log N)$
- `closest_pair`: $O(N \log N)$
- その他: $O(1)$