---
title: Functional Wavelet Matrix
documentation_of: ../src/structure/wavelet_matrix/functional_wavelet_matrix.hpp
---

座標圧縮版wavelet matrixに、値そのものの区間和を重みとして持たせたもの。

# テンプレート引数

```cpp
FunctionalWaveletMatrix<T, MAX_SIZE, Sum, ID_BIT_WIDTH>
```

- 値の型 `T`
- 配列長の上限 `MAX_SIZE`
- 和の型 `Sum`
- 圧縮後のidに使用するbit数 `ID_BIT_WIDTH`

`T` から `Sum` へ変換できる必要がある。

# コンストラクタ

```cpp
FunctionalWaveletMatrix(const vector<T>& values)
```

各要素の重みをその要素自身として構築する。

# 主な操作

```cpp
T kth_smallest(int l, int r, int k) const
T kth_largest(int l, int r, int k) const
int range_freq(int l, int r, const T& lower, const T& upper) const
Sum sum(int l, int r) const
Sum range_sum(int l, int r, const T& lower, const T& upper) const
Sum sum_k_smallest(int l, int r, int k) const
Sum sum_k_largest(int l, int r, int k) const
T median_floor(int l, int r) const
T median_ceil(int l, int r) const
```

その他の操作は `CompressedWeightedWaveletMatrix` と同じ。

## 時間計算量

- 構築: $O(N\log N + N\mathrm{ID\_BIT\_WIDTH})$
- クエリ: $O(\log N + \mathrm{ID\_BIT\_WIDTH})$
