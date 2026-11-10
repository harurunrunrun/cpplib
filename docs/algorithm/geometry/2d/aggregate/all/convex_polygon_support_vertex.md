---
title: Convex Polygon Support Vertex Aggregator (凸多角形の支持点集約ヘッダ)
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/convex_polygon_support_vertex.hpp
---

正規化済み凸多角形を入力とする版と、頂点列を入力とする版の支持点 API をまとめた後方互換の集約ヘッダである。

## 構成

| leaf header | 提供するオーバーロード |
| --- | --- |
| `convex_polygon_support_vertex_normalized.hpp` | `convex_polygon_support_vertex(const NormalizedConvexPolygon&, direction)` |
| `convex_polygon_support_vertex_points.hpp` | `convex_polygon_support_vertex(std::vector<Point>, direction)` |

## 集約される API

```cpp
std::size_t convex_polygon_support_vertex(const NormalizedConvexPolygon& polygon, const Point& direction);
std::size_t convex_polygon_support_vertex(std::vector<Point> polygon, const Point& direction);
```

指定した方向 `direction` との内積を最大にする頂点の、正規化後の添字を返す。同率なら小さい添字を選ぶ。正規化済みの版は、辺の偏角の列を二分探索して求める。

## 計算量

頂点数を $N$ とする。

| オーバーロード | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| `NormalizedConvexPolygon` | $O(\log N)$ | $O(1)$ |
| `std::vector<Point>` | $O(N)$ | $O(N)$ |

## 注意点

- 空集合では `CONVEX_POLYGON_NPOS` を返す。零ベクトルの方向に対しては、空でなければ $0$ を返す。
- 頂点列のオーバーロードに凸でない入力を与えると `std::invalid_argument` を送出する。
