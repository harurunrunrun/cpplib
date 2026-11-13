---
title: Dynamic Graph Connectivity (動的グラフ連結性) [DYNACON2]
documentation_of: ../../../../src/structure/graph/dynamic_connectivity/dynamic_graph_connectivity.hpp
---

一般の無向グラフについて、辺の追加・辺の削除・連結性の問い合わせをオンラインで扱う。内部では `OnlineDynamicConnectivity` を使う。

## API

### コンストラクタ

```cpp
DynamicGraphConnectivity<MAX_SIZE = -1> graph(n);
```

$n$ 個の孤立頂点からなるグラフを作る。`MAX_SIZE` は頂点数の上限で、省略（`-1`）すると `std::vector` を使う可変長版になる。

```cpp
DynamicGraphConnectivity<> graph(n);
DynamicGraphConnectivity<200000> fixed(n);
```

### add_edge

```cpp
bool add_edge(int left, int right)
```

辺を 1 本追加する。新しい端点対なら `true`、既存の端点対の多重度を増やした場合は `false` を返す。

### remove_edge

```cpp
bool remove_edge(int left, int right)
```

辺を 1 本削除する。削除できた場合は `true`、存在しない場合は `false` を返す。

### connected

```cpp
bool connected(int left, int right) const
```

2 頂点が連結かどうかを返す。

## 時間計算量

頂点数を $N$、異なる端点対の数を $M$ とする。

- コンストラクタ: $O(\log(N+1))$（固定長版は $O(\log \texttt{MAX_SIZE})$）
- `add_edge`、`remove_edge`: 償却 $O(\log^2 N+\log M)$
- `connected`: 償却 $O(\log N)$

## 空間計算量

$O(N\log(N+1)+M)$。

## 注意点

- 頂点数は `MAX_SIZE` 以下でなければならず、頂点は $[0, n)$ でなければならない。違反すると `std::runtime_error` を送出する。
- 自己ループは記録するが、連結性には影響しない。
- 多重辺は本数を管理し、削除は 1 本ずつ行う。
