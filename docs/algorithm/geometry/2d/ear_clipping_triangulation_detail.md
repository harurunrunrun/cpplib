---
title: Polygon Triangulation Detail (多角形三角形分割の内部実装)
documentation_of: ../../../../src/algorithm/geometry/2d/ear_clipping_triangulation_detail.hpp
---

単純多角形の検証、共線点除去、単調分割、面復元、単調面三角形分割を提供する内部leaf。

## 内部API

```cpp
std::vector<EarClippingTriangle>
simple_polygon_triangulation_detail::triangulate(
    const std::vector<Point>& polygon
);
```

`ear_clipping_detail` は入力向きの符号判定、`simple_polygon_triangulation_detail` は
前処理から三角形生成までのhelper群を保持する。公開入口はpoints leafに置かれる。

## 時間計算量

入力頂点数を $N$ とする。

| 内部処理 | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| 単純性検査・単調分割・面復元 | $O(N\log N)$ | $O(N)$ |
| 共線点除去・全単調面の三角形分割 | 合計 $O(N)$ | $O(N)$ |
| `triangulate(polygon)` 全体 | $O(N\log N)$ | $O(N)$ |

## 注意点

detail namespace内のhelper名・引数は互換性を保証しない。
このleafは `ear_clipping_triangle.hpp` にだけ型依存し、公開wrapperや
`simple_polygon_triangulation.hpp` をincludeしないため循環依存を作らない。
