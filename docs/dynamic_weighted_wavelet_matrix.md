---
title: Dynamic Weighted Wavelet Matrix (動的重み付きウェーブレット行列)
documentation_of: ../src/structure/wavelet_matrix/dynamic_weighted_wavelet_matrix.hpp
---

点更新できる重み付き整数列。
値に対する順序統計と、値の範囲で絞った重みの和を扱う。

# set

```cpp
wm.set(k, value, weight)
wm.set_value(k, value)
wm.set_weight(k, weight)
```

# 取得

```cpp
wm.access(k)
wm[k]
wm.weight(k)
```

# 和

```cpp
wm.sum(l, r)
wm.range_sum(l, r, upper)
wm.range_sum(l, r, lower, upper)
wm.sum_k_smallest(l, r, k)
wm.sum_k_largest(l, r, k)
```

`sum_k_smallest` と `sum_k_largest` は値が等しい場合、元の添字順で選ぶ。

# 時間計算量

$B = BLOCK\_SIZE$ とする。

- `set`: $O(B\log B)$
- `rank`, `range_freq`, `range_sum`: $O(B + \frac{N}{B}\log B)$
- `kth_smallest`, `kth_largest`: $O(BIT\_WIDTH \cdot (B + \frac{N}{B}\log B))$
- `sum_k_smallest`, `sum_k_largest`: $O(BIT\_WIDTH \cdot (B + \frac{N}{B}\log B))$

# 計算量（公開操作別）

$B=\mathtt{BLOCK\_SIZE}$、$D=\mathtt{BIT\_WIDTH}$、
$Q(L)=B+\lceil L/B\rceil\log B$、$S(L)=B+\lceil L/B\rceil$ とする。

- default constructor: $O(\mathtt{MAX\_SIZE})$
- vector/array constructor: $O(\mathtt{MAX\_SIZE}+N\log B)$
- `size`, `access`, `operator[]`, `weight`: $O(1)$
- `set`, `set_value`, `set_weight`: $O(B\log B)$
- `rank`, `range_freq`, `range_sum`: $O(Q(L))$
- `sum`: $O(S(L))$
- `select`: $O(Q(N)\log N)$
- `kth_smallest`, `kth_largest`, `sum_k_smallest`, `sum_k_largest`, `prev_value`, `next_value`: $O(DQ(L))$

# 構築・共通契約

default constructorは空列、vector/array constructorは入力列を構築する。`size` は列長、
`access/operator[]` は1点を返す。`set(k,value)` は1点を置換する。
点・出現番号・順序統計の `k` は0-indexed、列区間は半開区間 `[l,r)`、
値域は `[lower,upper)`。`select` は該当なしなら `size()`、
`prev_value/next_value` は `nullopt`。

列長容量、bit幅、点・区間・順序・値域の違反では `runtime_error`。
各公開APIの計算量は上記表の通り。

`weight(k)` は重みを返す。`set` は値と重み、`set_value/set_weight` は片方だけを更新する。
`sum/range_sum` は区間・値域の重み、`sum_k_smallest/sum_k_largest` は順序先頭の重みを返す。

# Constructor signature

```cpp
DynamicWeightedWaveletMatrix()
DynamicWeightedWaveletMatrix(const vector<T>& values, const vector<W>& weights)
DynamicWeightedWaveletMatrix(const array<T, N>& values, const array<W, N>& weights)
```
