---
title: Disjoint Set Union (素集合データ構造) [DSL_1_A] [unionfind]
documentation_of: ../../../../src/structure/dsu/basic/dsu.hpp
---

素集合データ構造（Union-Find）。union by size と経路圧縮を行う。

## テンプレート引数

```cpp
DSU<MAX_SIZE = -1>
```

- `MAX_SIZE`: 頂点数の上限。省略（`-1`）すると、実行時の頂点数に合わせて `std::vector` を確保する可変長版になる。

```cpp
DSU<> dsu(n);          // 可変長版
DSU<200000> fixed(n);  // 固定長版
```

## コンストラクタ

```cpp
explicit DSU(int n = MAX_SIZE)
```

$n$ 頂点 $0, 1, \dots, n-1$ がそれぞれ単独の集合である状態を作る。可変長版では `n` のデフォルトは $0$ である。

## merge / same

```cpp
bool merge(int u, int v)
bool same(int u, int v)
```

`merge` は $u$ と $v$ を含む集合を併合し、異なる集合が併合されたとき `true` を返す。
`same` は $u$ と $v$ が同じ集合に属するかを返す。

## leader / component_size / size / groups

```cpp
int leader(int v)
int component_size(int v)
int size() const
int groups() const
```

`leader` は $v$ を含む集合の代表元、`component_size` はその集合の要素数を返す。
`size` は頂点数 $n$、`groups` は現在の集合の個数を返す。

## 時間計算量

- コンストラクタ: $O(n)$。固定長版では本体の配列の初期化に $O(\texttt{MAX_SIZE})$ かかる
- `merge`、`same`、`leader`、`component_size`: 償却 $O(\alpha(n))$
- `size`、`groups`: $O(1)$

## 空間計算量

- 固定長版: $O(\texttt{MAX_SIZE})$
- 可変長版: $O(n)$
- `leader`、`same`、`component_size`、`merge` の追加領域: 代表探索が再帰するため、最悪 $O(\log n)$ のスタック
- コンストラクタ、`size`、`groups` の追加領域: $O(1)$

## 注意点

- $0 \le n \le \texttt{MAX_SIZE}$ でなければならない。範囲外ならコンストラクタが `std::runtime_error` を送出する。
- 頂点引数はすべて $[0, n)$ でなければならない。範囲外なら `std::runtime_error` を送出する。
- `merge` は新たな領域を消費しない。
