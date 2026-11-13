---
title: Functional Wavelet Matrix (多機能ウェーブレット行列)
documentation_of: ../../../../src/structure/wavelet_matrix/static/functional_wavelet_matrix.hpp
---

各要素の重みをその値自身とした座標圧縮重み付きウェーブレット行列。`CompressedWeightedWaveletMatrix<T, Sum, MAX_SIZE, ID_BIT_WIDTH>` を継承する。

## テンプレート引数

```cpp
FunctionalWaveletMatrix<T, MAX_SIZE = -1, Sum = long long, ID_BIT_WIDTH = 32>
```

- `T`: 値の型。`Sum` へ変換できる必要がある。
- `MAX_SIZE`: 列長の上限。省略時（`-1`）は `std::vector` を使う可変長版になり、入力長に応じた領域だけを確保する。非負の値を指定すると固定長配列版になる。
- `Sum`: 和の型
- `ID_BIT_WIDTH`: 圧縮後の ID に使う bit 数。省略時は `unsigned` の bit 数（通常 32）。

```cpp
using Variable = FunctionalWaveletMatrix<T>;
using Fixed = FunctionalWaveletMatrix<T, N>;
```

## コンストラクタ

```cpp
explicit FunctionalWaveletMatrix(const vector<T>& values)
explicit FunctionalWaveletMatrix(const array<T, N>& values)
```

各要素の重みを `static_cast<Sum>(values[k])` として構築する。

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

`size`, `value_count`, `value`, `access`, `operator[]`, `rank`, `quantile`, `range_min`, `range_max`, `prev_value`, `next_value` と、`range_freq` / `range_sum` の `upper` のみを取る版も継承する。シグネチャ・半開区間・戻り値・例外は Compressed Weighted Wavelet Matrix と同じである。

## 時間計算量

$N$ を列長、$D$ を相異なる値の個数、$H = \texttt{ID_BIT_WIDTH}$ とする。

- コンストラクタ: 可変長版は $O(N \log N + H N)$、固定長版は配列の初期化を含めて $O(N \log N + H \cdot \texttt{MAX_SIZE})$
- `size`, `value_count`, `value`, `sum`: $O(1)$
- `access`, `operator[]`, `kth_smallest`, `kth_largest`, `quantile`, `sum_k_smallest`, `sum_k_largest`, `range_min`, `range_max`, `median_floor`, `median_ceil`: $O(H)$
- `rank`, `range_freq`, `range_sum`, `prev_value`, `next_value`: $O(\log D + H)$

## 注意点

- 列の区間は半開区間 $[l, r)$、値域も半開区間 $[\mathit{lower}, \mathit{upper})$ である。
- 列長が `MAX_SIZE` を超える場合や、添字・区間・`k` が範囲外の場合などは `std::runtime_error` を送出する。
- 基底クラスの制約により、現状 `ID_BIT_WIDTH` に 32 以外を指定するとコンパイルエラーになる。
- 和が `Sum` の範囲に収まることは利用者が保証する。
