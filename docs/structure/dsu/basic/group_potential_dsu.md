---
title: Group Potential DSU (群ポテンシャル付き素集合) [unionfind_with_potential_non_commutative_group]
documentation_of: ../../../../src/structure/dsu/basic/group_potential_dsu.hpp
---

非可換群の値をポテンシャルとして持つ素集合データ構造。
`GroupPotentialDSU<Group>(int n)` は $n$ 頂点について、関係 $A_u = A_v \cdot x$ を管理する。
`Group` は型 `value_type` と、静的関数 `identity()`、`multiply(a, b)`、`inverse(a)` を提供する。

## API

```cpp
explicit GroupPotentialDSU<Group>(int n)
bool merge(int u, int v, const value_type& x)
optional<value_type> difference(int u, int v)
int leader(int v)
value_type weight(int v)
int size() const
```

- `merge(u, v, x)`: 関係 $A_u = A_v \cdot x$ を追加する。既存の関係と整合すれば `true` を返し（異なる成分なら併合する）、矛盾すれば状態を変えずに `false` を返す。
- `difference(u, v)`: 同じ成分なら $A_v^{-1} A_u$ を、異なる成分なら `nullopt` を返す。
- `leader(v)`: $v$ を含む成分の代表元を返す。
- `weight(v)`: $A_v = A_{\mathrm{leader}(v)} \cdot \mathrm{weight}(v)$ を満たす右因子を返す。
- `size()`: 頂点数を返す。

## ロールバック版

```cpp
explicit RollbackGroupPotentialDSU<Group>(int n)
bool merge(int u, int v, const value_type& x)
optional<value_type> difference(int u, int v) const
size_t snapshot() const
void rollback(size_t snapshot)
int size() const
```

`merge` と `difference` は上と同じ意味を持つ。経路圧縮を行わず、`snapshot()` で得た値を `rollback` に渡すと、その時点の関係集合に戻せる。
`snapshot()` の値はそれまでに成功した成分の併合回数であり、矛盾する関係や既に成り立つ関係の追加では増えない。

## 時間計算量

群演算を定数時間とし、頂点数を $N$ とする。

- 構築: $O(N)$
- `GroupPotentialDSU` の `merge`、`difference`、`leader`、`weight`: 償却 $O(\alpha(N))$
- `RollbackGroupPotentialDSU` の `merge`、`difference`: $O(\log(N+1))$
- `snapshot`、`size`: $O(1)$
- `rollback`: 取り消す併合の回数を $K$ として $O(K)$

## 空間計算量

$O(N)$。ロールバック版は併合の履歴としてさらに併合回数に比例する領域を使う。

## 注意点

- 頂点は $[0, n)$ でなければならない。範囲外は `std::out_of_range`、負の頂点数は `std::invalid_argument` を送出する。
- `rollback` に現在の `snapshot()` より大きい値を渡すと `std::out_of_range` を送出する。
- 群演算の結合則・逆元・単位元と、値の等値比較 `==` が必要である。積の順序を入れ替えてはならない。
