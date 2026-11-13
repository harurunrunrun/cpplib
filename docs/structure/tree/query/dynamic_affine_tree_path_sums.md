---
title: Dynamic Affine Tree Path Sums (辺写像更新付き木の任意根経路和) [point_set_tree_path_composite_sum] [point_set_tree_path_composite_sum_fixed_root]
documentation_of: ../../../../src/structure/tree/query/dynamic_affine_tree_path_sums.hpp
---

形が固定された木で、頂点値と辺の写像（一次関数）を更新しながら、全頂点の値を指定した根まで運んだ総和を求める。
`DynamicAffineTreePathSums<T>(values, edges)` で構築する。`edges` は `std::vector<AffineTreeEdge<T>>` で、
`AffineTreeEdge<T>{u, v, multiplier, addend}` の辺写像は両方向とも `x -> multiplier * x + addend` である。

## API

- `vertex_set(v, x)`: 頂点 `v` の値を `x` に置き換える。
- `edge_set(id, b, c)`: 登録順の辺 ID `id` の写像を `x -> b * x + c` に変更する。
- `path_sum(root = 0)`: 全頂点の値を、経路上の辺写像を順に適用しながら `root` へ運んだ総和を返す。
- `size()`: 頂点数を返す。

省略可能な第 3 コンストラクタ引数 `vertex_mass` を与えると、各始点 $y$ からの経路の寄与を
固定係数 `vertex_mass[y]` 倍できる。省略時はすべて 1 である。更新する値と `vertex_mass` は別物で、`vertex_mass` は構築後に変更できない。

## 時間計算量

$N$ を頂点数とする。構築は $O(N)$、各更新・取得は $O(\log^2(N+1))$、保存空間は $O(N)$ である。
heavy-light 分解の各 chain ごとに、上向き・下向きの二方向の一次式を集約する。

## 注意点

- 乗数 0 を許し、除算は使わない。辺の付け替えは行わない。
- 頂点・辺 ID が範囲外なら `out_of_range` を送出する。辺数が $N-1$（空なら 0）でない場合や `vertex_mass` の長さが頂点数と異なる場合は `invalid_argument` を送出する。初期の辺集合は木でなければならない。
- `T` は整数からの構築と加減乗算を提供し、すべての中間値を表現できる必要がある。空の木に対する `path_sum` は呼べない（`root` が範囲外になる）。
