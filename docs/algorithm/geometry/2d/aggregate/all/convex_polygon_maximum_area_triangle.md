---
title: Convex Polygon Maximum Area Triangle Aggregator (凸多角形内の最大面積三角形集約ヘッダ)
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/convex_polygon_maximum_area_triangle.hpp
---

凸多角形の頂点から選べる最大面積の三角形に関する型と、2 種類の入力を受け取る API をまとめて読み込む互換の集約ヘッダである。既存の `#include` はそのまま使える。

## 構成

| leaf header | 提供する API |
| --- | --- |
| `convex_polygon_maximum_area_triangle_result.hpp` | `ConvexPolygonMaximumAreaTriangleResult` |
| `convex_polygon_maximum_area_triangle_normalized.hpp` | 正規化済み凸多角形を受け取るオーバーロード |
| `convex_polygon_maximum_area_triangle_points.hpp` | `std::vector<Point>` を受け取るオーバーロード |

依存を最小にしたい場合は、必要な leaf header だけを直接 include する。

## 使い方

```cpp
#include "../../../../../../src/algorithm/geometry/2d/aggregate/all/convex_polygon_maximum_area_triangle.hpp"

auto result = convex_polygon_maximum_area_triangle(polygon);
```

## 返り値

```cpp
struct ConvexPolygonMaximumAreaTriangleResult {
    std::array<std::size_t, 3> indices;
    long double doubled_area;
    bool valid() const;
    long double area() const;
};
```

`indices` は正規化後の頂点列に対する添字で、昇順に並ぶ。`doubled_area` は符号なし面積の 2 倍、`area()` は通常の面積を返す。面積が等しい三角形が複数あれば、（許容誤差の範囲で同じ面積のもののうち）`indices` が辞書順最小のものを返す。

## API

```cpp
ConvexPolygonMaximumAreaTriangleResult convex_polygon_maximum_area_triangle(const NormalizedConvexPolygon& polygon);
ConvexPolygonMaximumAreaTriangleResult convex_polygon_maximum_area_triangle(std::vector<Point> polygon);
```

第 1・第 2 の頂点を走査し、第 3 の頂点を単調に進める rotating calipers で求める。

## 退化した入力

0 点、1 点、2 点、全点が同一直線上の場合は、`valid() == false`、面積 $0$ で、各添字は `CONVEX_POLYGON_NPOS` になる。

## 計算量

正規化後の頂点数を $N$ とする。

| API | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| 正規化済みのオーバーロード | $O(N^2)$ | $O(1)$ |
| `std::vector<Point>` のオーバーロード | $O(N^2)$ | 正規化に $O(N)$ |
| `valid`, `area` | $O(1)$ | $O(1)$ |

## 注意点

座標と中間値は有限な `long double` の範囲に収まる必要がある。
