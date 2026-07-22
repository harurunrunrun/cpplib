---
title: Dynamic Functional Wavelet Matrix (動的多機能ウェーブレット行列)
documentation_of: ../src/structure/wavelet_matrix/dynamic_functional_wavelet_matrix.hpp
---

`DynamicWeightedWaveletMatrix` の各重みを値自身に保つ派生型。
列全体の再構築を行わず、値の挿入・削除・置換を動的 bitvector の木へ反映する。

## 構築

```cpp
DynamicFunctionalWaveletMatrix()
DynamicFunctionalWaveletMatrix(const vector<T>& values)
DynamicFunctionalWaveletMatrix(const array<T, N>& values)
```

## 更新

```cpp
void set(int k, T value)
void set_value(int k, T value)
void insert(int position, T value)
T erase(int position)
void push_back(T value)
T pop_back()
```

`set` と `set_value` は値と重みをともに `value` へ置換する。
`insert` と `push_back` も重みに値自身を設定し、`erase` と `pop_back` は
削除した値を返す。

## クエリ

`size`, `access`, `operator[]`, `weight`, `rank`, `select`,
`range_freq`, `kth_smallest`, `kth_largest`, `prev_value`,
`next_value`, `sum`, `range_sum`, `sum_k_smallest`,
`sum_k_largest` を基底型から継承する。

## 時間計算量

$D = BIT_WIDTH$、現在の列長を $N$ とする。以下は randomized implicit treap
による期待時間計算量である。

- vector/array constructor: $O(DN)$
- default constructor, `size`: $O(1)$
- `weight`, `sum`: $O(\log N)$
- その他の公開更新・クエリ: $O(D\log N)$
- 空間計算量: $O(DN)$

## 注意点

添字・出現番号は0-indexed、区間は半開区間。`MAX_SIZE`、bit幅、添字、
区間、順序の違反では `runtime_error` を送出する。
基底型の `set_weight` を明示的に呼ぶと「重み=値」の不変条件を崩せるため、
多機能版では通常使用しない。
