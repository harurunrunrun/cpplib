---
title: Minimum Width of a Normalized Convex Polygon (正規化凸多角形の最小幅)
documentation_of: ../../../../../../src/algorithm/geometry/2d/scalar/polygon/convex_polygon_minimum_width_normalized.hpp
---

正規化済みの凸多角形を平行な 2 本の支持直線で挟んだときの幅の最小値を、rotating calipers で求める。

## API

```cpp
long double convex_polygon_minimum_width(
    const NormalizedConvexPolygon& polygon
);
```

各辺について、その辺の直線から最も遠い頂点までの距離を尺取りで求め、その最小値を返す。

## 時間計算量

頂点数を $N$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| 正規化済み版 `convex_polygon_minimum_width(polygon)` | $O(N)$ | $O(1)$ |

## 注意点

- 頂点数が $3$ 未満なら $0$ を返す。
- `polygon` は有効な `NormalizedConvexPolygon` でなければならない。
- 最遠頂点を進めるかどうかは、面積（外積の絶対値）の差を値の大きさに対する相対許容誤差で比較して決める。
