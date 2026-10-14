---
title: Affine Tree Path Sums (辺アフィン写像の全頂点経路和) [tree_path_composite_sum]
documentation_of: ../../../../src/algorithm/tree/dynamic_programming/affine_path_sums.hpp
---

`AffineTreeEdge<T>{u,v,multiplier,addend}` は両方向に同じ写像 `x -> multiplier*x+addend` を適用する辺。
`affine_path_sums(values,edges)` は、各根について全頂点の値を根へ運んだ総和を返す。
辺の写像は値のある頂点から根へ向かう順に適用する。零の乗数も許す。
任意の第 3 引数 `vertex_mass` を与えると各始点 y の寄与を `vertex_mass[y]` 倍する。
指定しない場合は全て 1。零や負の mass も値型が表現できれば使える。

## 時間計算量

頂点数 $N$ に対して $O(N)$ 時間、$O(N)$ 空間。再帰を使わない rerooting DP。

## 注意点

`edges` は頂点集合 `[0,N)` 上の木。空木は空配列を返す。
辺数・連結性・閉路に違反すると `invalid_argument`、端点範囲外は `out_of_range`。
`T` は整数からの構築と加減乗算を提供する。全中間値が表現できること。
