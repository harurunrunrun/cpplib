---
title: Online Dynamic Connectivity Internal (オンライン動的連結性内部実装)
documentation_of: ../../../../src/structure/graph/dynamic_connectivity/online_dynamic_connectivity_internal.hpp
---

`OnlineDynamicConnectivity` が各レベルの全域森を管理するために使う、splay 木による Euler Tour Forest。

## 型

```cpp
online_dynamic_connectivity_internal::EulerTourForest<MAX_SIZE = -1>
```

`MAX_SIZE` は頂点数の上限。ノードは `std::vector` に必要に応じて追加し、削除した辺のノードは再利用する。
1 つのインスタンスが持つノード数は、辺向きのノードを含めて最大 $3 \cdot \texttt{MAX_SIZE}$（`MAX_SIZE` を省略した可変長版では $3n$）である。

```cpp
online_dynamic_connectivity_internal::EulerTourForest<> forest(n);
online_dynamic_connectivity_internal::EulerTourForest<200000> fixed(n);
```

## API

- `explicit EulerTourForest(int n)`: $n$ 頂点の空の森を作る。
- `bool same(int u, int v)`: $u$, $v$ が同じ木に属するかを返す。
- `int component_size(int v)`: $v$ を含む木の頂点数を返す。
- `void link(int u, int v, bool exact)`: 異なる木を辺 $(u, v)$ で結ぶ。`exact` が `true` なら、その辺を現在のレベル固有の木辺として印を付ける。
- `void cut(int u, int v)`: 木辺 $(u, v)$ を削除する。
- `void add_non_tree(int u, int v)`: 非木辺を候補集合に追加する。
- `void erase_non_tree(int u, int v)`: 非木辺を候補集合から削除する。
- `pair<int, int> pop_exact_tree(int v)`: $v$ の成分にある印付きの木辺を 1 本返し、印を外す。存在しなければ `{-1, -1}` を返す。
- `pair<int, int> any_non_tree(int v)`: $v$ の成分に接続する非木辺を 1 本返す。存在しなければ `{-1, -1}` を返す。

コピーは禁止されており、ムーブ構築とムーブ代入はできる。

## 時間計算量

頂点数を $N$、非木辺の数を $M$ とする。

- コンストラクタ: $O(1)$
- `same`、`component_size`、`link`、`cut`、`pop_exact_tree`、`any_non_tree`: 償却 $O(\log N)$（頂点ノードの検索に使う `std::map` の $O(\log N)$ を含む）
- `add_non_tree`、`erase_non_tree`: splay 操作に加えて `std::map` / `std::set` の $O(\log N)$
- ムーブ構築・ムーブ代入: $O(1)$

## 空間計算量

本体の保存領域は、実体化した頂点・木辺・非木辺の数に比例し、$O(N+M)$ である。

- `add_non_tree`: 本体の `std::map` / `std::set` に辺のエントリを $O(1)$ 個追加する。一時領域は $O(1)$
- それ以外の関数の追加領域: $O(1)$

## 注意点

- `OnlineDynamicConnectivity` の内部部品であり、通常は直接使わない。
- 頂点は $[0, n)$、$0 \le n \le \texttt{MAX_SIZE}$ でなければならない。頂点の範囲は上位クラスが検査する（コンストラクタの $n$ の範囲違反はここでも `std::runtime_error` を送出する）。
- `link` は両端が未接続であること、`cut` は木辺が存在すること、`erase_non_tree` は非木辺が存在することを要求する。内部状態の違反やノード容量の超過では `std::runtime_error` を送出する。
