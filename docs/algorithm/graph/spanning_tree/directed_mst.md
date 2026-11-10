---
title: Directed MST (有向最小全域木) [directedmst] [GRL_2_B]
documentation_of: ../../../../src/algorithm/graph/spanning_tree/directed_mst.hpp
---

最小有向全域木（最小全域有向木）を求める。根 `root` から全頂点へ到達する有向木のうち、コストが最小のものを求める。

## 関数

```cpp
template <class T>
DirectedMstResult<T> directed_mst(
    int n,
    int root,
    const std::vector<DirectedMstEdge<T>>& edges,
    T inf = T{}
)
```

`edges` は `DirectedMstEdge<T>{from, to, cost}` の列である。

戻り値 `DirectedMstResult<T>` は次のメンバをもつ。

```cpp
T cost;
bool exists;
std::vector<int> parent;
std::vector<int> parent_edge;
```

- `exists`: 有向全域木が存在しなければ `false`。
- `cost`: 最小コスト。
- `parent[v]`: 木における `v` の親。根では `root`。
- `parent_edge[v]`: 親からの辺の、元の辺配列での添字。根では `-1`。多重辺があっても採用した辺を正しく復元する。

第 4 引数 `inf` は互換性のために受け取るだけで使わず、省略できる。負の辺コストも扱える。有向全域木のコストと縮約時の辺コストは `T` で表現できなければならない。辺コストが `inf` 以上でも正しく扱える。

```cpp
template <class T>
DirectedMstRootChoiceResult<T> directed_mst_with_root_choices(
    int n,
    const std::vector<int>& allowed_roots,
    const std::vector<DirectedMstEdge<T>>& edges
)
```

許可された根の中でコストが最小になる木を選び、メンバ `root` と `tree`（`DirectedMstResult<T>`）を返す。根の候補が空、またはどの候補からも全域木が作れない場合は `root == -1`、`tree.exists == false` である。複数の配送拠点の候補から、最適な拠点と有向配送木を同時に選ぶような問題に使える。

## 時間計算量

$N$ を元の頂点数、$M$ を元の辺数、$K$ を根の候補の個数とする。

- `directed_mst`: 償却 $O((N + M)\log(N + M))$
- `directed_mst_with_root_choices`: $O(K(N + M)\log(N + M))$

流入辺を lazy pairing heap に保持し、閉路の縮約でヒープを併合する。元の各辺は高々 1 回削除し、縮約の履歴から採用した辺を復元する。ヒープと復元は再帰を使わない。

## 空間計算量

$O(N + M)$

## 注意点

`n` が正でない場合、`root`（または根の候補）や辺の端点が $[0, n)$ の範囲外の場合は `runtime_error` を送出する。

ヒープの縮約と復元は [上流の実装](https://github.com/yosupo06/library-checker-problems/blob/04c8de378bab67be926325de2871f0babb8e6451/graph/directedmst/sol/correct.cpp) を基にし、添字による管理と証明書（親・採用辺）の返却に変更した。Apache-2.0 ライセンスの全文はヘッダ末尾に記載している。
