---
title: Point on a Preprocessed Convex Hull (前処理済み凸包上の点判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/type/definition/on_convex_hull_query.hpp
---

前処理済み凸多角形queryに対し、点が境界上にあるか判定する。

## API

```cpp
bool on_convex_hull(
    const ConvexPolygonQuery& polygon,
    const Point& point
);
```

`polygon.contains(point)` が境界を表す1を返す場合だけtrueを返す。

## 時間計算量

前処理後の頂点数を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| query overload | $O(\log N)$ | $O(1)$ |

## 注意点

`polygon` は有効な `ConvexPolygonQuery` でなければならない。
空集合ではfalse、点または線分へ退化した集合ではその境界上に対してtrueを返す。
