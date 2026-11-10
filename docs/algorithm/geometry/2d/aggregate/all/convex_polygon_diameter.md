---
title: Convex Polygon Diameter Aggregator (凸多角形の直径集約ヘッダ) [CGL_4_B]
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/convex_polygon_diameter.hpp
---

凸多角形の直径に関する型と、2 種類の入力を受け取る API をまとめて読み込む互換の集約ヘッダである。既存の `#include` はそのまま使える。

## 構成

| leaf header | 提供する API |
| --- | --- |
| `convex_polygon_diameter_result.hpp` | `ConvexPolygonDiameterResult` |
| `convex_polygon_diameter_normalized.hpp` | 正規化済み凸多角形を受け取るオーバーロード |
| `convex_polygon_diameter_points.hpp` | `std::vector<Point>` を受け取るオーバーロード |

依存を最小にしたい場合は、必要な leaf header だけを直接 include する。

## 使い方

凸多角形の最遠点対と直径を返す。

```cpp
#include "../../../../../../src/algorithm/geometry/2d/aggregate/all/convex_polygon_diameter.hpp"

ConvexPolygonDiameterResult result = convex_polygon_diameter(polygon);
```

## 返り値

```cpp
struct ConvexPolygonDiameterResult {
    std::size_t first, second;
    long double squared_distance;
    long double distance() const;
};
```

`first`, `second` は最遠点対の添字、`squared_distance` は距離の二乗、`distance()` は距離である。最遠点対が複数ある場合はいずれか 1 組を返す。

## API

```cpp
ConvexPolygonDiameterResult convex_polygon_diameter(const NormalizedConvexPolygon& polygon);
ConvexPolygonDiameterResult convex_polygon_diameter(std::vector<Point> polygon);
```

後者は弱凸な入力を正規化してから計算する。添字は正規化後の頂点列に対応する。

## 退化した入力

- 0 点: `first == second == CONVEX_POLYGON_NPOS`、距離 $0$。
- 1 点: `(first, second) == (0, 0)`、距離 $0$。
- 2 点、または全点が同一直線上: 正規化後の両端を返す。

## 計算量

正規化後の頂点数を $N$ とする。

| API | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| 正規化済みのオーバーロード | $O(N)$ | $O(N)$ |
| `std::vector<Point>` のオーバーロード | $O(N)$ | $O(N)$ |
| `ConvexPolygonDiameterResult::distance` | $O(1)$ | $O(1)$ |

各頂点からの最遠頂点を、暗黙の totally monotone 行列に対する SMAWK（`convex_polygon_farthest_vertices`）で求め、その最大を選ぶ。

## 注意点

座標と中間値は有限な `long double` の範囲に収まる必要がある。
