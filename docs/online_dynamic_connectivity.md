---
title: Online Dynamic Connectivity
documentation_of: ../src/structure/graph/online_dynamic_connectivity.hpp
---

一般無向グラフの辺追加・辺削除・連結性判定をオンラインに処理する。Holm--de Lichtenberg--Thorup のレベル構造を使い、各レベルの全域森を splay Euler Tour Tree で管理する。

木辺を削除したときは、分割された小さい側の同レベル木辺と非木辺を1レベル上げながら置換辺を探す。連結成分全体のDFSは行わない。

# テンプレート引数

```cpp
OnlineDynamicConnectivity<MAX_SIZE>
```

`MAX_SIZE` は頂点数の上限。Euler Tour Treeは各レベルにつき最大 `3 * MAX_SIZE` nodeを持ち、レベルは必要になった時点で確保される。各レベルの頂点nodeも、辺または操作で初めて使われた時点で遅延構築される。レベル数の上限は $\lceil\log_2(\mathtt{MAX\_SIZE})\rceil+1$。

単純辺の情報は `std::map` に保持するため、辺数の上限は固定しない。

# コンストラクタ

```cpp
OnlineDynamicConnectivity(int n = MAX_SIZE)
```

`n` 頂点の空グラフを作る。未構築の頂点nodeは孤立頂点として扱うため、全頂点を初期化しない。

## 時間計算量

- $O(\log \mathtt{MAX\_SIZE})$

HDLTレベルへの空pointer列だけを構築し、頂点nodeは遅延構築する。

# 辺の追加削除

```cpp
bool add_edge(int u, int v)
bool erase_edge(int u, int v)
bool link(int u, int v)
bool cut(int u, int v)
```

`add_edge` は辺を1本追加する。同じ端点の辺がすでに存在する場合は多重度だけを増やして `false`、新しい単純辺なら `true` を返す。自己ループも多重度を保持するが、連結成分には影響しない。

`erase_edge` は辺を1本削除する。辺が存在すれば `true`、存在しなければ `false` を返す。多重度が0になったときだけHDLTから単純辺を削除する。

`link` は `add_edge`、`cut` は `erase_edge` と同じ。

## 時間計算量

- amortized $O(\log^2 N + \log M)$

ここで $M$ は異なる端点対の数。

新しいHDLTレベルの作成は全頂点を走査せず、そのレベルで必要になった頂点だけを実体化する。このため、レベルの初回作成による単一操作中の追加 $O(N)$ は発生しない。

# 連結判定

```cpp
bool same(int u, int v)
int component_size(int v)
int groups()
int active_levels()
int edge_multiplicity(int u, int v)
```

`component_size` は `v` を含む連結成分の頂点数を返す。`groups` は連結成分数、`active_levels` は確保済みのHDLTレベル数、`edge_multiplicity` は端点対の多重度を返す。

## 時間計算量

- `same`: amortized $O(\log N)$
- `component_size`: amortized $O(\log N)$
- `groups`: $O(1)$
- `active_levels`: $O(\log N)$
- `edge_multiplicity`: $O(\log M)$
- `size`: $O(1)$

# 空間計算量

- $O(N\log N + M)$
