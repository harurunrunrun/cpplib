---
title: Support Vertex of a Normalized Convex Polygon (正規化凸多角形の支持点)
documentation_of: ../../../../../../src/algorithm/geometry/2d/index/point/convex_polygon_support_vertex_normalized.hpp
---

正規化済みの凸多角形から、指定方向との内積が最大になる頂点を求める。

## API

```cpp
std::size_t convex_polygon_support_vertex(
    const NormalizedConvexPolygon& polygon,
    const Point& direction
);
```

保存済みの単調な辺の偏角列 `edge_angles()` を二分探索し、見つかった位置の前後 2 頂点ずつを比較して最大の頂点を選ぶ。内積が同じなら小さい添字を返す。

## 時間計算量

頂点数を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| 正規化済み多角形版 | $O(\log N)$ | $O(1)$ |

## 注意点

- 空集合では `CONVEX_POLYGON_NPOS`、1 点では 0 を返す。
- 2 点では内積が大きい方の端点を返し、等しければ 0 を返す。
- 頂点数が 3 以上で、`direction` の長さが `GEOMETRY_EPS` 以下なら 0 を返す。
