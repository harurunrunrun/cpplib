---
title: Convex Polyhedron Query Centered Direction (凸多面体クエリ用中心化方向)
documentation_of: ../../../../../../src/algorithm/geometry/3d/result/polyhedron/convex_polyhedron_query_hierarchy_3d_centered_direction.hpp
---

`ConvexPolyhedronQueryHierarchy3D` の内部メンバ関数の定義で、包含階層の放射方向を厳密な 2 進有理数（dyadic）演算で求める。

## API

- `ConvexPolyhedronQueryHierarchy3D::centered_direction(point)`: 頂点数を $V$、頂点座標の和を $S$ として、$V \cdot \mathrm{point} - S$（頂点重心からの方向を $V$ 倍して分母を払ったもの）を `ExactVector3` で返す private メンバ関数。構築処理から呼ばれる。

## API別の時間計算量・空間計算量

時間・追加空間ともに $O(1)$。ただし各座標について定数回の多倍長 dyadic 演算を行うため、ビット計算量は入力の指数差に依存する。

## 注意点

private メンバ関数の実装を分割したヘッダであり、利用側が直接呼び出す API ではない。通常は後方互換のための集約ヘッダをインクルードして `ConvexPolyhedronQueryHierarchy3D` を構築する。
