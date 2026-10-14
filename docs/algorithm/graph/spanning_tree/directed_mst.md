---
title: Directed MST (有向最小全域木) [directedmst] [GRL_2_B]
documentation_of: ../../../../src/algorithm/graph/spanning_tree/directed_mst.hpp
---

最小有向全域木。rootから全頂点へ到達する有向木の最小コストを求める。

## 関数

```cpp
directed_mst<T>(n, root, edges, inf)
```

`edges` は `DirectedMstEdge<T>{from, to, cost}` の列。

`DirectedMstResult<T>` は次を持つ。

```cpp
T cost;
bool exists;
vector<int> parent;
vector<int> parent_edge;
```

存在しない場合 `exists` は `false`。存在する場合 `parent[v]` は親頂点、`parent_edge[v]` は元の辺配列での番号で、根ではそれぞれ `root` と `-1`。辺の復元は多重辺にも対応する。第4引数 `inf` は互換性のため受け取るが使用せず、省略できる。負の辺コストも扱う。

有向全域木のコストと縮約時の辺コストは `T` で表現できる必要がある。辺コストが `inf` 以上でも扱える。

`directed_mst_with_root_choices(n, allowed_roots, edges)` は許可された根の中で最小コストになる木を選び、`root` と `tree` を返す。根が空または全候補で到達不能なら `tree.exists == false`。根の候補数を $K$ とすると $O(K(N+M)\log(N+M))$。複数の配送拠点候補から最適な拠点と有向配送木を同時に選ぶ類題に使える。

## 時間計算量

$N$ を元の頂点数、$M$ を元の辺数とする。

- `directed_mst`: $O((N+M)\log(N+M))$ 償却

流入辺を lazy pairing heap に保持し、閉路の縮約でヒープを併合する。元の各辺は高々1回削除し、縮約履歴から採用辺を復元する。ヒープと復元は再帰を使わない。

## 空間計算量

- $O(N+M)$

## 注意点

頂点引数と隣接リストの行き先は、各APIで定めた頂点範囲内でなければならない。違反時は `runtime_error` を送出する。記載した計算量には引数検査とResultの構築を含む。

ヒープの縮約と復元は [上流の実装](https://github.com/yosupo06/library-checker-problems/blob/04c8de378bab67be926325de2871f0babb8e6451/graph/directedmst/sol/correct.cpp) を基に、添字による管理と証明書の返却へ変更した。Apache-2.0 の全文はヘッダ末尾に記載している。
