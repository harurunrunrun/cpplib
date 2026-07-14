---
title: Wavelet Matrix (ウェーブレット行列) [range_kth_smallest] [static_range_frequency]
documentation_of: ../src/structure/wavelet_matrix/wavelet_matrix.hpp
---

整数列に対する静的wavelet matrix。符号付き整数にも対応する。

# テンプレート引数

```cpp
WaveletMatrix<T, MAX_SIZE, BIT_WIDTH>
```

- 整数型 `T`
- 配列長の上限 `MAX_SIZE`
- 使用するbit数 `BIT_WIDTH`。省略時は `T` のbit数

符号付き整数では `BIT_WIDTH` を省略する。

# 要素取得

```cpp
T access(int k) const
T operator[](int k) const
```

## 時間計算量

- $O(\mathrm{BIT\_WIDTH})$

# rank / select

```cpp
int rank(T value, int r) const
int rank(T value, int l, int r) const
int select(T value, int k) const
```

`rank` は指定区間にある `value` の個数を返す。`select` は `value` が `k` 回目に現れる位置を返し、存在しない場合は `size()` を返す。

## 時間計算量

- `rank`: $O(\mathrm{BIT\_WIDTH})$
- `select`: $O(\mathrm{BIT\_WIDTH}\log \mathrm{MAX\_SIZE})$

# k番目

```cpp
T kth_smallest(int l, int r, int k) const
T kth_largest(int l, int r, int k) const
```

区間 $[l,r)$ の `k` 番目に小さい値または大きい値を返す。`k` は0-indexed。

## 時間計算量

- $O(\mathrm{BIT\_WIDTH})$

# 頻度

```cpp
int range_freq(int l, int r, T upper) const
int range_freq(int l, int r, T lower, T upper) const
```

区間 $[l,r)$ にある `upper` 未満、または `[lower, upper)` の値の個数を返す。

## 時間計算量

- $O(\mathrm{BIT\_WIDTH})$

# 前後の値

```cpp
optional<T> prev_value(int l, int r, T upper) const
optional<T> next_value(int l, int r, T lower) const
```

区間内の `upper` 未満の最大値、または `lower` 以上の最小値を返す。存在しない場合は `nullopt`。

## 時間計算量

- $O(\mathrm{BIT\_WIDTH})$

# 計算量（公開操作別）

$N$ を列長、$D=\mathtt{BIT\_WIDTH}$、$W=\lceil\mathtt{MAX\_SIZE}/64\rceil$ とする。

- vector/array constructor: 固定長作業配列とbit辞書の初期化を含めて $O(\mathtt{MAX\_SIZE}+D(N+W))$
- `size`: $O(1)$
- `access`, `operator[]`, `rank`, `kth_smallest`, `kth_largest`, `range_freq`, `prev_value`, `next_value`: $O(D)$
- `select`: $O(D\log N)$

## 注意点

```cpp
WaveletMatrix(const vector<T>& values)
WaveletMatrix(const array<T, N>& values)
int size() const
```

入力列を構築し、`size` は列長を返す。`access/operator[]` の点、`rank/select` の出現番号、
順序統計の `k` は0-indexed。列区間は半開区間 `[l,r)`、値域は
`[lower,upper)`。`select` は該当なしなら `size()`、前後値は `nullopt`。

列長容量、値のbit幅、点・区間・順序・値域の違反では `runtime_error`。
copyは禁止、moveは可能。各APIの計算量は上記表の通り。
