---
title: Euler Tour Forest (部分木加算対応 Euler Tour Forest) [dynamic_tree_subtree_add_subtree_sum] [dynamic_tree_vertex_add_subtree_sum]
documentation_of: ../../../../src/structure/tree/dynamic_forest/euler_tour_forest.hpp
---

動的な森のオイラーツアーを暗黙の treap で管理し、辺の追加・削除、連結成分・部分木単位の和と一様加算を扱う。
`EulerTourForest<T>(values)` は、`values[v]` を頂点 `v` の初期値とする孤立頂点からなる森を構築する。

## API

- `link(u, v)`: 別の連結成分に属する 2 頂点を辺で結び `true` を返す。既に同じ成分なら `false` を返す。
- `cut(u, v)`: 辺 `u-v` が存在すれば削除して `true` を、なければ `false` を返す。
- `connected(u, v)`: 2 頂点が同じ連結成分に属するかを返す。
- `component_sum(v)`, `component_add(v, x)`: `v` を含む連結成分の頂点値の和を返す / 全頂点に `x` を加える。
- `vertex_add(v, x)`: 頂点 `v` の値に `x` を加える。
- `subtree_sum(v, parent)`, `subtree_add(v, parent, x)`: 辺 `v-parent` を除いたときの `v` 側の部分木について、和を返す / 全頂点に `x` を加える。
- `size()`: 頂点数を返す。

## 時間計算量

$N$ を頂点数とする。構築は $O(N)$、各操作は乱数優先度とハッシュ表を用いて期待 $O(\log(N+1))$ である。保存空間は $O(N)$ である。
削除した辺のトークン（ノード）は再利用する。

## 注意点

- 頂点番号は `[0, N)` である。範囲外なら `out_of_range` を、部分木操作で指定した辺がなければ `invalid_argument` を送出する。
- 部分木操作は内部で一時的に辺を切り、同じ辺を戻す。木の根は固定しない。
- `T` は整数からの構築と加算・乗算・等値比較を提供し、すべての中間値を表現できる必要がある。
