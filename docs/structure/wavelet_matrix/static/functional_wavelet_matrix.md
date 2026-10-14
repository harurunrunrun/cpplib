---
title: Functional Wavelet Matrix (多機能ウェーブレット行列)
documentation_of: ../../../../src/structure/wavelet_matrix/static/functional_wavelet_matrix.hpp
---

座標圧縮版wavelet matrixに、値そのものの区間和を重みとして持たせたもの。

## テンプレート引数

```cpp
FunctionalWaveletMatrix<T, MAX_SIZE, Sum, ID_BIT_WIDTH>
```

- 値の型 `T`
- 配列長の上限 `MAX_SIZE`
- 和の型 `Sum`
- 圧縮後のidに使用するbit数 `ID_BIT_WIDTH`

`T` から `Sum` へ変換できる必要がある。

## コンストラクタ

```cpp
FunctionalWaveletMatrix(const vector<T>& values)
```

各要素の重みをその要素自身として構築する。

## 主な操作

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

- 構築: $O(N\log N+\mathrm{ID\_BIT\_WIDTH}\cdot\mathtt{MAX\_SIZE})$
- クエリ: $O(\log N + \mathrm{ID\_BIT\_WIDTH})$

## 計算量（公開操作別）

$D$ を異なる値の個数、$H=\mathtt{ID\_BIT\_WIDTH}$ とする。

- vector/array constructor: $O(N\log N+H\mathtt{MAX\_SIZE})$
- `size`, `value_count`, `value`, `sum`: $O(1)$
- `access`, `operator[]`, `kth_smallest`, `kth_largest`, `quantile`, `sum_k_smallest`, `sum_k_largest`, `range_min`, `range_max`, `median_floor`, `median_ceil`: $O(H)$
- `rank`, `range_freq`, `range_sum`, `prev_value`, `next_value`: $O(\log D+H)$

## 注意点

値自身を重みとした `CompressedWeightedWaveletMatrix` である。
`size/value_count/value/access/operator[]/rank/kth_smallest/kth_largest/quantile/range_freq/sum/`
`range_sum/sum_k_smallest/sum_k_largest/range_min/range_max/median_floor/median_ceil/prev_value/next_value`
を継承する。各signature、半開区間、戻り値、例外はCompressed Weighted Wavelet Matrixと同じ。
constructorはvector/arrayを受け、列長またはID bit幅の容量違反では `runtime_error`。

## 容量を省略した可変長版

容量のテンプレート引数 `MAX_SIZE` は省略でき、既定値 `-1` ではvectorを使う。
容量を指定した既存の固定配列版も同じヘッダで利用できる。

```cpp
using Variable = FunctionalWaveletMatrix<T>;
using Fixed = FunctionalWaveletMatrix<T, N>;
```

可変長版は入力長 $n$ とビット幅 $B$ に応じた配列だけを確保し、構築・照会の計算量は上記と同じ。
長さに依存する固定容量の領域は $O(n)$ に、ビット段ごとの領域は $O(Bn)$ になる。
整数添字の表現上限は残り、不正な添字や上限超過は検証する。
