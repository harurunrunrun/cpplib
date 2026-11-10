---
title: Convex Polygon Tangents Aggregator (点から凸多角形への接線集約ヘッダ)
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/convex_polygon_tangents.hpp
---

外部の点から凸多角形へ引いた接線に関する型と、2 種類の入力を受け取る API をまとめて読み込む後方互換の集約ヘッダである。

## 構成

| leaf header | 提供する API |
| --- | --- |
| `convex_polygon_tangents_result.hpp` | `ConvexPolygonTangentResult` |
| `convex_polygon_tangents_normalized.hpp` | 正規化済み凸多角形と点を受け取るオーバーロード |
| `convex_polygon_tangents_points.hpp` | `std::vector<Point>` と点を受け取るオーバーロード |

依存を最小にしたい場合は、必要な leaf header だけを直接 include する。従来どおり `convex_polygon_tangents.hpp` を include すれば全 API を使える。

## 使い方

```cpp
#include "../../../../../../src/algorithm/geometry/2d/aggregate/all/convex_polygon_tangents.hpp"

ConvexPolygonTangentResult result = convex_polygon_tangents(polygon, point);
```

## 返り値

```cpp
struct ConvexPolygonTangentResult {
    std::size_t left;
    std::size_t right;
};
```

有向直線 `point -> vertices()[left]` に対して多角形全体が左側に、`point -> vertices()[right]` に対して多角形全体が右側にある。すなわち多角形の任意の点 `p` について次が成り立つ。

```cpp
cross(vertices()[left] - point, p - point) >= 0
cross(vertices()[right] - point, p - point) <= 0
```

支持直線が辺と重なる場合は、条件を満たす端点のうち小さい添字を返す。

## API

```cpp
ConvexPolygonTangentResult convex_polygon_tangents(const NormalizedConvexPolygon& polygon, const Point& point);
ConvexPolygonTangentResult convex_polygon_tangents(std::vector<Point> polygon, const Point& point);
```

空でない凸集合に対して、`point` は集合の外部になければならない。内部または境界上にある場合は `std::invalid_argument` を送出する。

## 退化した入力

- 0 点: `left == right == CONVEX_POLYGON_NPOS`。
- 1 点: 外部の点からの左右の接点はどちらも $0$。
- 2 点、または全点が同一直線上: 通常は両端が左右の接点になる。外部の点がその直線上にあって両端が同等なら、小さい添字 $0$ を左右とも返す。

## 計算量

正規化後の頂点数を $N$ とする。

| API | 時間計算量 | 追加の空間計算量 |
| --- | --- | --- |
| 正規化済みのオーバーロード | $O(\log N)$ | $O(1)$ |
| `std::vector<Point>` のオーバーロード | $O(N)$ | $O(N)$ |

外部の点かどうかの判定と左右の接点の探索は、どちらも凸性を使った二分探索である。

## 注意点

座標と中間値は有限な `long double` の範囲に収まる必要がある。
