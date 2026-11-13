---
title: Weighted DSU (重み付き素集合データ構造) [DSL_1_B]
documentation_of: ../../../../src/structure/dsu/basic/weighted_dsu.hpp
---

差分制約付き（重み付き）素集合データ構造。各頂点のポテンシャルの差を管理する。

## テンプレート引数

```cpp
WeightedDSU<T, MAX_SIZE = -1>
```

- `T`: ポテンシャルの差の型。`+`、`-`、`==`、デフォルト構築 `T()`（零元）を持ち、`+` は可換群の演算として振る舞う必要がある
- `MAX_SIZE`: 頂点数の上限。省略（`-1`）すると、実行時の頂点数に合わせて `std::vector` を確保する可変長版になる

```cpp
WeightedDSU<long long> dsu(n);  // 可変長版
```

## コンストラクタ

```cpp
explicit WeightedDSU(int n = MAX_SIZE)
```

$n$ 頂点の集合を作る。可変長版では `n` のデフォルトは $0$ である。

## merge / diff

```cpp
bool merge(int u, int v, T w)
T diff(int u, int v)
optional<T> diff_if_connected(int u, int v)
```

- `merge(u, v, w)`: 制約 $\mathrm{potential}(v) - \mathrm{potential}(u) = w$ を追加する。矛盾しない場合は `true`、矛盾する場合は `false` を返す。同じ成分なら、既存の差と一致する場合だけ `true` を返す。
- `diff(u, v)`: $\mathrm{potential}(v) - \mathrm{potential}(u)$ を返す。
- `diff_if_connected(u, v)`: 連結なら同じ差を、非連結なら `nullopt` を返す。

## same / leader / weight

```cpp
bool same(int u, int v)
int leader(int v)
T weight(int v)
int component_size(int v)
int size() const
int groups() const
```

`weight(v)` は $\mathrm{potential}(v) - \mathrm{potential}(\mathrm{leader}(v))$ を返す。
`component_size` は成分の頂点数、`size` は頂点数、`groups` は連結成分数を返す。

## 時間計算量

- コンストラクタ: $O(n)$。固定長版では本体の配列の初期化に $O(\texttt{MAX_SIZE})$ かかる
- `merge`、`diff`、`diff_if_connected`、`same`、`leader`、`weight`、`component_size`: 償却 $O(\alpha(n))$
- `size`、`groups`: $O(1)$

## 空間計算量

- 固定長版: $O(\texttt{MAX_SIZE})$、可変長版: $O(n)$
- 代表探索を行う関数の追加領域: 再帰のため最悪 $O(\log n)$ のスタック
- コンストラクタ、`size`、`groups` の追加領域: $O(1)$
- `merge` は新たな領域を消費しない

## 注意点

- $0 \le n \le \texttt{MAX_SIZE}$ で、頂点は $[0, n)$ でなければならない。違反すると `std::runtime_error` を送出する。
- `diff(u, v)` は同じ成分であることを前提とし、実装は連結性を検査しない。非連結の場合の戻り値は意味を持たない。
- `diff_if_connected(u, v)` は連結性も同時に検査する。
