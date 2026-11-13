---
title: Edge Count Disjoint Set Union (辺数付き素集合データ構造)
documentation_of: ../../../../src/structure/dsu/basic/edge_count_dsu.hpp
---

辺の追加を扱う素集合データ構造。各連結成分について、頂点数と辺数を持つ。

## テンプレート引数

```cpp
EdgeCountDSU<MAX_SIZE = -1>
```

- `MAX_SIZE`: 頂点数の上限。省略（`-1`）すると、実行時の頂点数に合わせて `std::vector` を確保する可変長版になる。

```cpp
EdgeCountDSU<> dsu(n);
```

## コンストラクタ

```cpp
explicit EdgeCountDSU(int n = MAX_SIZE)
```

$n$ 頂点の空グラフを作る。可変長版では `n` のデフォルトは $0$ である。

## add_edge

```cpp
bool add_edge(int u, int v)
```

辺 $(u, v)$ を追加する。異なる連結成分が併合されたとき `true` を返す。
多重辺と自己ループも 1 本の辺として数える。

## same / leader

```cpp
bool same(int u, int v)
int leader(int v)
```

`same` は $u$ と $v$ が同じ連結成分に属するかを、`leader` は $v$ を含む成分の代表元を返す。

## 成分情報

```cpp
int component_size(int v)
int edges(int v)
int excess_edges(int v)
bool is_tree(int v)
bool is_unicyclic(int v)
int size() const
int groups() const
```

- `component_size(v)`: $v$ を含む成分の頂点数
- `edges(v)`: $v$ を含む成分の辺数
- `excess_edges(v)`: `edges(v) - component_size(v) + 1`
- `is_tree(v)`: 成分が木（辺数 $=$ 頂点数 $-1$）かどうか
- `is_unicyclic(v)`: 成分の辺数と頂点数が等しい（連結なので閉路をちょうど 1 つ持つ）かどうか
- `size()`: 頂点数、`groups()`: 連結成分数

## 時間計算量

- コンストラクタ: $O(n)$。固定長版では本体の配列の初期化に $O(\texttt{MAX_SIZE})$ かかる
- `add_edge`、`same`、`leader`、`component_size`、`edges`、`excess_edges`、`is_tree`、`is_unicyclic`: 償却 $O(\alpha(n))$
- `size`、`groups`: $O(1)$

## 空間計算量

- 固定長版: $O(\texttt{MAX_SIZE})$、可変長版: $O(n)$
- 代表探索を行う関数の追加領域: 再帰のため最悪 $O(\log n)$ のスタック
- コンストラクタ、`size`、`groups` の追加領域: $O(1)$
- 辺の追加は新たな領域を消費しない

## 注意点

- $0 \le n \le \texttt{MAX_SIZE}$ でなければならず、頂点は $[0, n)$ でなければならない。違反すると `std::runtime_error` を送出する。
- `add_edge` は成分が併合された場合だけ `true` を返す。同じ成分内の辺は辺数だけを増やす。
