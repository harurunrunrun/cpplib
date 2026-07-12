---
title: Compressed Weighted Wavelet Matrix
documentation_of: ../src/structure/wavelet_matrix/compressed_weighted_wavelet_matrix.hpp
---

値を座標圧縮して扱う重み付きwavelet matrix。任意の比較可能な型を値として使える。

# テンプレート引数

```cpp
CompressedWeightedWaveletMatrix<T, W, MAX_SIZE, ID_BIT_WIDTH>
```

- 値の型 `T`
- 重みの型 `W`
- 配列長の上限 `MAX_SIZE`
- 圧縮後のidに使用するbit数 `ID_BIT_WIDTH`

# コンストラクタ

```cpp
CompressedWeightedWaveletMatrix(const vector<T>& values, const vector<W>& weights)
```

`values[k]` に重み `weights[k]` を対応させて構築する。

## 時間計算量

- $O(N\log N + N\mathrm{ID\_BIT\_WIDTH})$

# 頻度 / 重みの総和

```cpp
int range_freq(int l, int r, const T& upper) const
int range_freq(int l, int r, const T& lower, const T& upper) const
W sum(int l, int r) const
W range_sum(int l, int r, const T& upper) const
W range_sum(int l, int r, const T& lower, const T& upper) const
W sum_k_smallest(int l, int r, int k) const
W sum_k_largest(int l, int r, int k) const
```

`sum_k_smallest` は区間 $[l,r)$ の小さい方から `k` 個の重みの総和を返す。

# k番目 / 中央値

```cpp
T kth_smallest(int l, int r, int k) const
T kth_largest(int l, int r, int k) const
T range_min(int l, int r) const
T range_max(int l, int r) const
T median_floor(int l, int r) const
T median_ceil(int l, int r) const
```

## 時間計算量

- 構築以外: $O(\log N + \mathrm{ID\_BIT\_WIDTH})$
