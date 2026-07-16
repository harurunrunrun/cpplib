---
title: Dynamic Functional Wavelet Matrix (動的多機能ウェーブレット行列)
documentation_of: ../src/structure/wavelet_matrix/dynamic_functional_wavelet_matrix.hpp
---

`DynamicWeightedWaveletMatrix` の重みを値自身にしたもの。
`set(k, value)` で値と重みの両方を更新する。

## 時間計算量

`DynamicWeightedWaveletMatrix` と同じ。

## 計算量（公開操作別）

$B=\mathtt{BLOCK\_SIZE}$、$D=\mathtt{BIT\_WIDTH}$、
$Q(L)=B+\lceil L/B\rceil\log B$、$S(L)=B+\lceil L/B\rceil$ とする。

- default constructor: $O(\mathtt{MAX\_SIZE})$
- vector/array constructor: $O(\mathtt{MAX\_SIZE}+N\log B)$
- `size`, `access`, `operator[]`, `weight`: $O(1)$
- `set`および継承した`set_value`, `set_weight`: $O(B\log B)$
- `rank`, `range_freq`, `range_sum`: $O(Q(L))$
- `sum`: $O(S(L))$
- `select`: $O(Q(N)\log N)$
- `kth_smallest`, `kth_largest`, `sum_k_smallest`, `sum_k_largest`, `prev_value`, `next_value`: $O(DQ(L))$

## 注意点

```cpp
void set(int k, T value)
```

`set` は値と重みを同じ `value` へ更新する。他のqueryは
`DynamicWeightedWaveletMatrix` の全公開APIを継承する。継承した `set_value/set_weight` を
単独使用すると「重み=値」の不変条件は保たれない。点・半開区間・bit幅・容量・例外契約は
基底型と同じで、各APIの計算量は上記表の通り。

## Constructor signature

```cpp
DynamicFunctionalWaveletMatrix()
DynamicFunctionalWaveletMatrix(const vector<T>& values)
DynamicFunctionalWaveletMatrix(const array<T, N>& values)
```
