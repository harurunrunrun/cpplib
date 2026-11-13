---
title: Wavelet Matrix (ウェーブレット行列) [range_kth_smallest]
documentation_of: ../../../../src/structure/wavelet_matrix/static/wavelet_matrix.hpp
---

整数列に対する静的なウェーブレット行列。符号付き整数にも対応する。

## テンプレート引数

```cpp
WaveletMatrix<T, MAX_SIZE = -1, BIT_WIDTH = (T の bit 数)>
```

- `T`: 整数型
- `MAX_SIZE`: 列長の上限。省略時（`-1`）は `std::vector` を使う可変長版になり、入力長に応じた領域だけを確保する。非負の値を指定すると固定長配列版になる。
- `BIT_WIDTH`: 使用する bit 数。省略時は `T` の bit 数。符号付き整数では省略しなければならない（`static_assert`）。

```cpp
using Variable = WaveletMatrix<T>;
using Fixed = WaveletMatrix<T, N>;
```

## コンストラクタ

```cpp
explicit WaveletMatrix(const vector<T>& values)
explicit WaveletMatrix(const array<T, N>& values)
int size() const
```

`values` から構築する。`size` は列長を返す。

## 要素取得

```cpp
T access(int k) const
T operator[](int k) const
```

`k` 番目（0-indexed）の値を返す。

## rank / select

```cpp
int rank(T value, int r) const
int rank(T value, int l, int r) const
int select(T value, int k) const
```

`rank` は区間 $[0, r)$ または $[l, r)$ にある `value` の個数を返す。`select` は `value` が `k` 回目（0-indexed）に現れる位置を返し、存在しない場合は `size()` を返す。

## k 番目の値

```cpp
T kth_smallest(int l, int r, int k) const
T kth_largest(int l, int r, int k) const
```

区間 $[l, r)$ で `k` 番目（0-indexed）に小さい値、または大きい値を返す。

## 頻度

```cpp
int range_freq(int l, int r, T upper) const
int range_freq(int l, int r, T lower, T upper) const
```

区間 $[l, r)$ にある `upper` 未満の値の個数、または $[\mathit{lower}, \mathit{upper})$ に含まれる値の個数を返す。

## 前後の値

```cpp
optional<T> prev_value(int l, int r, T upper) const
optional<T> next_value(int l, int r, T lower) const
```

区間 $[l, r)$ 内で `upper` 未満の最大値、または `lower` 以上の最小値を返す。存在しない場合は `nullopt` を返す。

## 時間計算量

$N$ を列長、$D = \texttt{BIT_WIDTH}$ とし、$W$ を固定長版では $\lceil \texttt{MAX_SIZE} / 64 \rceil$、可変長版では $\lceil N / 64 \rceil$ とする。

- コンストラクタ: $O(D(N + W))$（固定長版では作業配列の初期化に $O(\texttt{MAX_SIZE})$ が加わる）
- `size`: $O(1)$
- `access`, `operator[]`, `rank`, `kth_smallest`, `kth_largest`, `range_freq`, `prev_value`, `next_value`: $O(D)$
- `select`: $O(D \log N)$

## 空間計算量

$O(D W)$ ワードと $O(N)$（固定長版では $O(\texttt{MAX_SIZE})$）の作業配列。

## 注意点

- 列の区間は半開区間 $[l, r)$、値域も半開区間 $[\mathit{lower}, \mathit{upper})$ である。
- 列長が `MAX_SIZE` を超える場合、値が `BIT_WIDTH` bit に収まらない場合、添字・区間・`k` が範囲外の場合、`lower > upper` の場合は `std::runtime_error` を送出する。
- `range_freq` / `prev_value` / `next_value` などに渡す `upper` / `lower` が $2^{\texttt{BIT_WIDTH}}$ 以上の場合は、すべての要素より大きい値として扱う。`rank` / `select` に $2^{\texttt{BIT_WIDTH}}$ 以上の値を渡すと、出現しない値として扱う。
- コピーは禁止、ムーブは可能。
