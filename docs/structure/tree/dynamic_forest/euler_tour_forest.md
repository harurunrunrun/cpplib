---
title: Euler Tour Forest (部分木加算対応 Euler Tour Forest) [dynamic_tree_subtree_add_subtree_sum] [dynamic_tree_vertex_add_subtree_sum]
documentation_of: ../../../../src/structure/tree/dynamic_forest/euler_tour_forest.hpp
---

`EulerTourForest<T>(values)` は孤立頂点からなる森を構築する。

- `link(u,v)` は別成分を接続し `true`、既に同じ成分なら `false`。
- `cut(u,v)` は存在する辺を削除し `true`、なければ `false`。
- `connected(u,v)`, `component_sum(v)`, `component_add(v,x)`。
- `vertex_add(v,x)` は頂点の値へ加算。
- `subtree_sum(v,parent)`, `subtree_add(v,parent,x)` は指定辺の v 側の部分木を扱う。
- `size()` は頂点数。

## 時間計算量

構築 $O(N)$、全操作は乱数優先度を用いて期待 $O(\log(N+1))$、保存空間 $O(N)$。
削除した辺 token は再利用する。

## 注意点

頂点は `[0,N)`。範囲外は `out_of_range`、部分木操作で指定辺がないと `invalid_argument`。
部分木操作は内部で一時的に辺を切り、同じ辺を戻す。木の根は固定しない。
`T` は整数からの構築と加算・乗算・等値比較を提供し、全中間値を表現できる必要がある。
