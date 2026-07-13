---
title: Online Dynamic Connectivity Internal (オンライン動的連結性内部実装)
documentation_of: ../src/structure/graph/online_dynamic_connectivity_internal.hpp
---

`OnlineDynamicConnectivity` が各レベルの全域森を管理するために使う splay Euler Tour Forest。

## 型

```cpp
online_dynamic_connectivity_internal::EulerTourForest<MAX_SIZE>
```

`MAX_SIZE` は頂点数の上限。各インスタンスは辺向き node を含めて最大
`3 * MAX_SIZE` node を確保する。

## API

- `EulerTourForest(int n)`：`n` 頂点の空の森を作る。
- `bool same(int u, int v)`：`u`, `v` が同じ木に属するかを返す。
- `int component_size(int v)`：`v` を含む木の頂点数を返す。
- `void link(int u, int v, bool exact)`：異なる木を辺 `(u,v)` で結ぶ。`exact` はその辺を現在レベル固有の木辺として印付けする。
- `void cut(int u, int v)`：木辺 `(u,v)` を削除する。
- `void add_non_tree(int u, int v)`：非木辺を候補集合へ追加する。
- `void erase_non_tree(int u, int v)`：非木辺を候補集合から削除する。
- `pair<int,int> pop_exact_tree(int v)`：`v` の成分にある印付き木辺を1本返し、印を外す。存在しなければ `{-1,-1}`。
- `pair<int,int> any_non_tree(int v)`：`v` の成分に接続する非木辺を1本返す。存在しなければ `{-1,-1}`。

コピーは禁止され、move構築とmove代入に対応する。

## 時間計算量

頂点数を $N$ とする。constructorは $O(1)$。`same`, `component_size`, `link`,
`cut`, `pop_exact_tree`, `any_non_tree` はならし $O(\log N)$。
非木辺集合を変更する操作は splay 操作に加えて `std::map` / `std::set` の
$O(\log N)$ がかかる。保存領域は $O(\mathtt{MAX\_SIZE}+M)$。

## 空間計算量（API別の追加領域）

- constructor、`same`、`component_size`、`link`、`cut`、`pop_exact_tree`、`any_non_tree`: $O(1)$
- `add_non_tree`: 本体のmap/setへ辺entryを $O(1)$ 個追加し、一時領域は $O(1)$
- `erase_non_tree`: 保存entryを削除し、一時領域は $O(1)$
- move constructor / move assignment: $O(1)$

本体の保存領域は $O(\mathtt{MAX\_SIZE}+M)$ である。

## 注意点

- `OnlineDynamicConnectivity` の内部部品であり、通常は直接使用しない。
- 頂点は `[0,n)`、`0 <= n <= MAX_SIZE`。上位クラスがこの範囲を検査する。
- `link` は未接続な両端、`cut` は存在する木辺、`erase_non_tree` は存在する非木辺を要求する。内部状態またはnode容量の違反では `runtime_error`。
