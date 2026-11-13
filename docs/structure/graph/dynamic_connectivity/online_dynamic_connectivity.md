---
title: Online Dynamic Connectivity (オンライン動的連結性)
documentation_of: ../../../../src/structure/graph/dynamic_connectivity/online_dynamic_connectivity.hpp
---

一般の無向グラフの辺の追加・辺の削除・連結性判定をオンラインに処理する。Holm--de Lichtenberg--Thorup（HDLT）のレベル構造を使い、各レベルの全域森を splay 木による Euler Tour Tree で管理する。

木辺を削除したときは、分割された小さい側にある同じレベルの木辺と非木辺を 1 レベルずつ上げながら置換辺を探す。連結成分全体の DFS は行わない。

## テンプレート引数

```cpp
OnlineDynamicConnectivity<MAX_SIZE = -1>
```

`MAX_SIZE` は頂点数の上限。省略（`-1`）すると、実行時の頂点数 $N$ からレベル数を計算する可変長版になる。

```cpp
OnlineDynamicConnectivity<> graph(n);
OnlineDynamicConnectivity<200000> fixed(n);
```

Euler Tour Tree は各レベルにつき最大 $3 \cdot \texttt{MAX_SIZE}$ 個（可変長版では $3N$ 個）のノードを持つ。レベルは必要になった時点で確保し、各レベルの頂点ノードも、辺や操作で初めて使われた時点で遅延構築する。
レベル数の上限は $\lceil\log_2 \texttt{MAX_SIZE}\rceil+1$ である。
単純辺の情報は `std::map` に保持するため、辺数の上限は固定しない。コピーはできず、ムーブはできる。

## コンストラクタ

```cpp
explicit OnlineDynamicConnectivity(int n = MAX_SIZE)
```

$n$ 頂点の空グラフを作る。未構築の頂点ノードは孤立頂点として扱うため、全頂点の初期化は行わない。

## 辺の追加・削除

```cpp
bool add_edge(int u, int v)
bool erase_edge(int u, int v)
bool link(int u, int v)
bool cut(int u, int v)
```

`add_edge` は辺を 1 本追加する。同じ端点の辺が既に存在する場合は多重度だけを増やして `false` を、新しい単純辺なら `true` を返す。自己ループも多重度を保持するが、連結成分には影響しない。

`erase_edge` は辺を 1 本削除する。辺が存在すれば `true`、存在しなければ `false` を返す。多重度が $0$ になったときだけ HDLT から単純辺を削除する。

`link` は `add_edge` と、`cut` は `erase_edge` と同じである。

## 連結判定

```cpp
bool same(int u, int v) const
int component_size(int v) const
int groups() const
int active_levels() const
int edge_multiplicity(int u, int v) const
int size() const
```

`component_size` は $v$ を含む連結成分の頂点数を返す。`groups` は連結成分数、`active_levels` は確保済みの HDLT レベル数、`edge_multiplicity` は端点対の多重度、`size` は頂点数を返す。

## 時間計算量

$N$ を頂点数、$M$ を異なる端点対の数とする。

- コンストラクタ: レベルへのポインタ列だけを作るので $O(\log(N+1))$（固定長版は $O(\log \texttt{MAX_SIZE})$）
- `add_edge`、`erase_edge`、`link`、`cut`: 償却 $O(\log^2 N + \log M)$
- `same`、`component_size`: 償却 $O(\log N)$
- `groups`、`size`: $O(1)$
- `active_levels`: $O(\log N)$
- `edge_multiplicity`: $O(\log M)$

新しい HDLT レベルを作るときも全頂点は走査せず、そのレベルで必要になった頂点だけを実体化する。このため、レベルの初回作成によって 1 回の操作に $O(N)$ が加わることはない。

## 空間計算量

全体で $O(N\log(N+1) + M)$。

- コンストラクタ: レベルへのポインタ列 $O(\log(N+1))$ を本体に保存する。一時領域は $O(1)$
- `add_edge`、`link`: 辺のエントリと、必要なレベル・Euler Tour のノードを本体に保存する。総量は上の範囲に収まる。一時領域は $O(1)$
- `erase_edge`、`cut`: 置換辺の探索・レベルの引き上げの後も総量は同じ上界に収まる。一時領域は $O(1)$
- `same`、`component_size`、`groups`、`active_levels`、`edge_multiplicity`、`size`: 追加領域 $O(1)$

## 注意点

- $0 \le n \le \texttt{MAX_SIZE}$ で、すべての頂点引数は $[0, n)$ でなければならない。違反すると `std::runtime_error` を送出する。
- 自己ループと多重辺は記録するが、`groups`、`same`、`component_size` には影響しない。
- 内部のノード数が `int` の表現上限を超える場合も `std::runtime_error` を送出する。
