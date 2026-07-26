---
title: Convex Polygon Farthest Vertices Aggregator (凸多角形の最遠頂点集約ヘッダ)
documentation_of: ../../../../src/algorithm/geometry/2d/convex_polygon_farthest_vertices.hpp
---

正規化済み凸多角形入力と頂点列入力の最遠頂点APIをまとめる後方互換集約ヘッダ。

## 構成

| leaf header | 提供するoverload |
| --- | --- |
| `convex_polygon_farthest_vertices_normalized.hpp` | `convex_polygon_farthest_vertices(const NormalizedConvexPolygon&)` |
| `convex_polygon_farthest_vertices_points.hpp` | `convex_polygon_farthest_vertices(std::vector<Point>)` |

## 集約されるAPI

```cpp
std::vector<std::size_t> convex_polygon_farthest_vertices(
    const NormalizedConvexPolygon& polygon
);
std::vector<std::size_t> convex_polygon_farthest_vertices(
    std::vector<Point> polygon
);
```

返り値の要素 `result[i]` は正規化後の頂点 `i` から最も遠い頂点の添字である。

## API別の時間計算量・空間計算量

正規化後の頂点数を $N$ とする。

| overload | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| `NormalizedConvexPolygon` | $O(N)$ | $O(N)$ |
| `std::vector<Point>` | $O(N)$ | $O(N)$ |

## 注意点

- 0点では空列、1点では `{0}`、2点では `{1, 0}` を返す。
- 同距離なら対象頂点から反時計回りに最初に現れる頂点を選ぶ。
- 座標と距離計算は有限な `long double` の範囲に収まらなければならない。
