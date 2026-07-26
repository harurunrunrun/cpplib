---
title: Support Vertex of a Normalized Convex Polygon (正規化凸多角形の支持点)
documentation_of: ../../../../src/algorithm/geometry/2d/convex_polygon_support_vertex_normalized.hpp
---

正規化済み凸多角形から指定方向との内積を最大化する頂点を求める。

## API

```cpp
std::size_t convex_polygon_support_vertex(
    const NormalizedConvexPolygon& polygon,
    const Point& direction
);
```

保存済みの単調な辺角度列を二分探索し、同率なら小さい添字を返す。

## 時間計算量

頂点数を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| 正規化済みoverload | $O(\log N)$ | $O(1)$ |

## 注意点

- 空集合では `CONVEX_POLYGON_NPOS`、1点では0を返す。
- 2点では内積が大きい端点を返し、同率なら0を返す。
- directionの長さが許容誤差以下なら、空でない限り0を返す。
