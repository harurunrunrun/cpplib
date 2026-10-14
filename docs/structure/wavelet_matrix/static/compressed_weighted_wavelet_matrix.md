---
title: Compressed Weighted Wavelet Matrix (座標圧縮重み付きウェーブレット行列)
documentation_of: ../../../../src/structure/wavelet_matrix/static/compressed_weighted_wavelet_matrix.hpp
---

値を座標圧縮して扱う重み付きwavelet matrix。任意の比較可能な型を値として使える。

## テンプレート引数

```cpp
CompressedWeightedWaveletMatrix<T, W, MAX_SIZE, ID_BIT_WIDTH>
```

- 値の型 `T`
- 重みの型 `W`
- 配列長の上限 `MAX_SIZE`
- 圧縮後のidに使用するbit数 `ID_BIT_WIDTH`

## コンストラクタ

```cpp
CompressedWeightedWaveletMatrix(const vector<T>& values, const vector<W>& weights)
```

`values[k]` に重み `weights[k]` を対応させて構築する。

## 時間計算量

- $O(N\log N+\mathrm{ID\_BIT\_WIDTH}\cdot\mathtt{MAX\_SIZE})$

## 頻度 / 重みの総和

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

## k番目 / 中央値

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

## 計算量（公開操作別）

$D$ を異なる値の個数、$H=\mathtt{ID\_BIT\_WIDTH}$ とする。

- vector/array constructor: $O(N\log N+H\mathtt{MAX\_SIZE})$
- `size`, `value_count`, `value`, `sum`: $O(1)$
- `access`, `operator[]`, `kth_smallest`, `kth_largest`, `quantile`, `sum_k_smallest`, `sum_k_largest`, `range_min`, `range_max`, `median_floor`, `median_ceil`: $O(H)$
- `rank`, `range_freq`, `range_sum`, `prev_value`, `next_value`: $O(\log D+H)$

## 注意点

constructorは値列を座標圧縮し、対応する重み列を構築する。両vectorの長さは一致する必要がある。
`size/value_count/value/access/operator[]`、rank・順序統計・中央値・前後値の契約は
Compressed Wavelet Matrixと同じ。`sum/range_sum` は全値域・指定値域、
`sum_k_smallest/sum_k_largest` は順序先頭 `k` 個の重みを返す。

vector長不一致、列長容量、ID bit幅、点・区間・圧縮id・順序・値域では
`runtime_error`。copyは禁止、moveは可能。各APIの計算量は上記表の通り。

## 容量を省略した可変長版

容量のテンプレート引数 `MAX_SIZE` は省略でき、既定値 `-1` ではvectorを使う。
容量を指定した既存の固定配列版も同じヘッダで利用できる。

```cpp
using Variable = CompressedWeightedWaveletMatrix<T, W>;
using Fixed = CompressedWeightedWaveletMatrix<T, W, N>;
```

可変長版は入力長 $n$ とビット幅 $B$ に応じた配列だけを確保し、構築・照会の計算量は上記と同じ。
長さに依存する固定容量の領域は $O(n)$ に、ビット段ごとの領域は $O(Bn)$ になる。
整数添字の表現上限は残り、不正な添字や上限超過は検証する。
