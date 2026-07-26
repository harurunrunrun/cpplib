---
title: Point on a Convex Hull from Points (頂点列による凸包上の点判定)
documentation_of: ../../../../src/algorithm/geometry/2d/on_convex_hull_points.hpp
---

凸多角形の周回頂点列を前処理し、点が境界上にあるか判定する。

## API

```cpp
bool on_convex_hull(
    const std::vector<Point>& polygon,
    const Point& point
);
```

呼び出しごとに `ConvexPolygonQuery` を構築してquery overloadへ委譲する。

## 時間計算量

入力点数を $N$ とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| 頂点列overload | $O(N)$ | $O(N)$ |

## 注意点

入力は凸多角形の弱凸な周回列でなければならない。
同じ多角形への複数queryには `ConvexPolygonQuery` overloadを使う。
