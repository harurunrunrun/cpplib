---
title: Implicit Treap
documentation_of: ../src/structure/tree/implicit_treap.hpp
---

乱択平衡二分探索木による列。

# テンプレート引数

```cpp
ImplicitTreap<Monoid, MAX_SIZE>
```

- モノイド `Monoid`
- 同時に保持する要素数の上限 `MAX_SIZE`

`Monoid` は `S op(S, S)` と `S e()` を持つ。反転を扱うため、非可換モノイドでも使える。

# コンストラクタ

```cpp
ImplicitTreap(uint64_t seed = ...)
ImplicitTreap(const vector<S>& values, uint64_t seed = ...)
```

空列、または `values` で初期化する。

## 時間計算量

- 空列: $O(\mathtt{MAX\_SIZE})$
- `values`: $O(\mathtt{MAX\_SIZE}+N)$

固定容量の値・集約値・node領域を構築する。

# insert / erase

```cpp
void insert(int p, const S& x)
void push_front(const S& x)
void push_back(const S& x)
void erase(int p)
```

`insert(p, x)` は `p` 番目の直前に `x` を挿入する。

`erase(p)` は `p` 番目の要素を削除する。

## 時間計算量

- expected $O(\log N)$

# set / get

```cpp
void set(int p, const S& x)
S get(int p)
```

`p` 番目の要素にアクセスする。

## 時間計算量

- expected $O(\log N)$

# reverse

```cpp
void reverse(int l, int r)
```

区間 $[l,r)$ を反転する。

## 時間計算量

- expected $O(\log N)$

# prod

```cpp
S prod(int l, int r)
S all_prod()
```

区間 $[l,r)$、または列全体のモノイド積を返す。

## 時間計算量

- expected $O(\log N)$

`all_prod`, `size`, `empty`, `clear` は $O(1)$。
