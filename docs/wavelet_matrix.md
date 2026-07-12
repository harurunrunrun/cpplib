---
title: Wavelet Matrix
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
