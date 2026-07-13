---
title: Lazy Reversible Splay Tree
documentation_of: ../src/structure/bbst/lazy_reversible_splay_tree.hpp
---

Splay Tree による列。区間反転、区間作用、区間積を行う。

# テンプレート引数

```cpp
LazyReversibleSplayTree<MonoidActLen, MAX_SIZE>
```

- 作用付きモノイド `MonoidActLen`
- 同時に保持する要素数の上限 `MAX_SIZE`

`MonoidActLen` は次を持つ。

```cpp
S op(S a, long long len_a, S b, long long len_b)
S e()
S mapping(T f, S x, long long len)
T composition(T f, T g)
T id()
```

非可換モノイドにも対応する。

# コンストラクタ

```cpp
LazyReversibleSplayTree()
LazyReversibleSplayTree(const vector<S>& values)
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
void apply(int l, int r, const T& f)
void reverse(int l, int r)
S prod(int l, int r)
S all_prod()
```

半開区間 `[l,r)` を扱う。

## 時間計算量

- amortized $O(\log N)$

# API別計算量

$N$ を操作前の列長とし、モノイド演算・作用を $O(1)$ とする。

- default constructor: $O(\mathrm{MAX\_SIZE})$
- `LazyReversibleSplayTree(values)`: 固定長storage初期化 $O(\mathrm{MAX\_SIZE})$ と長さ $N$ の平衡木構築 $O(N)$
- `size`, `empty`, `clear`, `all_prod`: $O(1)$
- `insert`, `push_front`, `push_back`, `erase`, `set`, `get`: worst-case $O(N)$、操作列に対して償却 $O(\log(N+1))$
- `apply`, `reverse`, `prod`: worst-case $O(N)$、操作列に対して償却 $O(\log(N+1))$

区間操作は内部の定数回のsplit/mergeを含む。

## 空間計算量

- objectとtemplate実体ごとの固定長祖先stack: $O(\mathrm{MAX\_SIZE})$
- splayが使用する祖先stack要素数: $O(N)$ worst-case
- vector constructorの再帰stack: $O(\log(N+1))$
