---
title: Implicit Treap (暗黙トレープ) [range_reverse_range_sum]
documentation_of: ../../../../src/structure/tree/treap/implicit_treap.hpp
---

乱択平衡二分探索木（暗黙の treap）で列を管理し、任意位置の挿入・削除、区間反転、区間のモノイド積を扱う。

## テンプレート引数

```cpp
ImplicitTreap<Monoid, MAX_SIZE>
```

- モノイドのオブジェクト `Monoid`（非型テンプレート引数）
- 同時に保持する要素数の上限 `MAX_SIZE`（省略するとデフォルト値 `-1` の可変長版）

`Monoid` は `S op(S, S)` と `S e()` を持つ。反転を扱うため、非可換モノイドでも使える。

## コンストラクタ

```cpp
ImplicitTreap(uint64_t seed = ...)
ImplicitTreap(const vector<S>& values, uint64_t seed = ...)
```

空列、または `values` で初期化する。`values` からの構築はスタックを用いて線形時間で行う。

## 時間計算量

- 空列: 固定長版は $O(\texttt{MAX_SIZE})$、可変長版は $O(1)$
- `values`: 固定長版は $O(\texttt{MAX_SIZE}+N)$、可変長版は $O(N)$

固定長版は容量分の値・集約値・ノード領域を構築する。

## `insert` / `erase`

```cpp
void insert(int p, const S& x)
void push_front(const S& x)
void push_back(const S& x)
void erase(int p)
```

`insert(p, x)` は `p` 番目の直前に `x` を挿入する。

`erase(p)` は `p` 番目の要素を削除する。

## 時間計算量

- 期待 $O(\log N)$

## `set` / `get`

```cpp
void set(int p, const S& x)
S get(int p)
```

`p` 番目の要素にアクセスする。

## 時間計算量

- 期待 $O(\log N)$

## `reverse`

```cpp
void reverse(int l, int r)
```

区間 $[l,r)$ を反転する。

## 時間計算量

- 期待 $O(\log N)$

## `prod`

```cpp
S prod(int l, int r)
S all_prod()
```

区間 $[l,r)$、または列全体のモノイド積を返す。

## 時間計算量

- 期待 $O(\log N)$

`all_prod`, `size`, `empty`, `clear` は $O(1)$。

## 空間計算量（API 別の追加領域）

- コンストラクタ、`size`、`empty`、`clear`、`all_prod`: $O(1)$
- `insert`、`push_front`、`push_back`、`erase`、`set`、`get`、`reverse`、`prod`: 期待 $O(\log N)$ の再帰スタック
- ムーブコンストラクタ / ムーブ代入: $O(1)$

ノードは固定長版では容量 $O(\texttt{MAX_SIZE})$ の領域に保存し、1 回の挿入が消費するスロットは 1 個である。削除したスロットは再利用する。

## 注意点

- `insert(p, x)` は `0 <= p <= size()`、点操作は `0 <= p < size()` を要求する。
- 区間操作は `0 <= l <= r <= size()` を要求する。範囲違反では `runtime_error` を送出する。
- 固定長版で `insert` により要素数が `MAX_SIZE` を超える場合は `runtime_error` を送出する。
- `prod(l, r)` は左から右の順で非可換モノイドを合成し、空区間では単位元を返す。
- `seed` は優先度の生成に使い、同じ `seed` と操作列なら同じ結果になる。
- `clear` は $O(1)$ でノード領域を再利用する。コピーは禁止、ムーブは可能である。
- 容量を指定した固定配列版（`ImplicitTreap<Monoid, N>`）と可変長版（`ImplicitTreap<Monoid>`）は同じヘッダで利用できる。
