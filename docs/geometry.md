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

# 関数

```cpp
dot(a, b)
cross(a, b)
norm(p)
abs(p)
rotate90(p)
rotate(p, theta)
ccw(a, b, c)
projection(line, p)
reflection(line, p)
intersect(s, t)
distance(a, b)
distance_segment_point(s, p)
cross_point(a, b)
area(polygon)
contains(polygon, p)
is_convex(polygon)
convex_hull(points)
convex_diameter(convex_polygon)
convex_cut(polygon, line)
```

`contains` は外部なら `0`、辺上なら `1`、内部なら `2` を返す。

`ccw` は次を返す。

```cpp
CLOCKWISE = -1
COUNTER_CLOCKWISE = 1
ONLINE_BACK = 2
ONLINE_FRONT = -2
ON_SEGMENT = 0
```
