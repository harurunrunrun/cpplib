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
