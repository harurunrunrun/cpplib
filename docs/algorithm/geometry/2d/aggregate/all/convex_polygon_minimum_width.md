---
title: Convex Polygon Minimum Width Aggregator (凸多角形の最小幅集約ヘッダ)
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/convex_polygon_minimum_width.hpp
---

正規化済み凸多角形を入力とする版と、頂点列を入力とする版の最小幅 API をまとめた後方互換の集約ヘッダである。

## 構成

| leaf header | 提供するオーバーロード |
| --- | --- |
| `convex_polygon_minimum_width_normalized.hpp` | `convex_polygon_minimum_width(const NormalizedConvexPolygon&)` |
| `convex_polygon_minimum_width_points.hpp` | `convex_polygon_minimum_width(std::vector<Point>)` |

## 集約される API

```cpp
long double convex_polygon_minimum_width(const NormalizedConvexPolygon& polygon);
long double convex_polygon_minimum_width(std::vector<Point> polygon);
```

凸集合を平行な 2 本の支持直線で挟んだときの幅の最小値を返す。各辺について対蹠頂点を rotating calipers で進めて求める。

## 計算量

頂点数を $N$ とする。

| オーバーロード | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| `NormalizedConvexPolygon` | $O(N)$ | $O(1)$ |
| `std::vector<Point>` | $O(N)$ | $O(N)$ |

## 注意点

- 点または線分に退化した凸集合（正規化後の頂点数が $3$ 未満）の幅は $0$ である。
- 頂点列のオーバーロードに凸でない入力を与えると `std::invalid_argument` を送出する。
