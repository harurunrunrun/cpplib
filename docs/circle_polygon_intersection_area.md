---
title: Circle Polygon Intersection Area
documentation_of: ../src/algorithm/geometry/circle_polygon_intersection_area.hpp
---

円板と単純多角形の共通部分の面積を求める。多角形は時計回り、反時計回りのどちらでもよい。

```cpp
long double circle_polygon_intersection_area(
    const Circle& circle,
    const vector<Point>& polygon
)
```

符号付き面積が必要な場合は次を使う。

```cpp
long double signed_circle_polygon_intersection_area(
    const Circle& circle,
    const vector<Point>& polygon
)
```

## 計算量

$O(N)$。
