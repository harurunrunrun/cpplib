---
title: Reversible Splay Tree (反転可能スプレー木) [range_reverse_range_sum]
documentation_of: ../src/structure/bbst/reversible_splay_tree.hpp
---

Splay Tree による列。区間反転、区間積を行う。

# テンプレート引数

```cpp
ReversibleSplayTree<Monoid, MAX_SIZE>
```

- モノイド `Monoid`
- 同時に保持する要素数の上限 `MAX_SIZE`

`Monoid` は `S op(S, S)` と `S e()` を持つ。非可換モノイドにも対応する。

# コンストラクタ

```cpp
ReversibleSplayTree()
ReversibleSplayTree(const vector<S>& values)
```

# insert / erase / set / get

```cpp
void insert(int p, const S& x)
void push_front(const S& x)
void push_back(const S& x)
void erase(int p)
void set(int p, const S& x)
S get(int p)
```

## 時間計算量

- amortized $O(\log N)$

# 区間操作

```cpp
void reverse(int l, int r)
S prod(int l, int r)
S all_prod()
```

半開区間 `[l,r)` を扱う。

## 時間計算量

- amortized $O(\log N)$

# API別計算量

$N$ を操作前の列長とし、モノイド演算を $O(1)$ とする。

- default constructor: $O(\mathrm{MAX\_SIZE})$
- `ReversibleSplayTree(values)`: 固定長storage初期化 $O(\mathrm{MAX\_SIZE})$ と長さ $N$ の平衡木構築 $O(N)$
- `size`, `empty`, `clear`, `all_prod`: $O(1)$
- `insert`, `push_front`, `push_back`, `erase`, `set`, `get`: worst-case $O(N)$、操作列に対して償却 $O(\log(N+1))$
- `reverse`, `prod`: worst-case $O(N)$、操作列に対して償却 $O(\log(N+1))$

区間操作は内部の定数回のsplit/mergeを含む。

## 空間計算量

- objectとtemplate実体ごとの固定長祖先stack: $O(\mathrm{MAX\_SIZE})$
- splayが使用する祖先stack要素数: $O(N)$ worst-case
- vector constructorの再帰stack: $O(\log(N+1))$

## 注意点

- `insert(p,x)`は`0 <= p <= size()`、点操作は`0 <= p < size()`を要求する。
- 区間操作は`0 <= l <= r <= size()`。範囲違反は例外。
- `insert`で要素数が`MAX_SIZE`を超える場合は例外。
- `prod(l,r)`は左から右の順で非可換monoidを合成し、空区間では単位元を返す。
- `clear`は$O(1)$で固定arenaを再利用し、copyは禁止、moveは可能。
