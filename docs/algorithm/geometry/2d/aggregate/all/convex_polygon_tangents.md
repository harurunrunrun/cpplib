---
title: Convex Polygon Tangents Aggregator (点から凸多角形への接線集約ヘッダ)
documentation_of: ../../../../../../src/algorithm/geometry/2d/aggregate/all/convex_polygon_tangents.hpp
---

外点から凸多角形への接線に関する型と2種類の入力APIをまとめて読み込む
後方互換集約ヘッダ。

## 構成

| leaf header | 提供するAPI |
| --- | --- |
| `convex_polygon_tangents_result.hpp` | `ConvexPolygonTangentResult` |
| `convex_polygon_tangents_normalized.hpp` | 正規化済み凸多角形と点を受け取るoverload |
| `convex_polygon_tangents_points.hpp` | `vector<Point>` と点を受け取るoverload |

依存を最小にしたい場合は必要なleaf headerだけを直接includeする。従来の
`convex_polygon_tangents.hpp` は引き続き全APIを提供する。

## 集約されるAPI


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

有向直線 `point -> vertices()[left]` に対して多角形全体が左側、
`point -> vertices()[right]` に対して多角形全体が右側にある。

```cpp
cross(vertices()[left] - point, p - point) >= 0
cross(vertices()[right] - point, p - point) <= 0
```

support line が辺と重なる場合は、その条件を満たす端点のうち小さい添字を返す。

## API

```cpp
ConvexPolygonTangentResult convex_polygon_tangents(
    const NormalizedConvexPolygon& polygon,
    const Point& point
);

ConvexPolygonTangentResult convex_polygon_tangents(
    std::vector<Point> polygon,
    const Point& point
);
```

空でない凸集合に対し `point` は集合の外部でなければならない。内部または境界上では
`std::invalid_argument` を送出する。

## 退化入力

- 0点: `left == right == CONVEX_POLYGON_NPOS`。
- 1点: 外点からの左右接点はいずれも0。
- 2点または全点 collinear: 通常は両端が左右接点になる。外点が支持直線上にあり
  両端が同値なら、小さい添字0を左右とも返す。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| 正規化済み overload | $O(\log N)$ | $O(1)$ |
| `vector<Point>` overload | $O(N)$ | $O(N)$ |

外点判定と左右接点探索はいずれも凸性を使った二分探索である。

## 注意点

座標と中間演算は有限な `long double` の範囲に収まる必要がある。境界・退化判定には各APIで明記した許容誤差を用いる。
