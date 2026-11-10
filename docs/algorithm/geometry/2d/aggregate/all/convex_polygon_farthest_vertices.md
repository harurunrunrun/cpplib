---
title: Convex Polygon Farthest Vertices Aggregator (凸多角形の最遠頂点集約ヘッダ)
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/convex_polygon_farthest_vertices.hpp
---

正規化済み凸多角形を入力とする版と、頂点列を入力とする版の最遠頂点 API をまとめた後方互換の集約ヘッダである。

## 構成

| leaf header | 提供するオーバーロード |
| --- | --- |
| `convex_polygon_farthest_vertices_normalized.hpp` | `convex_polygon_farthest_vertices(const NormalizedConvexPolygon&)` |
| `convex_polygon_farthest_vertices_points.hpp` | `convex_polygon_farthest_vertices(std::vector<Point>)` |

## 集約される API

```cpp
std::vector<std::size_t> convex_polygon_farthest_vertices(const NormalizedConvexPolygon& polygon);
std::vector<std::size_t> convex_polygon_farthest_vertices(std::vector<Point> polygon);
```

返り値の要素 `result[i]` は、正規化後の頂点 `i` から最も遠い頂点の添字である。暗黙の totally monotone 行列に対する SMAWK で求める。

## 計算量

正規化後の頂点数を $N$ とする。

| オーバーロード | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| `NormalizedConvexPolygon` | $O(N)$ | $O(N)$ |
| `std::vector<Point>` | $O(N)$ | $O(N)$ |

## 注意点

- 0 点では空列、1 点では `{0}`、2 点では `{1, 0}` を返す。
- 同じ距離の頂点が複数あれば、対象の頂点から反時計回りに見て最初に現れる頂点を選ぶ。
- 座標と距離の計算は有限な `long double` の範囲に収まらなければならない。
