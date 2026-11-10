---
title: Point on a Convex Hull from Points (頂点列による凸包上の点判定)
documentation_of: ../../../../../../src/algorithm/geometry/2d/predicate/point_set_point/on_convex_hull_points.hpp
---

凸多角形の周回頂点列を前処理し、点が境界上にあるか判定する。

## API

```cpp
bool on_convex_hull(
    const std::vector<Point>& polygon,
    const Point& point
);
```

呼び出しごとに `ConvexPolygonQuery` を構築し、それを受け取るオーバーロードに処理を委ねる。境界上（頂点・辺上）なら `true`、内部または外部なら `false` を返す。

## 時間計算量

入力点数を $N$ とする。

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `on_convex_hull(polygon, point)`（頂点列版） | $O(N)$ | $O(N)$ |

## 注意点

- 入力は凸多角形を表す弱凸な周回列でなければならない（凸性は検査しない）。
- 同じ多角形に対して複数回判定する場合は、`ConvexPolygonQuery` を一度構築してそれを受け取るオーバーロードを使う。
