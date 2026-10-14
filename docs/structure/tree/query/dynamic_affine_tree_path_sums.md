---
title: Dynamic Affine Tree Path Sums (辺写像更新付き木の任意根経路和) [point_set_tree_path_composite_sum] [point_set_tree_path_composite_sum_fixed_root]
documentation_of: ../../../../src/structure/tree/query/dynamic_affine_tree_path_sums.hpp
---

`DynamicAffineTreePathSums<T>(values,edges)` は固定された木で辺写像・頂点値を更新する。
`AffineTreeEdge<T>` の辺写像は両方向とも `x -> multiplier*x+addend`。

- `vertex_set(v,x)`: 頂点の値を置き換える。
- `edge_set(id,b,c)`: 登録順の辺 ID の写像を `b*x+c` へ変更。
- `path_sum(root=0)`: 全頂点の値を root へ運んだ総和。
- `size()`。

任意の第 3 コンストラクタ引数 `vertex_mass` で、各始点 y の経路寄与を
固定係数 `vertex_mass[y]` 倍できる。省略時は全て 1。更新する値と mass は別で、mass は構築後に固定。

## 時間計算量

構築 $O(N)$、各更新・照会 $O(\log^2(N+1))$、保存空間 $O(N)$。
heavy-light 分解の chain ごとに上下二方向の一次式を集約する。

## 注意点

零の乗数を許し、除算を使わない。辺の付け替えは行わない。
頂点・辺の範囲外は `out_of_range`。初期辺集合は木であること。
`T` は整数からの構築と加減乗算を提供し、全中間値を表現できる必要がある。空木への和照会はできない。
