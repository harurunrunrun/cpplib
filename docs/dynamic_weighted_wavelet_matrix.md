---
title: Dynamic Weighted Wavelet Matrix (動的重み付きウェーブレット行列)
documentation_of: ../src/structure/wavelet_matrix/dynamic_weighted_wavelet_matrix.hpp
---

整数列への挿入・削除・置換と、各要素に付随する重みの更新を扱う。
動的 bitvector の平衡木を各 bit level に持つため、列全体の再構築や平方分割を行わない。

## テンプレート引数

```cpp
DynamicWeightedWaveletMatrix<T, W, MAX_SIZE, BIT_WIDTH, BLOCK_SIZE>
```

`MAX_SIZE` は列長の上限、`BIT_WIDTH` は値を表す bit 数。
`BLOCK_SIZE` は旧APIとのソース互換用で、現在の動的木の計算量には影響しない。

## 構築と取得

```cpp
DynamicWeightedWaveletMatrix()
DynamicWeightedWaveletMatrix(const vector<T>& values, const vector<W>& weights)
DynamicWeightedWaveletMatrix(const array<T, N>& values, const array<W, N>& weights)
int size() const
T access(int k) const
T operator[](int k) const
W weight(int k) const
```

値と重みの列長は一致していなければならない。

## 更新

```cpp
void insert(int position, T value, const W& weight)
pair<T, W> erase(int position)
void push_back(T value, const W& weight)
pair<T, W> pop_back()
void set(int k, T value, const W& weight)
void set_value(int k, T value)
void set_weight(int k, const W& weight)
```

`insert` は `position` の直前へ挿入する。`erase` と `pop_back` は削除した
`(value, weight)` を返す。`set` は値と重み、`set_value` と `set_weight` は
片方だけを置換する。

## 順序統計

```cpp
int rank(T value, int r) const
int rank(T value, int l, int r) const
int select(T value, int occurrence) const
T kth_smallest(int l, int r, int k) const
T kth_largest(int l, int r, int k) const
int range_freq(int l, int r, T upper) const
int range_freq(int l, int r, T lower, T upper) const
optional<T> prev_value(int l, int r, T upper) const
optional<T> next_value(int l, int r, T lower) const
```

`select` は指定した出現がなければ `size()` を返す。
`prev_value` は `upper` 未満の最大値、`next_value` は `lower` 以上の最小値を返す。

## 重みの集約

```cpp
W sum(int l, int r) const
W range_sum(int l, int r, T upper) const
W range_sum(int l, int r, T lower, T upper) const
W sum_k_smallest(int l, int r, int k) const
W sum_k_largest(int l, int r, int k) const
```

値が等しい要素を `sum_k_smallest` / `sum_k_largest` の境界で分ける場合は、
現在の列における順序を使う。

## 時間計算量

$D = BIT_WIDTH$、現在の列長を $N$ とする。動的 bitvector は
randomized implicit treap なので、以下は期待時間計算量である。

- vector/array constructor: $O(DN)$
- default constructor, `size`: $O(1)$
- `weight`, `sum`: $O(\log N)$
- `access`, `operator[]`: $O(D\log N)$
- `insert`, `erase`, `push_back`, `pop_back`: $O(D\log N)$
- `set`, `set_value`, `set_weight`: $O(D\log N)$
- `rank`, `select`, `range_freq`: $O(D\log N)$
- `kth_smallest`, `kth_largest`, `prev_value`, `next_value`: $O(D\log N)$
- `range_sum`, `sum_k_smallest`, `sum_k_largest`: $O(D\log N)$
- 空間計算量: $O(DN)$

## 注意点

添字と出現番号は0-indexed、列区間と値域は半開区間 `[l,r)`,
`[lower,upper)`。`W` はデフォルト構築、加算、減算をサポートする必要がある。
列長上限、bit幅、添字、区間、順序の違反では `runtime_error` を送出する。
