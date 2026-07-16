---
title: Dual Segment Tree (双対セグメント木)
documentation_of: ../src/structure/segtree/dualsegtree.hpp
---

作用付きモノイドに対する区間作用・一点取得。

## API

```cpp
DualSegtree<Monoid, MAX_SIZE>(int n = MAX_SIZE)
DualSegtree<Monoid, MAX_SIZE>(const vector<S>& values)
void set(int k, const S& value)
S get(int k)
void apply(int k, const F& action)
void apply(int l, int r, const F& action)
```

コンストラクタは長さ `n` の単位元列、または `values` の列を構築する。
`set` は `k` 番目を `value` に置換し、`get` は遅延作用を反映した `k` 番目を返す。
1点版 `apply` は `k` に、区間版は半開区間 `[l,r)` に `action` を作用させる。

`0 <= n <= MAX_SIZE`、`0 <= k < n`、`0 <= l <= r <= n` が必要である。
範囲外または容量超過では `runtime_error` を送出する。

## 時間計算量

- `DualSegtree(int)`, `DualSegtree(vector)`: $O(\mathtt{MAX\_SIZE})$
- `set`, `get`, `apply`: $O(\log \mathtt{MAX\_SIZE})$

## 注意点

`0 <= n <= MAX_SIZE`、点は `[0,n)`、区間は `[l,r)` で `0 <= l <= r <= n`。
範囲・容量違反では `runtime_error`。`Monoid` は作用の合成順と `mapping` の契約を
満たす必要がある。保存領域は $O(\mathtt{MAX\_SIZE})$、各更新・queryの追加空間計算量は
$O(1)$。
