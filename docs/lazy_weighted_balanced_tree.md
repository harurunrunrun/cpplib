---
title: Lazy Weighted Balanced Tree
documentation_of: ../src/structure/bbst/lazy_weighted_balanced_tree.hpp
---

重み平衡木による列。区間反転、区間作用、区間積を行う。

# テンプレート引数

```cpp
LazyWeightedBalancedTree<MonoidActLen, MAX_SIZE>
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
LazyWeightedBalancedTree()
LazyWeightedBalancedTree(const vector<S>& values)
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
- `LazyWeightedBalancedTree(values)`: 固定長storage初期化 $O(\mathrm{MAX\_SIZE})$ と長さ $N$ の平衡木構築 $O(N)$
- `size`, `empty`, `clear`, `all_prod`: $O(1)$
- `insert`, `push_front`, `push_back`, `erase`, `set`, `get`: 再平衡化を含めて償却 $O(\log(N+1))$
- `apply`, `reverse`, `prod`: 内部のsplit/mergeと再平衡化を含めて償却 $O(\log(N+1))$

乱数には依存しない。

## 空間計算量

- object全体: $O(\mathrm{MAX\_SIZE})$
- 各操作の再帰stack: $O(\log(N+1))$
