---
title: Polygon Triangulation Detail (多角形三角形分割の内部実装)
documentation_of: ../../../../../../src/algorithm/geometry/2d/detail/numeric/ear_clipping_triangulation_detail.hpp
---

単純多角形の検証、共線点の除去、単調多角形への分割、面の復元、単調な面の三角形分割を提供する内部ヘッダ。

## 内部API

```cpp
std::vector<EarClippingTriangle>
simple_polygon_triangulation_detail::triangulate(
    const std::vector<Point>& polygon
);
```

名前空間 `ear_clipping_detail` は入力の向きの符号判定（`signed_area_sign`）を、`simple_polygon_triangulation_detail` は前処理から三角形生成までの補助関数群を持つ。
公開の入口は `ear_clipping_triangulation_points.hpp` に置かれている。

`triangulate` は次の順に処理する。

1. 連続する重複頂点をまとめ、非有限座標なら `std::invalid_argument` を送出する。
2. Bentley–Ottmann 法で隣接しない辺同士の交差を検出し、単純でなければ `std::invalid_argument` を送出する。
3. 共線な中間頂点を除き、向きを反時計回りにそろえる。3 頂点未満または面積 0 なら `std::invalid_argument` を送出する。
4. 平面走査で単調多角形に分割し、各面を三角形分割する。

## 時間計算量

入力頂点数を $N$ とする。

| 内部処理 | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| 単純性検査・単調分割・面の復元 | $O(N \log N)$ | $O(N)$ |
| 共線点除去・全単調面の三角形分割 | 合計 $O(N)$ | $O(N)$ |
| `triangulate(polygon)` 全体 | $O(N \log N)$ | $O(N)$ |

## 注意点

- 名前空間 detail 内の補助関数の名前・引数は互換性を保証しない。
- このヘッダがインクルードするのは `ear_clipping_triangle.hpp`、`advanced_convex_geometry_detail.hpp`、`bentley_ottmann_intersections.hpp` などの下位ヘッダだけであり、公開ラッパーや `simple_polygon_triangulation.hpp` はインクルードしないので循環依存は生じない。
