---
title: Compressed Wavelet Matrix (座標圧縮ウェーブレット行列) [KQUERY] [MKTHNUM] [range_kth_smallest] [static_range_frequency]
documentation_of: ../../../../src/structure/wavelet_matrix/static/compressed_wavelet_matrix.hpp
---

値を座標圧縮して扱う静的なウェーブレット行列。`<` で比較でき、`!=` を持つ任意の型を値として使える。

## テンプレート引数

```cpp
CompressedWaveletMatrix<T, MAX_SIZE = -1, ID_BIT_WIDTH = 32>
```

- `T`: 値の型
- `MAX_SIZE`: 列長の上限。省略時（`-1`）は `std::vector` を使う可変長版になり、入力長に応じた領域だけを確保する。非負の値を指定すると固定長配列版になる。
- `ID_BIT_WIDTH`: 圧縮後の ID に使う bit 数。省略時は `unsigned` の bit 数（通常 32）。

```cpp
using Variable = CompressedWaveletMatrix<T>;
using Fixed = CompressedWaveletMatrix<T, N>;
```

相異なる値の個数を $D$ とすると、$D \le 2^{\texttt{ID_BIT_WIDTH}}$ が必要である。圧縮 ID は内部で符号なし整数として保持するため、たとえば 15 bit なら ID `0` から `32767` まで、すなわち 32768 種類の値を表現できる。列長はこの種類数を超えてもよい。

## コンストラクタ

```cpp
explicit CompressedWaveletMatrix(const vector<T>& values)
explicit CompressedWaveletMatrix(const array<T, N>& values)
```

`values` を座標圧縮して構築する。

## 列と圧縮 ID の情報

```cpp
int size() const
int value_count() const
const T& value(int id) const
```

`size` は列長、`value_count` は相異なる値の個数を返す。`value(id)` は圧縮 ID `id` に対応する元の値を返す。

## 要素取得

```cpp
T access(int k) const
T operator[](int k) const
```

`k` 番目（0-indexed）の値を返す。

## rank / select

```cpp
int rank(const T& value, int r) const
int rank(const T& value, int l, int r) const
int select(const T& value, int k) const
```

`rank` は区間 $[0, r)$ または $[l, r)$ にある `value` の個数を返す。`select` は `value` が `k` 回目（0-indexed）に現れる位置を返し、存在しない場合は `size()` を返す。

## k 番目の値 / 中央値

```cpp
T kth_smallest(int l, int r, int k) const
T kth_largest(int l, int r, int k) const
T quantile(int l, int r, int k) const
T range_min(int l, int r) const
T range_max(int l, int r) const
T median_floor(int l, int r) const
T median_ceil(int l, int r) const
```

区間 $[l, r)$ について、`kth_smallest` / `kth_largest` は `k` 番目（0-indexed）に小さい値 / 大きい値を返す。`quantile` は `kth_smallest` の別名である。`range_min` / `range_max` は最小値 / 最大値を返す。`median_floor` / `median_ceil` は小さい方から $\lfloor (r - l - 1) / 2 \rfloor$ 番目 / $\lfloor (r - l) / 2 \rfloor$ 番目の値を返す。

## 頻度

```cpp
int range_freq(int l, int r, const T& upper) const
int range_freq(int l, int r, const T& lower, const T& upper) const
int count_greater(int l, int r, const T& value) const
```

`range_freq` は区間 $[l, r)$ にある `upper` 未満の値の個数、または $[\mathit{lower}, \mathit{upper})$ に含まれる値の個数を返す。`count_greater` は同じ区間にある `value` より大きい値の個数を返す。

## 前後の値

```cpp
optional<T> prev_value(int l, int r, const T& upper) const
optional<T> next_value(int l, int r, const T& lower) const
```

区間 $[l, r)$ 内で `upper` 未満の最大値、または `lower` 以上の最小値を返す。存在しない場合は `nullopt` を返す。

## 時間計算量

$N$ を列長、$D$ を相異なる値の個数、$H = \texttt{ID_BIT_WIDTH}$ とし、$W$ を固定長版では $\lceil \texttt{MAX_SIZE} / 64 \rceil$、可変長版では $\lceil N / 64 \rceil$ とする。

- コンストラクタ: $O(N \log N + H(N + W))$（固定長版では作業配列の初期化に $O(\texttt{MAX_SIZE})$ が加わる）
- `size`, `value_count`, `value`: $O(1)$
- `access`, `operator[]`, `kth_smallest`, `kth_largest`, `quantile`, `range_min`, `range_max`, `median_floor`, `median_ceil`: $O(H)$
- `rank`, `range_freq`, `count_greater`, `prev_value`, `next_value`: $O(\log D + H)$
- `select`: $O(\log D + H \log N)$

## 注意点

- 列の区間は半開区間 $[l, r)$、値域も半開区間 $[\mathit{lower}, \mathit{upper})$ である。
- `range_min` / `range_max` / `median_floor` / `median_ceil` は空でない区間を前提とする。
- 列長が `MAX_SIZE` を超える場合、相異なる値の個数が $2^{\texttt{ID_BIT_WIDTH}}$ を超える場合、添字・区間・圧縮 ID・`k` が範囲外の場合、`lower > upper` の場合は `std::runtime_error` を送出する。
- 公開 API の圧縮 ID・添字・個数は `int` で、内部でのみ符号なし整数に変換する。
- コピーは禁止、ムーブは可能。
