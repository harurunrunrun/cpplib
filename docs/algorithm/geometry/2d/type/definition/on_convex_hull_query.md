---
title: Point on a Preprocessed Convex Hull (前処理済み凸包上の点判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/on_convex_hull_query.hpp
---

前処理済みの凸多角形 `ConvexPolygonQuery` に対し、点が境界上にあるかを判定する。

## API

```cpp
bool on_convex_hull(
    const ConvexPolygonQuery& polygon,
    const Point& point
);
```

`polygon.contains(point)` が境界を表す $1$ を返す場合だけ `true` を返す。

## 時間計算量

前処理後の頂点数を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `on_convex_hull(polygon, point)` | $O(\log N)$ | $O(1)$ |

## 注意点

`polygon` は有効な `ConvexPolygonQuery` でなければならない。
空集合では `false` を返し、点または線分に退化した集合では、その点・線分上にある点に対して `true` を返す。
