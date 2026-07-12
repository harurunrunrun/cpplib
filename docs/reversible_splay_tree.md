---
title: Reversible Splay Tree
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
