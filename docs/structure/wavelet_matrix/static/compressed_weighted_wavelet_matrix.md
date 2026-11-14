---
title: Compressed Weighted Wavelet Matrix (座標圧縮重み付きウェーブレット行列)
documentation_of: ../../../../src/structure/wavelet_matrix/static/compressed_weighted_wavelet_matrix.hpp
---

値を座標圧縮して扱う重み付きウェーブレット行列。`<` で比較でき、`!=` を持つ任意の型を値として使える。

## テンプレート引数

```cpp
CompressedWeightedWaveletMatrix<T, W, MAX_SIZE = -1, ID_BIT_WIDTH = 32>
```

- `T`: 値の型
- `W`: 重みの型。加算・減算と値初期化 `W{}`（零元）が必要
- `MAX_SIZE`: 列長の上限。省略時（`-1`）は `std::vector` を使う可変長版になり、入力長に応じた領域だけを確保する。非負の値を指定すると固定長配列版になる。
- `ID_BIT_WIDTH`: 圧縮後の ID に使う bit 数。省略時は `unsigned` の bit 数（通常 32）。

```cpp
using Variable = CompressedWeightedWaveletMatrix<T, W>;
using Fixed = CompressedWeightedWaveletMatrix<T, W, N>;
```

## コンストラクタ

```cpp
CompressedWeightedWaveletMatrix(const vector<T>& values, const vector<W>& weights)
CompressedWeightedWaveletMatrix(const array<T, N>& values, const array<W, N>& weights)
```

`values` を座標圧縮し、`values[k]` に重み `weights[k]` を対応させて構築する。2 つの列の長さは一致しなければならない。

## 列と圧縮 ID の情報 / 要素取得 / rank

```cpp
int size() const
int value_count() const
const T& value(int id) const
T access(int k) const
T operator[](int k) const
int rank(const T& value, int r) const
int rank(const T& value, int l, int r) const
```

`size` は列長、`value_count` は相異なる値の個数、`value(id)` は圧縮 ID `id` に対応する元の値を返す。`access` / `operator[]` は `k` 番目（0-indexed）の値を、`rank` は区間 $[0, r)$ または $[l, r)$ にある `value` の個数を返す。`select` は提供しない。

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

- `range_freq`: 区間 $[l, r)$ にある `upper` 未満の値の個数、または $[\mathit{lower}, \mathit{upper})$ に含まれる値の個数を返す。
- `sum`: 区間 $[l, r)$ の全要素の重みの総和を返す。
- `range_sum`: 区間 $[l, r)$ のうち、値が `upper` 未満、または $[\mathit{lower}, \mathit{upper})$ に含まれる要素の重みの総和を返す。
- `sum_k_smallest` / `sum_k_largest`: 区間 $[l, r)$ を値の小さい順 / 大きい順に並べたときの先頭 `k` 個の重みの総和を返す。$0 \le k \le r - l$ である。

## k 番目の値 / 中央値 / 前後の値

```cpp
T kth_smallest(int l, int r, int k) const
T kth_largest(int l, int r, int k) const
T quantile(int l, int r, int k) const
T range_min(int l, int r) const
T range_max(int l, int r) const
T median_floor(int l, int r) const
T median_ceil(int l, int r) const
optional<T> prev_value(int l, int r, const T& upper) const
optional<T> next_value(int l, int r, const T& lower) const
```

意味は Compressed Wavelet Matrix と同じである。`quantile` は `kth_smallest` の別名、`median_floor` / `median_ceil` は小さい方から $\lfloor (r - l - 1) / 2 \rfloor$ 番目 / $\lfloor (r - l) / 2 \rfloor$ 番目の値を返す。`prev_value` / `next_value` は該当なしなら `nullopt` を返す。

## 時間計算量

$N$ を列長、$D$ を相異なる値の個数、$H = \texttt{ID_BIT_WIDTH}$ とする。

- コンストラクタ: 可変長版は $O(N \log N + H N)$、固定長版は配列の初期化を含めて $O(N \log N + H \cdot \texttt{MAX_SIZE})$
- `size`, `value_count`, `value`, `sum`: $O(1)$
- `access`, `operator[]`, `kth_smallest`, `kth_largest`, `quantile`, `sum_k_smallest`, `sum_k_largest`, `range_min`, `range_max`, `median_floor`, `median_ceil`: $O(H)$
- `rank`, `range_freq`, `range_sum`, `prev_value`, `next_value`: $O(\log D + H)$

## 空間計算量

$O(H N)$（固定長版では $O(H \cdot \texttt{MAX_SIZE})$）。

## 注意点

- 列の区間は半開区間 $[l, r)$、値域も半開区間 $[\mathit{lower}, \mathit{upper})$ である。
- `range_min` / `range_max` / `median_floor` / `median_ceil` は空でない区間を前提とする。
- 2 つの入力列の長さが異なる場合、列長が `MAX_SIZE` を超える場合、添字・区間・圧縮 ID・`k` が範囲外の場合、`lower > upper` の場合は `std::runtime_error` を送出する。
- 圧縮 ID は内部で符号なし整数として保持するので、相異なる値の個数 $D$ は $D \le 2^{\texttt{ID_BIT_WIDTH}}$ を満たす必要がある。満たさない場合は構築時に `std::runtime_error` を送出する。
- コピーは禁止、ムーブは可能。
