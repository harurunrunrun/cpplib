---
title: Line-Convex Polygon Intersection Aggregator (直線と凸多角形の交差判定集約ヘッダ)
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/intersect_line_convex_polygon.hpp
---

正規化済み凸多角形を入力とする版と、頂点列を入力とする版の交差判定 API をまとめた後方互換の集約ヘッダである。

## 構成

| leaf header | 提供するオーバーロード |
| --- | --- |
| `intersect_line_convex_polygon_normalized.hpp` | `intersect_line_convex_polygon(line, const NormalizedConvexPolygon&)` |
| `intersect_line_convex_polygon_points.hpp` | `intersect_line_convex_polygon(line, std::vector<Point>)` |

## 集約される API

```cpp
bool intersect_line_convex_polygon(const Line& line, const NormalizedConvexPolygon& polygon);
bool intersect_line_convex_polygon(const Line& line, std::vector<Point> polygon);
```

無限直線と閉凸集合が、接触を含めて共有点を持つかどうかを判定する。

## 計算量

頂点数を $N$ とする。

| オーバーロード | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| `NormalizedConvexPolygon` | $O(\log N)$ | $O(1)$ |
| `std::vector<Point>` | $O(N)$ | $O(N)$ |

## 注意点

- 退化した直線には `std::invalid_argument` を送出する。空の凸集合には `false` を返す。
- 頂点列のオーバーロードに凸でない入力を与えると `std::invalid_argument` を送出する。
