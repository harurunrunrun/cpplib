---
title: Minimum Width of a Normalized Convex Polygon (正規化凸多角形の最小幅)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/polygon/convex_polygon_minimum_width_normalized.hpp
---

正規化済み凸多角形を平行な2支持直線で挟む幅の最小値をrotating calipersで求める。

## API

```cpp
long double convex_polygon_minimum_width(
    const NormalizedConvexPolygon& polygon
);
```

各辺と最遠頂点の距離を走査し、その最小値を返す。

## 時間計算量

頂点数を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| 正規化済みoverload | $O(N)$ | $O(1)$ |

## 注意点

- 3頂点未満では0を返す。
- `polygon` は有効な `NormalizedConvexPolygon` でなければならない。
- 面積差は値のscaleに対する相対許容誤差で比較する。
