---
title: Implicit Treap (暗黙トレープ) [deque] [range_reverse_range_sum]
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

## 空間計算量（API別の追加領域）

- constructor、`size`、`empty`、`clear`、`all_prod`: $O(1)$
- `insert`、`push_front`、`push_back`、`erase`、`set`、`get`、`reverse`、`prod`: expected $O(\log N)$ の再帰stack
- move constructor / move assignment: $O(1)$

nodeは固定arena $O(\mathtt{MAX\_SIZE})$ に保存し、1回の挿入が消費するslotは1個である。

## 注意点

- `insert(p,x)`は`0 <= p <= size()`、点操作は`0 <= p < size()`を要求する。
- 区間操作は`0 <= l <= r <= size()`。範囲違反は例外。
- `insert`で要素数が`MAX_SIZE`を超える場合は例外。
- `prod(l,r)`は左から右の順で非可換monoidを合成し、空区間では単位元を返す。
- seedはpriority生成に使用し、同じseedと操作列なら同じ結果になる。
- `clear`は$O(1)$で固定arenaを再利用し、copyは禁止、moveは可能。
