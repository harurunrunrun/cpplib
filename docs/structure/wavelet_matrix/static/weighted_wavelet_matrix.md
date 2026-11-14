---
title: Weighted Wavelet Matrix (重み付きウェーブレット行列)
documentation_of: ../../../../src/structure/wavelet_matrix/static/weighted_wavelet_matrix.hpp
---

各要素に重みを持つ静的なウェーブレット行列。値で絞り込んだ要素の重みの総和を求められる。

## テンプレート引数

```cpp
WeightedWaveletMatrix<T, W, MAX_SIZE = -1, BIT_WIDTH = (T の bit 数)>
```

- `T`: 整数型
- `W`: 重みの型。加算・減算と値初期化 `W{}`（零元）が必要
- `MAX_SIZE`: 列長の上限。省略時（`-1`）は `std::vector` を使う可変長版になり、入力長に応じた領域だけを確保する。非負の値を指定すると固定長配列版になる。
- `BIT_WIDTH`: 使用する bit 数。省略時は `T` の bit 数。符号付き整数では省略しなければならない（`static_assert`）。

```cpp
using Variable = WeightedWaveletMatrix<T, W>;
using Fixed = WeightedWaveletMatrix<T, W, N>;
```

## コンストラクタ

```cpp
WeightedWaveletMatrix(const vector<T>& values, const vector<W>& weights)
WeightedWaveletMatrix(const array<T, N>& values, const array<W, N>& weights)
int size() const
```

`values[k]` に重み `weights[k]` を対応させて構築する。2 つの列の長さは一致しなければならない。`size` は列長を返す。

## 要素取得

```cpp
T access(int k) const
T operator[](int k) const
```

`k` 番目（0-indexed）の値を返す。

## rank / k 番目の値

```cpp
int rank(T value, int r) const
int rank(T value, int l, int r) const
T kth_smallest(int l, int r, int k) const
T kth_largest(int l, int r, int k) const
```

`rank` は区間 $[0, r)$ または $[l, r)$ にある `value` の個数を返す。`kth_smallest` / `kth_largest` は区間 $[l, r)$ で `k` 番目（0-indexed）に小さい値 / 大きい値を返す。

## 頻度

```cpp
int range_freq(int l, int r, T upper) const
int range_freq(int l, int r, T lower, T upper) const
```

区間 $[l, r)$ にある `upper` 未満の値の個数、または $[\mathit{lower}, \mathit{upper})$ に含まれる値の個数を返す。

## 重みの総和

```cpp
W sum(int l, int r) const
W range_sum(int l, int r, T upper) const
W range_sum(int l, int r, T lower, T upper) const
W sum_k_smallest(int l, int r, int k) const
W sum_k_largest(int l, int r, int k) const
```

- `sum`: 区間 $[l, r)$ の全要素の重みの総和を返す。
- `range_sum`: 区間 $[l, r)$ のうち、値が `upper` 未満、または $[\mathit{lower}, \mathit{upper})$ に含まれる要素の重みの総和を返す。
- `sum_k_smallest`: 区間 $[l, r)$ を値の小さい順に並べたときの先頭 `k` 個の重みの総和を返す。$0 \le k \le r - l$ である。
- `sum_k_largest`: 同様に、値の大きい方から `k` 個の重みの総和を返す。

## 前後の値

```cpp
optional<T> prev_value(int l, int r, T upper) const
optional<T> next_value(int l, int r, T lower) const
```

区間 $[l, r)$ 内で `upper` 未満の最大値、または `lower` 以上の最小値を返す。存在しない場合は `nullopt` を返す。

## 時間計算量

$N$ を列長、$D = \texttt{BIT_WIDTH}$ とする。

- コンストラクタ: 可変長版は $O(D N)$、固定長版は配列の初期化を含めて $O(D \cdot \texttt{MAX_SIZE})$
- `size`, `sum`: $O(1)$
- `access`, `operator[]`, `rank`, `kth_smallest`, `kth_largest`, `range_freq`, `range_sum`, `sum_k_smallest`, `sum_k_largest`, `prev_value`, `next_value`: $O(D)$

## 空間計算量

各段に重みの累積和を持つため $O(D N)$（固定長版では $O(D \cdot \texttt{MAX_SIZE})$）。

## 注意点

- 列の区間は半開区間 $[l, r)$、値域も半開区間 $[\mathit{lower}, \mathit{upper})$ である。
- 2 つの入力列の長さが異なる場合、列長が `MAX_SIZE` を超える場合、値が `BIT_WIDTH` bit に収まらない場合、添字・区間・`k` が範囲外の場合、`lower > upper` の場合は `std::runtime_error` を送出する。
- `range_freq` / `range_sum` / `prev_value` / `next_value` などに渡す `upper` / `lower` が $2^{\texttt{BIT_WIDTH}}$ 以上の場合は、すべての要素より大きい値として扱う。`rank` に $2^{\texttt{BIT_WIDTH}}$ 以上の値を渡すと、出現しない値として扱う。
- コピーは禁止、ムーブは可能。
