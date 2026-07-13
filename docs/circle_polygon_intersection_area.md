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

## API別計算量

$N$ をpolygon頂点数とする。

| API | 時間計算量 | 追加空間 |
| --- | --- | --- |
| `signed_circle_polygon_intersection_area(circle,polygon)` | $O(N)$ | $O(1)$ |
| `circle_polygon_intersection_area(circle,polygon)` | $O(N)$ | $O(1)$ |

各辺と円の交点候補は高々2個であり、辺ごとの処理は $O(1)$。
